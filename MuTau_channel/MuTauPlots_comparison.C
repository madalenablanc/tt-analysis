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
const double PROTON_ACCEPTANCE = 0.245;  // kept for "before mixing" view
const double DY_SCALE = 1.004e-4;       // L×sigma/Sum_w for DY
const double TTBAR_SCALE = 1.0;         // ttbar already normalized via 0.15 factor

void MuTauPlots_comparison()
{
    gStyle->SetOptStat(0);

    // Open files
    // Data/QCD: use _proton_vars files (real proton info from PPS)
    // MC (DY/ttbar): use _pileup_protons files (mixed random protons)
    TFile *f_data   = TFile::Open("/eos/user/m/mblancco/samples_2018_mutau/Data_2018_UL_MuTau_nano_merged_proton_vars.root");
    TFile *f_qcd    = TFile::Open("/eos/user/m/mblancco/samples_2018_mutau/QCD_2018_UL_MuTau_nano_merged_proton_vars_new.root");
    TFile *f_dy_nomix     = TFile::Open("/eos/user/m/mblancco/samples_2018_mutau/DY_2018_UL_MuTau_nano_merged.root");
    TFile *f_ttbar_nomix  = TFile::Open("/eos/user/m/mblancco/samples_2018_mutau/ttjets_2018_UL_MuTau_nano_merged.root");
    TFile *f_dy_mix     = TFile::Open("/eos/user/m/mblancco/samples_2018_mutau/DY_2018_UL_MuTau_nano_merged_pileup_protons.root");
    TFile *f_ttbar_mix  = TFile::Open("/eos/user/m/mblancco/samples_2018_mutau/ttjets_2018_UL_MuTau_nano_merged_pileup_protons.root");
    TFile *f_signal = TFile::Open("/eos/user/m/mblancco/tau_analysis/MuTau_channel/MuTau_sinal_SM_2018_july.root");

    TTree *t_data   = (TTree*)f_data->Get("tree");
    TTree *t_qcd    = (TTree*)f_qcd->Get("tree");
    TTree *t_dy_nomix     = (TTree*)f_dy_nomix->Get("tree");
    TTree *t_ttbar_nomix  = (TTree*)f_ttbar_nomix->Get("tree");
    TTree *t_dy_mix     = (TTree*)f_dy_mix->Get("tree");
    TTree *t_ttbar_mix  = (TTree*)f_ttbar_mix->Get("tree");
    TTree *t_signal = (TTree*)f_signal->Get("tree");

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
        {"tau_pt", "tau_pt", "Tau p_{T};p_{T}^{#tau} [GeV];Events", 20, 100, 500},
        {"met_pt", "met", "Missing E_{T};MET [GeV];Events", 20, 0, 300},
    };
    int nplots = sizeof(plots) / sizeof(plots[0]);

    for (int p = 0; p < nplots; p++) {
        TString var = plots[p].var;
        TString name = plots[p].name;
        TString title = plots[p].title;
        int nbins = plots[p].nbins;
        double xmin = plots[p].xmin;
        double xmax = plots[p].xmax;

        struct ViewConfig {
            TString label;
            TString suffix;
            bool require_protons;
            bool use_mixed_mc;
        };

        ViewConfig views[2] = {
            {"Before proton mixing", "_before", false, false},
            {"After proton mixing", "_after", true, true},
        };

        // Create canvas with two columns, each with main plot + ratio
        TCanvas *c = new TCanvas("c_" + name, "", 1400, 900);

        // Create all pads first before drawing anything
        TPad *pad1_left = new TPad(Form("pad1_%s_0", name.Data()), "", 0.0, 0.3, 0.5, 1.0);
        pad1_left->SetLeftMargin(0.15);
        pad1_left->SetRightMargin(0.05);
        pad1_left->SetTopMargin(0.12);
        pad1_left->SetBottomMargin(0.02);

        TPad *pad2_left = new TPad(Form("pad2_%s_0", name.Data()), "", 0.0, 0.0, 0.5, 0.3);
        pad2_left->SetLeftMargin(0.15);
        pad2_left->SetRightMargin(0.05);
        pad2_left->SetTopMargin(0.02);
        pad2_left->SetBottomMargin(0.35);

        TPad *pad1_right = new TPad(Form("pad1_%s_1", name.Data()), "", 0.5, 0.3, 1.0, 1.0);
        pad1_right->SetLeftMargin(0.15);
        pad1_right->SetRightMargin(0.05);
        pad1_right->SetTopMargin(0.12);
        pad1_right->SetBottomMargin(0.02);

        TPad *pad2_right = new TPad(Form("pad2_%s_1", name.Data()), "", 0.5, 0.0, 1.0, 0.3);
        pad2_right->SetLeftMargin(0.15);
        pad2_right->SetRightMargin(0.05);
        pad2_right->SetTopMargin(0.02);
        pad2_right->SetBottomMargin(0.35);

        // Draw all pads
        c->cd();
        pad1_left->Draw();
        pad2_left->Draw();
        pad1_right->Draw();
        pad2_right->Draw();

        // Store pads in arrays for loop access
        TPad *pad1_arr[2] = {pad1_left, pad1_right};
        TPad *pad2_arr[2] = {pad2_left, pad2_right};

        // Store histograms for ratio calculation (not currently used but kept for potential future use)
        TH1F *h_data_arr[2];
        TH1F *h_mcsum_arr[2];

        // Loop over comparison views
        for (int icut = 0; icut < 2; icut++) {
            const ViewConfig &view = views[icut];

            TPad *pad1 = pad1_arr[icut];
            TPad *pad2 = pad2_arr[icut];

            TString suffix = view.suffix;

            // Select appropriate MC trees (before vs after mixing)
            TTree *curr_dy = view.use_mixed_mc ? t_dy_mix : t_dy_nomix;
            TTree *curr_ttbar = view.use_mixed_mc ? t_ttbar_mix : t_ttbar_nomix;
            if (!curr_dy || !curr_ttbar) {
                std::cerr << "ERROR: Missing DY/ttbar tree for view " << view.label << std::endl;
                continue;
            }

            // Cut strings - Data/QCD use Sum$(proton_multi_arm==X) to require protons when requested.
            // Mixed MC samples have xi_arm branches; non-mixed samples do not.
            TString cut_data = "sist_mass > 0";
            TString cut_qcd = "sist_mass > 0";
            if (view.require_protons) {
                cut_data += " && Sum$(proton_multi_arm==0) > 0 && Sum$(proton_multi_arm==1) > 0";
                cut_qcd += " && Sum$(proton_multi_arm==0) > 0 && Sum$(proton_multi_arm==1) > 0";
            }
            TString proton_cut_mc = "";
            if (view.require_protons && view.use_mixed_mc) {
                proton_cut_mc = " && xi_arm1_1 >= 0 && xi_arm2_1 >= 0";
            }
            TString cut_dy, cut_ttbar;
            if (view.use_mixed_mc) {
                // After mixing: use weight branch (gen_weight × SFs × 0.245)
                cut_dy     = TString::Format("(sist_mass > 0%s) * weight", proton_cut_mc.Data());
                cut_ttbar  = TString::Format("(sist_mass > 0%s) * weight", proton_cut_mc.Data());
            } else {
                // Before mixing: no weight branch available, use flat acceptance
                cut_dy     = "sist_mass > 0";
                cut_ttbar  = "sist_mass > 0";
            }
            // Signal: use weight branch (already contains proper normalization)
            TString cut_signal = "(sist_mass > 0) * weight";
            if (view.require_protons) {
                cut_signal = "(sist_mass > 0 && xi_arm1_1 >= 0 && xi_arm2_1 >= 0) * weight";
            }

            // Create histograms
            TH1F *h_data   = new TH1F("h_data_"   + name + suffix, title, nbins, xmin, xmax);
            TH1F *h_qcd    = new TH1F("h_qcd_"    + name + suffix, title, nbins, xmin, xmax);
            TH1F *h_dy     = new TH1F("h_dy_"     + name + suffix, title, nbins, xmin, xmax);
            TH1F *h_ttbar  = new TH1F("h_ttbar_"  + name + suffix, title, nbins, xmin, xmax);
            TH1F *h_signal = new TH1F("h_signal_" + name + suffix, title, nbins, xmin, xmax);

            // Fill histograms - proton cut applied to ALL samples
            t_data->Draw(var + " >> h_data_" + name + suffix, cut_data, "goff");
            t_qcd->Draw(var + " >> h_qcd_" + name + suffix, cut_qcd, "goff");
            curr_dy->Draw(var + " >> h_dy_" + name + suffix, cut_dy, "goff");
            curr_ttbar->Draw(var + " >> h_ttbar_" + name + suffix, cut_ttbar, "goff");
            t_signal->Draw(var + " >> h_signal_" + name + suffix, cut_signal, "goff");

            // Scale MC
            if (view.use_mixed_mc) {
                // After mixing: weight branch used, apply L×sigma/Sum_w
                h_dy->Scale(DY_SCALE);
                h_ttbar->Scale(TTBAR_SCALE);
            } else {
                // Before mixing: no weight branch, use flat normalization
                h_dy->Scale(PROTON_ACCEPTANCE * 1.81);
                h_ttbar->Scale(PROTON_ACCEPTANCE * 0.15);
            }
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
            THStack *stack = new THStack("stack_" + name + suffix, title);
            stack->Add(h_ttbar);
            stack->Add(h_qcd);
            stack->Add(h_dy);

            // MC uncertainty band
            TH1F *h_mcsum = (TH1F*)h_qcd->Clone("h_mcsum_" + name + suffix);
            h_mcsum->Add(h_dy);
            h_mcsum->Add(h_ttbar);
            h_mcsum->SetFillColor(kGray + 1);
            h_mcsum->SetFillStyle(3354);
            h_mcsum->SetMarkerSize(0);
            for (int i = 1; i <= h_mcsum->GetNbinsX(); i++) {
                double content = h_mcsum->GetBinContent(i);
                h_mcsum->SetBinError(i, sqrt(content));
            }

            // Store for ratio
            h_data_arr[icut] = h_data;
            h_mcsum_arr[icut] = h_mcsum;

            // Auto-scale Y axis
            double maxY = h_data->GetMaximum();
            if (h_signal->GetMaximum() > maxY) maxY = h_signal->GetMaximum();
            if (stack->GetMaximum() > maxY) maxY = stack->GetMaximum();

            // Draw main plot
            pad1->cd();
            stack->Draw("hist");
            stack->GetYaxis()->SetTitle("Events");
            stack->GetYaxis()->SetTitleSize(0.06);
            stack->GetYaxis()->SetLabelSize(0.05);
            stack->GetYaxis()->SetTitleOffset(1.1);
            stack->GetXaxis()->SetLabelSize(0);  // Hide x-axis labels
            stack->GetYaxis()->SetNdivisions(505);
            stack->SetMinimum(0.1);
            stack->SetMaximum(maxY * 1.5);

            h_signal->Draw("hist same");
            h_data->Draw("e same");
            h_mcsum->Draw("e2 same");

            // Legend (only on right plot)
            if (icut == 1) {
                TLegend *leg = new TLegend(0.50, 0.55, 0.92, 0.88);
                leg->SetBorderSize(0);
                leg->SetFillStyle(0);
                leg->AddEntry(h_data, "Data", "lep");
                leg->AddEntry(h_signal, "Signal", "l");
                leg->AddEntry(h_dy, "Drell-Yan", "f");
                leg->AddEntry(h_qcd, "QCD", "f");
                leg->AddEntry(h_ttbar, "t#bar{t}", "f");
                leg->Draw();
            }

            // Labels
            TLatex latex;
            latex.SetNDC();
            latex.SetTextSize(0.055);
            latex.SetTextFont(62);

            TString label = view.label;
            latex.DrawLatex(0.20, 0.92, label);

            latex.SetTextAlign(31);
            latex.SetTextSize(0.045);
            latex.DrawLatex(0.92, 0.92, "54.9 fb^{-1} (13 TeV)");

            // Draw ratio plot
            pad2->cd();

            TH1F *h_ratio = (TH1F*)h_data->Clone("h_ratio_" + name + suffix);
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

            // Print event counts
            if (name == "mass") {
                std::cout << "===== " << label << " =====" << std::endl;
                std::cout << "Data:   " << h_data->GetSumOfWeights() << std::endl;
                std::cout << "QCD:    " << h_qcd->GetSumOfWeights() << std::endl;
                std::cout << "DY:     " << h_dy->GetSumOfWeights() << std::endl;
                std::cout << "ttbar:  " << h_ttbar->GetSumOfWeights() << std::endl;
                std::cout << "Signal: " << h_signal->GetSumOfWeights() << std::endl;
                std::cout << "MC sum: " << h_mcsum->GetSumOfWeights() << std::endl;
                std::cout << "========================" << std::endl;
            }
        }

        c->Update();
        c->SaveAs("plot_comparison_" + name + ".png");
    }

    // Also make rapidity matching and mass difference (only with proton cuts)
    // NOTE: Data/QCD use _proton_vars files which have array branches (proton_multi_xi)
    //       MC/Signal use _pileup_protons files which have scalar branches (xi_arm1_1, xi_arm2_1)
    //       For derived variables, we can only use MC/Signal (scalar xi branches)
    PlotDef derived_plots[] = {
        {"sist_rap - 0.5*log(xi_arm1_1/xi_arm2_1)", "rap_match", "Rapidity Matching;y_{#mu#tau} - y_{pp};Events", 10, -2.2, 2.2},
        {"sist_mass - 13000*sqrt(xi_arm1_1*xi_arm2_1)", "mass_diff", "Mass Difference;m_{#mu#tau} - m_{pp} [GeV];Events", 10, -1500, 200},
    };

    for (int p = 0; p < 2; p++) {
        TString var = derived_plots[p].var;
        TString name = derived_plots[p].name;
        TString title = derived_plots[p].title;
        int nbins = derived_plots[p].nbins;
        double xmin = derived_plots[p].xmin;
        double xmax = derived_plots[p].xmax;

        TCanvas *c = new TCanvas("c_" + name, "", 700, 700);
        c->SetLeftMargin(0.15);
        c->SetRightMargin(0.05);
        c->SetTopMargin(0.10);
        c->SetBottomMargin(0.13);

        // These require valid xi values (scalar branches)
        TString cut_dy_der   = "(sist_mass > 0 && xi_arm1_1 > 0 && xi_arm2_1 > 0) * weight";
        TString cut_tt_der   = "(sist_mass > 0 && xi_arm1_1 > 0 && xi_arm2_1 > 0) * weight";
        TString cut_signal = "(sist_mass > 0 && xi_arm1_1 > 0 && xi_arm2_1 > 0) * weight";

        // Create histograms (no data/QCD for derived variables - they have array xi branches)
        TH1F *h_dy     = new TH1F("h_dy_"     + name, title, nbins, xmin, xmax);
        TH1F *h_ttbar  = new TH1F("h_ttbar_"  + name, title, nbins, xmin, xmax);
        TH1F *h_signal = new TH1F("h_signal_" + name, title, nbins, xmin, xmax);

        // Fill (MC and signal only)
        if (!t_dy_mix || !t_ttbar_mix) {
            std::cerr << "ERROR: Missing DY/ttbar mixed trees for derived plot " << name << std::endl;
            continue;
        }

        t_dy_mix->Draw(var + " >> h_dy_" + name, cut_dy_der, "goff");
        t_ttbar_mix->Draw(var + " >> h_ttbar_" + name, cut_tt_der, "goff");
        t_signal->Draw(var + " >> h_signal_" + name, cut_signal, "goff");

        // Scale MC
        h_dy->Scale(DY_SCALE);
        h_ttbar->Scale(TTBAR_SCALE);

        // Style
        h_dy->SetFillColor(kYellow);
        h_dy->SetLineWidth(0);

        h_ttbar->SetFillColor(kGreen);
        h_ttbar->SetLineWidth(0);

        h_signal->SetLineColor(kBlack);
        h_signal->SetLineWidth(3);
        h_signal->SetFillStyle(0);

        // Stack (MC only, no data/QCD)
        THStack *stack = new THStack("stack_" + name, title);
        stack->Add(h_ttbar);
        stack->Add(h_dy);

        // MC sum
        TH1F *h_mcsum = (TH1F*)h_dy->Clone("h_mcsum_" + name);
        h_mcsum->Add(h_ttbar);
        h_mcsum->SetFillColor(kGray + 1);
        h_mcsum->SetFillStyle(3354);
        h_mcsum->SetMarkerSize(0);
        for (int i = 1; i <= h_mcsum->GetNbinsX(); i++) {
            double content = h_mcsum->GetBinContent(i);
            h_mcsum->SetBinError(i, sqrt(content));
        }

        // Auto-scale
        double maxY = h_signal->GetMaximum();
        if (stack->GetMaximum() > maxY) maxY = stack->GetMaximum();

        // Draw
        stack->Draw("hist");
        stack->GetXaxis()->SetTitle(title.Tokenize(";")->At(1)->GetName());
        stack->GetYaxis()->SetTitle("Events");
        stack->GetXaxis()->SetTitleSize(0.04);
        stack->GetYaxis()->SetTitleSize(0.04);
        stack->GetXaxis()->SetLabelSize(0.035);
        stack->GetYaxis()->SetLabelSize(0.035);
        stack->SetMinimum(0.1);
        stack->SetMaximum(maxY * 1.5);

        h_signal->Draw("hist same");
        h_mcsum->Draw("e2 same");

        // Legend
        TLegend *leg = new TLegend(0.55, 0.65, 0.92, 0.88);
        leg->SetBorderSize(0);
        leg->SetFillStyle(0);
        leg->AddEntry(h_signal, "Signal", "l");
        leg->AddEntry(h_dy, "Drell-Yan", "f");
        leg->AddEntry(h_ttbar, "t#bar{t}", "f");
        leg->AddEntry((TObject*)0, "(Data/QCD: array xi)", "");
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
    }

    std::cout << "All plots saved!" << std::endl;
}
