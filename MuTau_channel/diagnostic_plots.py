#!/usr/bin/env python3
"""
Diagnostic plots to investigate Data/QCD event count issue
Compares distributions of key variables across samples
"""

import ROOT
import math

# Enable batch mode (no GUI windows)
ROOT.gROOT.SetBatch(True)
ROOT.gStyle.SetOptStat(0)

# Input files - using fase1 outputs (before merge) to get all branches
data_file = "/eos/user/m/mblancco/samples_2018_mutau/fase1_data/Data_2018_UL_skimmed_MuTau_cuts_0.root"  # Single file for testing
qcd_file = "/eos/user/m/mblancco/samples_2018_mutau/fase1_qcd/QCD_2018_UL_skimmed_MuTau_cuts_0.root"
dy_file = "/eos/user/m/mblancco/samples_2018_mutau/DY_2018_UL_MuTau_nano_merged_pileup_protons.root"
ttjets_file = "/eos/user/m/mblancco/samples_2018_mutau/ttjets_2018_UL_MuTau_nano_merged_pileup_protons.root"

# NOTE: Using single fase1 files for Data/QCD instead of merged files
# because merged files don't have proton branches saved

# Output directory
output_dir = "diagnostic_plots"
import os
os.makedirs(output_dir, exist_ok=True)

print("=" * 80)
print("DIAGNOSTIC PLOTS FOR DATA/QCD INVESTIGATION")
print("=" * 80)

# Open files and get trees
print("\nOpening files...")
f_data = ROOT.TFile.Open(data_file)
f_qcd = ROOT.TFile.Open(qcd_file)
f_dy = ROOT.TFile.Open(dy_file)
f_ttjets = ROOT.TFile.Open(ttjets_file)

t_data = f_data.Get("tree")
t_qcd = f_qcd.Get("tree")
t_dy = f_dy.Get("tree")
t_ttjets = f_ttjets.Get("tree")

# Get event counts
n_data = t_data.GetEntries()
n_qcd = t_qcd.GetEntries()
n_dy = t_dy.GetEntries()
n_ttjets = t_ttjets.GetEntries()

print(f"\nEvent counts:")
print(f"  Data:   {n_data:,}")
print(f"  QCD:    {n_qcd:,}")
print(f"  DY:     {n_dy:,}")
print(f"  ttjets: {n_ttjets:,}")

# Weights for normalization (adjust these based on your analysis)
# For now, just use 1.0 to see raw counts
w_data = 1.0
w_qcd = 1.0
w_dy = 1.81 * 0.13  # From your plot_m.py
w_ttjets = 0.15 * 0.13  # From your plot_m.py

print(f"\nWeights:")
print(f"  Data:   {w_data}")
print(f"  QCD:    {w_qcd}")
print(f"  DY:     {w_dy}")
print(f"  ttjets: {w_ttjets}")

# Variables to plot (common to all samples)
variables = [
    # (name, title, nbins, xmin, xmax, xlabel)
    ("mu_eta", "Muon #eta", 50, -2.5, 2.5, "Muon #eta"),
    ("tau_eta", "Tau #eta", 50, -2.5, 2.5, "Tau #eta"),
    ("mu_pt", "Muon p_{T}", 50, 0, 200, "Muon p_{T} [GeV]"),
    ("tau_pt", "Tau p_{T}", 50, 0, 300, "Tau p_{T} [GeV]"),
    ("mu_phi", "Muon #phi", 50, -3.14, 3.14, "Muon #phi"),
    ("tau_phi", "Tau #phi", 50, -3.14, 3.14, "Tau #phi"),
    ("tau_id1", "Tau VSjet ID", 256, 0, 256, "Tau VSjet ID"),
    ("tau_id2", "Tau VSe ID", 256, 0, 256, "Tau VSe ID"),
    ("tau_id3", "Tau VSmu ID", 16, 0, 16, "Tau VSmu ID"),
    ("mu_id", "Muon ID", 10, 0, 10, "Muon MVA ID"),
    ("sist_mass", "Invariant mass", 50, 0, 500, "M(#mu#tau) [GeV]"),
    ("sist_pt", "System p_{T}", 50, 0, 300, "p_{T}(#mu#tau) [GeV]"),
    ("acop", "Acoplanarity", 50, 0, 1, "Acoplanarity"),
    ("met_pt", "Missing E_{T}", 50, 0, 200, "E_{T}^{miss} [GeV]"),
]

