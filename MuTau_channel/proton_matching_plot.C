#include "TFile.h"
#include "TTree.h"
#include "TH2F.h"
#include "TCanvas.h"
#include "TLatex.h"
#include "TLine.h"
#include "TStyle.h"
#include "TGraph.h"
#include <iostream>
#include <cmath>
#include <vector>

// ── Configuration ─────────────────────────────────────────────────────────────

// Input files
const char* DATA_FILE   = "/eos/user/m/mblancco/samples_2018_mutau/Data_2018_UL_MuTau_nano_merged_proton_vars.root";
const char* SIGNAL_FILE = "MuTau_sinal_SM_2018_july.root";

// Proton xi relative resolution (used to compute per-event sigma via error propagation)
// Typical CMS-TOTEM Run 2 multiRP value ~5%.  Adjust to your calibration.
const double XI_REL_RES = 0.05;

// Matching window drawn as a green rectangle
const double MATCH_WINDOW = 2.0;

// Centre-of-mass energy
const double SQRTS = 13000.;

// Signal-enriched region proxy (plot 3):
// Events with acoplanarity < ACOP_MAX and system pT < PT_MAX are used.
// Replace with a proper BDT cut if you have per-event BDT scores.
const double ACOP_MAX  = 0.05;
const double PT_MAX    = 50.;

// ── fill_from_vectors ─────────────────────────────────────────────────────────
// For the raw data file: xi stored as per-proton vectors with an arm index.
// Optional kinematic pre-selection via acop_max / pt_max (set to -1 to disable).

void fill_from_vectors(const char* filename,
                       double acop_max, double pt_max,
                       std::vector<double>& vx, std::vector<double>& vy)
{
    TFile *f = TFile::Open(filename);
    if (!f || f->IsZombie()) { std::cerr << "Cannot open " << filename << "\n"; return; }
    TTree *t = (TTree*)f->Get("tree");
    if (!t) { std::cerr << "No tree in " << filename << "\n"; return; }

    std::vector<float> *proton_xi  = nullptr;
    std::vector<int>   *proton_arm = nullptr;
    double sist_mass = 0, sist_rap = 0;
    double acop = 0, sist_pt = 0;

    bool has_multi = (t->GetBranch("proton_multi_xi") != nullptr);
    t->SetBranchAddress(has_multi ? "proton_multi_xi"  : "proton_xi",  &proton_xi);
    t->SetBranchAddress(has_multi ? "proton_multi_arm" : "proton_arm", &proton_arm);
    t->SetBranchAddress("sist_mass", &sist_mass);
    t->SetBranchAddress("sist_rap",  &sist_rap);
    if (acop_max > 0) t->SetBranchAddress("acop",    &acop);
    if (pt_max   > 0) t->SetBranchAddress("sist_pt", &sist_pt);

    for (Long64_t i = 0; i < t->GetEntries(); i++) {
        t->GetEntry(i);
        if (!proton_xi || !proton_arm) continue;
        if (sist_mass <= 0) continue;
        if (acop_max > 0 && acop   > acop_max) continue;
        if (pt_max   > 0 && sist_pt > pt_max)  continue;

        double xi0 = -1, xi1 = -1;
        for (size_t j = 0; j < proton_arm->size(); j++) {
            if ((*proton_arm)[j] == 0 && xi0 < 0) xi0 = (*proton_xi)[j];
            if ((*proton_arm)[j] == 1 && xi1 < 0) xi1 = (*proton_xi)[j];
        }
        if (xi0 <= 0 || xi1 <= 0) continue;

        double m_pp  = SQRTS * sqrt(xi0 * xi1);
        double y_pp  = 0.5 * log(xi0 / xi1);
        double rel   = XI_REL_RES * sqrt(2.);
        double sig_m = 0.5 * m_pp * rel;
        double sig_y = 0.5 * rel;

        vx.push_back((m_pp - sist_mass) / sig_m);
        vy.push_back((y_pp - sist_rap)  / sig_y);
    }
    std::cout << "  " << vx.size() << " points from " << filename << "\n";
}

// ── fill_from_scalars ─────────────────────────────────────────────────────────
// For processed output files (sinal.cpp output): xi stored as scalar branches
// xi_arm1_1 (arm 0) and xi_arm2_1 (arm 1).

void fill_from_scalars(const char* filename,
                       std::vector<double>& vx, std::vector<double>& vy)
{
    TFile *f = TFile::Open(filename);
    if (!f || f->IsZombie()) { std::cerr << "Cannot open " << filename << "\n"; return; }
    TTree *t = (TTree*)f->Get("tree");
    if (!t) { std::cerr << "No tree in " << filename << "\n"; return; }

    double xi0 = 0, xi1 = 0, sist_mass = 0, sist_rap = 0;
    t->SetBranchAddress("xi_arm1_1", &xi0);
    t->SetBranchAddress("xi_arm2_1", &xi1);
    t->SetBranchAddress("sist_mass", &sist_mass);
    t->SetBranchAddress("sist_rap",  &sist_rap);

    for (Long64_t i = 0; i < t->GetEntries(); i++) {
        t->GetEntry(i);
        if (xi0 <= 0 || xi1 <= 0) continue;
        if (sist_mass <= 0) continue;

        double m_pp  = SQRTS * sqrt(xi0 * xi1);
        double y_pp  = 0.5 * log(xi0 / xi1);
        double rel   = XI_REL_RES * sqrt(2.);
        double sig_m = 0.5 * m_pp * rel;
        double sig_y = 0.5 * rel;

        vx.push_back((m_pp - sist_mass) / sig_m);
        vy.push_back((y_pp - sist_rap)  / sig_y);
    }
    std::cout << "  " << vx.size() << " points from " << filename << "\n";
}

