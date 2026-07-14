"""
Brazil plot for MuTau channel — single mass point (mH = 120 GeV).
Reads higgsCombine*.AsymptoticLimits.mH120.root and draws the
expected ±1σ/±2σ bands with the observed limit.

Run on lxplus:
  source /cvmfs/sft.cern.ch/lcg/views/LCG_106/x86_64-el9-gcc13-opt/setup.sh
  python3 plot_brazil.py
"""

import math
import ROOT

ROOT.gROOT.SetBatch(True)
ROOT.gStyle.SetOptStat(0)

INPUT_FILE = "higgsCombineMuTau_bdt_fixed.AsymptoticLimits.mH120.root"
OUTPUT     = "MuTau_brazil"   # produces .png and .pdf


def read_limits(path):
    f = ROOT.TFile.Open(path)
    if not f or f.IsZombie():
        raise RuntimeError(f"Cannot open {path}")
    tree = f.Get("limit")
    if not tree:
        raise RuntimeError("Tree 'limit' not found")
    results = {}
    for entry in tree:
        q   = getattr(entry, "quantileExpected", -1.0)
        val = getattr(entry, "limit", math.nan)
        if   q < 0:                   results["obs"]  = val
        elif abs(q - 0.500) < 1e-4:   results["exp0"] = val
        elif abs(q - 0.840) < 1e-4:   results["exp+1"] = val
        elif abs(q - 0.975) < 1e-4:   results["exp+2"] = val
        elif abs(q - 0.160) < 1e-4:   results["exp-1"] = val
        elif abs(q - 0.025) < 1e-4:   results["exp-2"] = val
    f.Close()
    for key in ("obs", "exp0", "exp+1", "exp-1", "exp+2", "exp-2"):
        if key not in results:
            raise RuntimeError(f"Missing '{key}' in limit tree")
    return results


def make_brazil(results, output):
    # Single mass point — draw as a horizontal band plot
    # x-axis = r = sigma/sigma_SM, y-axis = dummy label

    canvas = ROOT.TCanvas("c_brazil", "", 700, 500)
    canvas.SetCanvasSize(700, 500)
    canvas.cd()
    pad = ROOT.gPad
    pad.SetLeftMargin(0.16)
    pad.SetRightMargin(0.06)
    pad.SetTopMargin(0.16)
    pad.SetBottomMargin(0.14)
    pad.SetLogx(1)

    # y positions for the single point
    y0 = 1.0

    # ±2σ band
    g2 = ROOT.TGraphAsymmErrors(1)
    g2.SetPoint(0, results["exp0"], y0)
    g2.SetPointError(0,
        results["exp0"] - results["exp-2"],
        results["exp+2"] - results["exp0"],
        0.4, 0.4)
    g2.SetFillColor(ROOT.kYellow)
    g2.SetLineColor(ROOT.kYellow)
    g2.SetMarkerSize(0)

    # ±1σ band
    g1 = ROOT.TGraphAsymmErrors(1)
    g1.SetPoint(0, results["exp0"], y0)
    g1.SetPointError(0,
        results["exp0"] - results["exp-1"],
        results["exp+1"] - results["exp0"],
        0.4, 0.4)
    g1.SetFillColor(ROOT.kGreen + 1)
    g1.SetLineColor(ROOT.kGreen + 1)
    g1.SetMarkerSize(0)

    # expected median line
    g_exp = ROOT.TGraphErrors(1)
    g_exp.SetPoint(0, results["exp0"], y0)
    g_exp.SetPointError(0, 0, 0.4)
    g_exp.SetLineColor(ROOT.kBlack)
    g_exp.SetLineWidth(2)
    g_exp.SetLineStyle(2)
    g_exp.SetMarkerSize(0)

    # observed point
    g_obs = ROOT.TGraph(1)
    g_obs.SetPoint(0, results["obs"], y0)
    g_obs.SetMarkerStyle(20)
    g_obs.SetMarkerSize(1.4)
    g_obs.SetMarkerColor(ROOT.kBlack)
    g_obs.SetLineColor(ROOT.kBlack)
    g_obs.SetLineWidth(2)

    mg = ROOT.TMultiGraph()
    mg.Add(g2,    "2")
    mg.Add(g1,    "2")
    mg.Add(g_exp, "||")
    mg.Add(g_obs, "P")
    mg.Draw("A")

    xmax = max(results["obs"], results["exp+2"]) * 2.0
    xmin = results["exp-2"] * 0.4
    mg.GetXaxis().SetLimits(xmin, xmax)
    mg.SetMinimum(0.0)
    mg.SetMaximum(2.0)

    mg.GetXaxis().SetTitle("95% CL limit on r = #sigma / #sigma_{SM}")
    mg.GetXaxis().SetNdivisions(5)
    mg.GetXaxis().SetTitleSize(0.05)
    mg.GetXaxis().SetLabelSize(0.04)
    mg.GetXaxis().SetTitleOffset(1.0)
    mg.GetYaxis().SetLabelSize(0)
    mg.GetYaxis().SetTickLength(0)
    mg.GetYaxis().SetNdivisions(0)

    # Legend
    legend = ROOT.TLegend(0.55, 0.55, 0.88, 0.80)
    legend.SetBorderSize(0)
    legend.SetFillStyle(0)
    legend.SetTextFont(42)
    legend.SetTextSize(0.038)
    legend.SetMargin(0.20)
    legend.AddEntry(g_obs,  "Observed",        "P")
    legend.AddEntry(g_exp,  "Expected",        "L")
    legend.AddEntry(g1,     "Expected #pm1#sigma", "F")
    legend.AddEntry(g2,     "Expected #pm2#sigma", "F")
    legend.Draw()

    # CMS-TOTEM label (same as plot_m.py)
    cms = ROOT.TLatex()
    cms.SetNDC(True)
    cms.SetTextFont(62)
    cms.SetTextSize(0.055)
    cms.DrawLatex(0.16, 0.94, "CMS-TOTEM")

    prelim = ROOT.TLatex()
    prelim.SetNDC(True)
    prelim.SetTextFont(52)
    prelim.SetTextSize(0.042)
    prelim.DrawLatex(0.16, 0.88, "Preliminary")

    lumi = ROOT.TLatex()
    lumi.SetNDC(True)
    lumi.SetTextFont(62)
    lumi.SetTextSize(0.045)
    lumi.SetTextAlign(31)
    lumi.DrawLatex(0.94, 0.94, "54.9 fb^{-1} (13 TeV)")

    # Print values on plot
    vals = ROOT.TLatex()
    vals.SetNDC(False)
    vals.SetTextFont(42)
    vals.SetTextSize(0.038)
    vals.SetTextAlign(12)

    canvas.Update()
    canvas.SaveAs(f"{output}.png")
    canvas.SaveAs(f"{output}.pdf")
    print(f"Saved {output}.png and {output}.pdf")
    print()
    print(f"  Observed:        r < {results['obs']:.1f}")
    print(f"  Expected median: r < {results['exp0']:.1f}")
    print(f"  Expected -1σ:    r < {results['exp-1']:.1f}")
    print(f"  Expected +1σ:    r < {results['exp+1']:.1f}")
    print(f"  Expected -2σ:    r < {results['exp-2']:.1f}")
    print(f"  Expected +2σ:    r < {results['exp+2']:.1f}")


if __name__ == "__main__":
    results = read_limits(INPUT_FILE)
    make_brazil(results, OUTPUT)
