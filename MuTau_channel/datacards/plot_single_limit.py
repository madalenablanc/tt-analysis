#!/usr/bin/env python3
"""
Quick helper to summarize a single-mass Combine limit result.

Usage:
  ./plot_single_limit.py higgsCombineMuTau.AsymptoticLimits.mH120.root \
      --mass 120 --output MuTau_limit_point

Creates:
  - MuTau_limit_point.txt  : plain-text summary of observed/expected limits
  - MuTau_limit_point.png  : simple ROOT plot showing the point with ±1σ/±2σ bands
"""

import argparse
import math
import pathlib

import ROOT


def read_limits(path):
    """Return dict with observed/expected limits from the Combine tree."""
    source = ROOT.TFile.Open(path)
    if not source or source.IsZombie():
        raise RuntimeError(f"Cannot open {path}")
    tree = source.Get("limit")
    if not tree:
        raise RuntimeError("Tree 'limit' not found in file")

    results = {}
    for entry in tree:
        q = getattr(entry, "quantileExpected", -1.0)
        val = getattr(entry, "limit", math.nan)
        if q == -1.0:
            results["obs"] = val
        elif abs(q - 0.5) < 1e-6:
            results["exp0"] = val
        elif abs(q - 0.84) < 1e-6:
            results["exp+1"] = val
        elif abs(q - 0.975) < 1e-6:
            results["exp+2"] = val
        elif abs(q - 0.16) < 1e-6:
            results["exp-1"] = val
        elif abs(q - 0.025) < 1e-6:
            results["exp-2"] = val

    source.Close()
    required = ["obs", "exp0", "exp+1", "exp-1", "exp+2", "exp-2"]
    missing = [key for key in required if key not in results]
    if missing:
        raise RuntimeError(f"Missing entries in limit tree: {', '.join(missing)}")
    return results


def write_text(results, out_prefix):
    """Save a plain-text summary."""
    path = pathlib.Path(f"{out_prefix}.txt")
    with path.open("w") as handle:
        handle.write("Estimate\tLimit\n")
        handle.write(f"Observed\t{results['obs']:.3g}\n")
        handle.write(f"Expected\t{results['exp0']:.3g}\n")
        handle.write(f"Expected-1sigma\t{results['exp-1']:.3g}\n")
        handle.write(f"Expected+1sigma\t{results['exp+1']:.3g}\n")
        handle.write(f"Expected-2sigma\t{results['exp-2']:.3g}\n")
        handle.write(f"Expected+2sigma\t{results['exp+2']:.3g}\n")
    print(f"[summary] Wrote {path}")


def make_plot(results, mass, out_prefix, y_title):
    """Draw a simple point with ±1σ/±2σ bands using ROOT."""
    canvas = ROOT.TCanvas("c_limits", "limits", 600, 500)
    canvas.SetGrid()

    mg = ROOT.TMultiGraph()

    # 2 sigma band
    graph_2sigma = ROOT.TGraphAsymmErrors(1)
    graph_2sigma.SetPoint(0, mass, results["exp0"])
    graph_2sigma.SetPointError(
        0,
        0.0,
        0.0,
        results["exp0"] - results["exp-2"],
        results["exp+2"] - results["exp0"],
    )
    graph_2sigma.SetFillColor(ROOT.kYellow)
    graph_2sigma.SetLineColor(ROOT.kYellow)
    graph_2sigma.SetMarkerStyle(0)
    mg.Add(graph_2sigma, "2")

    # 1 sigma band
    graph_1sigma = ROOT.TGraphAsymmErrors(1)
    graph_1sigma.SetPoint(0, mass, results["exp0"])
    graph_1sigma.SetPointError(
        0,
        0.0,
        0.0,
        results["exp0"] - results["exp-1"],
        results["exp+1"] - results["exp0"],
    )
    graph_1sigma.SetFillColor(ROOT.kGreen + 1)
    graph_1sigma.SetLineColor(ROOT.kGreen + 1)
    graph_1sigma.SetMarkerStyle(0)
    mg.Add(graph_1sigma, "2")

    # Expected line
    graph_exp = ROOT.TGraph(1)
    graph_exp.SetPoint(0, mass, results["exp0"])
    graph_exp.SetLineColor(ROOT.kBlack)
    graph_exp.SetLineWidth(2)
    graph_exp.SetLineStyle(2)
    mg.Add(graph_exp, "L")

    # Observed point
    graph_obs = ROOT.TGraph(1)
    graph_obs.SetPoint(0, mass, results["obs"])
    graph_obs.SetMarkerStyle(20)
    graph_obs.SetMarkerSize(1.2)
    graph_obs.SetLineWidth(2)
    mg.Add(graph_obs, "P")

    mg.Draw("A")
    mg.GetXaxis().SetTitle("m_{H} [GeV]")
    mg.GetYaxis().SetTitle(y_title)
    mg.GetXaxis().SetLimits(mass - 5, mass + 5)
    mg.SetMinimum(0.0)
    mg.SetMaximum(max(results["exp+2"], results["obs"]) * 1.2)

    legend = ROOT.TLegend(0.55, 0.65, 0.88, 0.88)
    legend.SetBorderSize(0)
    legend.SetFillStyle(0)
    legend.AddEntry(graph_obs, "Observed", "P")
    legend.AddEntry(graph_exp, "Expected", "L")
    legend.AddEntry(graph_1sigma, "Expected ±1 σ", "F")
    legend.AddEntry(graph_2sigma, "Expected ±2 σ", "F")
    legend.Draw()

    cms = ROOT.TLatex()
    cms.SetNDC(True)
    cms.SetTextFont(62)
    cms.SetTextSize(0.05)
    cms.DrawLatex(0.12, 0.92, "CMS")

    canvas.SaveAs(f"{out_prefix}.png")
    canvas.SaveAs(f"{out_prefix}.pdf")
    print(f"[plot] Saved {out_prefix}.png/pdf")


def main():
    parser = argparse.ArgumentParser(description="Summarize a single Combine limit output.")
    parser.add_argument("root_file", help="higgsCombine*.AsymptoticLimits.mH*.root file")
    parser.add_argument("--mass", type=float, default=125.0, help="mass value for x-axis")
    parser.add_argument("--output", default="limit_summary", help="output file prefix")
    parser.add_argument("--y-title", default="95% CL limit on r", help="y axis title")
    args = parser.parse_args()

    results = read_limits(args.root_file)
    write_text(results, args.output)
    make_plot(results, args.mass, args.output, args.y_title)


if __name__ == "__main__":
    ROOT.gROOT.SetBatch(True)
    main()
