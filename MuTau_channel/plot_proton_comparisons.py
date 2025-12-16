#!/usr/bin/env python3
"""
Script to create comparison plots between data and MC:
1. Physics variables with no proton requirements
2. Number of protons per event distributions
"""

import ROOT
import os
import sys

# Enable batch mode to avoid displaying plots
ROOT.gROOT.SetBatch(True)
ROOT.gStyle.SetOptStat(0)

# ============================= Configuration =============================

# Input files
DATA_FILE = "/eos/user/m/mblancco/samples_2018_mutau/fase1_data_proton_vars/Data_2018_UL_merged.root"
MC_FILE = "/eos/user/m/mblancco/samples_2018_mutau/Data_2018_UL_MuTau_nano_merged_pileup_protons.root"

# Output directory for plots
OUTPUT_DIR = "plots_proton_comparison"
os.makedirs(OUTPUT_DIR, exist_ok=True)

# Tree name
TREE_NAME = "tree"

# ============================= Helper Functions =============================

def setup_histogram_style(h_data, h_mc):
    """Apply consistent styling to data and MC histograms."""
    # Data: black markers
    h_data.SetMarkerStyle(20)
    h_data.SetMarkerSize(0.8)
    h_data.SetMarkerColor(ROOT.kBlack)
    h_data.SetLineColor(ROOT.kBlack)
    h_data.SetLineWidth(2)

    # MC: filled histogram
    h_mc.SetFillColor(ROOT.kAzure - 9)
    h_mc.SetLineColor(ROOT.kAzure + 2)
    h_mc.SetLineWidth(2)
    h_mc.SetFillStyle(1001)


def make_ratio_plot(h_data, h_mc):
    """Create ratio plot (Data/MC)."""
    h_ratio = h_data.Clone(f"{h_data.GetName()}_ratio")
    h_ratio.Divide(h_mc)
    h_ratio.SetMarkerStyle(20)
    h_ratio.SetMarkerSize(0.8)
    h_ratio.SetMarkerColor(ROOT.kBlack)
    h_ratio.SetLineColor(ROOT.kBlack)
    h_ratio.GetYaxis().SetTitle("Data/MC")
    h_ratio.GetYaxis().SetTitleSize(0.12)
    h_ratio.GetYaxis().SetTitleOffset(0.5)
    h_ratio.GetYaxis().SetLabelSize(0.10)
    h_ratio.GetXaxis().SetTitleSize(0.12)
    h_ratio.GetXaxis().SetLabelSize(0.10)
    h_ratio.SetMinimum(0.5)
    h_ratio.SetMaximum(1.5)
    return h_ratio


