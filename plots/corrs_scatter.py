
import ROOT
import os
import numpy as np
import ctypes

filename="/eos/user/m/mblancco/samples_2018_tautau/fase0_2/ttJetscode_GammaGammaTauTau_SignalMC_SM_18UL_23k_NANOAODSIM_fase0_with_xi_and_deltaphi.root"

file = ROOT.TFile.Open(filename)

if not file or file.IsZombie():
    print(f"Error: Could not open file {filename}")
    exit(1)

tree_name = "tree"  # Replace with the actual TTree name
tuples = file.Get(tree_name)

if not tuples:
    print(f"Error: Could not find TTree '{tree_name}' in file {filename}")
    file.Close()
    exit(1)


vars_corrs=[]
f=open("corrs_vars.txt","r")

for line in f:
    vars_corrs.append(line.strip().split(","))

print(vars_corrs)

f.close()


def plot_scatter(vars_pair,tree):
    var1=vars_pair[0]
    var2=vars_pair[1]

    #scatter = ROOT.TScatter()

    x_list = []
    y_list = [] 
    colors_list = []

    for event in tuples:
        x_value = getattr(event, var1)
        y_value = getattr(event, var2)
        x_list.append(x_value) 
        colors_list.append(ROOT.kRed)
        y_list.append(y_value)  
        colors_list.append(ROOT.kBlue)
    
    # Convert lists to numpy arrays
    x = np.array(x_list, dtype=np.float64)
    y = np.array(y_list, dtype=np.float64)
    colors = np.array(colors_list, dtype=np.float64)  # Colors

    # Convert numpy arrays to C-style arrays (pointers)
    x_ptr = x.ctypes.data_as(ctypes.POINTER(ctypes.c_double))
    y_ptr = y.ctypes.data_as(ctypes.POINTER(ctypes.c_double))
    colors_ptr = colors.ctypes.data_as(ctypes.POINTER(ctypes.c_double))
    n_points = len(x)

    
    # Create the TScatter object
    scatter = ROOT.TScatter(n_points, x_ptr, y_ptr,colors_ptr)  # Pass C-style pointers

    # Customize the scatter plot
    scatter.SetMarkerStyle(20)
    scatter.SetMarkerColor(ROOT.kBlue)
    scatter.SetTitle(f"Scatter Plot of {var1} vs {var2}")

    # Draw and save the scatter plot
    canvas = ROOT.TCanvas("canvas", "Scatter Plot", 800, 600)
    scatter.Draw("AP")  # "A" for axes, "P" for points
    filenam="/eos/user/m/mblancco/plots/scatter/scatter_plot_"+var1+"_vs_" + var2 + ".png"
    canvas.SaveAs(filenam)



for pair in vars_corrs:
    plot_scatter(pair,tuples)

#plot_scatter(vars_corrs,tuples)



file.Close()



