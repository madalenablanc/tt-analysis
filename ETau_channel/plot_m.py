#!/usr/bin/env python3

import ROOT
import math
import os

def main():
    # Set ROOT to batch mode to avoid display issues
    ROOT.gROOT.SetBatch(True)
    
    # Create output directory if it doesn't exist
    if not os.path.exists("output_plots"):
        os.makedirs("output_plots")
    
    # Open input ROOT files
    dy_file = ROOT.TFile("/eos/cms/store/user/jjhollar/TauTau_NanoAOD_Madalena/BackgroundSamples/ETau/DY_2018_UL_skimmed_ETau_nano_fase1total-protons_2018.root")
    ttjets_file = ROOT.TFile("/eos/cms/store/user/jjhollar/TauTau_NanoAOD_Madalena/BackgroundSamples/ETau/ttJets_2018_UL_skimmed_ETau_nano_fase1total-protons_2018.root")
    qcd_file = ROOT.TFile("/eos/cms/store/user/jjhollar/TauTau_NanoAOD_Madalena/BackgroundSamples/ETau/QCD_2018_UL_skimmed_ETau_nano_fase1total-protons_2018.root")
    sinal_file = ROOT.TFile("ETau_sinal_SM_july.root")
    
    # Create output file
    output_file = ROOT.TFile("DY_CR_e_tau_UL_2018_shapes.root", "RECREATE")
    
    # Define weights
    w_qcd = 1.0
    w_ttjets = 0.15
    w_dy = 1.81
    
    # Define histogram parameters for mass (simplifying to just mass plot first)
    min_m, max_m, bin_m = 0, 1200, 30
    
    # Create histograms for mass only first
    m_qcd = ROOT.TH1D("m_qcd", "m_qcd", bin_m, min_m, max_m)
    m_dy = ROOT.TH1D("m_dy", "m_dy", bin_m, min_m, max_m)
    m_ttjets = ROOT.TH1D("m_ttjets", "m_ttjets", bin_m, min_m, max_m)
    m_sinal = ROOT.TH1D("m_sinal", "m_sinal", bin_m, min_m, max_m)
    
    # Get trees
    tree_dy = dy_file.Get("tree")
    tree_qcd = qcd_file.Get("tree")
    tree_ttjets = ttjets_file.Get("tree")
    tree_sinal = sinal_file.Get("tree")
    
    # Get number of events
    n_evt_dy = tree_dy.GetEntries()
    n_evt_qcd = tree_qcd.GetEntries()
    n_evt_ttjets = tree_ttjets.GetEntries()
    n_evt_sinal = tree_sinal.GetEntries()
    
    print(f"Processing {n_evt_qcd} QCD events...")
    
    # Fill QCD histograms
    n_qcd = 0
    for i in range(n_evt_qcd):
        tree_qcd.GetEvent(i)
        if tree_qcd.GetLeaf("sist_mass").GetValue(0) >= 0:
            n_qcd += w_qcd
            sist_mass = tree_qcd.GetLeaf("sist_mass").GetValue(0)
            m_qcd.Fill(sist_mass, w_qcd)
    
    print(f"QCD events processed: {n_qcd}")
    
    # Fill DY histograms
    print(f"Processing {n_evt_dy} DY events...")
    n_dy = 0
    for i in range(n_evt_dy):
        tree_dy.GetEvent(i)
        if tree_dy.GetLeaf("sist_mass").GetValue(0) >= 0:
            w_dy_event = tree_dy.GetLeaf("weight").GetValue(0)
            n_dy += w_dy_event
            sist_mass = tree_dy.GetLeaf("sist_mass").GetValue(0)
            m_dy.Fill(sist_mass, w_dy_event)
    
    print(f"DY events processed: {n_dy * 1.81}")
    
    # Fill ttjets histograms
    print(f"Processing {n_evt_ttjets} ttjets events...")
    n_tt = 0
    for i in range(n_evt_ttjets):
        tree_ttjets.GetEvent(i)
        if tree_ttjets.GetLeaf("sist_mass").GetValue(0) >= 0:
            w_ttjets_event = tree_ttjets.GetLeaf("weight").GetValue(0)
            n_tt += w_ttjets_event
            sist_mass = tree_ttjets.GetLeaf("sist_mass").GetValue(0)
            m_ttjets.Fill(sist_mass, w_ttjets_event)
    
    print(f"ttjets events processed: {n_tt * 0.15}")
    
    # Fill signal histograms
    print(f"Processing {n_evt_sinal} signal events...")
    for i in range(n_evt_sinal):
        tree_sinal.GetEvent(i)
        if tree_sinal.GetLeaf("sist_mass").GetValue(0) >= 0:
            w_sinal = tree_sinal.GetLeaf("weight_sm").GetValue(0) * 5000
            sist_mass = tree_sinal.GetLeaf("sist_mass").GetValue(0)
            m_sinal.Fill(sist_mass, w_sinal)
    
    print("Signal events processed successfully")
    
    # Set histogram properties
    m_qcd.SetFillColor(ROOT.kRed)
    m_qcd.SetLineWidth(0)
    
    m_dy.SetFillColor(ROOT.kYellow)
    m_dy.SetLineWidth(0)
    
    m_ttjets.SetFillColor(ROOT.kGreen)
    m_ttjets.SetLineWidth(0)
    
    m_sinal.SetLineColor(ROOT.kBlack)
    m_sinal.SetLineWidth(3)
    
    # Write histograms to output file
    m_dy.Write()
    m_ttjets.Write()
    output_file.Close()
    
    print("Creating mass plot...")
    
    # Create mass plot without THStack to avoid segfaults
    canvas = ROOT.TCanvas("c1", "c1", 700, 800)
    canvas.cd()
    
    # Clone histograms for stacking manually
    m_stack = m_ttjets.Clone("m_stack")
    m_stack.Add(m_qcd)
    m_stack.Add(m_dy)
    
    # Create legend
    legend = ROOT.TLegend(0.18, 0.7, 0.48, 0.9)
    legend.SetBorderSize(0)
    legend.AddEntry(m_ttjets, "t #bar{t}", "f")
    legend.AddEntry(m_dy, "Drell Yan", "f")
    legend.AddEntry(m_qcd, "QCD (Data driven)", "f")
    legend.AddEntry(m_sinal, "Signal (x5000)", "l")
    
    # Draw histograms
    m_ttjets.Draw("hist")
    m_qcd.Draw("hist same")
    m_dy.Draw("hist same")
    m_sinal.Draw("hist same")
    
    # Set axis properties
    m_ttjets.GetXaxis().SetTitle("Invariant mass of the central system [GeV]")
    m_ttjets.GetYaxis().SetTitle("Events")
    m_ttjets.GetXaxis().SetNdivisions(5)
    m_ttjets.GetYaxis().SetNdivisions(5)
    m_ttjets.GetXaxis().SetTitleSize(0.03)
    m_ttjets.GetYaxis().SetTitleSize(0.03)
    m_ttjets.GetXaxis().SetLabelSize(0.03)
    m_ttjets.GetYaxis().SetLabelSize(0.03)
    m_ttjets.GetXaxis().SetLabelOffset(0.01)
    m_ttjets.GetYaxis().SetLabelOffset(0.01)
    m_ttjets.GetXaxis().SetTitleOffset(1.0)
    m_ttjets.GetYaxis().SetRangeUser(0.1, 200)
    
    # Draw legend
    legend.Draw()
    
    # Add CMS labels
    cms_label = ROOT.TLatex()
    cms_label.SetTextAlign(31)
    cms_label.SetTextSize(0.04)
    cms_label.SetTextFont(62)
    cms_label.DrawLatexNDC(0.90, 0.92, "54.9 fb^{-1} (13 TeV)")
    
    cms_label1 = ROOT.TLatex()
    cms_label1.SetTextAlign(31)
    cms_label1.SetTextSize(0.04)
    cms_label1.SetTextFont(62)
    cms_label1.DrawLatexNDC(0.5, 0.92, "CMS-TOTEM Preliminary")
    
    canvas.Update()
    canvas.SaveAs("output_plots/mass_etau.png")
    
    print("Mass plot created successfully: output_plots/mass_etau.png")
    
    # Clean up
    canvas.Clear()
    del canvas
    
    # Close files
    dy_file.Close()
    ttjets_file.Close()
    qcd_file.Close()
    sinal_file.Close()
    
    print("Script completed successfully!")
    return 0

if __name__ == "__main__":
    main()