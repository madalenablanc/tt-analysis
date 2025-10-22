#!/usr/bin/env python3

import math
import os
from pathlib import Path

import ROOT


def ensure_tree(file_handle, tree_name):
    tree = file_handle.Get(tree_name)
    if not tree:
        raise RuntimeError(f"TTree '{tree_name}' not found in {file_handle.GetName()}")
    return tree


def ensure_file(path):
    handle = ROOT.TFile.Open(path)
    if not handle or handle.IsZombie():
        raise RuntimeError(f"Cannot open file: {path}")
    return handle


def configure_axes(stack, x_title, y_title, y_range=None):
    x_axis = stack.GetXaxis()
    y_axis = stack.GetYaxis()
    x_axis.SetTitle(x_title)
    y_axis.SetTitle(y_title)
    x_axis.SetNdivisions(5)
    y_axis.SetNdivisions(5)
    x_axis.SetTitleSize(0.03)
    y_axis.SetTitleSize(0.03)
    x_axis.SetLabelSize(0.03)
    y_axis.SetLabelSize(0.03)
    x_axis.SetLabelOffset(0.01)
    y_axis.SetLabelOffset(0.01)
    x_axis.SetTitleOffset(1.0)
    if y_range:
        stack.SetMinimum(y_range[0])
        stack.SetMaximum(y_range[1])


def auto_y_range(stack, signal_hist, data_hist=None, ymin=0.0, scale=1.25):
    ymax_stack = stack.GetMaximum()
    ymax_signal = signal_hist.GetMaximum()
    ymax_data = data_hist.GetMaximum() if data_hist else 0.0
    ymax = max(ymax_stack, ymax_signal, ymax_data)
    if ymax <= 0:
        ymax = 1.0
    return ymin, ymax * scale


def create_legend():
    legend = ROOT.TLegend(0.18, 0.60, 0.48, 0.82)
    legend.SetBorderSize(0)
    legend.SetFillStyle(0)
    legend.SetTextFont(42)
    legend.SetTextSize(0.035)
    legend.SetMargin(0.20)
    return legend


def add_standard_labels():
    cms = ROOT.TLatex()
    cms.SetNDC(True)
    cms.SetTextFont(62)
    cms.SetTextSize(0.055)
    cms.DrawLatex(0.12, 0.94, "CMS-TOTEM")

    prelim = ROOT.TLatex()
    prelim.SetNDC(True)
    prelim.SetTextFont(52)  # italic
    prelim.SetTextSize(0.042)
    prelim.DrawLatex(0.12, 0.89, "Preliminary")

    lumi = ROOT.TLatex()
    lumi.SetNDC(True)
    lumi.SetTextFont(62)
    lumi.SetTextSize(0.045)
    lumi.SetTextAlign(31)
    lumi.DrawLatex(0.92, 0.94, "54.9 fb^{-1} (13 TeV)")


def draw_and_save(
    name,
    stack,
    signal_hist,
    legend,
    x_title,
    y_title,
    output_path,
    y_range=None,
    data_hist=None,
):
    canvas = ROOT.TCanvas(name, name, 700, 800)
    canvas.SetCanvasSize(700, 800)
    canvas.cd()
    pad = ROOT.gPad
    pad.SetPad(0.0, 0.0, 1.0, 1.0)
    pad.SetLeftMargin(0.16)
    pad.SetRightMargin(0.06)
    pad.SetTopMargin(0.16)
    pad.SetBottomMargin(0.14)
    pad.Draw()

    stack.Draw("hist")
    configure_axes(stack, x_title, y_title, y_range=y_range)

    signal_hist.SetLineWidth(3)
    signal_hist.SetLineStyle(1)
    signal_hist.Draw("hist same")

    if data_hist:
        data_hist.Draw("E1 same")

    legend.Draw()
    add_standard_labels()

    canvas.Update()
    canvas.SaveAs(str(output_path))