// ── draw_matching_plot ────────────────────────────────────────────────────────

void draw_matching_plot(const std::vector<double>& vx, const std::vector<double>& vy,
                        const char* subtitle, const char* outfile)
{
    TCanvas *c = new TCanvas(outfile, "", 700, 700);
    c->SetLeftMargin(0.14);
    c->SetBottomMargin(0.13);
    c->SetTopMargin(0.08);
    c->SetRightMargin(0.05);

    TH2F *frame = new TH2F(TString("frame_") + outfile, "",
                           1, -20, 20, 1, -20, 20);
    frame->GetXaxis()->SetTitle("(m_{pp} #minus m_{#mu#tau}) / #sigma_{m}");
    frame->GetYaxis()->SetTitle("(y_{pp} #minus y_{#mu#tau}) / #sigma_{y}");
    frame->GetXaxis()->SetTitleSize(0.042);
    frame->GetYaxis()->SetTitleSize(0.042);
    frame->GetXaxis()->SetLabelSize(0.037);
    frame->GetYaxis()->SetLabelSize(0.037);
    frame->GetXaxis()->SetTitleOffset(1.1);
    frame->GetYaxis()->SetTitleOffset(1.5);
    frame->Draw();

    if (!vx.empty()) {
        TGraph *gr = new TGraph(vx.size(), vx.data(), vy.data());
        gr->SetMarkerStyle(4);
        gr->SetMarkerSize(0.55);
        gr->SetMarkerColor(kBlack);
        gr->Draw("p same");
    }

    // 2σ matching rectangle
    TLine *sides[4] = {
        new TLine(-MATCH_WINDOW,  MATCH_WINDOW,  MATCH_WINDOW,  MATCH_WINDOW),
        new TLine(-MATCH_WINDOW, -MATCH_WINDOW,  MATCH_WINDOW, -MATCH_WINDOW),
        new TLine(-MATCH_WINDOW, -MATCH_WINDOW, -MATCH_WINDOW,  MATCH_WINDOW),
        new TLine( MATCH_WINDOW, -MATCH_WINDOW,  MATCH_WINDOW,  MATCH_WINDOW)
    };
    for (TLine *l : sides) {
        l->SetLineColor(kGreen + 1);
        l->SetLineWidth(2);
        l->Draw();
    }

    // Legend box for the rectangle
    TLine *lb[4] = {
        new TLine(0.70, 0.875, 0.76, 0.875),
        new TLine(0.70, 0.840, 0.76, 0.840),
        new TLine(0.70, 0.840, 0.70, 0.875),
        new TLine(0.76, 0.840, 0.76, 0.875)
    };
    for (TLine *l : lb) {
        l->SetNDC(); l->SetLineColor(kGreen+1); l->SetLineWidth(2); l->Draw();
    }
    TLatex leg;
    leg.SetNDC(); leg.SetTextFont(42); leg.SetTextSize(0.033);
    leg.DrawLatex(0.78, 0.850, "2#sigma matching");

    // Subtitle (sample label) inside plot
    TLatex sub;
    sub.SetNDC(); sub.SetTextFont(42); sub.SetTextSize(0.038);
    sub.DrawLatex(0.17, 0.87, subtitle);

    // Lumi label
    TLatex lumi;
    lumi.SetNDC(); lumi.SetTextAlign(31); lumi.SetTextFont(42); lumi.SetTextSize(0.035);
    lumi.DrawLatex(0.95, 0.935, "54.9 fb^{-1} (13 TeV)");

    c->Update();
    c->SaveAs(outfile);
    std::cout << "Saved " << outfile << "\n";
}

// ── main ──────────────────────────────────────────────────────────────────────

void proton_matching_plot()
{
    gStyle->SetOptStat(0);

    // ── Plot 1: All data ─────────────────────────────────────────────────────
    std::cout << "\n=== Plot 1: Data ===\n";
    {
        std::vector<double> vx, vy;
        fill_from_vectors(DATA_FILE, -1, -1, vx, vy);
        draw_matching_plot(vx, vy,
            "#mu#tau data, 2018",
            "matching_data.png");
    }

    // ── Plot 2: Signal MC ────────────────────────────────────────────────────
    std::cout << "\n=== Plot 2: Signal MC ===\n";
    {
        std::vector<double> vx, vy;
        fill_from_scalars(SIGNAL_FILE, vx, vy);
        draw_matching_plot(vx, vy,
            "Signal MC (#gamma#gamma#rightarrow#mu#tau)",
            "matching_signal.png");
    }

    // ── Plot 3: Data, signal-enriched region ─────────────────────────────────
    // Proxy: acop < ACOP_MAX && sist_pt < PT_MAX (correlated with BDT signal region).
    // To use a proper BDT cut: run TMVAClassificationApplication.C on data,
    // save a tree with per-event BDT scores, then add a BDT branch cut here.
    std::cout << "\n=== Plot 3: Data (signal-enriched proxy: acop < "
              << ACOP_MAX << ", pT < " << PT_MAX << " GeV) ===\n";
    {
        std::vector<double> vx, vy;
        fill_from_vectors(DATA_FILE, ACOP_MAX, PT_MAX, vx, vy);
        draw_matching_plot(vx, vy,
            "Data: acop < 0.05, p_{T}^{sys} < 50 GeV",
            "matching_data_enriched.png");
    }
}
