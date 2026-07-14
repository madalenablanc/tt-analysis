#!/usr/bin/env python3
"""
Control region plots for the μτh channel.
Mirrors the structure of TauTau/plot_cr.cpp.

Three CRs:
  DY  CR : sist_mass ∈ [40,100] GeV, acop < 0.3, n_b_jet == 0, OS
  TT  CR : n_b_jet ≥ 1, acop > 0.3, sist_mass > 400 GeV, OS
  QCD CR : sist_mass > 100 GeV, n_b_jet == 0, acop < 0.8, SS (same-sign)

Four variables plotted per CR:
  acoplanarity, invariant mass, transverse momentum, rapidity

Run on lxplus:
  source /cvmfs/sft.cern.ch/lcg/views/LCG_106/x86_64-el9-gcc13-opt/setup.sh
  python3 plot_cr.py
"""

import os
import ROOT

ROOT.gROOT.SetBatch(True)
ROOT.gStyle.SetOptStat(0)
ROOT.gStyle.SetEndErrorSize(0)
TH1 = ROOT.TH1

# ── Input files ────────────────────────────────────────────────────────────────
DATA_FILE = "/eos/home-m/mblancco/samples_2018_mutau/Data_2018_UL_MuTau_nano_merged_proton_vars.root"
DY_FILE   = "/eos/home-m/mblancco/samples_2018_mutau/DY_2018_UL_MuTau_nano_merged_pileup_protons.root"
QCD_FILE  = "/eos/home-m/mblancco/samples_2018_mutau/QCD_2018_UL_MuTau_nano_merged_proton_vars_new.root"
TT_FILE   = "/eos/home-m/mblancco/samples_2018_mutau/ttjets_2018_UL_MuTau_nano_merged_pileup_protons.root"

OUT_DIR = "output_plots_cr"
os.makedirs(OUT_DIR, exist_ok=True)

# MC normalisation (from save_shapes.cpp)
DY_SCALE = 1.004e-4

# ── Histogram parameters: (nbins, xmin, xmax) per CR × variable ───────────────
CR_PARAMS = {
    "DY": {
        "aco":  (5,   0.05, 0.30),
        "mass": (8,   40,   100),
        "pt":   (5,   150,  400),
        "rap":  (5,  -2.0,  2.0),
    },
    "TT": {
        "aco":  (10,  0.3,  1.0),
        "mass": (8,   400,  800),
        "pt":   (5,   0,    400),
        "rap":  (5,  -2.0,  2.0),
    },
    "QCD": {
        "aco":  (8,   0.0,  0.8),
        "mass": (8,   100,  500),
        "pt":   (5,   150,  400),
        "rap":  (5,  -2.0,  2.0),
    },
}

VAR_TITLES = {
    "aco":  "Acoplanarity of the central system",
    "mass": "Invariant mass of the central system [GeV]",
    "pt":   "Transverse momentum of the central system [GeV]",
    "rap":  "Rapidity of the central system",
}

# Stack order: dominant process on top (last added = topmost)
STACK_ORDER = {
    "DY":  ["qcd", "ttjets", "dy"],
    "TT":  ["qcd", "dy", "ttjets"],
    "QCD": ["dy", "ttjets", "qcd"],
}

FILL_COLORS = {
    "dy":     ROOT.kYellow,
    "ttjets": ROOT.kGreen,
    "qcd":    ROOT.kRed,
}

LEGEND_LABELS = {
    "dy":     "Drell Yan",
    "ttjets": "t#bar{t}",
    "qcd":    "QCD (Data driven)",
}

CR_LABELS = {
    "DY":  "DY CR",
    "TT":  "t#bar{t} CR",
    "QCD": "QCD CR",
}

# ── Helpers ────────────────────────────────────────────────────────────────────

def get_leaf(tree, *names, default=0.0):
    """Return the value of the first existing leaf among the given names."""
    for name in names:
        lf = tree.GetLeaf(name)
        if lf:
            return lf.GetValue(0)
    return default


