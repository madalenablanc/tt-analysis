import ROOT

# Open the ROOT file
file = ROOT.TFile("/eos/cms/store/user/jjhollar/TauTau_NanoAOD_Madalena/BackgroundSamples/TauhTauh/Dados_2018_UL_skimmed_TauTau_nano_fase1total-protons_2018.root", "READ")

# Method 1: Plot a histogram that's already in the file
# hist = file.Get("histogram_name")
# canvas = ROOT.TCanvas("c1", "Canvas", 800, 600)
# hist.Draw()
# canvas.SaveAs("plot.png")

# Method 2: Plot a variable from a TTree
tree = file.Get("tree")  # Replace with your tree name

# Create a histogram for the variable
hist = ROOT.TH1F("h1", "Variable Title;X-axis;Y-axis", 100, -1, 5)  # Adjust bins and range

# Fill histogram from tree variable
tree.Draw("n_b_jet>>h1")  # Replace 'variable_name' with your variable

# Create canvas and draw
canvas = ROOT.TCanvas("c1", "Canvas", 800, 600)
hist.SetLineColor(ROOT.kBlue)
hist.SetFillColor(ROOT.kBlue-10)
hist.Draw()

# Add title and labels
hist.SetTitle("Your Plot Title")
hist.GetXaxis().SetTitle("X-axis Label")
hist.GetYaxis().SetTitle("Y-axis Label")

# Save the plot
canvas.SaveAs("output_plot.png")  # Can also use .pdf, .eps, .svg, etc.

# Optional: Save as ROOT file too
output_file = ROOT.TFile("output.root", "RECREATE")
hist.Write()
output_file.Close()

# Clean up
file.Close()
print("Plot saved successfully!")