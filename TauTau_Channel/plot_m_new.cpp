// ROOT macro to read input files, fill histograms, and save styled plots
#include <iostream>
#include <cmath>
#include <vector>
#include <string>
#include "TApplication.h"
#include "THStack.h"
#include "TH1D.h"
#include "TCanvas.h"
#include "TFile.h"
#include "TTree.h"
#include "TPad.h"
#include "TLegend.h"
#include "TLatex.h"
#include "TStyle.h"
#include "TSystem.h"

using namespace std;

void stylePlot(THStack* stack) {
    stack->GetXaxis()->SetTitleSize(0.045);
    stack->GetYaxis()->SetTitleSize(0.045);
    stack->GetXaxis()->SetLabelSize(0.04);
    stack->GetYaxis()->SetLabelSize(0.04);
    stack->GetXaxis()->SetTitleOffset(1.2);
    stack->GetYaxis()->SetTitleOffset(1.6);
    stack->GetXaxis()->SetLabelOffset(0.01);
    stack->GetYaxis()->SetLabelOffset(0.01);
    stack->GetXaxis()->SetNdivisions(505);
    stack->GetYaxis()->SetNdivisions(505);
}

void styleCanvas(TCanvas& c) {
    c.SetCanvasSize(700, 800);
    c.cd();
    gPad->SetLeftMargin(0.15);
    gPad->SetRightMargin(0.05);
    gPad->SetTopMargin(0.10);
    gPad->SetBottomMargin(0.13);
    gPad->SetTicks();
    gPad->Update();
}

void drawAndSavePlot(TCanvas& canvas, THStack* stack, TH1D* signal, TH1D* data, TH1D* errorBand,
    TLegend* legend, const string& xAxisTitle, const string& filename) {
styleCanvas(canvas);

// ✨ Draw once so axes are created
stack->Draw("HIST");
canvas.Update();  // <--- make sure ROOT generates axis

// Now apply style
stylePlot(stack);

signal->Draw("SAME HIST");
data->Draw("SAME E1");
errorBand->Draw("SAME E2");
legend->Draw();

stack->GetXaxis()->SetTitle(xAxisTitle.c_str());
stack->GetYaxis()->SetTitle("Events");
signal->SetLineWidth(3);

TLatex label1, label2;
label1.SetTextAlign(31);
label1.SetTextSize(0.04);
label1.SetTextFont(62);
label1.DrawLatexNDC(0.90, 0.92, "54.9 fb^{-1} (13 TeV)");

label2.SetTextAlign(31);
label2.SetTextSize(0.04);
label2.SetTextFont(62);
label2.DrawLatexNDC(0.5, 0.92, "CMS-TOTEM Preliminary");

canvas.Update();
canvas.SaveAs(filename.c_str());
}


int main() {
    TApplication app("app", nullptr, nullptr);
    gSystem->mkdir("output_plots", true);

    // Input files (update these paths as needed)
    // TFile data("/eos/cms/store/user/jjhollar/TauTau_NanoAOD_Madalena/BackgroundSamples/TauhTauh/Dados_2018_UL_skimmed_TauTau.root");
    // TFile dy("/eos/cms/store/user/jjhollar/TauTau_NanoAOD_Madalena/BackgroundSamples/TauhTauh/DY_2018_UL_skimmed_TauTau.root");
    // TFile ttjets("/eos/cms/store/user/jjhollar/TauTau_NanoAOD_Madalena/BackgroundSamples/TauhTauh/ttJets_2018_UL_skimmed_TauTau.root");
    // TFile qcd("/eos/cms/store/user/jjhollar/TauTau_NanoAOD_Madalena/BackgroundSamples/TauhTauh/QCD_2018_UL_skimmed_TauTau.root");
    // TFile sinal("/eos/cms/store/user/jjhollar/TauTau_NanoAOD_Madalena/BackgroundSamples/TauhTauh/TauTau_sinal_PIC_july_2018.root");
    TFile data("/eos/cms/store/user/jjhollar/TauTau_NanoAOD_Madalena/BackgroundSamples/TauhTauh/Dados_2018_UL_skimmed_TauTau_nano_fase1total-protons_2018.root");;
    TFile dy("/eos/cms/store/user/jjhollar/TauTau_NanoAOD_Madalena/BackgroundSamples/TauhTauh/DY_2018_UL_skimmed_TauTau_nano_fase1total-protons_2018.root");
    TFile ttjets("/eos/cms/store/user/jjhollar/TauTau_NanoAOD_Madalena/BackgroundSamples/TauhTauh/ttJets_2018_UL_skimmed_TauTau_nano_fase1total-protons_2018.root");
    TFile qcd("/eos/cms/store/user/jjhollar/TauTau_NanoAOD_Madalena/BackgroundSamples/TauhTauh/QCD_2018_UL_skimmed_TauTau_nano_fase1total-protons_2018.root");
	TFile sinal("TauTau_sinal_PIC_july_2018.root");
    


    TTree* tree_data = (TTree*)data.Get("tree");
    TTree* tree_dy = (TTree*)dy.Get("tree");
    TTree* tree_ttjets = (TTree*)ttjets.Get("tree");
    TTree* tree_qcd = (TTree*)qcd.Get("tree");
    TTree* tree_signal = (TTree*)sinal.Get("tree");

    // Variables to fill histograms
    float var;
    const int bins = 10;
    float xmin = 0.0, xmax = 1.0;

    // Histograms per plot (reusing structure from previous script)
    vector<string> names = {"aco", "mass", "rapidity_matching", "pt_central",
                            "mass_diff", "rapidity_central", "tau_pt", "met"};

    vector<string> xTitles = {
        "Acoplanarity of the central system",
        "Invariant mass of the central system [GeV]",
        "Rapidity matching",
        "Transverse momentum of the central system [GeV]",
        "Mass difference (CMS-PPS) [GeV]",
        "Rapidity of the central system",
        "Transverse momentum of the hadronic tau [GeV]",
        "MET [GeV]"
    };

    vector<THStack*> stacks;
    vector<TH1D*> signals;
    vector<TH1D*> datas;
    vector<TH1D*> errors;
    vector<TLegend*> legends;
    vector<TCanvas*> canvases;

    for (int i = 0; i < 8; i++) {
        stacks.push_back(new THStack(Form("stack_%d", i), ""));
        signals.push_back(new TH1D(Form("signal_%d", i), "", bins, xmin, xmax));
        datas.push_back(new TH1D(Form("data_%d", i), "", bins, xmin, xmax));
        errors.push_back(new TH1D(Form("error_%d", i), "", bins, xmin, xmax));
        legends.push_back(new TLegend(0.18, 0.7, 0.48, 0.9));
        legends[i]->SetBorderSize(0);
        legends[i]->AddEntry(signals[i], "Signal (x500)", "l");
        legends[i]->AddEntry(datas[i], "Data", "lep");
        legends[i]->AddEntry(errors[i], "Uncertainty", "f");
        canvases.push_back(new TCanvas(Form("c%d", i + 1)));
    }

    // Fill with example values
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 100; j++) {
            float value = (float)rand() / RAND_MAX * (xmax - xmin) + xmin;
            datas[i]->Fill(value);
            signals[i]->Fill(value, 0.5);
            errors[i]->Fill(value);
        }
        stacks[i]->Add(datas[i]);
    }

    for (int i = 0; i < 8; i++) {
        string path = "output_plots/" + names[i] + ".png";
        drawAndSavePlot(*canvases[i], stacks[i], signals[i], datas[i], errors[i], legends[i], xTitles[i], path);
    }

    return 0;
}
