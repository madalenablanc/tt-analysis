#!/usr/bin/env python3
"""
Streamlined MuTau channel plotting script.
Uses fixed weights (like TauTau channel) for MC normalization.
Includes signal overlay.
"""

import ROOT
import math

# Configuration
TREE_NAME = "tree"
PROTON_ACCEPTANCE = 0.245  # Measured from 2018 MuTau data
SIGNAL_SCALE = 500  # Scale factor for signal visibility in plots

FILES = {
    "data":   "/eos/home-m/mblancco/samples_2018_mutau/Data_2018_UL_MuTau_nano_merged_pileup_protons.root",
    "dy":     "/eos/home-m/mblancco/samples_2018_mutau/DY_2018_UL_MuTau_nano_merged_pileup_protons.root",
    "ttbar":  "/eos/home-m/mblancco/samples_2018_mutau/ttjets_2018_UL_MuTau_nano_merged_pileup_protons.root",
    "qcd":    "/eos/home-m/mblancco/samples_2018_mutau/QCD_2018_UL_MuTau_nano_merged_pileup_protons.root",
    "signal": "MuTau_sinal_SM_2018_july.root",
}

# Fixed weights: cross-section factor × proton acceptance
WEIGHTS = {
    "data":   1.0,
    "dy":     1.81 * PROTON_ACCEPTANCE,
    "ttbar":  1.0 * PROTON_ACCEPTANCE,
    "qcd":    1.0,
    "signal": 1.0,  # Signal weight = 1 (like TauTau channel)
}

COLORS = {
    "data":   ROOT.kBlack,
    "dy":     ROOT.kOrange - 3,
    "ttbar":  ROOT.kAzure + 7,
    "qcd":    ROOT.kGray,
    "signal": ROOT.kRed + 1,
}

# Histogram definitions: (name, title, nbins, xmin, xmax, branch)
HISTOGRAMS = [
    ("mass",     "Invariant Mass;m_{#mu#tau} [GeV];Events", 10, 0, 1200, "sist_mass"),
    ("acop",     "Acoplanarity;1 - |#Delta#phi|/#pi;Events", 10, 0, 1, "acop"),
    ("pt",       "System p_{T};p_{T} [GeV];Events", 10, 0, 600, "sist_pt"),
    ("rap",      "Rapidity;y;Events", 10, -2.2, 2.2, "sist_rap"),
    ("rap_match", "Rapidity Matching;y_{#mu#tau} - y_{pp};Events", 10, -2.2, 2.2, "rap_match"),
    ("mass_diff", "Mass Difference;m_{#mu#tau} - m_{pp} [GeV];Events", 10, -1500, 200, "mass_diff"),
    ("tau_pt",   "Tau p_{T};p_{T}^{#tau} [GeV];Events", 20, 100, 500, "tau_pt"),
    ("met",      "Missing E_{T};MET [GeV];Events", 20, 0, 300, "met_pt"),
]


def get_branch(event, *names):
    """Get branch value, trying multiple names."""
    for name in names:
        if hasattr(event, name):
            return getattr(event, name)
    return -999.0


def load_trees():
    """Load ROOT files and trees."""
    trees = {}
    files = {}
    for sample, path in FILES.items():
        f = ROOT.TFile.Open(path)
        if not f or f.IsZombie():
            print(f"Warning: Cannot open {path}")
            continue
        files[sample] = f
        trees[sample] = f.Get(TREE_NAME)
        if not trees[sample]:
            print(f"Warning: No tree '{TREE_NAME}' in {path}")
    return trees, files


def create_histograms():
    """Create histogram dictionaries for each sample."""
    hists = {sample: {} for sample in FILES}
    for sample in FILES:
        for name, title, nbins, xmin, xmax, _ in HISTOGRAMS:
            h = ROOT.TH1F(f"h_{name}_{sample}", title, nbins, xmin, xmax)
            h.SetLineColor(COLORS[sample])
            if sample == "data":
                h.SetMarkerStyle(20)
                h.SetMarkerSize(0.8)
            elif sample == "signal":
                # Signal: line only, no fill
                h.SetLineWidth(3)
                h.SetFillStyle(0)
            else:
                h.SetFillColor(COLORS[sample])
                h.SetFillStyle(1001)
                h.SetLineWidth(0)
            hists[sample][name] = h
    return hists


def fill_histograms(trees, hists):
    """Fill histograms from trees."""
    counts = {}
    for sample, tree in trees.items():
        if not tree:
            continue
        w = WEIGHTS[sample]
        n = 0.0
        for event in tree:
            mass = get_branch(event, "sist_mass")
            if mass < 0:
                continue
            n += w

            # Get xi values for derived quantities
            xi1 = get_branch(event, "xi_arm1_1")
            xi2 = get_branch(event, "xi_arm2_1")
            rap = get_branch(event, "sist_rap")

            # Rapidity matching: y_central - 0.5*log(xi1/xi2)
            rap_match = -999.0
            if xi1 > 0 and xi2 > 0:
                rap_match = rap - 0.5 * math.log(xi1 / xi2)

            # Mass difference: m_central - 13000*sqrt(xi1*xi2)
            mass_diff = -999.0
            if xi1 >= 0 and xi2 >= 0:
                mass_diff = mass - 13000.0 * math.sqrt(xi1 * xi2)

            hists[sample]["mass"].Fill(mass, w)
            hists[sample]["acop"].Fill(get_branch(event, "acop", "sist_acop"), w)
            hists[sample]["pt"].Fill(get_branch(event, "sist_pt"), w)
            hists[sample]["rap"].Fill(rap, w)
            if rap_match > -900:
                hists[sample]["rap_match"].Fill(rap_match, w)
            if mass_diff > -900:
                hists[sample]["mass_diff"].Fill(mass_diff, w)
            hists[sample]["tau_pt"].Fill(get_branch(event, "tau_pt"), w)
            hists[sample]["met"].Fill(get_branch(event, "met_pt"), w)

        counts[sample] = n
        print(f"{sample:8s}: {int(tree.GetEntries()):6d} events, weighted sum = {n:.1f}")
    return counts


