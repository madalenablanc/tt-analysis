// Updated version of the ETau plotting macro using proper double types and SetBranchAddress
#include <iostream>
#include <cmath>
#include "TApplication.h"
#include "TH1D.h"
#include "TCanvas.h"
#include "TFile.h"
#include "TTree.h"
#include "TLegend.h"
#include "TLatex.h"
#include "TStyle.h"
#include "TPaveText.h"
#include "THStack.h"

using namespace std;

int main() {
    TApplication app("app", nullptr, nullptr);

    TFile *file = TFile::Open("ETau_sinal_SM_july.root");
    TTree *tree = (TTree*)file->Get("tree");

    double sist_acop, sist_mass, sist_rap, sist_pt;
    double xi_arm1_1, xi_arm2_1;
    double tau_pt, e_pt;
    double weight_sm;

    tree->SetBranchAddress("sist_acop", &sist_acop);
    tree->SetBranchAddress("sist_mass", &sist_mass);
    tree->SetBranchAddress("sist_rap", &sist_rap);
    tree->SetBranchAddress("sist_pt", &sist_pt);
    tree->SetBranchAddress("xi_arm1_1", &xi_arm1_1);
    tree->SetBranchAddress("xi_arm2_1", &xi_arm2_1);
    tree->SetBranchAddress("tau_pt", &tau_pt);
    tree->SetBranchAddress("e_pt", &e_pt);
    tree->SetBranchAddress("weight_sm", &weight_sm);

    const int n_bins = 50;

    TH1D *h_aco = new TH1D("h_aco", "Acoplanarity;Acoplanarity;Events", n_bins, 0, 1);
    TH1D *h_mass = new TH1D("h_mass", "Invariant Mass;Mass [GeV];Events", n_bins, 0, 1500);
    TH1D *h_rapidity = new TH1D("h_rapidity", "Central System Rapidity;y;Events", n_bins, -2.5, 2.5);
    TH1D *h_pt = new TH1D("h_pt", "Central System pT;p_{T} [GeV];Events", n_bins, 0, 500);
    TH1D *h_mass_diff = new TH1D("h_mass_diff", "Mass Difference;M_{central} - M_{protons} [GeV];Events", n_bins, -1500, 500);
    TH1D *h_rap_match = new TH1D("h_rap_match", "Rapidity Matching;y_{central} - y_{protons};Events", n_bins, -2.5, 2.5);
    TH1D *h_tau_pt = new TH1D("h_tau_pt", "Tau pT;p_{T}^{#tau} [GeV];Events", n_bins, 0, 500);
    TH1D *h_e_pt = new TH1D("h_e_pt", "Electron pT;p_{T}^{e} [GeV];Events", n_bins, 0, 300);

    Long64_t nentries = tree->GetEntries();
    for (Long64_t i = 0; i < nentries; i++) {
        tree->GetEntry(i);

        double weight = weight_sm * 5000.0; // scale signal

        h_aco->Fill(sist_acop, weight);
        h_mass->Fill(sist_mass, weight);
        h_rapidity->Fill(sist_rap, weight);
        h_pt->Fill(sist_pt, weight);
        h_mass_diff->Fill(sist_mass - 13000.0 * sqrt(xi_arm1_1 * xi_arm2_1), weight);
        h_rap_match->Fill(sist_rap - 0.5 * log(xi_arm1_1 / xi_arm2_1), weight);
        h_tau_pt->Fill(tau_pt, weight);
        h_e_pt->Fill(e_pt, weight);
    }

    gStyle->SetOptStat(0);

    TCanvas *c = new TCanvas("c", "", 800, 600);

    auto draw_and_save = [&](TH1D *hist, const char *filename) {
        hist->SetLineColor(kBlue+2);
        hist->SetLineWidth(2);
        hist->Draw("hist");

        TLatex label;
        label.SetTextAlign(31);
        label.SetTextSize(0.04);
        label.SetTextFont(62);
        label.DrawLatexNDC(0.90, 0.92, "54.9 fb^{-1} (13 TeV)");

        TLatex cmsLabel;
        cmsLabel.SetTextAlign(11);
        cmsLabel.SetTextSize(0.045);
        cmsLabel.SetTextFont(62);
        cmsLabel.DrawLatexNDC(0.12, 0.92, "CMS-TOTEM Preliminary");

        c->SaveAs(filename);
    };

    draw_and_save(h_aco, "output_plots/sist_acop_etau_refactored.png");
    draw_and_save(h_mass, "output_plots/sist_mass_etau_refactored.png");
    draw_and_save(h_rapidity, "output_plots/sist_rap_etau_refactored.png");
    draw_and_save(h_pt, "output_plots/sist_pt_etau_refactored.png");
    draw_and_save(h_mass_diff, "output_plots/mass_diff_etau_refactored.png");
    draw_and_save(h_rap_match, "output_plots/rapidity_match_etau_refactored.png");
    draw_and_save(h_tau_pt, "output_plots/tau_pt_etau_refactored.png");
    draw_and_save(h_e_pt, "output_plots/e_pt_etau_refactored.png");

    delete c;
    file->Close();
    delete file;

    return 0;
}