def create_comparison_plot(var_name, var_title, nbins, xmin, xmax, logy=False):
    """
    Create a comparison plot between data and MC for a given variable.
    Returns the canvas.
    """
    print(f"Creating plot for {var_name}...")

    # Open files
    f_data = ROOT.TFile.Open(DATA_FILE)
    f_mc = ROOT.TFile.Open(MC_FILE)

    if not f_data or f_data.IsZombie():
        print(f"ERROR: Could not open data file {DATA_FILE}")
        return None
    if not f_mc or f_mc.IsZombie():
        print(f"ERROR: Could not open MC file {MC_FILE}")
        return None

    t_data = f_data.Get(TREE_NAME)
    t_mc = f_mc.Get(TREE_NAME)

    if not t_data:
        print(f"ERROR: Tree {TREE_NAME} not found in data file")
        return None
    if not t_mc:
        print(f"ERROR: Tree {TREE_NAME} not found in MC file")
        return None

    # Create histograms
    h_data = ROOT.TH1F(f"h_data_{var_name}", "", nbins, xmin, xmax)
    h_mc = ROOT.TH1F(f"h_mc_{var_name}", "", nbins, xmin, xmax)

    # Fill histograms
    t_data.Draw(f"{var_name}>>h_data_{var_name}", "", "goff")
    t_mc.Draw(f"{var_name}>>h_mc_{var_name}", "weight", "goff")

    # Normalize MC to data
    if h_data.Integral() > 0 and h_mc.Integral() > 0:
        h_mc.Scale(h_data.Integral() / h_mc.Integral())

    # Style histograms
    setup_histogram_style(h_data, h_mc)

    # Create canvas with two pads
    canvas = ROOT.TCanvas(f"c_{var_name}", "", 800, 800)

    # Upper pad for distributions
    pad1 = ROOT.TPad("pad1", "", 0, 0.3, 1, 1)
    pad1.SetBottomMargin(0.02)
    pad1.SetLeftMargin(0.12)
    pad1.SetRightMargin(0.05)
    if logy:
        pad1.SetLogy()
    pad1.Draw()
    pad1.cd()

    # Draw histograms
    h_mc.SetTitle("")
    h_mc.GetYaxis().SetTitle("Events")
    h_mc.GetYaxis().SetTitleSize(0.05)
    h_mc.GetYaxis().SetLabelSize(0.04)

    max_val = max(h_data.GetMaximum(), h_mc.GetMaximum())
    h_mc.SetMaximum(max_val * 1.3 if not logy else max_val * 10)

    h_mc.Draw("HIST")
    h_data.Draw("E1 SAME")

    # Legend
    legend = ROOT.TLegend(0.65, 0.75, 0.92, 0.92)
    legend.SetBorderSize(0)
    legend.SetFillStyle(0)
    legend.AddEntry(h_data, "Data", "lep")
    legend.AddEntry(h_mc, "MC (pileup protons)", "f")
    legend.Draw()

    # CMS label
    latex = ROOT.TLatex()
    latex.SetNDC()
    latex.SetTextSize(0.045)
    latex.DrawLatex(0.12, 0.92, "#bf{CMS} #it{Preliminary}")
    latex.SetTextSize(0.035)
    latex.DrawLatex(0.65, 0.92, "59.7 fb^{-1} (13 TeV)")

    # Lower pad for ratio
    canvas.cd()
    pad2 = ROOT.TPad("pad2", "", 0, 0.05, 1, 0.3)
    pad2.SetTopMargin(0.02)
    pad2.SetBottomMargin(0.35)
    pad2.SetLeftMargin(0.12)
    pad2.SetRightMargin(0.05)
    pad2.SetGridy()
    pad2.Draw()
    pad2.cd()

    # Create and draw ratio
    h_ratio = make_ratio_plot(h_data, h_mc)
    h_ratio.GetXaxis().SetTitle(var_title)
    h_ratio.Draw("E1")

    # Draw line at 1
    line = ROOT.TLine(xmin, 1, xmax, 1)
    line.SetLineColor(ROOT.kRed)
    line.SetLineStyle(2)
    line.Draw()

    canvas.cd()

    # Save
    output_name = f"{OUTPUT_DIR}/{var_name}_comparison.png"
    canvas.SaveAs(output_name)
    print(f"  Saved: {output_name}")

    # Clean up
    f_data.Close()
    f_mc.Close()

    return canvas