def add_standard_labels(pad):
    pad.cd()
    cms = ROOT.TLatex()
    cms.SetNDC(True)
    cms.SetTextFont(62)
    cms.SetTextSize(0.065)
    cms.DrawLatex(0.16, 0.93, "CMS-TOTEM")

    prelim = ROOT.TLatex()
    prelim.SetNDC(True)
    prelim.SetTextFont(52)
    prelim.SetTextSize(0.052)
    prelim.DrawLatex(0.16, 0.88, "Preliminary")

    lumi = ROOT.TLatex()
    lumi.SetNDC(True)
    lumi.SetTextFont(62)
    lumi.SetTextSize(0.052)
    lumi.SetTextAlign(31)
    lumi.DrawLatex(0.94, 0.93, "54.9 fb^{-1} (13 TeV)")


# ── Histogram filling ──────────────────────────────────────────────────────────

def fill_histograms():
    """
    Loop over the four input trees and fill CR histograms.

    The main data file contains OS events only (fase1_data.py applies OS cut).
    For the QCD CR (SS requirement), 'data' points are therefore taken from the
    QCD file (same-sign data-driven sample).
    """

    ROOT.TH1.AddDirectory(False)

    # Initialise histogram dict: hists[cr][var][sample]
    hists = {}
    for cr in ["DY", "TT", "QCD"]:
        hists[cr] = {}
        for var in ["aco", "mass", "pt", "rap"]:
            nb, xmin, xmax = CR_PARAMS[cr][var]
            hists[cr][var] = {}
            for sample in ["data", "dy", "qcd", "ttjets"]:
                h = ROOT.TH1D(f"h_{cr}_{var}_{sample}", "", nb, xmin, xmax)
                h.SetDirectory(0)
                hists[cr][var][sample] = h

    # Sample configuration: (label, file path, mu-charge branch name)
    # QCD NanoAOD processing used 'muon_charge'; others used 'mu_charge'
    samples = [
        ("data",   DATA_FILE, "mu_charge"),
        ("dy",     DY_FILE,   "mu_charge"),
        ("ttjets", TT_FILE,   "mu_charge"),
        ("qcd",    QCD_FILE,  "muon_charge"),
    ]

    for label, path, mu_ch_branch in samples:
        f = ROOT.TFile.Open(path)
        if not f or f.IsZombie():
            raise RuntimeError(f"Cannot open {path}")
        tree = f.Get("tree")
        if not tree:
            raise RuntimeError(f"TTree 'tree' not found in {path}")

        has_protons = (tree.GetBranch("proton_multi_arm") is not None and
                       tree.GetBranch("proton_multi_xi")  is not None)

        n = tree.GetEntries()
        print(f"  Processing {label:8s} – {n} events  (proton branches: {has_protons})")

        for i in range(n):
            tree.GetEntry(i)

            # ── Require one proton on each arm (mirrors save_shapes.cpp) ──────
            # Direct attribute access is the safe PyROOT way for vector branches
            if has_protons:
                xi1, xi2 = -1.0, -1.0
                for arm, xi in zip(tree.proton_multi_arm, tree.proton_multi_xi):
                    if arm == 0 and xi1 < 0:
                        xi1 = float(xi)
                    if arm == 1 and xi2 < 0:
                        xi2 = float(xi)
                if xi1 <= 0 or xi2 <= 0:
                    continue

            sist_mass = get_leaf(tree, "sist_mass")
            if sist_mass < 0:
                continue

            acop     = get_leaf(tree, "acop")
            sist_pt  = get_leaf(tree, "sist_pt")
            sist_rap = get_leaf(tree, "sist_rap")
            n_b_jet  = int(get_leaf(tree, "n_b_jet"))

            # Charge – try the expected branch name then fall back
            mu_ch  = get_leaf(tree, mu_ch_branch, "mu_charge", "muon_charge")
            tau_ch = get_leaf(tree, "tau_charge")
            is_os  = (mu_ch * tau_ch < 0)
            is_ss  = (mu_ch * tau_ch > 0)

            # CR flags
            is_dy_cr  = (40  <= sist_mass <= 100) and (acop < 0.3) and (n_b_jet == 0) and is_os
            is_tt_cr  = (sist_mass > 400)          and (acop > 0.3) and (n_b_jet >= 1) and is_os
            is_qcd_cr = (sist_mass > 100)          and (acop < 0.8) and (n_b_jet == 0) and is_ss

            # Weight
            if label == "data":
                w = 1.0
            elif label == "dy":
                w = get_leaf(tree, "event_weight", "weight", default=1.0) * DY_SCALE
            elif label == "ttjets":
                w = get_leaf(tree, "event_weight", "weight", default=1.0)
            else:  # qcd
                w = 1.0

            vals = {
                "aco":  acop,
                "mass": sist_mass,
                "pt":   sist_pt,
                "rap":  sist_rap,
            }

            for cr, flag in [("DY", is_dy_cr), ("TT", is_tt_cr), ("QCD", is_qcd_cr)]:
                if not flag:
                    continue
                for var, val in vals.items():
                    if cr == "QCD" and label == "data":
                        continue  # main data is OS-only; no SS events here
                    if cr == "QCD" and label == "qcd":
                        # SS data fills both 'data' points and the QCD stack entry
                        hists[cr][var]["data"].Fill(val, w)
                        hists[cr][var]["qcd"].Fill(val, w)
                        continue
                    hists[cr][var][label].Fill(val, w)

        f.Close()

    return hists


