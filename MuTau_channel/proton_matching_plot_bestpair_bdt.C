#include "TFile.h"
#include "TTree.h"
#include "TH2F.h"
#include "TCanvas.h"
#include "TLatex.h"
#include "TLine.h"
#include "TStyle.h"
#include "TGraph.h"
#include "TBranch.h"
#include <iostream>
#include <cmath>
#include <vector>
#include <string>
#include <limits>

// ================================================================
// Proton matching plot for MuTau
//  - chooses the best proton pair (minimum chi2 in the matching plane)
//  - can apply a real BDT cut if a BDT branch is present in the tree
//  - still uses the same simple xi-resolution model as the original macro
//    (replace XI_REL_RES with your calibrated resolution if available)
// ================================================================

// ---- Input files ------------------------------------------------
const char* DATA_FILE   = "/eos/user/m/mblancco/samples_2018_mutau/Data_2018_UL_MuTau_nano_merged_proton_vars.root";
const char* SIGNAL_FILE = "MuTau_sinal_SM_2018_july.root";

// ---- Matching model ---------------------------------------------
const double XI_REL_RES  = 0.05;     // placeholder resolution model
const double MATCH_WINDOW = 2.0;
const double SQRTS        = 13000.0;

// ---- BDT selection ----------------------------------------------
// Leave BDT_BRANCH empty to auto-detect among common branch names.
const char* BDT_BRANCH = "";
const double BDT_CUT   = 0.30;       // adjust to your preferred signal-enriched region

// ---- Plot ranges -------------------------------------------------
const double AXIS_MIN = -20.0;
const double AXIS_MAX =  20.0;

struct MatchPoint {
    bool ok = false;
    double x = 0.0;
    double y = 0.0;
    double chi2 = 0.0;
};

bool compute_match_point(double xi0, double xi1, double sist_mass, double sist_rap, MatchPoint &out) {
    if (xi0 <= 0.0 || xi1 <= 0.0 || sist_mass <= 0.0) return false;

    const double m_pp  = SQRTS * std::sqrt(xi0 * xi1);
    const double y_pp  = 0.5 * std::log(xi0 / xi1);
    const double rel   = XI_REL_RES * std::sqrt(2.0);
    const double sig_m = 0.5 * m_pp * rel;
    const double sig_y = 0.5 * rel;

    if (sig_m <= 0.0 || sig_y <= 0.0) return false;

    out.ok   = true;
    out.x    = (m_pp - sist_mass) / sig_m;
    out.y    = (y_pp - sist_rap)  / sig_y;
    out.chi2 = out.x * out.x + out.y * out.y;
    return true;
}

MatchPoint choose_best_pair(const std::vector<double>& arm0,
                            const std::vector<double>& arm1,
                            double sist_mass, double sist_rap) {
    MatchPoint best;
    best.ok = false;
    best.chi2 = std::numeric_limits<double>::infinity();

    for (double xi0 : arm0) {
        for (double xi1 : arm1) {
            MatchPoint cand;
            if (!compute_match_point(xi0, xi1, sist_mass, sist_rap, cand)) continue;
            if (!best.ok || cand.chi2 < best.chi2) best = cand;
        }
    }
    return best;
}

const char* autodetect_bdt_branch(TTree* t) {
    if (BDT_BRANCH && std::string(BDT_BRANCH).size() > 0) {
        if (t->GetBranch(BDT_BRANCH)) return BDT_BRANCH;
        std::cerr << "Requested BDT branch '" << BDT_BRANCH << "' not found.\n";
        return nullptr;
    }

    static const char* candidates[] = {
        "bdt", "BDT", "BDT_output", "bdt_output",
        "mva_BDT", "MVA_BDT", "BDTScore", "bdt_score",
        "mva_bdt", "tmva_bdt"
    };

    for (const char* name : candidates) {
        if (t->GetBranch(name)) return name;
    }
    return nullptr;
}

