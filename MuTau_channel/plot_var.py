import ROOT

# --- User Configurable ---
input_file = "/eos/user/m/mblancco/tau_analysis/MuTau_channel/MuTau_sinal_SM_2018_july.root"    # Change to your file
tree_name = "tree"                # TTree name
variable = "xi_arm2_1"                # Variable to plot
bins = 50                         # Number of bins
x_min = 0                         # X-axis min
x_max = 0.2                     # X-axis max
output_plot = "xi_arm2_mutau_signal.png"    # Output image file

# --- Open the ROOT file ---
f = ROOT.TFile.Open(input_file)
if not f or f.IsZombie():
    raise IOError(f"Cannot open file: {input_file}")

tree = f.Get(tree_name)
if not tree:
    raise IOError(f"TTree '{tree_name}' not found in {input_file}")

# --- Create histogram ---
tt=variable+" merged"
hist = ROOT.TH1F("hist", f"{tt};{variable};Entries", bins, x_min, x_max)

# --- Fill histogram from tree ---
tree.Draw(f"{variable}>>hist", "", "goff")  # goff = no GUI drawing

# --- Plot it ---
canvas = ROOT.TCanvas("canvas", "canvas", 800, 600)
hist.Draw()
canvas.SaveAs(output_plot)

print(f"✔ Plot saved as {output_plot}")