# ── Drawing ────────────────────────────────────────────────────────────────────

def draw_cr_plot(cr, var, hists, out_path):
    """Draw one CR × variable stacked plot with ratio panel."""

    h_data   = hists[cr][var]["data"]
    h_dy     = hists[cr][var]["dy"]
    h_qcd    = hists[cr][var]["qcd"]
    h_ttjets = hists[cr][var]["ttjets"]

    mc_map = {"dy": h_dy, "qcd": h_qcd, "ttjets": h_ttjets}

    # Style MC
    for sname, h in mc_map.items():
        h.SetFillColor(FILL_COLORS[sname])
        h.SetLineColor(ROOT.kBlack)
        h.SetLineWidth(1)

    # Style data
    h_data.SetMarkerStyle(20)
    h_data.SetMarkerSize(0.8)
    h_data.SetLineColor(ROOT.kBlack)
    h_data.SetLineWidth(1)

    # Total MC for uncertainty band and ratio
    h_mcsum = h_dy.Clone(f"h_{cr}_{var}_mcsum")
    h_mcsum.Add(h_qcd)
    h_mcsum.Add(h_ttjets)
    h_mcsum.SetFillColor(ROOT.kGray + 1)
    h_mcsum.SetFillStyle(3244)
    h_mcsum.SetMarkerSize(0)
    h_mcsum.SetLineWidth(0)
    h_mcsum.SetDirectory(0)

    # Build stack (bottom to top matches STACK_ORDER)
    stack = ROOT.THStack(f"stack_{cr}_{var}", "")
    for sname in STACK_ORDER[cr]:
        stack.Add(mc_map[sname])

    # ── Canvas ──
    cname = f"c_{cr}_{var}"
    canvas = ROOT.TCanvas(cname, cname, 700, 800)

    pad1 = ROOT.TPad(f"{cname}_p1", "", 0, 0.3, 1, 1.0)
    pad1.SetLeftMargin(0.16)
    pad1.SetRightMargin(0.06)
    pad1.SetTopMargin(0.12)
    pad1.SetBottomMargin(0.02)
    pad1.SetTicks(1, 1)
    pad1.Draw()

    pad2 = ROOT.TPad(f"{cname}_p2", "", 0, 0, 1, 0.3)
    pad2.SetLeftMargin(0.16)
    pad2.SetRightMargin(0.06)
    pad2.SetTopMargin(0.02)
    pad2.SetBottomMargin(0.42)
    pad2.SetTicks(1, 1)
    pad2.Draw()

    # ── Main pad ──
    pad1.cd()

    stack.Draw("hist")
    ymax = max(stack.GetMaximum(),
               h_data.GetMaximum() if h_data.GetMaximum() > 0 else 0)
    stack.SetMinimum(0)
    stack.SetMaximum(ymax * 1.65)
    stack.GetYaxis().SetTitle("Events")
    stack.GetYaxis().SetTitleSize(0.055)
    stack.GetYaxis().SetTitleOffset(1.1)
    stack.GetYaxis().SetLabelSize(0.05)
    stack.GetYaxis().SetNdivisions(5)
    stack.GetXaxis().SetLabelSize(0)
    stack.GetYaxis().SetMaxDigits(3)

    h_mcsum.Draw("e2 same")
    h_data.Draw("E same")

    # Legend (top-right for most variables, top-left for rapidity)
    if var == "rap":
        legend = ROOT.TLegend(0.18, 0.52, 0.58, 0.82)
    else:
        legend = ROOT.TLegend(0.55, 0.52, 0.92, 0.82)
    legend.SetBorderSize(0)
    legend.SetFillStyle(0)
    legend.SetTextFont(42)
    legend.SetTextSize(0.042)
    legend.SetMargin(0.15)
    legend.AddEntry(h_data, "Data (2018)", "lep")
    # Add legend entries in visual (top-of-stack first) order
    for sname in reversed(STACK_ORDER[cr]):
        legend.AddEntry(mc_map[sname], LEGEND_LABELS[sname], "f")
    legend.Draw()

    add_standard_labels(pad1)

    # CR label
    cr_label = ROOT.TLatex()
    cr_label.SetNDC(True)
    cr_label.SetTextFont(42)
    cr_label.SetTextSize(0.048)
    cr_label.DrawLatex(0.16, 0.78, CR_LABELS[cr])

    # ── Ratio pad ──
    pad2.cd()

    h_ratio = h_data.Clone(f"{cname}_ratio")
    h_ratio.Divide(h_mcsum)
    h_ratio.SetTitle("")
    h_ratio.SetMarkerStyle(20)
    h_ratio.SetMarkerSize(0.8)
    h_ratio.SetLineColor(ROOT.kBlack)

    h_ratio.GetYaxis().SetTitle("Data / Bkg")
    h_ratio.GetYaxis().SetTitleSize(0.12)
    h_ratio.GetYaxis().SetTitleOffset(0.55)
    h_ratio.GetYaxis().SetLabelSize(0.10)
    h_ratio.GetYaxis().SetNdivisions(505)
    h_ratio.GetYaxis().SetRangeUser(0.0, 2.0)

    h_ratio.GetXaxis().SetTitle(VAR_TITLES[var])
    h_ratio.GetXaxis().SetTitleSize(0.10)
    h_ratio.GetXaxis().SetTitleOffset(1.1)
    h_ratio.GetXaxis().SetLabelSize(0.10)
    h_ratio.GetXaxis().SetNdivisions(5)
    h_ratio.GetXaxis().CenterTitle(True)
    h_ratio.GetXaxis().SetTickLength(0.08)

    # Uncertainty band centred on 1
    h_ratio_unc = h_mcsum.Clone(f"{cname}_ratio_unc")
    for b in range(1, h_ratio_unc.GetNbinsX() + 1):
        mc  = h_mcsum.GetBinContent(b)
        err = h_mcsum.GetBinError(b)
        h_ratio_unc.SetBinContent(b, 1.0)
        h_ratio_unc.SetBinError(b, err / mc if mc > 0 else 0.0)
    h_ratio_unc.SetFillColor(ROOT.kGray + 1)
    h_ratio_unc.SetFillStyle(3001)
    h_ratio_unc.SetMarkerSize(0)
    h_ratio_unc.SetLineWidth(0)

    h_ratio.Draw("E0")
    h_ratio_unc.Draw("e2 same")
    h_ratio.Draw("E0 same")

    xmin = h_ratio.GetXaxis().GetXmin()
    xmax = h_ratio.GetXaxis().GetXmax()
    line = ROOT.TLine(xmin, 1.0, xmax, 1.0)
    line.SetLineColor(ROOT.kRed)
    line.SetLineStyle(2)
    line.SetLineWidth(2)
    line.Draw()

    canvas.Update()
    canvas.SaveAs(out_path)
    canvas.SaveAs(out_path.replace(".png", ".pdf"))
    canvas.Close()


