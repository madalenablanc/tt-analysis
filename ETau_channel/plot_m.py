#!/usr/bin/env python3

import ROOT
import math
import os

def main():
    # Set ROOT to batch mode
    ROOT.gROOT.SetBatch(True)
    
    print("Starting data processing...")
    
    # Open input ROOT files
    try:
        dy_file = ROOT.TFile("/eos/cms/store/user/jjhollar/TauTau_NanoAOD_Madalena/BackgroundSamples/ETau/DY_2018_UL_skimmed_ETau_nano_fase1total-protons_2018.root")
        ttjets_file = ROOT.TFile("/eos/cms/store/user/jjhollar/TauTau_NanoAOD_Madalena/BackgroundSamples/ETau/ttJets_2018_UL_skimmed_ETau_nano_fase1total-protons_2018.root")
        qcd_file = ROOT.TFile("/eos/cms/store/user/jjhollar/TauTau_NanoAOD_Madalena/BackgroundSamples/ETau/QCD_2018_UL_skimmed_ETau_nano_fase1total-protons_2018.root")
        sinal_file = ROOT.TFile("ETau_sinal_SM_july.root")
    except Exception as e:
        print(f"Error opening files: {e}")
        return 1
    
    # Define histogram parameters for mass
    min_m, max_m, bin_m = 0, 1200, 30
    
    # Create simple histograms
    m_qcd = ROOT.TH1D("m_qcd", "QCD Mass", bin_m, min_m, max_m)
    m_dy = ROOT.TH1D("m_dy", "DY Mass", bin_m, min_m, max_m)
    m_ttjets = ROOT.TH1D("m_ttjets", "ttjets Mass", bin_m, min_m, max_m)
    m_sinal = ROOT.TH1D("m_sinal", "Signal Mass", bin_m, min_m, max_m)
    
    # Get trees
    tree_dy = dy_file.Get("tree")
    tree_qcd = qcd_file.Get("tree")
    tree_ttjets = ttjets_file.Get("tree")
    tree_sinal = sinal_file.Get("tree")
    
    print("Processing QCD events...")
    # Fill QCD histograms - simplified
    for i in range(min(1000, tree_qcd.GetEntries())):  # Limit to 1000 events for testing
        tree_qcd.GetEvent(i)
        try:
            mass = tree_qcd.GetLeaf("sist_mass").GetValue(0)
            if mass >= 0:
                m_qcd.Fill(mass, 1.0)
        except:
            continue
    
    print("Processing DY events...")
    # Fill DY histograms - simplified
    for i in range(min(1000, tree_dy.GetEntries())):
        tree_dy.GetEvent(i)
        try:
            mass = tree_dy.GetLeaf("sist_mass").GetValue(0)
            weight = tree_dy.GetLeaf("weight").GetValue(0)
            if mass >= 0:
                m_dy.Fill(mass, weight)
        except:
            continue
    
    print("Processing ttjets events...")
    # Fill ttjets histograms - simplified  
    for i in range(min(1000, tree_ttjets.GetEntries())):
        tree_ttjets.GetEvent(i)
        try:
            mass = tree_ttjets.GetLeaf("sist_mass").GetValue(0)
            weight = tree_ttjets.GetLeaf("weight").GetValue(0)
            if mass >= 0:
                m_ttjets.Fill(mass, weight)
        except:
            continue
    
    print("Processing signal events...")
    # Fill signal histograms - simplified
    for i in range(min(1000, tree_sinal.GetEntries())):
        tree_sinal.GetEvent(i)
        try:
            mass = tree_sinal.GetLeaf("sist_mass").GetValue(0)
            weight = tree_sinal.GetLeaf("weight_sm").GetValue(0) * 5000
            if mass >= 0:
                m_sinal.Fill(mass, weight)
        except:
            continue
    
    print("Data processing complete!")
    
    # Print some statistics instead of plotting
    print(f"QCD histogram entries: {m_qcd.GetEntries()}")
    print(f"DY histogram entries: {m_dy.GetEntries()}")
    print(f"ttjets histogram entries: {m_ttjets.GetEntries()}")
    print(f"Signal histogram entries: {m_sinal.GetEntries()}")
    
    print(f"QCD integral: {m_qcd.Integral()}")
    print(f"DY integral: {m_dy.Integral()}")
    print(f"ttjets integral: {m_ttjets.Integral()}")
    print(f"Signal integral: {m_sinal.Integral()}")
    
    # Save histograms to output file without plotting
    print("Saving histograms...")
    try:
        output_file = ROOT.TFile("DY_CR_e_tau_UL_2018_shapes.root", "RECREATE")
        m_qcd.Write()
        m_dy.Write()
        m_ttjets.Write()
        m_sinal.Write()
        output_file.Close()
        print("Histograms saved successfully!")
    except Exception as e:
        print(f"Error saving histograms: {e}")
    
    # Close input files
    try:
        dy_file.Close()
        ttjets_file.Close()
        qcd_file.Close()
        sinal_file.Close()
        print("Files closed successfully!")
    except Exception as e:
        print(f"Error closing files: {e}")
    
    print("Script completed successfully without plotting!")
    
    # Now try a very simple plot with matplotlib instead of ROOT
    try:
        import matplotlib.pyplot as plt
        import numpy as np
        
        print("Creating plot with matplotlib...")
        
        # Extract data from ROOT histograms
        n_bins = m_qcd.GetNbinsX()
        bin_edges = [m_qcd.GetBinLowEdge(i) for i in range(1, n_bins+2)]
        
        qcd_values = [m_qcd.GetBinContent(i) for i in range(1, n_bins+1)]
        dy_values = [m_dy.GetBinContent(i) for i in range(1, n_bins+1)]
        ttjets_values = [m_ttjets.GetBinContent(i) for i in range(1, n_bins+1)]
        signal_values = [m_sinal.GetBinContent(i) for i in range(1, n_bins+1)]
        
        # Create matplotlib plot
        fig, ax = plt.subplots(figsize=(10, 8))
        
        # Plot stacked histograms
        bin_centers = [(bin_edges[i] + bin_edges[i+1])/2 for i in range(len(bin_edges)-1)]
        width = bin_edges[1] - bin_edges[0]
        
        ax.bar(bin_centers, ttjets_values, width=width, label='$t\\bar{t}$', color='green', alpha=0.7)
        ax.bar(bin_centers, qcd_values, width=width, bottom=ttjets_values, label='QCD (Data driven)', color='red', alpha=0.7)
        
        # Add DY on top
        bottom_values = [ttjets_values[i] + qcd_values[i] for i in range(len(ttjets_values))]
        ax.bar(bin_centers, dy_values, width=width, bottom=bottom_values, label='Drell Yan', color='yellow', alpha=0.7)
        
        # Plot signal as line
        ax.plot(bin_centers, signal_values, 'k-', linewidth=3, label='Signal (x5000)')
        
        ax.set_xlabel('Invariant mass of the central system [GeV]')
        ax.set_ylabel('Events')
        ax.set_title('CMS-TOTEM Preliminary, 54.9 fb$^{-1}$ (13 TeV)')
        ax.legend()
        ax.set_yscale('log')
        ax.set_ylim(0.1, 200)
        
        # Create output directory
        if not os.path.exists("output_plots"):
            os.makedirs("output_plots")
        
        plt.savefig("output_plots/mass_etau_matplotlib.png", dpi=150, bbox_inches='tight')
        print("Matplotlib plot saved: output_plots/mass_etau_matplotlib.png")
        
    except ImportError:
        print("matplotlib not available, skipping plot creation")
    except Exception as e:
        print(f"Error creating matplotlib plot: {e}")
    
    return 0

if __name__ == "__main__":
    main()