def create_proton_count_plot():
    """
    Create comparison plots for number of protons per event.
    """
    print("\nCreating proton count distributions...")

    # Open files
    f_data = ROOT.TFile.Open(DATA_FILE)
    f_mc = ROOT.TFile.Open(MC_FILE)

    if not f_data or f_data.IsZombie():
        print(f"ERROR: Could not open data file {DATA_FILE}")
        return None
    if not f_mc or f_mc.IsZombie():
        print(f"ERROR: Could not open MC file {MC_FILE}")
        return None

    t_data = f_data.Get(TREE_NAME)
    t_mc = f_mc.Get(TREE_NAME)

    # Create histograms
    h_data_multi = ROOT.TH1F("h_data_nproton_multi", "", 10, 0, 10)
    h_mc_multi = ROOT.TH1F("h_mc_nproton_multi", "", 10, 0, 10)

    h_data_single = ROOT.TH1F("h_data_nproton_single", "", 10, 0, 10)
    h_mc_single = ROOT.TH1F("h_mc_nproton_single", "", 10, 0, 10)

    # Fill histograms
    t_data.Draw("nproton_multi>>h_data_nproton_multi", "", "goff")
    t_mc.Draw("nproton_multi>>h_mc_nproton_multi", "weight", "goff")

    t_data.Draw("nproton_single>>h_data_nproton_single", "", "goff")
    t_mc.Draw("nproton_single>>h_mc_nproton_single", "weight", "goff")

    # Normalize MC to data
    if h_data_multi.Integral() > 0 and h_mc_multi.Integral() > 0:
        h_mc_multi.Scale(h_data_multi.Integral() / h_mc_multi.Integral())
    if h_data_single.Integral() > 0 and h_mc_single.Integral() > 0:
        h_mc_single.Scale(h_data_single.Integral() / h_mc_single.Integral())

    # --- Multi-RP plot ---
    setup_histogram_style(h_data_multi, h_mc_multi)

    canvas_multi = ROOT.TCanvas("c_nproton_multi", "", 800, 800)

    pad1 = ROOT.TPad("pad1", "", 0, 0.3, 1, 1)
    pad1.SetBottomMargin(0.02)
    pad1.SetLeftMargin(0.12)
    pad1.SetRightMargin(0.05)
    pad1.Draw()
    pad1.cd()

    h_mc_multi.SetTitle("")
    h_mc_multi.GetYaxis().SetTitle("Events")
    h_mc_multi.GetYaxis().SetTitleSize(0.05)
    h_mc_multi.SetMaximum(max(h_data_multi.GetMaximum(), h_mc_multi.GetMaximum()) * 1.3)

    h_mc_multi.Draw("HIST")
    h_data_multi.Draw("E1 SAME")

    legend = ROOT.TLegend(0.60, 0.70, 0.92, 0.92)
    legend.SetBorderSize(0)
    legend.SetFillStyle(0)
    legend.AddEntry(h_data_multi, "Data", "lep")
    legend.AddEntry(h_mc_multi, "MC (pileup protons)", "f")
    legend.Draw()

    latex = ROOT.TLatex()
    latex.SetNDC()
    latex.SetTextSize(0.045)
    latex.DrawLatex(0.12, 0.92, "#bf{CMS} #it{Preliminary}")
    latex.SetTextSize(0.035)
    latex.DrawLatex(0.60, 0.92, "59.7 fb^{-1} (13 TeV)")

    canvas_multi.cd()
    pad2 = ROOT.TPad("pad2", "", 0, 0.05, 1, 0.3)
    pad2.SetTopMargin(0.02)
    pad2.SetBottomMargin(0.35)
    pad2.SetLeftMargin(0.12)
    pad2.SetRightMargin(0.05)
    pad2.SetGridy()
    pad2.Draw()
    pad2.cd()

    h_ratio_multi = make_ratio_plot(h_data_multi, h_mc_multi)
    h_ratio_multi.GetXaxis().SetTitle("Number of multi-RP protons")
    h_ratio_multi.Draw("E1")

    line = ROOT.TLine(0, 1, 10, 1)
    line.SetLineColor(ROOT.kRed)
    line.SetLineStyle(2)
    line.Draw()

    output_name = f"{OUTPUT_DIR}/nproton_multiRP_comparison.png"
    canvas_multi.SaveAs(output_name)
    print(f"  Saved: {output_name}")

    # --- Single-RP plot ---
    setup_histogram_style(h_data_single, h_mc_single)

    canvas_single = ROOT.TCanvas("c_nproton_single", "", 800, 800)

    pad1 = ROOT.TPad("pad1", "", 0, 0.3, 1, 1)
    pad1.SetBottomMargin(0.02)
    pad1.SetLeftMargin(0.12)
    pad1.SetRightMargin(0.05)
    pad1.Draw()
    pad1.cd()

    h_mc_single.SetTitle("")
    h_mc_single.GetYaxis().SetTitle("Events")
    h_mc_single.GetYaxis().SetTitleSize(0.05)
    h_mc_single.SetMaximum(max(h_data_single.GetMaximum(), h_mc_single.GetMaximum()) * 1.3)

    h_mc_single.Draw("HIST")
    h_data_single.Draw("E1 SAME")

    legend = ROOT.TLegend(0.60, 0.70, 0.92, 0.92)
    legend.SetBorderSize(0)
    legend.SetFillStyle(0)
    legend.AddEntry(h_data_single, "Data", "lep")
    legend.AddEntry(h_mc_single, "MC (pileup protons)", "f")
    legend.Draw()

    latex = ROOT.TLatex()
    latex.SetNDC()
    latex.SetTextSize(0.045)
    latex.DrawLatex(0.12, 0.92, "#bf{CMS} #it{Preliminary}")
    latex.SetTextSize(0.035)
    latex.DrawLatex(0.60, 0.92, "59.7 fb^{-1} (13 TeV)")

    canvas_single.cd()
    pad2 = ROOT.TPad("pad2", "", 0, 0.05, 1, 0.3)
    pad2.SetTopMargin(0.02)
    pad2.SetBottomMargin(0.35)
    pad2.SetLeftMargin(0.12)
    pad2.SetRightMargin(0.05)
    pad2.SetGridy()
    pad2.Draw()
    pad2.cd()

    h_ratio_single = make_ratio_plot(h_data_single, h_mc_single)
    h_ratio_single.GetXaxis().SetTitle("Number of single-RP protons")
    h_ratio_single.Draw("E1")

    line = ROOT.TLine(0, 1, 10, 1)
    line.SetLineColor(ROOT.kRed)
    line.SetLineStyle(2)
    line.Draw()

    output_name = f"{OUTPUT_DIR}/nproton_singleRP_comparison.png"
    canvas_single.SaveAs(output_name)
    print(f"  Saved: {output_name}")

    # Clean up
    f_data.Close()
    f_mc.Close()


