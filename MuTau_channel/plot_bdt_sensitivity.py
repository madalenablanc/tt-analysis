"""
Cumulative S, B, and S/sqrt(B) as a function of minimum BDT-score threshold.
Reads bdt_* histograms from MuTau_shapes.root.

Run on lxplus with:
  source /cvmfs/sft.cern.ch/lcg/views/LCG_106/x86_64-el9-gcc13-opt/setup.sh
  python3 plot_bdt_sensitivity.py
"""

import os
import math
import ROOT

ROOT.gROOT.SetBatch(True)
ROOT.gStyle.SetOptStat(0)

SHAPES_FILE = "MuTau_shapes.root"
OUT_DIR     = "plots_shapes"
os.makedirs(OUT_DIR, exist_ok=True)


def load_hist(f, name):
    h = f.Get(name)
    if not h:
        raise RuntimeError(f"Histogram '{name}' not found in {SHAPES_FILE}")
    h.SetDirectory(0)
    return h


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
    prelim.DrawLatex(0.16, 0.86, "Preliminary")

    lumi = ROOT.TLatex()
    lumi.SetNDC(True)
    lumi.SetTextFont(62)
    lumi.SetTextSize(0.052)
    lumi.SetTextAlign(31)
    lumi.DrawLatex(0.94, 0.93, "54.9 fb^{-1} (13 TeV)")


def main():
    f = ROOT.TFile.Open(SHAPES_FILE)
    if not f or f.IsZombie():
        raise RuntimeError(f"Cannot open {SHAPES_FILE}")

    h_sig    = load_hist(f, "bdt_sinal")
    h_qcd    = load_hist(f, "bdt_qcd")
    h_dy     = load_hist(f, "bdt_dy")
    h_ttjets = load_hist(f, "bdt_ttjets")

    nbins = h_sig.GetNbinsX()

    # For each bin threshold compute integrals from that bin to the end
    thresholds = []
    S_vals     = []
    B_vals     = []
    SoSqrtB    = []

    for i in range(1, nbins + 2):   # i = first bin of integration window
        t   = h_sig.GetXaxis().GetBinLowEdge(i)
        S   = h_sig.Integral(i, nbins)
        B   = (h_dy.Integral(i, nbins) +
               h_qcd.Integral(i, nbins) +
               h_ttjets.Integral(i, nbins))
        ssb = S / math.sqrt(B) if B > 0 else 0.0
        thresholds.append(t)
        S_vals.append(S)
        B_vals.append(B)
        SoSqrtB.append(ssb)

    n = len(thresholds)

    # Build TGraphs
    g_S = ROOT.TGraph(n)
    g_B = ROOT.TGraph(n)
    g_ssb = ROOT.TGraph(n)
    for i, (t, s, b, ssb) in enumerate(zip(thresholds, S_vals, B_vals, SoSqrtB)):
        g_S.SetPoint(i, t, s)
        g_B.SetPoint(i, t, b)
        g_ssb.SetPoint(i, t, ssb)

    g_S.SetLineColor(ROOT.kBlack)
    g_S.SetLineWidth(2)
    g_S.SetLineStyle(2)

    g_B.SetLineColor(ROOT.kRed)
    g_B.SetLineWidth(2)
    g_B.SetLineStyle(1)

    g_ssb.SetLineColor(ROOT.kBlue + 1)
    g_ssb.SetLineWidth(2)
    g_ssb.SetLineStyle(1)

    # ---- Canvas with two pads ----
    canvas = ROOT.TCanvas("c_sens", "", 700, 800)
    canvas.SetCanvasSize(700, 800)

    pad1 = ROOT.TPad("p1", "", 0, 0.35, 1, 1)
    pad1.SetLeftMargin(0.16)
    pad1.SetRightMargin(0.06)
    pad1.SetTopMargin(0.12)
    pad1.SetBottomMargin(0.02)
    pad1.Draw()

    pad2 = ROOT.TPad("p2", "", 0, 0, 1, 0.35)
    pad2.SetLeftMargin(0.16)
    pad2.SetRightMargin(0.06)
    pad2.SetTopMargin(0.02)
    pad2.SetBottomMargin(0.30)
    pad2.Draw()

    # --- Top pad: S and B ---
    pad1.cd()

    mg1 = ROOT.TMultiGraph()
    mg1.Add(g_B, "L")
    mg1.Add(g_S, "L")
    mg1.Draw("A")

    mg1.GetXaxis().SetLimits(-0.8, 0.8)
    mg1.GetXaxis().SetLabelSize(0)
    mg1.GetYaxis().SetTitle("Cumulative yield")
    mg1.GetYaxis().SetTitleSize(0.055)
    mg1.GetYaxis().SetTitleOffset(1.1)
    mg1.GetYaxis().SetLabelSize(0.05)
    mg1.GetYaxis().SetNdivisions(5)
    mg1.SetMinimum(0)

    leg = ROOT.TLegend(0.55, 0.62, 0.88, 0.82)
    leg.SetBorderSize(0)
    leg.SetFillStyle(0)
    leg.SetTextFont(42)
    leg.SetTextSize(0.048)
    leg.SetMargin(0.20)
    leg.AddEntry(g_S,   "Signal S",            "L")
    leg.AddEntry(g_B,   "Total background B",  "L")
    leg.Draw()

    add_standard_labels(pad1)

    # --- Bottom pad: S/sqrt(B) ---
    pad2.cd()

    mg2 = ROOT.TMultiGraph()
    mg2.Add(g_ssb, "L")
    mg2.Draw("A")

    mg2.GetXaxis().SetLimits(-0.8, 0.8)
    mg2.GetXaxis().SetTitle("Minimum BDT score")
    mg2.GetXaxis().SetTitleSize(0.10)
    mg2.GetXaxis().SetTitleOffset(1.0)
    mg2.GetXaxis().SetLabelSize(0.08)
    mg2.GetXaxis().SetNdivisions(5)
    mg2.GetYaxis().SetTitle("S / #sqrt{B}")
    mg2.GetYaxis().SetTitleSize(0.10)
    mg2.GetYaxis().SetTitleOffset(0.7)
    mg2.GetYaxis().SetLabelSize(0.08)
    mg2.GetYaxis().SetNdivisions(505)
    mg2.SetMinimum(0)

    canvas.Update()
    out = os.path.join(OUT_DIR, "bdt_cumulative_sensitivity.png")
    canvas.SaveAs(out)
    canvas.SaveAs(out.replace(".png", ".pdf"))
    print(f"Saved {out}")

    f.Close()


if __name__ == "__main__":
    main()
