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

// ==============================
// OLD weights (from slides_mutau_05_02)
//   MC cut: (selection && proton_cut) * 0.245
//   Then Scale(1.81) for DY, Scale(0.15) for ttbar
//   -> No weight branch, no generator_weight, no muon SFs
// ==============================
const double OLD_PROTON_ACCEPTANCE = 0.245;
const double OLD_DY_FACTOR = 1.81;
const double OLD_TTBAR_FACTOR = 0.15;

// ==============================
// NEW weights (corrected normalization)
//   MC cut: (selection && proton_cut) * weight
//   weight branch = generator_weight * muon_SFs * 0.245 (DY)
//   weight branch = 0.15 * muon_SFs * 0.245 (ttbar)
//   Then Scale(1.004e-4) for DY, Scale(1.0) for ttbar
// ==============================
const double NEW_DY_SCALE = 1.004e-4;
const double NEW_TTBAR_SCALE = 1.0;

void MuTauPlots_weight_comparison()
{
    gStyle->SetOptStat(0);

    // Open files
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

    struct PlotDef {
        const char* var;
        const char* name;
        const char* title;
        int nbins;
        double xmin, xmax;
    };

    PlotDef plots[] = {
        {"sist_mass", "mass", "Invariant mass;m(#mu,#tau) [GeV];Events", 20, 0, 1000},
        {"acop", "acop", "Acoplanarity;1 - |#Delta#phi|/#pi;Events", 20, 0, 1},
        {"sist_pt", "pt", "System p_{T};p_{T}(#mu,#tau) [GeV];Events", 20, 0, 200},
        {"sist_rap", "rap", "System rapidity;y(#mu,#tau);Events", 20, -3, 3},
    };
    int nplots = sizeof(plots) / sizeof(plots[0]);

    // Proton cuts
    TString proton_cut_data = "Sum$(proton_multi_arm==0)>0 && Sum$(proton_multi_arm==1)>0";
    TString proton_cut_mc = "xi_arm1_1 >= 0 && xi_arm2_1 >= 0";

    for (int p = 0; p < nplots; p++) {
        TString var = plots[p].var;
        TString name = plots[p].name;
        TString title = plots[p].title;
        int nbins = plots[p].nbins;
        double xmin = plots[p].xmin;
        double xmax = plots[p].xmax;

        // Canvas: 2 columns (old | new), each with main + ratio pad
        TCanvas *c = new TCanvas("c_" + name, "", 1400, 900);

        // --- LEFT COLUMN: OLD WEIGHTS ---
        TPad *pad1_old = new TPad("pad1_old_" + name, "", 0.0, 0.3, 0.5, 1.0);
        pad1_old->SetLeftMargin(0.15);
        pad1_old->SetRightMargin(0.05);
        pad1_old->SetTopMargin(0.12);
        pad1_old->SetBottomMargin(0.02);

        TPad *pad2_old = new TPad("pad2_old_" + name, "", 0.0, 0.0, 0.5, 0.3);
        pad2_old->SetLeftMargin(0.15);
        pad2_old->SetRightMargin(0.05);
        pad2_old->SetTopMargin(0.02);
        pad2_old->SetBottomMargin(0.35);

        // --- RIGHT COLUMN: NEW WEIGHTS ---
        TPad *pad1_new = new TPad("pad1_new_" + name, "", 0.5, 0.3, 1.0, 1.0);
        pad1_new->SetLeftMargin(0.15);
        pad1_new->SetRightMargin(0.05);
        pad1_new->SetTopMargin(0.12);
        pad1_new->SetBottomMargin(0.02);

        TPad *pad2_new = new TPad("pad2_new_" + name, "", 0.5, 0.0, 1.0, 0.3);
        pad2_new->SetLeftMargin(0.15);
        pad2_new->SetRightMargin(0.05);
        pad2_new->SetTopMargin(0.02);
        pad2_new->SetBottomMargin(0.35);

        c->cd();
        pad1_old->Draw();
        pad2_old->Draw();
        pad1_new->Draw();
        pad2_new->Draw();

        // ============================================================
        //  OLD WEIGHTS (slides_mutau_05_02)
        //  MC: flat weight = PROTON_ACCEPTANCE (no weight branch)
        //  Then Scale(1.81) for DY, Scale(0.15) for ttbar
        // ============================================================
        TString cut_data_old = TString::Format("sist_mass > 0 && %s", proton_cut_data.Data());
        TString cut_qcd_old  = TString::Format("sist_mass > 0 && %s", proton_cut_data.Data());
        TString cut_mc_old   = TString::Format("(sist_mass > 0 && %s) * %f",
                                                proton_cut_mc.Data(), OLD_PROTON_ACCEPTANCE);
        TString cut_sig_old  = TString::Format("(sist_mass > 0 && %s) * weight",
                                                proton_cut_mc.Data());

        TH1F *h_data_old   = new TH1F("h_data_old_"   + name, title, nbins, xmin, xmax);
        TH1F *h_qcd_old    = new TH1F("h_qcd_old_"    + name, title, nbins, xmin, xmax);
        TH1F *h_dy_old     = new TH1F("h_dy_old_"     + name, title, nbins, xmin, xmax);
        TH1F *h_ttbar_old  = new TH1F("h_ttbar_old_"  + name, title, nbins, xmin, xmax);
        TH1F *h_signal_old = new TH1F("h_signal_old_" + name, title, nbins, xmin, xmax);

        t_data->Draw(var + " >> h_data_old_" + name, cut_data_old, "goff");
        t_qcd->Draw(var + " >> h_qcd_old_" + name, cut_qcd_old, "goff");
        t_dy->Draw(var + " >> h_dy_old_" + name, cut_mc_old, "goff");
        t_ttbar->Draw(var + " >> h_ttbar_old_" + name, cut_mc_old, "goff");
        t_signal->Draw(var + " >> h_signal_old_" + name, cut_sig_old, "goff");

        h_dy_old->Scale(OLD_DY_FACTOR);
        h_ttbar_old->Scale(OLD_TTBAR_FACTOR);
        h_signal_old->Scale(5000);

        // ============================================================
        //  NEW WEIGHTS (corrected)
        //  MC: uses weight branch (gen_weight * muon_SFs * 0.245)
        //  Then Scale(1.004e-4) for DY, Scale(1.0) for ttbar
        // ============================================================
        TString cut_data_new = TString::Format("sist_mass > 0 && %s", proton_cut_data.Data());
        TString cut_qcd_new  = TString::Format("sist_mass > 0 && %s", proton_cut_data.Data());
        TString cut_dy_new   = TString::Format("(sist_mass > 0 && %s) * weight",
                                                proton_cut_mc.Data());
        TString cut_tt_new   = TString::Format("(sist_mass > 0 && %s) * weight",
                                                proton_cut_mc.Data());
        TString cut_sig_new  = TString::Format("(sist_mass > 0 && %s) * weight",
                                                proton_cut_mc.Data());

        TH1F *h_data_new   = new TH1F("h_data_new_"   + name, title, nbins, xmin, xmax);
        TH1F *h_qcd_new    = new TH1F("h_qcd_new_"    + name, title, nbins, xmin, xmax);
        TH1F *h_dy_new     = new TH1F("h_dy_new_"     + name, title, nbins, xmin, xmax);
        TH1F *h_ttbar_new  = new TH1F("h_ttbar_new_"  + name, title, nbins, xmin, xmax);
        TH1F *h_signal_new = new TH1F("h_signal_new_" + name, title, nbins, xmin, xmax);

        t_data->Draw(var + " >> h_data_new_" + name, cut_data_new, "goff");
        t_qcd->Draw(var + " >> h_qcd_new_" + name, cut_qcd_new, "goff");
        t_dy->Draw(var + " >> h_dy_new_" + name, cut_dy_new, "goff");
        t_ttbar->Draw(var + " >> h_ttbar_new_" + name, cut_tt_new, "goff");
        t_signal->Draw(var + " >> h_signal_new_" + name, cut_sig_new, "goff");

        h_dy_new->Scale(NEW_DY_SCALE);
        h_ttbar_new->Scale(NEW_TTBAR_SCALE);
        h_signal_new->Scale(5000);

        // ============================================================
        //  DRAW BOTH SIDES
        // ============================================================
        struct Side {
            TPad *pad1;
            TPad *pad2;
            TH1F *h_data;
            TH1F *h_qcd;
            TH1F *h_dy;
            TH1F *h_ttbar;
            TH1F *h_signal;
            const char* label;
            const char* detail;
        };

        Side sides[2] = {
            {pad1_old, pad2_old, h_data_old, h_qcd_old, h_dy_old, h_ttbar_old, h_signal_old,
             "Old weights (slides 05/02)", "MC: 0.245, DY #times1.81, t#bar{t} #times0.15"},
            {pad1_new, pad2_new, h_data_new, h_qcd_new, h_dy_new, h_ttbar_new, h_signal_new,
             "New weights (corrected)", "MC: weight branch, DY #times1e-4"},
        };

        for (int s = 0; s < 2; s++) {
            Side &side = sides[s];

            // Style
            side.h_data->SetMarkerStyle(20);
            side.h_data->SetMarkerSize(0.8);
            side.h_data->SetLineColor(kBlack);

            side.h_qcd->SetFillColor(kRed);
            side.h_qcd->SetLineWidth(0);

            side.h_dy->SetFillColor(kYellow);
            side.h_dy->SetLineWidth(0);

            side.h_ttbar->SetFillColor(kGreen);
            side.h_ttbar->SetLineWidth(0);

            side.h_signal->SetLineColor(kBlack);
            side.h_signal->SetLineWidth(3);
            side.h_signal->SetFillStyle(0);

            // Stack
            THStack *stack = new THStack(TString::Format("stack_%s_%d", name.Data(), s), title);
            stack->Add(side.h_ttbar);
            stack->Add(side.h_qcd);
            stack->Add(side.h_dy);

            // MC sum
            TH1F *h_mcsum = (TH1F*)side.h_qcd->Clone(TString::Format("h_mcsum_%s_%d", name.Data(), s));
            h_mcsum->Add(side.h_dy);
            h_mcsum->Add(side.h_ttbar);
            h_mcsum->SetFillColor(kGray + 1);
            h_mcsum->SetFillStyle(3354);
            h_mcsum->SetMarkerSize(0);
            for (int i = 1; i <= h_mcsum->GetNbinsX(); i++) {
                double content = h_mcsum->GetBinContent(i);
                h_mcsum->SetBinError(i, sqrt(fabs(content)));
            }

            // Y axis range
            double maxY = side.h_data->GetMaximum();
            if (side.h_signal->GetMaximum() > maxY) maxY = side.h_signal->GetMaximum();
            if (stack->GetMaximum() > maxY) maxY = stack->GetMaximum();

            // Main plot
            side.pad1->cd();
            stack->Draw("hist");
            stack->GetYaxis()->SetTitle("Events");
            stack->GetYaxis()->SetTitleSize(0.06);
            stack->GetYaxis()->SetLabelSize(0.05);
            stack->GetYaxis()->SetTitleOffset(1.1);
            stack->GetXaxis()->SetLabelSize(0);
            stack->GetYaxis()->SetNdivisions(505);
            stack->SetMinimum(0.1);
            stack->SetMaximum(maxY * 1.5);

            side.h_signal->Draw("hist same");
            side.h_data->Draw("e same");
            h_mcsum->Draw("e2 same");

            // Legend (right side only)
            if (s == 1) {
                TLegend *leg = new TLegend(0.45, 0.55, 0.92, 0.88);
                leg->SetBorderSize(0);
                leg->SetFillStyle(0);
                leg->AddEntry(side.h_data, "Data", "lep");
                leg->AddEntry(side.h_signal, "Signal (#times5000)", "l");
                leg->AddEntry(side.h_dy, "Drell-Yan", "f");
                leg->AddEntry(side.h_qcd, "QCD", "f");
                leg->AddEntry(side.h_ttbar, "t#bar{t}", "f");
                leg->Draw();
            }

            // Title label
            TLatex latex;
            latex.SetNDC();
            latex.SetTextSize(0.050);
            latex.SetTextFont(62);
            latex.DrawLatex(0.18, 0.92, side.label);

            // Subtitle with weight details
            latex.SetTextSize(0.040);
            latex.SetTextFont(42);
            latex.DrawLatex(0.18, 0.86, side.detail);

            latex.SetTextAlign(31);
            latex.SetTextSize(0.040);
            latex.SetTextFont(42);
            latex.DrawLatex(0.92, 0.92, "54.9 fb^{-1} (13 TeV)");

            // Ratio plot
            side.pad2->cd();
            TH1F *h_ratio = (TH1F*)side.h_data->Clone(TString::Format("h_ratio_%s_%d", name.Data(), s));
            h_ratio->Divide(h_mcsum);

            h_ratio->SetMarkerStyle(20);
            h_ratio->SetMarkerSize(0.8);
            h_ratio->SetLineColor(kBlack);
            h_ratio->SetTitle("");
            h_ratio->GetYaxis()->SetTitle("Data/Sim");
            h_ratio->GetYaxis()->SetTitleSize(0.14);
            h_ratio->GetYaxis()->SetTitleOffset(0.45);
            h_ratio->GetYaxis()->SetLabelSize(0.12);
            h_ratio->GetYaxis()->SetNdivisions(505);
            h_ratio->GetYaxis()->SetRangeUser(0, 2);
            h_ratio->GetXaxis()->SetTitle(title.Tokenize(";")->At(1)->GetName());
            h_ratio->GetXaxis()->SetTitleSize(0.14);
            h_ratio->GetXaxis()->SetTitleOffset(0.9);
            h_ratio->GetXaxis()->SetLabelSize(0.12);

            h_ratio->Draw("e");

            TLine *line = new TLine(xmin, 1, xmax, 1);
            line->SetLineColor(kRed);
            line->SetLineStyle(2);
            line->SetLineWidth(2);
            line->Draw();

            // Print yields
            std::cout << "===== " << name << " [" << side.label << "] =====" << std::endl;
            std::cout << "  Data:    " << side.h_data->GetSumOfWeights() << std::endl;
            std::cout << "  QCD:     " << side.h_qcd->GetSumOfWeights() << std::endl;
            std::cout << "  DY:      " << side.h_dy->GetSumOfWeights() << std::endl;
            std::cout << "  ttbar:   " << side.h_ttbar->GetSumOfWeights() << std::endl;
            std::cout << "  Signal:  " << side.h_signal->GetSumOfWeights() << std::endl;
            std::cout << "  MC sum:  " << h_mcsum->GetSumOfWeights() << std::endl;
            double ratio_val = (h_mcsum->GetSumOfWeights() > 0) ?
                side.h_data->GetSumOfWeights() / h_mcsum->GetSumOfWeights() : 0;
            std::cout << "  Data/MC: " << ratio_val << std::endl;
            std::cout << "==========================================" << std::endl;
        }

        c->Update();
        c->SaveAs("plot_weight_comparison_" + name + ".png");
    }

    std::cout << "\nWeight comparison plots saved!" << std::endl;
    std::cout << "\nOLD: MC weight = 0.245 (flat), DY x1.81, ttbar x0.15" << std::endl;
    std::cout << "     No generator_weight, no muon scale factors" << std::endl;
    std::cout << "\nNEW: MC weight = weight branch (gen_w * muon_SFs * 0.245)" << std::endl;
    std::cout << "     DY x1.004e-4 (= L*sigma/Sum_w), ttbar x1.0" << std::endl;
}