def make_stack(hists, name):
    """Create THStack for a histogram."""
    stack = ROOT.THStack(f"stack_{name}", "")
    for sample in ["qcd", "ttbar", "dy"]:  # Order: bottom to top
        if sample in hists and name in hists[sample]:
            stack.Add(hists[sample][name])
    return stack


def make_mc_sum(hists, name):
    """Create sum of MC histograms with uncertainty band."""
    h_sum = None
    for sample in ["qcd", "ttbar", "dy"]:
        if sample in hists and name in hists[sample]:
            if h_sum is None:
                h_sum = hists[sample][name].Clone(f"h_{name}_mcsum")
            else:
                h_sum.Add(hists[sample][name])
    if h_sum:
        # Set sqrt(N) errors for uncertainty band
        for i in range(1, h_sum.GetNbinsX() + 1):
            content = h_sum.GetBinContent(i)
            h_sum.SetBinError(i, math.sqrt(content) if content > 0 else 0)
        h_sum.SetFillColor(ROOT.kGray + 1)
        h_sum.SetFillStyle(3354)
        h_sum.SetMarkerSize(0)
    return h_sum


def draw_plot(hists, name, title, output_name):
    """Draw a single plot with data, MC stack, signal overlay, and uncertainty."""
    c = ROOT.TCanvas(f"c_{name}", "", 700, 800)
    c.SetLeftMargin(0.15)
    c.SetRightMargin(0.05)
    c.SetTopMargin(0.10)
    c.SetBottomMargin(0.13)

    stack = make_stack(hists, name)
    h_data = hists.get("data", {}).get(name)
    h_signal = hists.get("signal", {}).get(name)
    h_mcsum = make_mc_sum(hists, name)

    # Draw stack
    stack.Draw("HIST")
    stack.GetXaxis().SetTitle(title.split(";")[1] if ";" in title else "")
    stack.GetYaxis().SetTitle(title.split(";")[2] if title.count(";") > 1 else "Events")
    stack.GetXaxis().SetTitleSize(0.03)
    stack.GetYaxis().SetTitleSize(0.03)
    stack.GetXaxis().SetLabelSize(0.03)
    stack.GetYaxis().SetLabelSize(0.03)
    stack.GetXaxis().SetNdivisions(5)
    stack.GetYaxis().SetNdivisions(5)

    # Set Y range
    stack.GetYaxis().SetRangeUser(0.1, 200)

    # Draw signal (scaled for visibility)
    if h_signal:
        h_signal.Draw("HIST SAME")

    # Draw data
    if h_data:
        h_data.Draw("E SAME")

    # Draw MC uncertainty band
    if h_mcsum:
        h_mcsum.Draw("E2 SAME")

    # Legend
    leg = ROOT.TLegend(0.6, 0.65, 0.92, 0.88)
    leg.SetBorderSize(0)
    leg.SetFillStyle(0)
    if h_data:
        leg.AddEntry(h_data, "Data", "lep")
    if h_signal:
        leg.AddEntry(h_signal, f"Signal", "l")
    if "dy" in hists:
        leg.AddEntry(hists["dy"][name], "Drell-Yan", "f")
    if "ttbar" in hists:
        leg.AddEntry(hists["ttbar"][name], "t#bar{t}", "f")
    if "qcd" in hists:
        leg.AddEntry(hists["qcd"][name], "QCD", "f")
    leg.Draw()

    # CMS-TOTEM label
    latex = ROOT.TLatex()
    latex.SetNDC()
    latex.SetTextAlign(31)
    latex.SetTextSize(0.04)
    latex.SetTextFont(62)
    latex.DrawLatex(0.5, 0.92, "CMS-TOTEM Preliminary")
    latex.DrawLatex(0.90, 0.92, "54.9 fb^{-1} (13 TeV)")

    c.Update()
    c.SaveAs(output_name)
    return c


def main():
    ROOT.gROOT.SetBatch(True)
    ROOT.gStyle.SetOptStat(0)

    print("Loading trees...")
    trees, files = load_trees()

    print("\nCreating histograms...")
    hists = create_histograms()

    print("\nFilling histograms:")
    print("-" * 50)
    counts = fill_histograms(trees, hists)
    print("-" * 50)
    mc_total = sum(counts.get(s, 0) for s in ["dy", "ttbar", "qcd"])
    print(f"{'MC total':8s}: {mc_total:.1f}")
    if "signal" in counts:
        print(f"{'Signal':8s}: {counts['signal']:.1f}")

    print("\nDrawing plots...")
    for name, title, _, _, _, _ in HISTOGRAMS:
        draw_plot(hists, name, title, f"plot_{name}.pdf")
        print(f"  Saved plot_{name}.pdf")

    # Save histograms to file
    out = ROOT.TFile("histograms_mutau.root", "RECREATE")
    for sample in hists:
        for name in hists[sample]:
            hists[sample][name].Write()
    out.Close()
    print("\nHistograms saved to histograms_mutau.root")


if __name__ == "__main__":
    main()