# Proton variables (only exist in Data/QCD, not DY/ttjets)
proton_variables = [
    ("xi_arm1_1", "Proton #xi Arm 1 (1st)", 50, 0, 0.2, "Proton #xi (Arm 1, 1st)"),
    ("xi_arm2_1", "Proton #xi Arm 2 (1st)", 50, 0, 0.2, "Proton #xi (Arm 2, 1st)"),
]

print(f"\nGenerating {len(variables)} diagnostic plots...")

for i, (var, title, nbins, xmin, xmax, xlabel) in enumerate(variables):
    print(f"\n[{i+1}/{len(variables)}] Creating plot for {var}...")

    # Create canvas
    c = ROOT.TCanvas(f"c_{var}", f"c_{var}", 800, 600)
    c.SetLeftMargin(0.12)
    c.SetRightMargin(0.05)

    # Create histograms
    h_data = ROOT.TH1F(f"h_data_{var}", "", nbins, xmin, xmax)
    h_qcd = ROOT.TH1F(f"h_qcd_{var}", "", nbins, xmin, xmax)
    h_dy = ROOT.TH1F(f"h_dy_{var}", "", nbins, xmin, xmax)
    h_ttjets = ROOT.TH1F(f"h_ttjets_{var}", "", nbins, xmin, xmax)

    # Fill histograms
    t_data.Draw(f"{var} >> h_data_{var}", f"{w_data}", "goff")
    t_qcd.Draw(f"{var} >> h_qcd_{var}", f"{w_qcd}", "goff")
    t_dy.Draw(f"{var} >> h_dy_{var}", f"{w_dy}", "goff")
    t_ttjets.Draw(f"{var} >> h_ttjets_{var}", f"{w_ttjets}", "goff")

    # Style for Data (black points with error bars)
    h_data.SetMarkerStyle(20)
    h_data.SetMarkerSize(0.8)
    h_data.SetMarkerColor(ROOT.kBlack)
    h_data.SetLineColor(ROOT.kBlack)
    h_data.SetLineWidth(2)

    # Style for backgrounds (filled histograms)
    h_qcd.SetFillColor(ROOT.kYellow)
    h_qcd.SetLineColor(ROOT.kBlack)
    h_qcd.SetLineWidth(1)

    h_dy.SetFillColor(ROOT.kAzure + 1)
    h_dy.SetLineColor(ROOT.kBlack)
    h_dy.SetLineWidth(1)

    h_ttjets.SetFillColor(ROOT.kRed - 7)
    h_ttjets.SetLineColor(ROOT.kBlack)
    h_ttjets.SetLineWidth(1)

    # Create stacked histogram for backgrounds
    hs = ROOT.THStack(f"hs_{var}", f"{title}")
    hs.Add(h_ttjets)
    hs.Add(h_dy)
    hs.Add(h_qcd)

    # Create sum for error band
    h_sum = h_qcd.Clone(f"h_sum_{var}")
    h_sum.Add(h_dy)
    h_sum.Add(h_ttjets)
    h_sum.SetFillColor(ROOT.kGray + 1)
    h_sum.SetFillStyle(3354)
    h_sum.SetMarkerSize(0)

    # Set errors on sum
    for j in range(1, h_sum.GetNbinsX() + 1):
        content = h_sum.GetBinContent(j)
        error = math.sqrt(content) if content > 0 else 0
        h_sum.SetBinError(j, error)

    # Draw
    hs.Draw("HIST")
    h_sum.Draw("E2 SAME")
    hs.Draw("HIST SAME")  # Redraw to be on top
    h_data.Draw("E1 SAME")

    # Set axis labels and titles
    hs.GetXaxis().SetTitle(xlabel)
    hs.GetYaxis().SetTitle("Events")
    hs.GetYaxis().SetTitleOffset(1.4)

    # Get maximum for y-axis
    max_data = h_data.GetMaximum()
    max_mc = hs.GetMaximum()
    y_max = max(max_data, max_mc) * 1.3
    hs.SetMaximum(y_max)

    # Legend
    leg = ROOT.TLegend(0.65, 0.65, 0.93, 0.92)
    leg.SetBorderSize(0)
    leg.SetFillStyle(0)
    leg.SetTextSize(0.035)
    leg.AddEntry(h_data, f"Data ({n_data})", "lep")
    leg.AddEntry(h_qcd, f"QCD ({n_qcd})", "f")
    leg.AddEntry(h_dy, f"DY ({n_dy})", "f")
    leg.AddEntry(h_ttjets, f"t#bar{{t}} ({n_ttjets})", "f")
    leg.AddEntry(h_sum, "Stat. unc.", "f")
    leg.Draw()

    # CMS label
    latex = ROOT.TLatex()
    latex.SetNDC()
    latex.SetTextSize(0.04)
    latex.DrawLatex(0.12, 0.92, "#bf{CMS} #it{Preliminary}")
    latex.DrawLatex(0.65, 0.92, "59.7 fb^{-1} (13 TeV)")

    # Save
    output_path = f"{output_dir}/{var}.png"
    c.SaveAs(output_path)
    print(f"  Saved: {output_path}")

    # Also save with log scale for some variables
    if var in ["n_pu", "tau_id1", "tau_id2", "tau_id3", "n_protons_arm0", "n_protons_arm1"]:
        c.SetLogy(1)
        output_path_log = f"{output_dir}/{var}_log.png"
        c.SaveAs(output_path_log)
        print(f"  Saved: {output_path_log}")
        c.SetLogy(0)

    # Clean up
    c.Close()