void draw_matching_plot(const std::vector<double>& vx, const std::vector<double>& vy,
                        const char* subtitle, const char* outfile) {
    TCanvas *c = new TCanvas(outfile, "", 700, 700);
    c->SetLeftMargin(0.14);
    c->SetBottomMargin(0.13);
    c->SetTopMargin(0.08);
    c->SetRightMargin(0.05);

    TH2F *frame = new TH2F(TString("frame_") + outfile, "",
                           1, AXIS_MIN, AXIS_MAX, 1, AXIS_MIN, AXIS_MAX);
    frame->GetXaxis()->SetTitle("(m_{pp} - m_{#mu#tau}) / #sigma_{m}");
    frame->GetYaxis()->SetTitle("(y_{pp} - y_{#mu#tau}) / #sigma_{y}");
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

    TLine *lb[4] = {
        new TLine(0.70, 0.875, 0.76, 0.875),
        new TLine(0.70, 0.840, 0.76, 0.840),
        new TLine(0.70, 0.840, 0.70, 0.875),
        new TLine(0.76, 0.840, 0.76, 0.875)
    };
    for (TLine *l : lb) {
        l->SetNDC(); l->SetLineColor(kGreen + 1); l->SetLineWidth(2); l->Draw();
    }
    TLatex leg;
    leg.SetNDC(); leg.SetTextFont(42); leg.SetTextSize(0.033);
    leg.DrawLatex(0.78, 0.850, "2#sigma matching");

    TLatex sub;
    sub.SetNDC(); sub.SetTextFont(42); sub.SetTextSize(0.038);
    sub.DrawLatex(0.08, 0.87, subtitle);

    TLatex lumi;
    lumi.SetNDC(); lumi.SetTextAlign(31); lumi.SetTextFont(42); lumi.SetTextSize(0.035);
    lumi.DrawLatex(0.95, 0.935, "54.9 fb^{-1} (13 TeV)");

    c->Update();
    c->SaveAs(outfile);
    std::cout << "Saved " << outfile << "\n";
}

void fill_from_vectors_bestpair(const char* filename,
                                bool require_bdt_cut,
                                std::vector<double>& vx,
                                std::vector<double>& vy) {
    TFile *f = TFile::Open(filename);
    if (!f || f->IsZombie()) { std::cerr << "Cannot open " << filename << "\n"; return; }
    TTree *t = (TTree*)f->Get("tree");
    if (!t) { std::cerr << "No tree in " << filename << "\n"; return; }

    std::vector<float> *proton_xi  = nullptr;
    std::vector<int>   *proton_arm = nullptr;
    std::vector<float> *proton_multi_xi  = nullptr;
    std::vector<int>   *proton_multi_arm = nullptr;
    double sist_mass = 0.0, sist_rap = 0.0;
    double bdt = -999.0;

    const bool has_multi = (t->GetBranch("proton_multi_xi") != nullptr);
    if (has_multi) {
        t->SetBranchAddress("proton_multi_xi",  &proton_multi_xi);
        t->SetBranchAddress("proton_multi_arm", &proton_multi_arm);
    } else {
        t->SetBranchAddress("proton_xi",  &proton_xi);
        t->SetBranchAddress("proton_arm", &proton_arm);
    }
    t->SetBranchAddress("sist_mass", &sist_mass);
    t->SetBranchAddress("sist_rap",  &sist_rap);

    const char* bdt_name = nullptr;
    if (require_bdt_cut) {
        bdt_name = autodetect_bdt_branch(t);
        if (!bdt_name) {
            std::cerr << "No BDT branch found in " << filename << ".\n";
            std::cerr << "Either set BDT_BRANCH explicitly or point this macro to a tree that stores the BDT score.\n";
            return;
        }
        t->SetBranchAddress(bdt_name, &bdt);
        std::cout << "  using BDT branch: " << bdt_name << " with cut > " << BDT_CUT << "\n";
    }

    for (Long64_t i = 0; i < t->GetEntries(); ++i) {
        t->GetEntry(i);
        if (sist_mass <= 0.0) continue;
        if (require_bdt_cut && bdt <= BDT_CUT) continue;

        std::vector<double> arm0, arm1;
        if (has_multi) {
            if (!proton_multi_xi || !proton_multi_arm) continue;
            for (size_t j = 0; j < proton_multi_arm->size(); ++j) {
                if ((*proton_multi_xi)[j] <= 0) continue;
                if ((*proton_multi_arm)[j] == 0) arm0.push_back((*proton_multi_xi)[j]);
                if ((*proton_multi_arm)[j] == 1) arm1.push_back((*proton_multi_xi)[j]);
            }
        } else {
            if (!proton_xi || !proton_arm) continue;
            for (size_t j = 0; j < proton_arm->size(); ++j) {
                if ((*proton_xi)[j] <= 0) continue;
                if ((*proton_arm)[j] == 0) arm0.push_back((*proton_xi)[j]);
                if ((*proton_arm)[j] == 1) arm1.push_back((*proton_xi)[j]);
            }
        }

        if (arm0.empty() || arm1.empty()) continue;
        MatchPoint best = choose_best_pair(arm0, arm1, sist_mass, sist_rap);
        if (!best.ok) continue;

        vx.push_back(best.x);
        vy.push_back(best.y);
    }

    std::cout << "  " << vx.size() << " points from " << filename << "\n";
}