# ── Combine into 3×4 grid ──────────────────────────────────────────────────────

def combine_cr_plots():
    """
    Combine all 12 individual CR plots into a single 3×4 figure.
    Layout: DY (top), TT (middle), QCD (bottom) × aco, mass, pt, rap
    """
    import matplotlib.pyplot as plt
    import matplotlib.image as mpimg

    row_order  = ["DY", "TT", "QCD"]
    col_order  = ["aco", "mass", "pt", "rap"]
    col_titles = [VAR_TITLES[v] for v in col_order]
    row_labels = [CR_LABELS[cr].replace("#bar{t}", "t̄") for cr in row_order]

    fig, axes = plt.subplots(3, 4, figsize=(22, 16))
    fig.suptitle(
        r"$\mu\tau_h$ final state: DY (top), $t\bar{t}$ (middle), QCD (bottom) CRs."
        "\nAcoplanarity, invariant mass, transverse momentum, rapidity of the central system.",
        fontsize=13, y=0.99,
    )

    for row, cr in enumerate(row_order):
        for col, var in enumerate(col_order):
            png = os.path.join(OUT_DIR, f"{cr}_CR_{var}.png")
            ax  = axes[row, col]
            if os.path.exists(png):
                ax.imshow(mpimg.imread(png))
                print(f"  Loaded {png}")
            else:
                ax.text(0.5, 0.5, f"Missing\n{png}",
                        ha="center", va="center", transform=ax.transAxes)
            ax.axis("off")
            if row == 0:
                ax.set_title(col_titles[col], fontsize=11, pad=6)

    for row, label in enumerate(row_labels):
        fig.text(0.01, 0.83 - row * 0.285, label, fontsize=12,
                 fontweight="bold", rotation=90, va="center", ha="center")

    plt.tight_layout(rect=[0.03, 0.01, 0.99, 0.96])
    plt.subplots_adjust(hspace=0.04, wspace=0.04)

    out_png = os.path.join(OUT_DIR, "combined_control_regions.png")
    out_pdf = os.path.join(OUT_DIR, "combined_control_regions.pdf")
    plt.savefig(out_png, dpi=200, bbox_inches="tight", facecolor="white")
    plt.savefig(out_pdf, bbox_inches="tight", facecolor="white")
    print(f"\n  Combined plot → {out_png}")
    print(f"  Combined plot → {out_pdf}")


# ── Main ───────────────────────────────────────────────────────────────────────

def main():
    print("=" * 60)
    print("μτh control region plots")
    print("=" * 60)

    print("\nFilling histograms...")
    hists = fill_histograms()

    print("\nDrawing plots...")
    for cr in ["DY", "TT", "QCD"]:
        for var in ["aco", "mass", "pt", "rap"]:
            out = os.path.join(OUT_DIR, f"{cr}_CR_{var}.png")
            draw_cr_plot(cr, var, hists, out)
            print(f"  Saved {out}")

    print("\nCombining into grid...")
    combine_cr_plots()

    print("\n" + "=" * 60)
    print(f"Done. Output in {OUT_DIR}/")
    print("  12 individual plots: DY/TT/QCD × aco/mass/pt/rap")
    print("  combined_control_regions.png / .pdf")
    print("=" * 60)


if __name__ == "__main__":
    main()