print("\n" + "=" * 80)
print("CREATING PROTON VARIABLE PLOTS (Data and QCD only)")
print("=" * 80)

for i, (var, title, nbins, xmin, xmax, xlabel) in enumerate(proton_variables):
    print(f"\n[{i+1}/{len(proton_variables)}] Creating proton plot for {var}...")

    c = ROOT.TCanvas(f"c_{var}", f"c_{var}", 800, 600)
    c.SetLeftMargin(0.12)
    c.SetRightMargin(0.05)

    # Only Data and QCD have these variables
    h_data_p = ROOT.TH1F(f"h_data_p_{var}", "", nbins, xmin, xmax)
    h_qcd_p = ROOT.TH1F(f"h_qcd_p_{var}", "", nbins, xmin, xmax)

    t_data.Draw(f"{var} >> h_data_p_{var}", f"{w_data}", "goff")
    t_qcd.Draw(f"{var} >> h_qcd_p_{var}", f"{w_qcd}", "goff")

    # Style
    h_data_p.SetMarkerStyle(20)
    h_data_p.SetMarkerColor(ROOT.kBlack)
    h_data_p.SetLineColor(ROOT.kBlack)
    h_data_p.SetLineWidth(2)

    h_qcd_p.SetFillColor(ROOT.kYellow)
    h_qcd_p.SetLineColor(ROOT.kBlack)
    h_qcd_p.SetLineWidth(1)

    # Stack
    hs_p = ROOT.THStack(f"hs_p_{var}", f"{title}")
    hs_p.Add(h_qcd_p)

    hs_p.Draw("HIST")
    h_data_p.Draw("E1 SAME")

    hs_p.GetXaxis().SetTitle(xlabel)
    hs_p.GetYaxis().SetTitle("Events")
    hs_p.GetYaxis().SetTitleOffset(1.4)

    max_data_p = h_data_p.GetMaximum()
    max_qcd_p = h_qcd_p.GetMaximum()
    y_max_p = max(max_data_p, max_qcd_p) * 1.3
    hs_p.SetMaximum(y_max_p)

    # Legend
    leg_p = ROOT.TLegend(0.65, 0.75, 0.93, 0.92)
    leg_p.SetBorderSize(0)
    leg_p.SetFillStyle(0)
    leg_p.SetTextSize(0.035)
    leg_p.AddEntry(h_data_p, f"Data ({n_data})", "lep")
    leg_p.AddEntry(h_qcd_p, f"QCD ({n_qcd})", "f")
    leg_p.Draw()

    # CMS label
    latex_p = ROOT.TLatex()
    latex_p.SetNDC()
    latex_p.SetTextSize(0.04)
    latex_p.DrawLatex(0.12, 0.92, "#bf{CMS} #it{Preliminary}")
    latex_p.DrawLatex(0.55, 0.92, "Data/QCD only")

    output_path_p = f"{output_dir}/{var}.png"
    c.SaveAs(output_path_p)
    print(f"  Saved: {output_path_p}")

    c.SetLogy(1)
    output_path_p_log = f"{output_dir}/{var}_log.png"
    c.SaveAs(output_path_p_log)
    print(f"  Saved: {output_path_p_log}")
    c.SetLogy(0)

    c.Close()