def main():
    ROOT.gROOT.SetBatch(True)
    ROOT.gStyle.SetOptStat(0)

    tree_name = "tree"
    file_paths = {
        "dy": "/eos/home-m/mblancco/samples_2018_mutau/DY_2018_UL_MuTau_nano_merged_pileup_protons.root",
        "ttjets": "/eos/home-m/mblancco/samples_2018_mutau/ttjets_2018_UL_MuTau_nano_merged_pileup_protons.root",
        "qcd": "/eos/home-m/mblancco/samples_2018_mutau/QCD_2018_UL_MuTau_nano_merged_proton_vars.root",
        "sinal": "MuTau_sinal_SM_2018_july.root",
        "data":  "/eos/home-m/mblancco/samples_2018_mutau/Data_2018_UL_MuTau_nano_merged_proton_vars.root"
    }

    dy_file = ensure_file(file_paths["dy"])
    ttjets_file = ensure_file(file_paths["ttjets"])
    qcd_file = ensure_file(file_paths["qcd"])
    sinal_file = ensure_file(file_paths["sinal"])
    data_file = ensure_file(file_paths["data"])

    tree_dy = ensure_tree(dy_file, tree_name)
    tree_ttjets = ensure_tree(ttjets_file, tree_name)
    tree_qcd = ensure_tree(qcd_file, tree_name)
    tree_sinal = ensure_tree(sinal_file, tree_name)
    tree_data = ensure_tree(data_file, tree_name)

    binning = {
        "aco": (20, 0.0, 1.0),
        "m": (30, 0.0, 1200.0),
        "r": (10, -2.2, 2.2),
        "pt": (20, 100.0, 600.0),
        "mm": (20, -1500.0, 200.0),
        "ra": (10, -2.2, 2.2),
        "tau": (20, 100.0, 500.0),
        "met": (20, 0.0, 300.0),
    }

    aco_qcd = ROOT.TH1D("aco_qcd", "aco_qcd", *binning["aco"])
    aco_dy = ROOT.TH1D("aco_dy", "aco_dy", *binning["aco"])
    aco_ttjets = ROOT.TH1D("aco_ttjets", "aco_ttjets", *binning["aco"])
    aco_sinal = ROOT.TH1D("aco_sinal", "aco_sinal", *binning["aco"])

    m_qcd = ROOT.TH1D("m_qcd", "m_qcd", *binning["m"])
    m_dy = ROOT.TH1D("m_dy", "m_dy", *binning["m"])
    m_ttjets = ROOT.TH1D("m_ttjets", "m_ttjets", *binning["m"])
    m_sinal = ROOT.TH1D("m_sinal", "m_sinal", *binning["m"])

    r_qcd = ROOT.TH1D("r_qcd", "r_qcd", *binning["r"])
    r_dy = ROOT.TH1D("r_dy", "r_dy", *binning["r"])
    r_ttjets = ROOT.TH1D("r_ttjets", "r_ttjets", *binning["r"])
    r_sinal = ROOT.TH1D("r_sinal", "r_sinal", *binning["r"])

    pt_qcd = ROOT.TH1D("pt_qcd", "pt_qcd", *binning["pt"])
    pt_dy = ROOT.TH1D("pt_dy", "pt_dy", *binning["pt"])
    pt_ttjets = ROOT.TH1D("pt_ttjets", "pt_ttjets", *binning["pt"])
    pt_sinal = ROOT.TH1D("pt_sinal", "pt_sinal", *binning["pt"])

    mm_qcd = ROOT.TH1D("mm_qcd", "mm_qcd", *binning["mm"])
    mm_dy = ROOT.TH1D("mm_dy", "mm_dy", *binning["mm"])
    mm_ttjets = ROOT.TH1D("mm_ttjets", "mm_ttjets", *binning["mm"])
    mm_sinal = ROOT.TH1D("mm_sinal", "mm_sinal", *binning["mm"])

    ra_qcd = ROOT.TH1D("ra_qcd", "ra_qcd", *binning["ra"])
    ra_dy = ROOT.TH1D("ra_dy", "ra_dy", *binning["ra"])
    ra_ttjets = ROOT.TH1D("ra_ttjets", "ra_ttjets", *binning["ra"])
    ra_sinal = ROOT.TH1D("ra_sinal", "ra_sinal", *binning["ra"])

    tau_qcd = ROOT.TH1D("tau_qcd", "tau_qcd", *binning["tau"])
    tau_dy = ROOT.TH1D("tau_dy", "tau_dy", *binning["tau"])
    tau_ttjets = ROOT.TH1D("tau_ttjets", "tau_ttjets", *binning["tau"])
    tau_sinal = ROOT.TH1D("tau_sinal", "tau_sinal", *binning["tau"])

    met_qcd = ROOT.TH1D("met_qcd", "met_qcd", *binning["met"])
    met_dy = ROOT.TH1D("met_dy", "met_dy", *binning["met"])
    met_ttjets = ROOT.TH1D("met_ttjets", "met_ttjets", *binning["met"])
    met_sinal = ROOT.TH1D("met_sinal", "met_sinal", *binning["met"])
    met_data = ROOT.TH1D("met_data", "met_data", *binning["met"])
    tau_data = ROOT.TH1D("tau_data", "tau_data", *binning["tau"])
    ra_data = ROOT.TH1D("ra_data", "ra_data", *binning["ra"])
    mm_data = ROOT.TH1D("mm_data", "mm_data", *binning["mm"])
    pt_data = ROOT.TH1D("pt_data", "pt_data", *binning["pt"])
    r_data = ROOT.TH1D("r_data", "r_data", *binning["r"])
    m_data = ROOT.TH1D("m_data", "m_data", *binning["m"])
    aco_data = ROOT.TH1D("aco_data", "aco_data", *binning["aco"])

    for hist in [
        aco_qcd,
        aco_dy,
        aco_ttjets,
        aco_sinal,
        m_qcd,
        m_dy,
        m_ttjets,
        m_sinal,
        r_qcd,
        r_dy,
        r_ttjets,
        r_sinal,
        pt_qcd,
        pt_dy,
        pt_ttjets,
        pt_sinal,
        mm_qcd,
        mm_dy,
        mm_ttjets,
        mm_sinal,
        ra_qcd,
        ra_dy,
        ra_ttjets,
        ra_sinal,
        tau_qcd,
        tau_dy,
        tau_ttjets,
        tau_sinal,
        met_qcd,
        met_dy,
        met_ttjets,
        met_sinal,
        met_data,
        tau_data,
        ra_data,
        mm_data,
        pt_data,
        r_data,
        m_data,
        aco_data,
    ]:
        hist.Sumw2()

    w_qcd = 1.0
    n_qcd = 0.0
    for event in tree_qcd:
        if event.sist_mass < 0:
            continue
        n_qcd += w_qcd
        aco_qcd.Fill(event.acop, w_qcd)
        m_qcd.Fill(event.sist_mass, w_qcd)
        if event.xi_arm1_1 > 0 and event.xi_arm2_1 > 0:
            rap_match = event.sist_rap - 0.5 * math.log(event.xi_arm1_1 / event.xi_arm2_1)
            r_qcd.Fill(rap_match, w_qcd)
        pt_qcd.Fill(event.sist_pt, w_qcd)
        if event.xi_arm1_1 >= 0 and event.xi_arm2_1 >= 0:
            mass_diff = event.sist_mass - 13000.0 * math.sqrt(event.xi_arm1_1 * event.xi_arm2_1)
            mm_qcd.Fill(mass_diff, w_qcd)
        ra_qcd.Fill(event.sist_rap, w_qcd)
        tau_qcd.Fill(event.tau_pt, w_qcd)
        met_qcd.Fill(event.mu_pt, w_qcd)

    w_dy = 1.81
    n_dy = 0.0
    for event in tree_dy:
        if event.sist_mass < 0:
            continue
        w_dy = event.weight
        n_dy += w_dy
        aco_dy.Fill(event.acop, w_dy)
        m_dy.Fill(event.sist_mass, w_dy)
        if event.xi_arm1_1 > 0 and event.xi_arm2_1 > 0:
            rap_match = event.sist_rap - 0.5 * math.log(event.xi_arm1_1 / event.xi_arm2_1)
            r_dy.Fill(rap_match, w_dy)
        pt_dy.Fill(event.sist_pt, w_dy)
        if event.xi_arm1_1 >= 0 and event.xi_arm2_1 >= 0:
            mass_diff = event.sist_mass - 13000.0 * math.sqrt(event.xi_arm1_1 * event.xi_arm2_1)
            mm_dy.Fill(mass_diff, w_dy)
        ra_dy.Fill(event.sist_rap, w_dy)
        tau_dy.Fill(event.tau_pt, w_dy)
        met_dy.Fill(event.mu_pt, w_dy)

    w_ttjets = 0.15
    n_ttjets = 0.0
    for event in tree_ttjets:
        if event.sist_mass < 0:
            continue
        w_ttjets = event.weight
        n_ttjets += w_ttjets
        aco_ttjets.Fill(event.acop, w_ttjets)
        m_ttjets.Fill(event.sist_mass, w_ttjets)
        if event.xi_arm1_1 > 0 and event.xi_arm2_1 > 0:
            rap_match = event.sist_rap - 0.5 * math.log(event.xi_arm1_1 / event.xi_arm2_1)
            r_ttjets.Fill(rap_match, w_ttjets)
        pt_ttjets.Fill(event.sist_pt, w_ttjets)
        if event.xi_arm1_1 >= 0 and event.xi_arm2_1 >= 0:
            mass_diff = event.sist_mass - 13000.0 * math.sqrt(event.xi_arm1_1 * event.xi_arm2_1)
            mm_ttjets.Fill(mass_diff, w_ttjets)
        ra_ttjets.Fill(event.sist_rap, w_ttjets)
        tau_ttjets.Fill(event.tau_pt, w_ttjets)
        met_ttjets.Fill(event.mu_pt, w_ttjets)

    w_sinal = 0.0
    for event in tree_sinal:
        if event.sist_mass < 0:
            continue
        w_sinal = event.weight * 5000.0
        m_sinal.Fill(event.sist_mass, w_sinal)
        aco_sinal.Fill(event.acop, w_sinal)
        if event.xi_arm1_1 > 0 and event.xi_arm2_1 > 0:
            rap_match = event.sist_rap - 0.5 * math.log(event.xi_arm1_1 / event.xi_arm2_1)
            r_sinal.Fill(rap_match, w_sinal)
        pt_sinal.Fill(event.sist_pt, w_sinal)
        if event.xi_arm1_1 >= 0 and event.xi_arm2_1 >= 0:
            mass_diff = event.sist_mass - 13000.0 * math.sqrt(event.xi_arm1_1 * event.xi_arm2_1)
            mm_sinal.Fill(mass_diff, w_sinal)
        ra_sinal.Fill(event.sist_rap, w_sinal)
        tau_sinal.Fill(event.tau_pt, w_sinal)
        met_sinal.Fill(event.mu_pt, w_sinal)

    n_data = 0.0
    for event in tree_data:
        if event.sist_mass < 0:
            continue
        weight = getattr(event, "weight", 1.0)
        n_data += weight
        aco_data.Fill(event.acop, weight)
        m_data.Fill(event.sist_mass, weight)
        if event.xi_arm1_1 > 0 and event.xi_arm2_1 > 0:
            rap_match = event.sist_rap - 0.5 * math.log(event.xi_arm1_1 / event.xi_arm2_1)
            r_data.Fill(rap_match, weight)
        pt_data.Fill(event.sist_pt, weight)
        if event.xi_arm1_1 >= 0 and event.xi_arm2_1 >= 0:
            mass_diff = event.sist_mass - 13000.0 * math.sqrt(event.xi_arm1_1 * event.xi_arm2_1)
            mm_data.Fill(mass_diff, weight)
        ra_data.Fill(event.sist_rap, weight)
        tau_data.Fill(event.tau_pt, weight)
        met_data.Fill(event.mu_pt, weight)

    for hist in [
        aco_qcd,
        r_qcd,
        pt_qcd,
        mm_qcd,
        ra_qcd,
        tau_qcd,
        met_qcd,
    ]:
        hist.SetFillColor(ROOT.kRed)
        hist.SetLineWidth(0)

    for hist in [
        aco_dy,
        m_dy,
        r_dy,
        pt_dy,
        mm_dy,
        ra_dy,
        tau_dy,
        met_dy,
    ]:
        hist.SetFillColor(ROOT.kYellow)
        hist.SetLineWidth(0)

    for hist in [
        aco_ttjets,
        m_ttjets,
        r_ttjets,
        pt_ttjets,
        mm_ttjets,
        ra_ttjets,
        tau_ttjets,
        met_ttjets,
    ]:
        hist.SetFillColor(ROOT.kGreen)
        hist.SetLineWidth(0)

    for hist in [
        aco_sinal,
        m_sinal,
        r_sinal,
        pt_sinal,
        mm_sinal,
        ra_sinal,
        tau_sinal,
        met_sinal,
    ]:
        hist.SetLineColor(ROOT.kBlack)
        hist.SetFillStyle(0)

    for hist in [
        aco_data,
        m_data,
        r_data,
        pt_data,
        mm_data,
        ra_data,
        tau_data,
        met_data,
    ]:
        hist.SetMarkerStyle(20)
        hist.SetMarkerSize(1.2)
        hist.SetLineColor(ROOT.kBlack)
        hist.SetLineWidth(2)

    sum_aco = aco_dy.Clone("sum_aco_ttjets")
    sum_aco.Add(aco_qcd)
    sum_aco.Add(aco_ttjets)

    sum_m = m_ttjets.Clone("sum_m_ttjets")
    sum_m.Add(m_qcd)
    sum_m.Add(m_dy)

    sum_r = r_dy.Clone("sum_r_ttjets")
    sum_r.Add(r_qcd)
    sum_r.Add(r_ttjets)

    sum_pt = pt_dy.Clone("sum_pt_ttjets")
    sum_pt.Add(pt_qcd)
    sum_pt.Add(pt_ttjets)

    sum_mm = mm_dy.Clone("sum_mm_ttjets")
    sum_mm.Add(mm_qcd)
    sum_mm.Add(mm_ttjets)

    sum_ra = ra_dy.Clone("sum_ra_ttjets")
    sum_ra.Add(ra_qcd)
    sum_ra.Add(ra_ttjets)

    sum_tau = tau_dy.Clone("sum_tau_ttjets")
    sum_tau.Add(tau_qcd)
    sum_tau.Add(tau_ttjets)

    sum_met = met_dy.Clone("sum_met_ttjets")
    sum_met.Add(met_qcd)
    sum_met.Add(met_ttjets)

    aco = ROOT.THStack("aco", "")
    aco.Add(aco_ttjets)
    aco.Add(aco_qcd)
    aco.Add(aco_dy)

    m_stack = ROOT.THStack("m", "")
    m_stack.Add(m_ttjets)
    m_stack.Add(m_qcd)
    m_stack.Add(m_dy)

    r_stack = ROOT.THStack("r", "")
    r_stack.Add(r_ttjets)
    r_stack.Add(r_qcd)
    r_stack.Add(r_dy)

    pt_stack = ROOT.THStack("pt", "")
    pt_stack.Add(pt_ttjets)
    pt_stack.Add(pt_qcd)
    pt_stack.Add(pt_dy)

    mm_stack = ROOT.THStack("mm", "")
    mm_stack.Add(mm_ttjets)
    mm_stack.Add(mm_qcd)
    mm_stack.Add(mm_dy)

    ra_stack = ROOT.THStack("ra", "")
    ra_stack.Add(ra_ttjets)
    ra_stack.Add(ra_qcd)
    ra_stack.Add(ra_dy)

    tau_stack = ROOT.THStack("tau", "")
    tau_stack.Add(tau_ttjets)
    tau_stack.Add(tau_qcd)
    tau_stack.Add(tau_dy)

    met_stack = ROOT.THStack("met", "")
    met_stack.Add(met_ttjets)
    met_stack.Add(met_qcd)
    met_stack.Add(met_dy)

    l_aco = create_legend()
    l_aco.AddEntry(aco_ttjets, "t \\bar{t}", "f")
    l_aco.AddEntry(aco_dy, "Drell Yan", "f")
    l_aco.AddEntry(aco_qcd, "QCD (Data driven)", "f")
    l_aco.AddEntry(aco_data, "Data", "lep")
    l_aco.AddEntry(aco_sinal, "Signal (x5000)", "l")

    l_m = create_legend()
    l_m.AddEntry(m_ttjets, "t \\bar{t}", "f")
    l_m.AddEntry(m_dy, "Drell Yan", "f")
    l_m.AddEntry(m_qcd, "QCD (Data driven)", "f")
    l_m.AddEntry(m_data, "Data", "lep")
    l_m.AddEntry(m_sinal, "Signal (x 5000)", "l")

    l_r = create_legend()
    l_r.AddEntry(r_ttjets, "t \\bar{t}", "f")
    l_r.AddEntry(r_dy, "Drell Yan", "f")
    l_r.AddEntry(r_qcd, "QCD (Data driven)", "f")
    l_r.AddEntry(r_data, "Data", "lep")
    l_r.AddEntry(r_sinal, "Signal (x 5000)", "l")

    l_pt = create_legend()
    l_pt.AddEntry(pt_ttjets, "t \\bar{t}", "f")
    l_pt.AddEntry(pt_dy, "Drell Yan", "f")
    l_pt.AddEntry(pt_qcd, "QCD (Data driven)", "f")
    l_pt.AddEntry(pt_data, "Data", "lep")
    l_pt.AddEntry(pt_sinal, "Signal (x 5000)", "l")

    l_mm = create_legend()
    l_mm.AddEntry(mm_ttjets, "t \\bar{t}", "f")
    l_mm.AddEntry(mm_dy, "Drell Yan", "f")
    l_mm.AddEntry(mm_qcd, "QCD (Data driven)", "f")
    l_mm.AddEntry(mm_data, "Data", "lep")
    l_mm.AddEntry(mm_sinal, "Signal (x 5000)", "l")

    l_ra = create_legend()
    l_ra.AddEntry(ra_ttjets, "t \\bar{t}", "f")
    l_ra.AddEntry(ra_dy, "Drell Yan", "f")
    l_ra.AddEntry(ra_qcd, "QCD (Data driven)", "f")
    l_ra.AddEntry(ra_data, "Data", "lep")
    l_ra.AddEntry(ra_sinal, "Signal (x 5000)", "l")

    l_tau = create_legend()
    l_tau.AddEntry(tau_ttjets, "t \\bar{t}", "f")
    l_tau.AddEntry(tau_dy, "Drell Yan", "f")
    l_tau.AddEntry(tau_qcd, "QCD (Data driven)", "f")
    l_tau.AddEntry(tau_data, "Data", "lep")
    l_tau.AddEntry(tau_sinal, "Signal (x 5000)", "l")

    l_met = create_legend()
    l_met.AddEntry(met_ttjets, "t \\bar{t}", "f")
    l_met.AddEntry(met_dy, "Drell Yan", "f")
    l_met.AddEntry(met_qcd, "QCD (Data driven)", "f")
    l_met.AddEntry(met_data, "Data", "lep")
    l_met.AddEntry(met_sinal, "Signal (x 5000)", "l")

    output_root = ROOT.TFile("DY_CR_e_mu_UL_2018_shapes.root", "RECREATE")
    aco_dy.Write()
    aco_ttjets.Write()
    output_root.Close()

    output_dir = Path(__file__).resolve().parent / "plots_py"
    os.makedirs(output_dir, exist_ok=True)

    draw_and_save(
        "c1",
        aco,
        aco_sinal,
        l_aco,
        "Acoplanarity of the central system",
        "Events",
        output_dir / "aco.png",
        y_range=auto_y_range(aco, aco_sinal, data_hist=aco_data, ymin=0.0),
        data_hist=aco_data,
    )

    draw_and_save(
        "c2",
        m_stack,
        m_sinal,
        l_m,
        "Invariant mass of the central system [GeV]",
        "Events",
        output_dir / "mass.png",
        y_range=auto_y_range(m_stack, m_sinal, data_hist=m_data, ymin=0.0),
        data_hist=m_data,
    )

    draw_and_save(
        "c3",
        r_stack,
        r_sinal,
        l_r,
        "Rapidity matching",
        "Events",
        output_dir / "rapidity_matching.png",
        y_range=auto_y_range(r_stack, r_sinal, data_hist=r_data, ymin=0.0),
        data_hist=r_data,
    )

    draw_and_save(
        "c4",
        pt_stack,
        pt_sinal,
        l_pt,
        "Transverse momentum of the central system [GeV]",
        "Events",
        output_dir / "pt.png",
        y_range=auto_y_range(pt_stack, pt_sinal, data_hist=pt_data, ymin=0.0),
        data_hist=pt_data,
    )

    draw_and_save(
        "c5",
        mm_stack,
        mm_sinal,
        l_mm,
        "Mass difference (CMS-PPS) [GeV]",
        "Events",
        output_dir / "mass_difference.png",
        y_range=auto_y_range(mm_stack, mm_sinal, data_hist=mm_data, ymin=0.0),
        data_hist=mm_data,
    )

    draw_and_save(
        "c6",
        ra_stack,
        ra_sinal,
        l_ra,
        "Rapidity of the central system",
        "Events",
        output_dir / "rapidity.png",
        y_range=auto_y_range(ra_stack, ra_sinal, data_hist=ra_data, ymin=0.0),
        data_hist=ra_data,
    )

    draw_and_save(
        "c7",
        tau_stack,
        tau_sinal,
        l_tau,
        "Transverse momentum of the hadronic tau [GeV]",
        "Events",
        output_dir / "tau_pt.png",
        y_range=auto_y_range(tau_stack, tau_sinal, data_hist=tau_data, ymin=0.0),
        data_hist=tau_data,
    )

    draw_and_save(
        "c8",
        met_stack,
        met_sinal,
        l_met,
        "Muon transverse momentum [GeV]",
        "Events",
        output_dir / "muon_pt.png",
        y_range=auto_y_range(met_stack, met_sinal, data_hist=met_data, ymin=0.0),
        data_hist=met_data,
    )

    dy_file.Close()
    ttjets_file.Close()
    qcd_file.Close()
    sinal_file.Close()
    data_file.Close()


if __name__ == "__main__":
    main()
