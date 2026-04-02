#include "TFile.h"
#include "TTree.h"
#include "TH1F.h"
#include "THStack.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TLatex.h"
#include "TStyle.h"
#include "TString.h"
#include "TPad.h"
#include "TLine.h"
#include <iostream>
#include <cmath>

// MC normalization: Scale = (L × sigma) / Sum(generator_weights)
// DY weight branch: generator_weight × muon_SFs × 0.245 → needs DY_SCALE
// ttbar weight branch: 0.15 × muon_SFs × 0.245 → already normalized
const double DY_SCALE = 1.004e-4;       // L×sigma/Sum_w for DY
const double TTBAR_SCALE = 1.0;         // ttbar already normalized via 0.15 factor

void MuTauPlots_leptons()
{
    gStyle->SetOptStat(0);

    // Open files - use pileup_protons for MC (after mixing)
    TFile *f_data   = TFile::Open("/eos/user/m/mblancco/samples_2018_mutau/Data_2018_UL_MuTau_nano_merged_proton_vars.root");
    TFile *f_qcd    = TFile::Open("/eos/user/m/mblancco/samples_2018_mutau/QCD_2018_UL_MuTau_nano_merged_proton_vars_new.root");
    TFile *f_dy     = TFile::Open("/eos/user/m/mblancco/samples_2018_mutau/DY_2018_UL_MuTau_nano_merged_pileup_protons.root");
    TFile *f_ttbar  = TFile::Open("/eos/user/m/mblancco/samples_2018_mutau/ttjets_2018_UL_MuTau_nano_merged_pileup_protons.root");
    TFile *f_signal = TFile::Open("/eos/user/m/mblancco/tau_analysis/MuTau_channel/MuTau_sinal_SM_2018_july.root");

    TTree *t_data   = (TTree*)f_data->Get("tree");
    TTree *t_qcd    = (TTree*)f_qcd->Get("tree");
    TTree *t_dy     = (TTree*)f_dy->Get("tree");
    TTree *t_ttbar  = (TTree*)f_ttbar->Get("tree");
    TTree *t_signal = (TTree*)f_signal->Get("tree");

    // Define histograms for each variable
    struct PlotDef {
        const char* var;
        const char* name;
        const char* title;
        int nbins;
        double xmin, xmax;
    };

    // Note: MC/signal files use "mu_" prefix, Data/QCD use "muon_" prefix
    // We need to handle this difference

    PlotDef plots[] = {
        // Muon variables (use mu_ for MC compatibility)
        {"mu_pt", "muon_pt", "Muon p_{T};p_{T}^{#mu} [GeV];Events", 20, 35, 300},
        {"mu_eta", "muon_eta", "Muon #eta;#eta^{#mu};Events", 20, -2.4, 2.4},
        {"mu_mass", "muon_mass", "Muon mass;m_{#mu} [GeV];Events", 20, 0, 0.2},
        // Tau variables
        {"tau_pt", "tau_pt", "Tau p_{T};p_{T}^{#tau} [GeV];Events", 20, 100, 500},
        {"tau_eta", "tau_eta", "Tau #eta;#eta^{#tau};Events", 20, -2.4, 2.4},
        {"tau_mass", "tau_mass", "Tau mass;m_{#tau} [GeV];Events", 20, 0, 3},
    };
    int nplots = sizeof(plots) / sizeof(plots[0]);

    for (int p = 0; p < nplots; p++) {
        TString var = plots[p].var;
        TString name = plots[p].name;
        TString title = plots[p].title;
        int nbins = plots[p].nbins;
        double xmin = plots[p].xmin;
        double xmax = plots[p].xmax;

        // Create canvas with two pads (main plot + ratio)
        TCanvas *c = new TCanvas("c_" + name, "", 800, 900);

        // Upper pad for main plot
        TPad *pad1 = new TPad("pad1_" + name, "", 0, 0.3, 1, 1);
        pad1->SetLeftMargin(0.15);
        pad1->SetRightMargin(0.05);
        pad1->SetTopMargin(0.10);
        pad1->SetBottomMargin(0.02);
        pad1->Draw();

        // Lower pad for ratio
        TPad *pad2 = new TPad("pad2_" + name, "", 0, 0, 1, 0.3);
        pad2->SetLeftMargin(0.15);
        pad2->SetRightMargin(0.05);
        pad2->SetTopMargin(0.02);
        pad2->SetBottomMargin(0.35);
        pad2->Draw();

        // Proton requirements (matching comparison "after" view)
        // Data/QCD: require protons on both arms (real protons)
        TString proton_cut_data = "Sum$(proton_multi_arm==0)>0 && Sum$(proton_multi_arm==1)>0";
        TString cut_base = TString::Format("sist_mass > 0 && %s", proton_cut_data.Data());

        // MC: require xi values from pileup protons, use weight branch
        TString proton_cut_mc = "xi_arm1_1 >= 0 && xi_arm2_1 >= 0";
        TString cut_dy = TString::Format("(sist_mass > 0 && %s) * weight", proton_cut_mc.Data());
        TString cut_ttbar = TString::Format("(sist_mass > 0 && %s) * weight", proton_cut_mc.Data());

        // Data/QCD use "muon_" prefix, MC/Signal use "mu_" prefix
        TString var_data = var;
        if (var.BeginsWith("mu_")) {
            var_data = "muon_" + var(3, var.Length() - 3);  // mu_pt -> muon_pt
        }

        // Create histograms
        TH1F *h_data   = new TH1F("h_data_"   + name, title, nbins, xmin, xmax);
        TH1F *h_qcd    = new TH1F("h_qcd_"    + name, title, nbins, xmin, xmax);
        TH1F *h_dy     = new TH1F("h_dy_"     + name, title, nbins, xmin, xmax);
        TH1F *h_ttbar  = new TH1F("h_ttbar_"  + name, title, nbins, xmin, xmax);
        TH1F *h_signal = new TH1F("h_signal_" + name, title, nbins, xmin, xmax);

        // Signal: use weight branch (already contains proper normalization), with proton cut
        TString cut_signal = TString::Format("(sist_mass > 0 && %s) * weight", proton_cut_mc.Data());

        // Fill histograms - use var_data for Data/QCD, var for MC/Signal
        t_data->Draw(var_data + " >> h_data_" + name, cut_base, "goff");
        t_qcd->Draw(var_data + " >> h_qcd_" + name, cut_base, "goff");
        t_dy->Draw(var + " >> h_dy_" + name, cut_dy, "goff");
        t_ttbar->Draw(var + " >> h_ttbar_" + name, cut_ttbar, "goff");
        t_signal->Draw(var + " >> h_signal_" + name, cut_signal, "goff");

        // Scale MC: DY needs L×sigma/Sum_w, ttbar already normalized
        h_dy->Scale(DY_SCALE);
        h_ttbar->Scale(TTBAR_SCALE);
        h_signal->Scale(5000);

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

        // Create stack (bottom to top: ttbar, QCD, DY)
        THStack *stack = new THStack("stack_" + name, title);
        stack->Add(h_ttbar);
        stack->Add(h_qcd);
        stack->Add(h_dy);

        // MC sum for uncertainty band and ratio
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

        // Auto-scale Y axis
        double maxY = h_data->GetMaximum();
        if (h_signal->GetMaximum() > maxY) maxY = h_signal->GetMaximum();
        if (stack->GetMaximum() > maxY) maxY = stack->GetMaximum();

        // Draw main plot
        pad1->cd();
        stack->Draw("hist");
        stack->GetYaxis()->SetTitle("Events");
        stack->GetYaxis()->SetTitleSize(0.055);
        stack->GetYaxis()->SetLabelSize(0.05);
        stack->GetYaxis()->SetTitleOffset(1.0);
        stack->GetXaxis()->SetLabelSize(0);  // Hide x-axis labels on main plot
        stack->GetYaxis()->SetNdivisions(505);
        stack->SetMinimum(0.1);
        stack->SetMaximum(maxY * 1.4);

        h_signal->Draw("hist same");
        h_data->Draw("e same");
        h_mcsum->Draw("e2 same");

        // Legend
        TLegend *leg = new TLegend(0.65, 0.55, 0.92, 0.88);
        leg->SetBorderSize(0);
        leg->SetFillStyle(0);
        leg->AddEntry(h_data, "Data", "lep");
        leg->AddEntry(h_signal, "Signal", "l");
        leg->AddEntry(h_dy, "Drell-Yan", "f");
        leg->AddEntry(h_qcd, "QCD", "f");
        leg->AddEntry(h_ttbar, "t#bar{t}", "f");
        leg->Draw();

        // Labels
        TLatex latex;
        latex.SetNDC();
        latex.SetTextSize(0.05);
        latex.SetTextFont(62);
        latex.DrawLatex(0.18, 0.92, "CMS-TOTEM Preliminary");
        latex.SetTextAlign(31);
        latex.SetTextSize(0.04);
        latex.DrawLatex(0.92, 0.92, "54.9 fb^{-1} (13 TeV)");

        // Draw ratio plot
        pad2->cd();

        // Create ratio histogram
        TH1F *h_ratio = (TH1F*)h_data->Clone("h_ratio_" + name);
        h_ratio->Divide(h_mcsum);

        // Style ratio
        h_ratio->SetMarkerStyle(20);
        h_ratio->SetMarkerSize(0.8);
        h_ratio->SetLineColor(kBlack);
        h_ratio->SetTitle("");
        h_ratio->GetYaxis()->SetTitle("Data/Simulation");
        h_ratio->GetYaxis()->SetTitleSize(0.12);
        h_ratio->GetYaxis()->SetTitleOffset(0.45);
        h_ratio->GetYaxis()->SetLabelSize(0.10);
        h_ratio->GetYaxis()->SetNdivisions(505);
        h_ratio->GetYaxis()->SetRangeUser(0, 2);
        h_ratio->GetXaxis()->SetTitle(title.Tokenize(";")->At(1)->GetName());
        h_ratio->GetXaxis()->SetTitleSize(0.14);
        h_ratio->GetXaxis()->SetTitleOffset(0.9);
        h_ratio->GetXaxis()->SetLabelSize(0.10);

        h_ratio->Draw("e");

        // Draw line at ratio = 1
        TLine *line = new TLine(xmin, 1, xmax, 1);
        line->SetLineColor(kRed);
        line->SetLineStyle(2);
        line->SetLineWidth(2);
        line->Draw();

        // Print event counts
        std::cout << "===== " << name << " =====" << std::endl;
        std::cout << "Data:   " << h_data->GetSumOfWeights() << std::endl;
        std::cout << "QCD:    " << h_qcd->GetSumOfWeights() << std::endl;
        std::cout << "DY:     " << h_dy->GetSumOfWeights() << std::endl;
        std::cout << "ttbar:  " << h_ttbar->GetSumOfWeights() << std::endl;
        std::cout << "Signal: " << h_signal->GetSumOfWeights() << std::endl;
        std::cout << "MC sum: " << h_mcsum->GetSumOfWeights() << std::endl;
        std::cout << "========================" << std::endl;

        c->Update();
        c->SaveAs("plot_" + name + ".png");
    }

    std::cout << "lepton plots saved" << std::endl;
}
