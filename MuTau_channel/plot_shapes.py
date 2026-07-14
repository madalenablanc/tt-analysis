"""
Plot shapes from MuTau_shapes.root (output of save_shapes.cpp).
Style matches the MuTau channel conventions in plot_m.py / plot_bdt_real_norm.C.
Includes data/simulation ratio panel.

Run on lxplus with:
  source /cvmfs/sft.cern.ch/lcg/views/LCG_106/x86_64-el9-gcc13-opt/setup.sh
  python3 plot_shapes.py
"""

import os
import ROOT

ROOT.gROOT.SetBatch(True)
ROOT.gStyle.SetOptStat(0)
ROOT.gStyle.SetEndErrorSize(0)

SHAPES_FILE = "MuTau_shapes.root"
OUT_DIR = "plots_shapes"
os.makedirs(OUT_DIR, exist_ok=True)

SIGNAL_SCALE = 5000

# (prefix, x-axis title, output filename, log-scale y-axis)
VARIABLES = [
    ("m",    "Invariant mass of the central system [GeV]", "mass",              False),
    ("pt",   "Transverse momentum of the central system [GeV]", "pt",           False),
    ("aco",  "Acoplanarity of the central system", "aco",                       False),
    ("ra",   "Rapidity of the central system", "rapidity",                      False),
    ("r",    "Rapidity matching", "rapidity_matching",                          False),
    ("mm",   "Mass difference (CMS-PPS) [GeV]", "mass_difference",             False),
    ("tau",  "Transverse momentum of the hadronic tau [GeV]", "tau_pt",        False),
    ("met",  "Missing transverse energy [GeV]", "met_pt",                       False),
    ("mupt", "Muon transverse momentum [GeV]", "muon_pt",                       False),
    ("bdt",  "BDT score", "bdt",                                                True),
    ("lk",   "Likelihood score", "likelihood",                                  True),
    ("fish", "Fisher score", "fisher",                                          True),
]

LEGEND_RIGHT = {"m", "met", "mupt", "tau", "pt", "bdt"}


def create_legend(var=""):
    if var == "mm":
        legend = ROOT.TLegend(0.58, 0.62, 0.92, 0.88)
    elif var in LEGEND_RIGHT:
        legend = ROOT.TLegend(0.52, 0.62, 0.92, 0.88)
    elif var in {"ra", "r"}:
        legend = ROOT.TLegend(0.18, 0.55, 0.64, 0.85)
    else:
        # Left-side legend sits below the CMS label block
        legend = ROOT.TLegend(0.18, 0.50, 0.64, 0.80)
    legend.SetBorderSize(0)
    legend.SetFillStyle(0)
    legend.SetTextFont(42)
    legend.SetTextSize(0.042)
    legend.SetMargin(0.15)
    return legend


def style_histogram(hist, fill_color):
    hist.SetFillColor(fill_color)
    hist.SetLineColor(ROOT.kBlack)
    hist.SetLineWidth(1)


def add_standard_labels():
    cms = ROOT.TLatex()
    cms.SetNDC(True)
    cms.SetTextFont(62)
    cms.SetTextSize(0.065)
    cms.DrawLatex(0.16, 0.93, "CMS")

    status = ROOT.TLatex()
    status.SetNDC(True)
    status.SetTextFont(52)
    status.SetTextSize(0.052)
    status.DrawLatex(0.16, 0.89, "Work in progress")

    lumi = ROOT.TLatex()
    lumi.SetNDC(True)
    lumi.SetTextFont(62)
    lumi.SetTextSize(0.052)
    lumi.SetTextAlign(31)
    lumi.DrawLatex(0.94, 0.93, "54.9 fb^{-1} (13 TeV)")


