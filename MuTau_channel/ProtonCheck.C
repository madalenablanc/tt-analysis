#include "TFile.h"
#include "TTree.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TLatex.h"
#include "TStyle.h"
#include "TString.h"
#include <iostream>

void ProtonCheck()
{
    gStyle->SetOptStat(0);

    // Open files
    // Data with real protons (_proton_vars)
    TFile *f_data_real = TFile::Open("/eos/user/m/mblancco/samples_2018_mutau/Data_2018_UL_MuTau_nano_merged_proton_vars.root");
    // MC with pileup protons (_pileup_protons)
    TFile *f_dy_pu = TFile::Open("/eos/user/m/mblancco/samples_2018_mutau/DY_2018_UL_MuTau_nano_merged_pileup_protons.root");
    TFile *f_ttbar_pu = TFile::Open("/eos/user/m/mblancco/samples_2018_mutau/ttjets_2018_UL_MuTau_nano_merged_pileup_protons.root");

    TTree *t_data = (TTree*)f_data_real->Get("tree");
    TTree *t_dy = (TTree*)f_dy_pu->Get("tree");
    TTree *t_ttbar = (TTree*)f_ttbar_pu->Get("tree");

    // ===== 1. Number of protons per arm (Data) =====
    TCanvas *c1 = new TCanvas("c1", "Proton multiplicity", 1200, 500);
    c1->Divide(2, 1);

    c1->cd(1);
    TH1F *h_nproton_arm0 = new TH1F("h_nproton_arm0", "Protons in Arm 0 (Data);N protons;Events", 5, 0, 5);
    TH1F *h_nproton_arm1 = new TH1F("h_nproton_arm1", "Protons in Arm 1 (Data);N protons;Events", 5, 0, 5);
    h_nproton_arm0->SetLineColor(kBlue);
    h_nproton_arm0->SetLineWidth(2);
    h_nproton_arm1->SetLineColor(kRed);
    h_nproton_arm1->SetLineWidth(2);
    t_data->Draw("Sum$(proton_multi_arm==0) >> h_nproton_arm0", "", "goff");
    t_data->Draw("Sum$(proton_multi_arm==1) >> h_nproton_arm1", "", "goff");
    h_nproton_arm0->Draw("hist");
    h_nproton_arm1->Draw("hist same");
    TLegend *leg1 = new TLegend(0.6, 0.7, 0.88, 0.88);
    leg1->AddEntry(h_nproton_arm0, "Arm 0 (sector 45)", "l");
    leg1->AddEntry(h_nproton_arm1, "Arm 1 (sector 56)", "l");
    leg1->SetBorderSize(0);
    leg1->Draw();

    c1->cd(2);
    // 2D: N protons arm0 vs arm1
    TH2F *h_nproton_2d = new TH2F("h_nproton_2d", "Proton multiplicity (Data);N protons Arm 0;N protons Arm 1", 5, 0, 5, 5, 0, 5);
    t_data->Draw("Sum$(proton_multi_arm==1):Sum$(proton_multi_arm==0) >> h_nproton_2d", "", "goff");
    h_nproton_2d->Draw("colz text");

    c1->SaveAs("proton_multiplicity_data.png");

    // ===== 2. Xi distributions comparison: Data vs MC =====
    TCanvas *c2 = new TCanvas("c2", "Xi distributions", 1200, 500);
    c2->Divide(2, 1);

    // For data: proton_multi_xi is the xi value array, proton_multi_arm is arm array
    // For MC: use xi_arm1_1, xi_arm2_1 directly
    c2->cd(1);
    TH1F *h_xi_arm1_data = new TH1F("h_xi_arm1_data", "Xi Arm 0;#xi;Events (normalized)", 50, 0, 0.2);
    TH1F *h_xi_arm1_dy = new TH1F("h_xi_arm1_dy", "", 50, 0, 0.2);
    h_xi_arm1_data->SetLineColor(kBlack);
    h_xi_arm1_data->SetMarkerStyle(20);
    h_xi_arm1_data->SetMarkerSize(0.8);
    h_xi_arm1_dy->SetLineColor(kRed);
    h_xi_arm1_dy->SetLineWidth(2);
    h_xi_arm1_dy->SetFillColor(kRed);
    h_xi_arm1_dy->SetFillStyle(3004);

    // Data: use proton_multi_xi for arm 0 protons
    t_data->Draw("proton_multi_xi >> h_xi_arm1_data", "proton_multi_arm==0", "goff");
    t_dy->Draw("xi_arm1_1 >> h_xi_arm1_dy", "xi_arm1_1 > 0", "goff");

    if (h_xi_arm1_data->Integral() > 0) h_xi_arm1_data->Scale(1.0/h_xi_arm1_data->Integral());
    if (h_xi_arm1_dy->Integral() > 0) h_xi_arm1_dy->Scale(1.0/h_xi_arm1_dy->Integral());
    double max1 = std::max(h_xi_arm1_data->GetMaximum(), h_xi_arm1_dy->GetMaximum());
    h_xi_arm1_dy->SetMaximum(max1 * 1.3);
    h_xi_arm1_dy->Draw("hist");
    h_xi_arm1_data->Draw("e same");

    TLegend *leg2 = new TLegend(0.55, 0.7, 0.88, 0.88);
    leg2->AddEntry(h_xi_arm1_data, "Data (real protons)", "lep");
    leg2->AddEntry(h_xi_arm1_dy, "DY MC (pileup protons)", "f");
    leg2->SetBorderSize(0);
    leg2->Draw();

    TLatex latex;
    latex.SetNDC();
    latex.SetTextSize(0.04);
    latex.DrawLatex(0.15, 0.92, "Arm 0 (sector 45)");

    c2->cd(2);
    TH1F *h_xi_arm2_data = new TH1F("h_xi_arm2_data", "Xi Arm 1;#xi;Events (normalized)", 50, 0, 0.2);
    TH1F *h_xi_arm2_dy = new TH1F("h_xi_arm2_dy", "", 50, 0, 0.2);
    h_xi_arm2_data->SetLineColor(kBlack);
    h_xi_arm2_data->SetMarkerStyle(20);
    h_xi_arm2_data->SetMarkerSize(0.8);
    h_xi_arm2_dy->SetLineColor(kRed);
    h_xi_arm2_dy->SetLineWidth(2);
    h_xi_arm2_dy->SetFillColor(kRed);
    h_xi_arm2_dy->SetFillStyle(3004);

    t_data->Draw("proton_multi_xi >> h_xi_arm2_data", "proton_multi_arm==1", "goff");
    t_dy->Draw("xi_arm2_1 >> h_xi_arm2_dy", "xi_arm2_1 > 0", "goff");

    if (h_xi_arm2_data->Integral() > 0) h_xi_arm2_data->Scale(1.0/h_xi_arm2_data->Integral());
    if (h_xi_arm2_dy->Integral() > 0) h_xi_arm2_dy->Scale(1.0/h_xi_arm2_dy->Integral());
    double max2 = std::max(h_xi_arm2_data->GetMaximum(), h_xi_arm2_dy->GetMaximum());
    h_xi_arm2_dy->SetMaximum(max2 * 1.3);
    h_xi_arm2_dy->Draw("hist");
    h_xi_arm2_data->Draw("e same");

    TLegend *leg3 = new TLegend(0.55, 0.7, 0.88, 0.88);
    leg3->AddEntry(h_xi_arm2_data, "Data (real protons)", "lep");
    leg3->AddEntry(h_xi_arm2_dy, "DY MC (pileup protons)", "f");
    leg3->SetBorderSize(0);
    leg3->Draw();

    latex.DrawLatex(0.15, 0.92, "Arm 1 (sector 56)");

    c2->SaveAs("proton_xi_comparison.png");

    // ===== 3. Proton acceptance check (MC) =====
    TCanvas *c3 = new TCanvas("c3", "MC proton check", 800, 600);

    TH1F *h_mc_check = new TH1F("h_mc_check", "MC Pileup Protons Check;Category;Events", 4, 0, 4);
    h_mc_check->GetXaxis()->SetBinLabel(1, "Total");
    h_mc_check->GetXaxis()->SetBinLabel(2, "#xi_{1} > 0");
    h_mc_check->GetXaxis()->SetBinLabel(3, "#xi_{2} > 0");
    h_mc_check->GetXaxis()->SetBinLabel(4, "Both > 0");

    double n_total = t_dy->GetEntries();
    double n_xi1 = t_dy->GetEntries("xi_arm1_1 > 0");
    double n_xi2 = t_dy->GetEntries("xi_arm2_1 > 0");
    double n_both = t_dy->GetEntries("xi_arm1_1 > 0 && xi_arm2_1 > 0");

    h_mc_check->SetBinContent(1, n_total);
    h_mc_check->SetBinContent(2, n_xi1);
    h_mc_check->SetBinContent(3, n_xi2);
    h_mc_check->SetBinContent(4, n_both);

    h_mc_check->SetFillColor(kBlue);
    h_mc_check->Draw("hist");

    latex.SetTextSize(0.035);
    latex.DrawLatex(0.15, 0.85, Form("Total: %.0f", n_total));
    latex.DrawLatex(0.15, 0.80, Form("#xi_{arm1} > 0: %.0f (%.1f%%)", n_xi1, 100*n_xi1/n_total));
    latex.DrawLatex(0.15, 0.75, Form("#xi_{arm2} > 0: %.0f (%.1f%%)", n_xi2, 100*n_xi2/n_total));
    latex.DrawLatex(0.15, 0.70, Form("Both > 0: %.0f (%.1f%%)", n_both, 100*n_both/n_total));

    c3->SaveAs("proton_mc_check.png");

    // ===== 4. Data proton acceptance =====
    TCanvas *c4 = new TCanvas("c4", "Data proton check", 800, 600);

    TH1F *h_data_check = new TH1F("h_data_check", "Data Proton Check;Category;Events", 4, 0, 4);
    h_data_check->GetXaxis()->SetBinLabel(1, "Total");
    h_data_check->GetXaxis()->SetBinLabel(2, "Arm0 > 0");
    h_data_check->GetXaxis()->SetBinLabel(3, "Arm1 > 0");
    h_data_check->GetXaxis()->SetBinLabel(4, "Both > 0");

    double d_total = t_data->GetEntries();
    double d_arm0 = t_data->GetEntries("Sum$(proton_multi_arm==0) > 0");
    double d_arm1 = t_data->GetEntries("Sum$(proton_multi_arm==1) > 0");
    double d_both = t_data->GetEntries("Sum$(proton_multi_arm==0) > 0 && Sum$(proton_multi_arm==1) > 0");

    h_data_check->SetBinContent(1, d_total);
    h_data_check->SetBinContent(2, d_arm0);
    h_data_check->SetBinContent(3, d_arm1);
    h_data_check->SetBinContent(4, d_both);

    h_data_check->SetFillColor(kGreen+2);
    h_data_check->Draw("hist");

    latex.DrawLatex(0.15, 0.85, Form("Total: %.0f", d_total));
    latex.DrawLatex(0.15, 0.80, Form("Arm0 > 0: %.0f (%.1f%%)", d_arm0, 100*d_arm0/d_total));
    latex.DrawLatex(0.15, 0.75, Form("Arm1 > 0: %.0f (%.1f%%)", d_arm1, 100*d_arm1/d_total));
    latex.DrawLatex(0.15, 0.70, Form("Both > 0: %.0f (%.1f%%)", d_both, 100*d_both/d_total));
    latex.SetTextColor(kRed);
    latex.DrawLatex(0.15, 0.63, Form("Acceptance = %.1f%% (expected ~24.5%%)", 100*d_both/d_total));

    c4->SaveAs("proton_data_check.png");

    // ===== 5. Xi1 vs Xi2 (2D) - MC only since data arrays are harder to correlate =====
    TCanvas *c5 = new TCanvas("c5", "Xi correlations (MC)", 800, 600);

    TH2F *h_xi_2d_mc = new TH2F("h_xi_2d_mc", "MC (pileup protons): #xi correlations;#xi_{arm1_1};#xi_{arm2_1}", 50, 0, 0.2, 50, 0, 0.2);
    t_dy->Draw("xi_arm2_1:xi_arm1_1 >> h_xi_2d_mc", "xi_arm1_1 > 0 && xi_arm2_1 > 0", "goff");
    h_xi_2d_mc->Draw("colz");

    c5->SaveAs("proton_xi_2d.png");

    // Print summary
    std::cout << "\n===== SUMMARY =====" << std::endl;
    std::cout << "Data total events: " << d_total << std::endl;
    std::cout << "Data with both arms: " << d_both << " (" << 100*d_both/d_total << "%)" << std::endl;
    std::cout << "\nMC (DY) total events: " << n_total << std::endl;
    std::cout << "MC with both xi > 0: " << n_both << " (" << 100*n_both/n_total << "%)" << std::endl;
    std::cout << "\nExpected proton acceptance: 24.5%" << std::endl;
    std::cout << "===================" << std::endl;

    std::cout << "\nAll plots saved!" << std::endl;
}