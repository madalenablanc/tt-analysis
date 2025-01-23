import ROOT
import os
import numpy as np
import ctypes


filename="/eos/user/m/mblancco/samples_2018_tautau/fase0_2/ttJetscode_GammaGammaTauTau_SignalMC_SM_18UL_23k_NANOAODSIM_fase0_with_xi_and_deltaphi.root"

file = ROOT.TFile.Open(filename)


tree_name = "tree"  # Replace with the actual TTree name
tree = file.Get(tree_name)

vars_corrs=[]
f=open("corrs_vars.txt","r")

for line in f:
    vars_corrs.append(line.strip().split(","))

print(vars_corrs)

f.close()

def creat_scatter(v1,v2,tree,nbinsx,minx,maxx,nbinsy,miny,maxy):

    hist=ROOT.TH2F(v1+" "+v2,v1+v2,nbinsx,minx,maxx,nbinsy,miny,maxy)

    for event in tree:
        x_value = getattr(event, v1)
        y_value = getattr(event, v2)

        hist.Fill(x_value,y_value)

    # tf1=ROOT.TF1("tf1",hist,)    
    hist.Fit("pol1")
    c1 = ROOT.TCanvas("canvas", "Scatter Plots", 800, 600)
    hist.Draw()
    hist.Draw("colz")
    c1.SaveAs("/eos/user/m/mblancco/tau_analysis/plots/scatter_plots/"+"scatter_"+v1+"_"+v2+".png")

#creat_scatter("tau0_pt","tau1_pt",tree,100,0,900,100,0,900)

hist2=ROOT.TH2F("m_diff","m_diff",75,-500,2000,75,-3,3)

for event in tree: #diff mass vs diff rapidty
    p_mass=event.p_invariant_mass
    t_mass=event.invariant_mass_tt_pair

    p_rapidity=event.p_rapidity
    tt_rapidity=event.tt_rapidity

    hist2.Fill(p_mass-t_mass,p_rapidity-tt_rapidity)

c2 = ROOT.TCanvas("canvas", "Scatter Plots", 800, 600)
hist2.Draw()
hist2.Draw("colz")
c2.SaveAs("/eos/user/m/mblancco/tau_analysis/plots/scatter_plots/"+"diffs_p_tt.png")
    

dirr="/eos/user/m/mblancco/tau_analysis/plots/scatter_plots/"

if not os.path.exists(dirr):
    os.makedirs(dirr)


for pair in vars_corrs:
    v1=pair[0]
    v2=pair[1]
    nbinsx=int(pair[2])
    xmin=float(pair[3])
    xmax=float(pair[4])
    nbinsy=int(pair[5])
    ymin=float(pair[6])
    ymax=float(pair[7])
    # print(nbinsx)
    # print(nbinsy)
    # print()
    creat_scatter(v1,v2,tree,nbinsx,xmin,xmax,nbinsy,ymin,ymax)
    print("creating scatter of pair")


# creat_scatter("invariant_mass_tt_pair","p_invariant_mass",tree,100,0,2000,100,0,2000)




# hs =ROOT.THStack("hs","Histograms");

# hist_tau0_pt = ROOT.TH2F("tau0_pt", "Tau+ pT; pT [GeV]; Events", 100,0,900,100,0,150)
# hist_tau1_pt = ROOT.TH2F("tau1_pt", "Tau- pT; pT [GeV]; Events", 100,0,900,100,0,150)   
# hist_new=ROOT.TH2F("tau1_pt", "Tau- pT; pT [GeV]; Events", 100,0,900,100,0,900)   

# for event in tree:

#     hist_new.Fill(event.tau0_pt,event.tau1_pt)    
    #hist_tau0_pt.Fill(event.tau0_pt)
    # hist_tau1_pt.Fill(event.tau1_pt)

# hist_tau0_pt.SetLineColor(ROOT.kBlue)
# hist_tau1_pt.SetLineColor(ROOT.kRed)

# hs.Add(hist_tau0_pt)
# hs.Add(hist_tau1_pt)   


# canvas = ROOT.TCanvas("canvas", "THStack Example", 800, 600)
# # canvas.Divide(2, 1) 
# hist_new.Draw()
# hist_new.Draw ( "colz" )
# # canvas.cd(1)
# hs.Draw("hist") 


# canvas.cd(2)
# scatter.Draw("AP")

# legend = ROOT.TLegend(0.7, 0.7, 0.9, 0.9)  # Position: x1, y1, x2, y2
# legend.AddEntry(h1, "Dataset 1", "f")  # "f" for filled
# legend.AddEntry(h2, "Dataset 2", "f")
# legend.AddEntry(h3, "Dataset 3", "f")
# legend.Draw()

# canvas.SaveAs("stacked_histograms.png")

# canvas.SaveAs("stack_and_scatter.png")

# def plot_hists(v1,v2,x_range):
#     hs =ROOT.THStack("hs","Histograms");

#     hist_tau0_pt = ROOT.TH1F("tau0_pt", "Tau+ pT; pT [GeV]; Events", 100, 0, 900)
#     hist_tau1_pt = ROOT.TH1F("tau1_pt", "Tau- pT; pT [GeV]; Events", 100, 0, 900)   

#     for event in tree:

#         hist_tau0_pt.Fill(event.tau0_pt)
#         hist_tau1_pt.Fill(event.tau1_pt)

#     hist_tau0_pt.SetLineColor(ROOT.kBlue)
#     hist_tau1_pt.SetLineColor(ROOT.kRed)

#     hs.Add(hist_tau0_pt)
#     hs.Add(hist_tau1_pt)   

    


# # Function to save histograms without showing them
# def save_histogram(stack, filename):
#     canvas = ROOT.TCanvas("canvas", "", 800, 600)  # Create a canvas
#     canvas.SetBatch(True)  # Enable batch mode to suppress display
#     hist.Draw("HIST")  # Draw the histogram on the canvas
#     canvas.SaveAs(filename)  # Save the canvas as a file
#     canvas.Close()  # Close the canvas to free memory
#     print(f"Saved {filename}")