# ============================= Main Plotting =============================

def main():
    print("="*70)
    print("Creating Data vs MC comparison plots")
    print("="*70)

    # Define variables to plot (variable_name, title, nbins, xmin, xmax, logy)
    variables = [
        # Muon/Tau kinematics
        ("muon_pt", "Muon p_{T} [GeV]", 50, 0, 200, False),
        ("tau_pt", "Tau p_{T} [GeV]", 50, 0, 300, False),
        ("muon_eta", "Muon #eta", 50, -3, 3, False),
        ("tau_eta", "Tau #eta", 50, -3, 3, False),
        ("muon_phi", "Muon #phi", 50, -3.2, 3.2, False),
        ("tau_phi", "Tau #phi", 50, -3.2, 3.2, False),

        # System kinematics
        ("sist_mass", "M_{#mu#tau} [GeV]", 50, 0, 500, False),
        ("sist_pt", "p_{T}^{#mu#tau} [GeV]", 50, 0, 300, False),
        ("acop", "Acoplanarity", 50, 0, 1, False),

        # MET
        ("met_pt", "E_{T}^{miss} [GeV]", 50, 0, 200, False),

        # Jets
        ("jet_pt", "Leading jet p_{T} [GeV]", 50, 0, 200, True),
        ("n_b_jet", "Number of b-jets", 5, 0, 5, False),
    ]

    print("\n1. Creating physics variable comparison plots (no proton requirements)...")
    print("-" * 70)

    canvases = []
    for var_name, var_title, nbins, xmin, xmax, logy in variables:
        c = create_comparison_plot(var_name, var_title, nbins, xmin, xmax, logy)
        if c:
            canvases.append(c)

    print("\n2. Creating proton count distribution plots...")
    print("-" * 70)
    create_proton_count_plot()

    print("\n" + "="*70)
    print(f"✅ All plots saved to: {OUTPUT_DIR}/")
    print("="*70)


if __name__ == "__main__":
    main()