print("\n" + "=" * 80)
print("CREATING SUMMARY PLOT: Event counts comparison")
print("=" * 80)

# Create a summary histogram showing event counts
c_summary = ROOT.TCanvas("c_summary", "c_summary", 800, 600)
c_summary.SetLeftMargin(0.15)
c_summary.SetBottomMargin(0.15)

h_counts = ROOT.TH1F("h_counts", "Event Counts;Sample;Number of Events", 4, 0, 4)
h_counts.GetXaxis().SetBinLabel(1, "Data")
h_counts.GetXaxis().SetBinLabel(2, "QCD")
h_counts.GetXaxis().SetBinLabel(3, "DY")
h_counts.GetXaxis().SetBinLabel(4, "t#bar{t}")

h_counts.SetBinContent(1, n_data)
h_counts.SetBinContent(2, n_qcd)
h_counts.SetBinContent(3, n_dy)
h_counts.SetBinContent(4, n_ttjets)

h_counts.SetFillColor(ROOT.kAzure + 1)
h_counts.SetLineColor(ROOT.kBlack)
h_counts.SetLineWidth(2)
h_counts.GetYaxis().SetTitleOffset(1.6)
h_counts.GetXaxis().SetLabelSize(0.05)
h_counts.SetMinimum(0)

h_counts.Draw("HIST")

# Add text with counts
latex = ROOT.TLatex()
latex.SetTextAlign(22)
latex.SetTextSize(0.04)
for i in range(1, 5):
    count = h_counts.GetBinContent(i)
    x = h_counts.GetBinCenter(i)
    y = count * 1.05
    latex.DrawLatex(x, y, f"{int(count):,}")

# CMS label
latex2 = ROOT.TLatex()
latex2.SetNDC()
latex2.SetTextSize(0.04)
latex2.DrawLatex(0.15, 0.92, "#bf{CMS} #it{Preliminary}")
latex2.DrawLatex(0.55, 0.92, "MuTau Channel, 2018")

c_summary.SaveAs(f"{output_dir}/event_counts.png")
print(f"Saved: {output_dir}/event_counts.png")

print("\n" + "=" * 80)
print("CREATING RATIO PLOTS")
print("=" * 80)

# Create ratio plots (Data/MC) for key variables
ratio_vars = [
    ("mu_eta", "Muon #eta", 50, -2.5, 2.5),
    ("tau_eta", "Tau #eta", 50, -2.5, 2.5),
    ("tau_id3", "Tau VSmu ID", 16, 0, 16),
]

