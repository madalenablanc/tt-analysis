#include "TFile.h"
#include "TH1D.h"
#include "THStack.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TLatex.h"
#include "TStyle.h"
#include "TPad.h"
#include "TLine.h"
#include <iostream>
#include <cmath>

void plot_bdt_real_norm()
{
    gStyle->SetOptStat(0);

    TFile *f = TFile::Open("MuTau_shapes.root");
    if (!f || f->IsZombie()) {
        std::cerr << "Cannot open MuTau_shapes.root\n";
        return;
    }

    TH1D *h_data   = (TH1D*)f->Get("bdt_data");
    TH1D *h_qcd    = (TH1D*)f->Get("bdt_qcd");
    TH1D *h_dy     = (TH1D*)f->Get("bdt_dy");
    TH1D *h_ttjets = (TH1D*)f->Get("bdt_ttjets");
    TH1D *h_sinal  = (TH1D*)f->Get("bdt_sinal");

    if (!h_data || !h_qcd || !h_dy || !h_ttjets || !h_sinal) {
        std::cerr << "Missing histograms in MuTau_shapes.root\n";
        return;
    }

    // Clone to avoid modifying the file
    h_data   = (TH1D*)h_data->Clone("bdt_data_c");
    h_qcd    = (TH1D*)h_qcd->Clone("bdt_qcd_c");
    h_dy     = (TH1D*)h_dy->Clone("bdt_dy_c");
    h_ttjets = (TH1D*)h_ttjets->Clone("bdt_ttjets_c");
    h_sinal  = (TH1D*)h_sinal->Clone("bdt_sinal_c");

    // Print yields
    std::cout << "=== BDT histogram yields (real normalization) ===" << std::endl;
    std::cout << "Data:    " << h_data->Integral()   << std::endl;
    std::cout << "QCD:     " << h_qcd->Integral()    << std::endl;
    std::cout << "DY:      " << h_dy->Integral()     << std::endl;
    std::cout << "ttbar:   " << h_ttjets->Integral() << std::endl;
    std::cout << "Signal:  " << h_sinal->Integral()  << " (sigma_SM = 4.7 fb)" << std::endl;
    double bg_total = h_qcd->Integral() + h_dy->Integral() + h_ttjets->Integral();
    std::cout << "Total BG:" << bg_total << std::endl;
    std::cout << "S/B:     " << h_sinal->Integral() / bg_total << std::endl;

    // Style
    h_data->SetMarkerStyle(20);
    h_data->SetMarkerSize(0.8);
    h_data->SetLineColor(kBlack);

    h_qcd->SetFillColor(kRed);
    h_qcd->SetLineWidth(0);
    h_dy->SetFillColor(kYellow);
    h_dy->SetLineWidth(0);
    h_ttjets->SetFillColor(kGreen);
    h_ttjets->SetLineWidth(0);

    // Signal at real normalization: solid red line
    h_sinal->SetLineColor(kRed+1);
    h_sinal->SetLineWidth(3);
    h_sinal->SetFillStyle(0);

    THStack *stack = new THStack("stack_bdt", "");
    stack->Add(h_ttjets);
    stack->Add(h_qcd);
    stack->Add(h_dy);

    TH1D *h_mcsum = (TH1D*)h_dy->Clone("bdt_mcsum");
    h_mcsum->Add(h_qcd);
    h_mcsum->Add(h_ttjets);
    h_mcsum->SetFillColor(kGray+1);
    h_mcsum->SetFillStyle(3354);
    h_mcsum->SetMarkerSize(0);

    // Canvas with ratio pad
    TCanvas *c = new TCanvas("c_bdt", "", 800, 900);
    TPad *pad1 = new TPad("pad1", "", 0, 0.3, 1, 1);
    pad1->SetLeftMargin(0.14); pad1->SetRightMargin(0.05);
    pad1->SetTopMargin(0.10);  pad1->SetBottomMargin(0.02);
    pad1->Draw();
    TPad *pad2 = new TPad("pad2", "", 0, 0, 1, 0.3);
    pad2->SetLeftMargin(0.14); pad2->SetRightMargin(0.05);
    pad2->SetTopMargin(0.02);  pad2->SetBottomMargin(0.35);
    pad2->Draw();

    pad1->cd();
    stack->Draw("hist");
    stack->GetYaxis()->SetTitle("Events");
    stack->GetYaxis()->SetTitleSize(0.055);
    stack->GetYaxis()->SetLabelSize(0.05);
    stack->GetYaxis()->SetTitleOffset(1.0);
    stack->GetXaxis()->SetLabelSize(0);
    stack->SetMinimum(0.001);
    double maxY = std::max({h_data->GetMaximum(), stack->GetMaximum()});
    stack->SetMaximum(maxY * 1.5);

    h_mcsum->Draw("e2 same");
    h_sinal->Draw("hist same");
    h_data->Draw("e same");

    TLegend *leg = new TLegend(0.55, 0.52, 0.92, 0.88);
    leg->SetBorderSize(0); leg->SetFillStyle(0);
    leg->AddEntry(h_data,   "Data",                "lep");
    leg->AddEntry(h_sinal,  "Signal (real norm.)", "l");
    leg->AddEntry(h_dy,     "Drell-Yan",           "f");
    leg->AddEntry(h_qcd,    "QCD",                 "f");
    leg->AddEntry(h_ttjets, "t#bar{t}",            "f");
    leg->Draw();

    TLatex lat;
    lat.SetNDC(); lat.SetTextFont(42);
    lat.SetTextAlign(31); lat.SetTextSize(0.04);
    lat.DrawLatex(0.92, 0.92, "54.9 fb^{-1} (13 TeV)");
    lat.SetTextAlign(11); lat.SetTextSize(0.048);
    lat.DrawLatex(0.17, 0.87, "BDT output (#mu#tau, 2018)");

    // Signal yield annotation
    TLatex ann;
    ann.SetNDC(); ann.SetTextFont(42); ann.SetTextSize(0.035);
    ann.DrawLatex(0.17, 0.81, Form("Signal yield: %.4f events", h_sinal->Integral()));

    // Ratio pad
    pad2->cd();
    TH1D *h_ratio = (TH1D*)h_data->Clone("ratio");
    h_ratio->Divide(h_mcsum);
    h_ratio->SetTitle("");
    h_ratio->GetYaxis()->SetTitle("Data / Bkg");
    h_ratio->GetYaxis()->SetTitleSize(0.12);
    h_ratio->GetYaxis()->SetTitleOffset(0.45);
    h_ratio->GetYaxis()->SetLabelSize(0.10);
    h_ratio->GetYaxis()->SetRangeUser(0, 2);
    h_ratio->GetYaxis()->SetNdivisions(505);
    h_ratio->GetXaxis()->SetTitle("BDT score");
    h_ratio->GetXaxis()->SetTitleSize(0.14);
    h_ratio->GetXaxis()->SetTitleOffset(0.9);
    h_ratio->GetXaxis()->SetLabelSize(0.10);

    TH1D *h_ratio_unc = (TH1D*)h_mcsum->Clone("ratio_unc");
    for (int i = 1; i <= h_ratio_unc->GetNbinsX(); i++) {
        double mc  = h_mcsum->GetBinContent(i);
        double err = h_mcsum->GetBinError(i);
        h_ratio_unc->SetBinContent(i, 1.0);
        h_ratio_unc->SetBinError(i, mc > 0 ? err/mc : 0.0);
    }
    h_ratio_unc->SetFillColor(kGray+1);
    h_ratio_unc->SetFillStyle(1001);
    h_ratio_unc->SetMarkerSize(0);
    h_ratio_unc->SetLineWidth(0);

    h_ratio->Draw("e");
    h_ratio_unc->Draw("e2 same");
    h_ratio->Draw("e same");
    TLine *line = new TLine(h_ratio->GetXaxis()->GetXmin(), 1,
                            h_ratio->GetXaxis()->GetXmax(), 1);
    line->SetLineColor(kRed); line->SetLineStyle(2); line->SetLineWidth(2);
    line->Draw();

    c->Update();
    c->SaveAs("bdt_signal_real_norm.png");
    std::cout << "Saved bdt_signal_real_norm.png\n";
}
