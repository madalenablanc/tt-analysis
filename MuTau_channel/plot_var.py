import ROOT

# --- User Configurable ---
input_file = "/eos/user/m/mblancco/samples_2018_mutau/ttjets_2018_UL_skimmed_MuTau_nano_1.root"     # Change to your file
tree_name = "tree"                # TTree name
variable = "sist_mass"                # Variable to plot
bins = 50                         # Number of bins
x_min = 0                         # X-axis min
x_max = 500                       # X-axis max
output_plot = "mu_pt_plot.png"    # Output image file

# --- Open the ROOT file ---
f = ROOT.TFile.Open(input_file)
if not f or f.IsZombie():
    raise IOError(f"Cannot open file: {input_file}")

tree = f.Get(tree_name)
if not tree:
    raise IOError(f"TTree '{tree_name}' not found in {input_file}")

# --- Create histogram ---
hist = ROOT.TH1F("hist", f"{variable};{variable};Entries", bins, x_min, x_max)

# --- Fill histogram from tree ---
tree.Draw(f"{variable}>>hist", "", "goff")  # goff = no GUI drawing

# --- Plot it ---
canvas = ROOT.TCanvas("canvas", "canvas", 800, 600)
hist.Draw()
canvas.SaveAs(output_plot)

print(f"✔ Plot saved as {output_plot}")
