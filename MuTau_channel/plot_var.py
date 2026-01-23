import ROOT

# --- User Configurable ---
input_file = "/eos/home-m/mblancco/samples_2018_mutau/Data_2018_UL_MuTau_nano_merged_proton_vars.root"    # Change to your file
tree_name = "tree"                # TTree name
variable = "nproton_multi"                # Variable to plot
bins = 20                       # Number of bins
x_min = 0                         # X-axis min
x_max = 10           # X-axis max
output_plot = "nproton_multi.png"    # Output image file

def plot_n_protons(file_mc,file_data,tree_name,bins,x_min,x_max,variable_mc,variable_data):

    f_mc = ROOT.TFile.Open(file_mc)
    if not f_mc or f_mc.IsZombie():
        raise IOError(f"Cannot open file: {file_mc}")

    f_data = ROOT.TFile.Open(file_data)
    if not f_data or f_data.IsZombie():
        raise IOError(f"Cannot open file: {file_data}")
    tree_mc = f_mc.Get(tree_name)
    if not tree_mc:
        raise IOError(f"TTree '{tree_name}' not found in {file_mc}")

    tree_data = f_data.Get(tree_name)
    if not tree_data:
        raise IOError(f"TTree '{tree_name}' not found in {file_data}")

    tt="MC vs data protons"

    # Create MC histogram
    hist_mc = ROOT.TH1F("hist_mc", f"{tt};{variable_mc};Entries", bins, x_min, x_max)
    hist_mc.SetLineColor(ROOT.kBlue)
    hist_mc.SetFillColor(ROOT.kBlue)
    hist_mc.SetFillStyle(3004)

    # Create data histogram (will be drawn as outline)
    hist_data = ROOT.TH1F("hist_data", f"{tt};{variable_data};Entries", bins, x_min, x_max)
    hist_data.SetLineColor(ROOT.kBlack)
    hist_data.SetLineWidth(2)

    # Fill histograms
    tree_mc.Draw(f"{variable_mc}>>hist_mc", "", "goff")
    tree_data.Draw(f"{variable_data}>>hist_data", "", "goff")

    # Create canvas and draw
    canvas = ROOT.TCanvas("canvas", "canvas", 800, 600)
    canvas.SetLogy()  # Set y-axis to log scale
    hist_mc.Draw("HIST")
    hist_data.Draw("HIST SAME")
    hist_mc.GetYaxis().SetRangeUser(0.1, 1e8)

    # Add legend
    legend = ROOT.TLegend(0.7, 0.7, 0.9, 0.9)
    legend.AddEntry(hist_mc, "MC", "f")
    legend.AddEntry(hist_data, "Data", "l")
    legend.Draw()

    canvas.Update()
    canvas.SaveAs("arm0_data_vs_mc.png")
    print(f"✔ Plot saved")

def plot_v(input_file,tree_name,variable,bins,x_min,x_max,output_plot):
# --- Open the ROOT file ---
    f = ROOT.TFile.Open(input_file)
    if not f or f.IsZombie():
        raise IOError(f"Cannot open file: {input_file}")

    tree = f.Get(tree_name)
    if not tree:
        raise IOError(f"TTree '{tree_name}' not found in {input_file}")

    # --- Create histogram ---
    tt=variable
    hist = ROOT.TH1F("hist", f"{tt};{variable};Entries", bins, x_min, x_max)

    # --- Fill histogram from tree ---
    tree.Draw(f"{variable}>>hist", "", "goff")  # goff = no GUI drawing

    # --- Plot it ---
    canvas = ROOT.TCanvas("canvas", "canvas", 800, 600)
    canvas.SetLogy()  # Set y-axis to log scale
    hist.Draw()
    hist.GetYaxis().SetRangeUser(0.1, 1e7)
    canvas.Update()
    canvas.SaveAs(output_plot)

    print(f"✔ Plot saved as {output_plot}")


input_mc="/eos/cms/store/group/phys_smp/Exclusive_DiTau/proton_pool_2018/proton_pool_2018.root"
input_data="/eos/user/m/mblancco/tau_analysis/MuTau_channel/Data_2018_UL_MuTau_nano_merged_proton_vars.root"
plot_n_protons(input_mc,input_file,"tree",10,0,10,"n_pu","nproton_multi")

# plot_v(input_file,"tree",variable,bins,x_min,x_max,output_plot)