for var, title, nbins, xmin, xmax in ratio_vars:
    print(f"\nCreating ratio plot for {var}...")

    # Create canvas with two pads
    c_ratio = ROOT.TCanvas(f"c_ratio_{var}", f"c_ratio_{var}", 800, 800)

    # Upper pad
    pad1 = ROOT.TPad("pad1", "pad1", 0, 0.3, 1, 1.0)
    pad1.SetBottomMargin(0.02)
    pad1.Draw()
    pad1.cd()

    # Get histograms (recreate them)
    h_data_r = ROOT.TH1F(f"h_data_r_{var}", "", nbins, xmin, xmax)
    h_qcd_r = ROOT.TH1F(f"h_qcd_r_{var}", "", nbins, xmin, xmax)
    h_dy_r = ROOT.TH1F(f"h_dy_r_{var}", "", nbins, xmin, xmax)
    h_ttjets_r = ROOT.TH1F(f"h_ttjets_r_{var}", "", nbins, xmin, xmax)

    t_data.Draw(f"{var} >> h_data_r_{var}", f"{w_data}", "goff")
    t_qcd.Draw(f"{var} >> h_qcd_r_{var}", f"{w_qcd}", "goff")
    t_dy.Draw(f"{var} >> h_dy_r_{var}", f"{w_dy}", "goff")
    t_ttjets.Draw(f"{var} >> h_ttjets_r_{var}", f"{w_ttjets}", "goff")

    # Style
    h_data_r.SetMarkerStyle(20)
    h_data_r.SetMarkerColor(ROOT.kBlack)
    h_data_r.SetLineColor(ROOT.kBlack)

    h_qcd_r.SetFillColor(ROOT.kYellow)
    h_dy_r.SetFillColor(ROOT.kAzure + 1)
    h_ttjets_r.SetFillColor(ROOT.kRed - 7)

    # Stack
    hs_r = ROOT.THStack(f"hs_r_{var}", "")
    hs_r.Add(h_ttjets_r)
    hs_r.Add(h_dy_r)
    hs_r.Add(h_qcd_r)

    hs_r.Draw("HIST")
    h_data_r.Draw("E1 SAME")

    hs_r.GetYaxis().SetTitle("Events")
    hs_r.GetYaxis().SetTitleSize(0.05)
    hs_r.GetYaxis().SetLabelSize(0.045)

    # Legend
    leg_r = ROOT.TLegend(0.65, 0.65, 0.90, 0.90)
    leg_r.SetBorderSize(0)
    leg_r.SetFillStyle(0)
    leg_r.AddEntry(h_data_r, "Data", "lep")
    leg_r.AddEntry(h_qcd_r, "QCD", "f")
    leg_r.AddEntry(h_dy_r, "DY", "f")
    leg_r.AddEntry(h_ttjets_r, "t#bar{t}", "f")
    leg_r.Draw()

    c_ratio.cd()

    # Lower pad (ratio)
    pad2 = ROOT.TPad("pad2", "pad2", 0, 0.05, 1, 0.3)
    pad2.SetTopMargin(0.02)
    pad2.SetBottomMargin(0.35)
    pad2.Draw()
    pad2.cd()

    # Calculate ratio
    h_sum_r = h_qcd_r.Clone(f"h_sum_r_{var}")
    h_sum_r.Add(h_dy_r)
    h_sum_r.Add(h_ttjets_r)

    h_ratio = h_data_r.Clone(f"h_ratio_{var}")
    h_ratio.Divide(h_sum_r)

    # Style ratio
    h_ratio.SetMarkerStyle(20)
    h_ratio.SetMarkerColor(ROOT.kBlack)
    h_ratio.SetLineColor(ROOT.kBlack)
    h_ratio.GetYaxis().SetTitle("Data / MC")
    h_ratio.GetYaxis().SetTitleSize(0.12)
    h_ratio.GetYaxis().SetTitleOffset(0.5)
    h_ratio.GetYaxis().SetLabelSize(0.10)
    h_ratio.GetXaxis().SetTitle(title)
    h_ratio.GetXaxis().SetTitleSize(0.14)
    h_ratio.GetXaxis().SetLabelSize(0.12)
    h_ratio.SetMinimum(0.0)
    h_ratio.SetMaximum(2.0)

    h_ratio.Draw("E1")

    # Draw line at 1
    line = ROOT.TLine(xmin, 1.0, xmax, 1.0)
    line.SetLineColor(ROOT.kRed)
    line.SetLineStyle(2)
    line.Draw()

    c_ratio.SaveAs(f"{output_dir}/{var}_ratio.png")
    print(f"  Saved: {output_dir}/{var}_ratio.png")

print("\n" + "=" * 80)
print("DIAGNOSTIC PLOTS COMPLETE!")
print("=" * 80)
print(f"\nAll plots saved to: {output_dir}/")
print("\nKey plots to check:")
print(f"  1. {output_dir}/tau_id3.png - Shows VSmu distribution")
print(f"  2. {output_dir}/n_pu.png - Shows number of protons")
print(f"  3. {output_dir}/event_counts.png - Overall event count comparison")
print(f"  4. {output_dir}/tau_eta_ratio.png - Data/MC ratio for tau eta")
print(f"  5. {output_dir}/tau_id3_ratio.png - Data/MC ratio for VSmu")
print("\nLook for:")
print("  - Is Data significantly lower than MC everywhere?")
print("  - Does VSmu distribution look different for Data vs MC?")
print("  - Are proton distributions similar across samples?")
print("=" * 80)
