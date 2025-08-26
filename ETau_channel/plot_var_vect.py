import ROOT

# --- User Configurable ---
input_file  = "/eos/cms/store/user/jjhollar/TauTau_NanoAOD_Madalena/BackgroundSamples/ETau/Dados_2018_UL_skimmed_ETau_nano_fase0_merge.root"
tree_path   = "analyzer/ntp1"      # <- full path to the TTree
variable    = "tau_id_antimu"             # vector<float> branch
bins, x_min, x_max = 10, 0, 2
output_plot = "tau_id_antimu_plot_newv.png"

# Choose how to treat vector branches:
#   "first" -> plot the leading element [0] safely (Alt$ protects empty vectors)
#   "all"   -> plot all elements of the vector across all events
VECTOR_MODE = "first"
INDEX = 0  # used only when VECTOR_MODE == "first"

# --- Open the ROOT file ---
f = ROOT.TFile.Open(input_file)
if not f or f.IsZombie():
    raise IOError(f"Cannot open file: {input_file}")

# Get the tree from the directory path
tree = f.Get(tree_path)  # works with "dir/object" paths
if not tree:
    # fallback: get directory then object
    d = f.Get("analyzer")
    tree = d.Get("ntp1") if d else None
if not tree:
    raise IOError(f"TTree '{tree_path}' not found in {input_file}")

# --- Histogram ---
title = f"{variable} ({'leading element' if VECTOR_MODE=='first' else 'all elements'})"
hist = ROOT.TH1F("hist", f"{title};{variable};Entries", bins, x_min, x_max)

# --- Fill histogram ---
if VECTOR_MODE == "first":
    expr = f"Alt$({variable}[{INDEX}], -1)"   # safe leading element
    sel  = f"{expr} >= 0"
    tree.Draw(f"{expr}>>hist", sel, "goff")
elif VECTOR_MODE == "all":
    # TTree::Draw iterates over all elements of vector branches if no index is specified
    tree.Draw(f"{variable}>>hist", "", "goff")
else:
    raise ValueError("VECTOR_MODE must be 'first' or 'all'.")

# --- Plot ---
c = ROOT.TCanvas("canvas", "canvas", 800, 600)
hist.Draw()
c.SaveAs(output_plot)
print(f"✔ Plot saved as {output_plot}")