void fill_from_scalars_bestpair(const char* filename,
                                bool require_bdt_cut,
                                std::vector<double>& vx,
                                std::vector<double>& vy) {
    TFile *f = TFile::Open(filename);
    if (!f || f->IsZombie()) { std::cerr << "Cannot open " << filename << "\n"; return; }
    TTree *t = (TTree*)f->Get("tree");
    if (!t) { std::cerr << "No tree in " << filename << "\n"; return; }

    double xi_arm1_1 = 0.0, xi_arm1_2 = 0.0, xi_arm2_1 = 0.0, xi_arm2_2 = 0.0;
    double sist_mass = 0.0, sist_rap = 0.0;
    double bdt = -999.0;

    t->SetBranchAddress("xi_arm1_1", &xi_arm1_1);
    if (t->GetBranch("xi_arm1_2")) t->SetBranchAddress("xi_arm1_2", &xi_arm1_2);
    t->SetBranchAddress("xi_arm2_1", &xi_arm2_1);
    if (t->GetBranch("xi_arm2_2")) t->SetBranchAddress("xi_arm2_2", &xi_arm2_2);
    t->SetBranchAddress("sist_mass", &sist_mass);
    t->SetBranchAddress("sist_rap",  &sist_rap);

    const char* bdt_name = nullptr;
    if (require_bdt_cut) {
        bdt_name = autodetect_bdt_branch(t);
        if (!bdt_name) {
            std::cerr << "No BDT branch found in " << filename << ".\n";
            std::cerr << "Either set BDT_BRANCH explicitly or point this macro to a tree that stores the BDT score.\n";
            return;
        }
        t->SetBranchAddress(bdt_name, &bdt);
        std::cout << "  using BDT branch: " << bdt_name << " with cut > " << BDT_CUT << "\n";
    }

    for (Long64_t i = 0; i < t->GetEntries(); ++i) {
        t->GetEntry(i);
        if (sist_mass <= 0.0) continue;
        if (require_bdt_cut && bdt <= BDT_CUT) continue;

        std::vector<double> arm0, arm1;
        if (xi_arm1_1 > 0.0) arm0.push_back(xi_arm1_1);
        if (xi_arm1_2 > 0.0) arm0.push_back(xi_arm1_2);
        if (xi_arm2_1 > 0.0) arm1.push_back(xi_arm2_1);
        if (xi_arm2_2 > 0.0) arm1.push_back(xi_arm2_2);

        if (arm0.empty() || arm1.empty()) continue;
        MatchPoint best = choose_best_pair(arm0, arm1, sist_mass, sist_rap);
        if (!best.ok) continue;

        vx.push_back(best.x);
        vy.push_back(best.y);
    }

    std::cout << "  " << vx.size() << " points from " << filename << "\n";
}

void proton_matching_plot_bestpair_bdt() {
    gStyle->SetOptStat(0);

    std::cout << "\n=== Plot 1: Data (best proton pair) ===\n";
    {
        std::vector<double> vx, vy;
        fill_from_vectors_bestpair(DATA_FILE, false, vx, vy);
        draw_matching_plot(vx, vy,
            "#mu#tau data, 2018 (best proton pair)",
            "matching_data_bestpair.png");
    }

    std::cout << "\n=== Plot 2: Signal MC (best proton pair) ===\n";
    {
        std::vector<double> vx, vy;
        fill_from_scalars_bestpair(SIGNAL_FILE, false, vx, vy);
        draw_matching_plot(vx, vy,
            "Signal MC (#gamma#gamma #rightarrow #mu#tau, best pair)",
            "matching_signal_bestpair.png");
    }

    std::cout << "\n=== Plot 3: Data (best proton pair, BDT-enriched) ===\n";
    {
        std::vector<double> vx, vy;
        fill_from_vectors_bestpair(DATA_FILE, true, vx, vy);
        draw_matching_plot(vx, vy,
            Form("#mu#tau data, 2018 (best pair, BDT > %.2f)", BDT_CUT),
            "matching_data_bdt_bestpair.png");
    }
}