def draw_and_save(name, stack, signal_hist, data_hist, h_mcsum, legend, x_title, output_path, logy=False):
    canvas = ROOT.TCanvas(name, name, 700, 800)
    canvas.SetCanvasSize(700, 800)

    # Main pad
    pad1 = ROOT.TPad(f"{name}_p1", "", 0, 0.3, 1, 1)
    pad1.SetLeftMargin(0.16)
    pad1.SetRightMargin(0.06)
    pad1.SetTopMargin(0.12)
    pad1.SetBottomMargin(0.02)
    pad1.SetTicks(1, 1)
    pad1.Draw()

    # Ratio pad
    pad2 = ROOT.TPad(f"{name}_p2", "", 0, 0, 1, 0.3)
    pad2.SetLeftMargin(0.16)
    pad2.SetRightMargin(0.06)
    pad2.SetTopMargin(0.02)
    pad2.SetBottomMargin(0.42)
    pad2.SetTicks(1, 1)
    pad2.Draw()

    # --- Main pad ---
    pad1.cd()
    if logy:
        pad1.SetLogy()

    stack.Draw("hist")

    ymax = max(stack.GetMaximum(), signal_hist.GetMaximum(),
               data_hist.GetMaximum() if data_hist.GetMaximum() > 0 else 0)
    if logy:
        stack.SetMinimum(0.03)
        stack.SetMaximum(ymax * 20)
    else:
        stack.SetMinimum(0)
        stack.SetMaximum(ymax * 1.65)

    stack.GetYaxis().SetTitle("Events")
    stack.GetYaxis().SetTitleSize(0.055)
    stack.GetYaxis().SetTitleOffset(1.1)
    stack.GetYaxis().SetLabelSize(0.05)
    stack.GetYaxis().SetNdivisions(5)
    stack.GetXaxis().SetLabelSize(0)  # hidden — shown in ratio pad
    stack.GetYaxis().SetMaxDigits(3)

    h_mcsum.Draw("e2 same")

    signal_hist.SetLineWidth(4)
    signal_hist.SetLineStyle(2)
    signal_hist.Draw("hist same")

    data_hist.Draw("same E")

    legend.Draw()
    add_standard_labels()

    # --- Ratio pad ---
    pad2.cd()

    h_ratio = data_hist.Clone(f"{name}_ratio")
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
    h_ratio.GetYaxis().SetRangeUser(0.55, 1.45)

    h_ratio.GetXaxis().SetTitle(x_title)
    h_ratio.GetXaxis().SetTitleSize(0.10)
    h_ratio.GetXaxis().SetTitleOffset(1.1)
    h_ratio.GetXaxis().SetLabelSize(0.10)
    h_ratio.GetXaxis().SetNdivisions(5)
    h_ratio.GetXaxis().CenterTitle(True)
    h_ratio.GetXaxis().SetTickLength(0.08)

    h_ratio.SetMarkerStyle(20)
    h_ratio.SetMarkerSize(0.8)
    h_ratio.Draw("E0")

    line = ROOT.TLine(h_ratio.GetXaxis().GetXmin(), 1,
                      h_ratio.GetXaxis().GetXmax(), 1)
    line.SetLineColor(ROOT.kGray + 2)
    line.SetLineStyle(2)
    line.SetLineWidth(2)
    line.Draw()

    canvas.Update()
    canvas.SaveAs(str(output_path))
    canvas.SaveAs(str(output_path).replace(".png", ".pdf"))
    canvas.Close()


def load_hist(rootfile, name):
    h = rootfile.Get(name)
    if not h:
        return None
    h.SetDirectory(0)
    return h


def main():
    f = ROOT.TFile.Open(SHAPES_FILE)
    if not f or f.IsZombie():
        raise RuntimeError(f"Cannot open {SHAPES_FILE}")

    print(f"Opened {SHAPES_FILE}")

    for var, x_title, out_name, logy in VARIABLES:
        h_data   = load_hist(f, f"{var}_data")
        h_qcd    = load_hist(f, f"{var}_qcd")
        h_dy     = load_hist(f, f"{var}_dy")
        h_ttjets = load_hist(f, f"{var}_ttjets")
        h_sinal  = load_hist(f, f"{var}_sinal")

        if h_data is None or h_sinal is None:
            print(f"  Skipping {var}: histograms not found")
            continue

        if h_qcd    is None: h_qcd    = h_data.Clone(f"{var}_qcd");    h_qcd.Reset()
        if h_dy     is None: h_dy     = h_data.Clone(f"{var}_dy");     h_dy.Reset()
        if h_ttjets is None: h_ttjets = h_data.Clone(f"{var}_ttjets"); h_ttjets.Reset()

        # Background styles
        style_histogram(h_ttjets, ROOT.TColor.GetColor("#66c2a5"))
        style_histogram(h_qcd, ROOT.TColor.GetColor("#fc8d62"))
        style_histogram(h_dy, ROOT.TColor.GetColor("#ffd92f"))

        # Total MC sum for ratio
        h_mcsum = h_dy.Clone(f"{var}_mcsum")
        h_mcsum.Add(h_qcd)
        h_mcsum.Add(h_ttjets)
        h_mcsum.SetFillColor(ROOT.kGray + 2)
        h_mcsum.SetFillStyle(3354)
        h_mcsum.SetMarkerSize(0)
        h_mcsum.SetLineWidth(0)

        # Signal
        h_sinal.SetLineColor(ROOT.kBlack)
        h_sinal.SetFillStyle(0)
        h_sinal_scaled = h_sinal.Clone(f"{var}_sinal_scaled")
        h_sinal_scaled.Scale(SIGNAL_SCALE)

        # Data
        h_data.SetLineColor(ROOT.kBlack)
        h_data.SetLineWidth(1)
        h_data.SetMarkerStyle(20)
        h_data.SetMarkerSize(1.0)
        h_data.SetMarkerColor(ROOT.kBlack)

        # Stack: bottom to top = ttjets, qcd, dy
        stack = ROOT.THStack(f"stack_{var}", "")
        stack.Add(h_ttjets)
        stack.Add(h_qcd)
        stack.Add(h_dy)

        legend = create_legend(var)
        legend.AddEntry(h_data,         "Data (2018)",            "lep")
        legend.AddEntry(h_ttjets,       "t#bar{t}",               "f")
        legend.AddEntry(h_dy,           "Drell Yan",              "f")
        legend.AddEntry(h_qcd,          "QCD (Data driven)",      "f")
        legend.AddEntry(h_sinal_scaled, f"Signal (x{SIGNAL_SCALE})", "l")

        out_path = os.path.join(OUT_DIR, f"{out_name}.png")
        draw_and_save(f"c_{var}", stack, h_sinal_scaled, h_data,
                      h_mcsum, legend, x_title, out_path, logy=logy)
        print(f"  Saved {out_path}")

    f.Close()
    print(f"\nDone. Plots saved to {OUT_DIR}/")


if __name__ == "__main__":
    main()
