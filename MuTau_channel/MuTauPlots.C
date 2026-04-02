#include "TFile.h"
#include "TTree.h"
#include "TH1F.h"
#include "THStack.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TLatex.h"
#include "TStyle.h"
#include "TString.h"
#include <iostream>

// Configuration
const double PROTON_ACCEPTANCE = 0.245;
const double DY_WEIGHT = 1.81 * PROTON_ACCEPTANCE;
const double TTBAR_WEIGHT = 1.0 * PROTON_ACCEPTANCE;

void MuTauPlots()
{
    gStyle->SetOptStat(0);

    // Open files
    TFile *f_data   = TFile::Open("/eos/user/m/mblancco/samples_2018_mutau/Data_2018_UL_MuTau_nano_merged_pileup_protons.root");
    TFile *f_qcd    = TFile::Open("/eos/user/m/mblancco/samples_2018_mutau/QCD_2018_UL_MuTau_nano_merged_pileup_protons.root");
    TFile *f_dy     = TFile::Open("/eos/user/m/mblancco/samples_2018_mutau/DY_2018_UL_MuTau_nano_merged_pileup_protons.root");
    TFile *f_ttbar  = TFile::Open("/eos/user/m/mblancco/samples_2018_mutau/ttjets_2018_UL_MuTau_nano_merged_pileup_protons.root");
    TFile *f_signal = TFile::Open("MuTau_sinal_SM_2018_july.root");

    TTree *t_data   = (TTree*)f_data->Get("tree");
    TTree *t_qcd    = (TTree*)f_qcd->Get("tree");
    TTree *t_dy     = (TTree*)f_dy->Get("tree");
    TTree *t_ttbar  = (TTree*)f_ttbar->Get("tree");
    TTree *t_signal = (TTree*)f_signal->Get("tree");

    // Cut strings (no additional cuts since files are already processed)
    TString cut_data   = "sist_mass > 0";
    TString cut_qcd    = "sist_mass > 0";
    TString cut_mc     = TString::Format("(sist_mass > 0) * %f", PROTON_ACCEPTANCE);
    TString cut_signal = "sist_mass > 0";

    // Define histograms for each variable
    struct PlotDef {
        const char* var;
        const char* name;
        const char* title;
        int nbins;
        double xmin, xmax;
    };

    PlotDef plots[] = {
        {"sist_mass", "mass", "Invariant Mass;m_{#mu#tau} [GeV];Events", 10, 0, 1200},
        {"acop", "acop", "Acoplanarity;1 - |#Delta#phi|/#pi;Events", 10, 0, 1},
        {"sist_pt", "pt", "System p_{T};p_{T} [GeV];Events", 10, 0, 600},
        {"sist_rap", "rap", "Rapidity;y;Events", 10, -2.2, 2.2},
        {"sist_rap - 0.5*log(xi_arm1_1/xi_arm2_1)", "rap_match", "Rapidity Matching;y_{#mu#tau} - y_{pp};Events", 10, -2.2, 2.2},
        {"sist_mass - 13000*sqrt(xi_arm1_1*xi_arm2_1)", "mass_diff", "Mass Difference;m_{#mu#tau} - m_{pp} [GeV];Events", 10, -1500, 200},
        {"tau_pt", "tau_pt", "Tau p_{T};p_{T}^{#tau} [GeV];Events", 20, 100, 500},
        {"met_pt", "met", "Missing E_{T};MET [GeV];Events", 20, 0, 300},
    };
    int nplots = sizeof(plots) / sizeof(plots[0]);

    // Additional cuts for derived variables (need valid xi values)
    TString cut_rap_match = "sist_mass > 0 && xi_arm1_1 > 0 && xi_arm2_1 > 0";
    TString cut_mass_diff = "sist_mass > 0 && xi_arm1_1 >= 0 && xi_arm2_1 >= 0";

    for (int p = 0; p < nplots; p++) {
        TString var = plots[p].var;
        TString name = plots[p].name;
        TString title = plots[p].title;
        int nbins = plots[p].nbins;
        double xmin = plots[p].xmin;
        double xmax = plots[p].xmax;

        // Create histograms
        TH1F *h_data   = new TH1F("h_data_"   + name, title, nbins, xmin, xmax);
        TH1F *h_qcd    = new TH1F("h_qcd_"    + name, title, nbins, xmin, xmax);
        TH1F *h_dy     = new TH1F("h_dy_"     + name, title, nbins, xmin, xmax);
        TH1F *h_ttbar  = new TH1F("h_ttbar_"  + name, title, nbins, xmin, xmax);
        TH1F *h_signal = new TH1F("h_signal_" + name, title, nbins, xmin, xmax);

        // Set cuts based on variable type
        TString data_cut = cut_data;
        TString qcd_cut = cut_qcd;
        TString mc_cut = cut_mc;
        TString sig_cut = cut_signal;

        if (name == "rap_match") {
            data_cut = cut_rap_match;
            qcd_cut = cut_rap_match;
            mc_cut = TString::Format("(%s) * %f", cut_rap_match.Data(), PROTON_ACCEPTANCE);
            sig_cut = cut_rap_match;
        } else if (name == "mass_diff") {
            data_cut = cut_mass_diff;
            qcd_cut = cut_mass_diff;
            mc_cut = TString::Format("(%s) * %f", cut_mass_diff.Data(), PROTON_ACCEPTANCE);
            sig_cut = cut_mass_diff;
        }

        // Fill histograms using TTree::Draw
        t_data->Draw(var + " >> h_data_" + name, data_cut, "goff");
        t_qcd->Draw(var + " >> h_qcd_" + name, qcd_cut, "goff");
        t_dy->Draw(var + " >> h_dy_" + name, mc_cut, "goff");
        t_ttbar->Draw(var + " >> h_ttbar_" + name, mc_cut, "goff");
        t_signal->Draw(var + " >> h_signal_" + name, sig_cut, "goff");

        // Scale MC for cross-section
        h_dy->Scale(1.81);
        h_ttbar->Scale(1.0);

        // Scale signal: normalize to expected yield (cross-section × lumi / N_generated)
        // Signal is very small - scale down for visibility comparison
        h_signal->Scale(0.25);

        // Style
        h_data->SetMarkerStyle(20);
        h_data->SetMarkerSize(0.8);
        h_data->SetLineColor(kBlack);

        h_qcd->SetFillColor(kRed);
        h_qcd->SetLineWidth(0);

        h_dy->SetFillColor(kYellow);
        h_dy->SetLineWidth(0);

        h_ttbar->SetFillColor(kGreen);
        h_ttbar->SetLineWidth(0);

        h_signal->SetLineColor(kBlack);
        h_signal->SetLineWidth(3);
        h_signal->SetFillStyle(0);

        // Create stack (order: bottom to top)
        THStack *stack = new THStack("stack_" + name, title);
        stack->Add(h_qcd);
        stack->Add(h_ttbar);
        stack->Add(h_dy);

        // Create MC sum for uncertainty band
        TH1F *h_mcsum = (TH1F*)h_qcd->Clone("h_mcsum_" + name);
        h_mcsum->Add(h_dy);
        h_mcsum->Add(h_ttbar);
        h_mcsum->SetFillColor(kGray + 1);
        h_mcsum->SetFillStyle(3354);
        h_mcsum->SetMarkerSize(0);
        for (int i = 1; i <= h_mcsum->GetNbinsX(); i++) {
            double content = h_mcsum->GetBinContent(i);
            h_mcsum->SetBinError(i, sqrt(content));
        }

        // Draw
        TCanvas *c = new TCanvas("c_" + name, "", 700, 800);
        c->SetLeftMargin(0.15);
        c->SetRightMargin(0.05);
        c->SetTopMargin(0.10);
        c->SetBottomMargin(0.13);

        stack->Draw("hist");
        stack->GetXaxis()->SetTitle(title.Tokenize(";")->At(1)->GetName());
        stack->GetYaxis()->SetTitle("Events");
        stack->GetXaxis()->SetTitleSize(0.03);
        stack->GetYaxis()->SetTitleSize(0.03);
        stack->GetXaxis()->SetLabelSize(0.03);
        stack->GetYaxis()->SetLabelSize(0.03);
        stack->GetXaxis()->SetNdivisions(505);
        stack->GetYaxis()->SetNdivisions(505);
        // Auto-scale Y axis based on maximum content
        double maxY = h_data->GetMaximum();
        if (h_signal->GetMaximum() > maxY) maxY = h_signal->GetMaximum();
        if (stack->GetMaximum() > maxY) maxY = stack->GetMaximum();
        stack->SetMinimum(0.1);
        stack->SetMaximum(maxY * 1.5);

        h_signal->Draw("hist same");
        h_data->Draw("e same");
        h_mcsum->Draw("e2 same");

        // Legend
        TLegend *leg = new TLegend(0.6, 0.65, 0.92, 0.88);
        leg->SetBorderSize(0);
        leg->SetFillStyle(0);
        leg->AddEntry(h_data, "Data", "lep");
        leg->AddEntry(h_signal, "Signal", "l");
        leg->AddEntry(h_dy, "Drell-Yan", "f");
        leg->AddEntry(h_ttbar, "t#bar{t}", "f");
        leg->AddEntry(h_qcd, "QCD", "f");
        leg->Draw();

        // Labels
        TLatex latex;
        latex.SetNDC();
        latex.SetTextAlign(31);
        latex.SetTextSize(0.04);
        latex.SetTextFont(62);
        latex.DrawLatex(0.5, 0.92, "CMS-TOTEM Preliminary");
        latex.DrawLatex(0.92, 0.92, "54.9 fb^{-1} (13 TeV)");

        c->Update();
        c->SaveAs("plot_" + name + ".png");

        // Print stats for mass plot
        if (name == "mass") {
            std::cout << "===== Event counts =====" << std::endl;
            std::cout << "Data:   " << h_data->GetSumOfWeights() << std::endl;
            std::cout << "QCD:    " << h_qcd->GetSumOfWeights() << std::endl;
            std::cout << "DY:     " << h_dy->GetSumOfWeights() << std::endl;
            std::cout << "ttbar:  " << h_ttbar->GetSumOfWeights() << std::endl;
            std::cout << "Signal: " << h_signal->GetSumOfWeights() << std::endl;
            std::cout << "MC sum: " << h_mcsum->GetSumOfWeights() << std::endl;
            std::cout << "========================" << std::endl;
        }
    }

    std::cout << "All plots saved!" << std::endl;
}