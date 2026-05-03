#include <TFile.h>
#include <TH1.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <TROOT.h>
#include <TStyle.h>
#include <TColor.h>
#include <THStack.h>
#include <TApplication.h>
#include <TAxis.h>
#include <cmath>
#include <iostream>

int main() { // plots bdt output - MuTau channel

    TApplication app("app",NULL,NULL);

    // Open files with BDT output histograms
    TFile *fileDY = TFile::Open("TMVApp_DY.root");
    TFile *fileQCD = TFile::Open("TMVApp_QCD.root");
    TFile *fileTTJets = TFile::Open("TMVApp_ttjets.root");
    TFile *fileSinal = TFile::Open("TMVApp_sinal.root");
    TFile *fileDados = TFile::Open("TMVApp_data.root");

    // Extract BDT histograms
    TH1F *histDY = (TH1F*)fileDY->Get("MVA_BDT");
    TH1F *histQCD = (TH1F*)fileQCD->Get("MVA_BDT");
    TH1F *histTTJets = (TH1F*)fileTTJets->Get("MVA_BDT");
    TH1F *histSinal = (TH1F*)fileSinal->Get("MVA_BDT");
    TH1F *histDados = (TH1F*)fileDados->Get("MVA_BDT");

    TH1F sum_bkg;

    histDY->Rebin(4);
    histQCD->Rebin(4);
    histTTJets->Rebin(4);
    histSinal->Rebin(4);
    histDados->Rebin(4);

    // MC normalization
    // DY needs extra scale: DY_SCALE = 1.004e-4
    // ttbar: already normalized (TTBAR_SCALE = 1.0)
    // QCD: data-driven, no extra scaling
    // Signal: scaled x5000 for visibility
    histDY->Scale(1.004e-4);
    histSinal->Scale(5000.0);

    // Print yields for diagnostics
    std::cout << "===== BDT Output Yields =====" << std::endl;
    std::cout << "Data:   " << histDados->GetSumOfWeights() << std::endl;
    std::cout << "QCD:    " << histQCD->GetSumOfWeights() << std::endl;
    std::cout << "DY:     " << histDY->GetSumOfWeights() << std::endl;
    std::cout << "ttbar:  " << histTTJets->GetSumOfWeights() << std::endl;
    std::cout << "Signal: " << histSinal->GetSumOfWeights() << std::endl;
    std::cout << "=============================" << std::endl;

    sum_bkg = *histDY;
    sum_bkg.Add(histQCD);
    sum_bkg.Add(histTTJets);
    sum_bkg.SetFillColor(kGray+1);
    sum_bkg.SetFillStyle(3354);

    for (int i = 1; i <= sum_bkg.GetNbinsX(); ++i) {
        double content = sum_bkg.GetBinContent(i);
        double error = sqrt(content);
        sum_bkg.SetBinError(i, error);
    }

    // Create canvas
    TCanvas *canvas = new TCanvas("canvas", "Stacked Histogram", 800, 600);

    // Create stack
    THStack *stack = new THStack("stack", "Stacked Histogram");

    // Set colors
    histDY->SetFillColor(kYellow);
    histQCD->SetFillColor(kRed);
    histTTJets->SetFillColor(kGreen);
    histDY->SetLineColor(kYellow);
    histQCD->SetLineColor(kRed);
    histTTJets->SetLineColor(kGreen);
    histSinal->SetLineColor(kBlack);
    histSinal->SetLineWidth(3);
    histDados->SetLineColor(kBlack);
    histDados->SetLineWidth(2);

    // Add backgrounds to stack
    stack->Add(histDY);
    stack->Add(histQCD);
    stack->Add(histTTJets);

    // Draw
    stack->SetMinimum(0.01);
    stack->SetMaximum(10000);

    canvas->SetLogy();

    stack->Draw("histo");
    histSinal->Draw("histo && same");
    histDados->Draw("e && same");
    sum_bkg.Draw("same && E2");

    // Legend
    // TLegend *legend = new TLegend(0.7, 0.7, 0.9, 0.9);
    // legend->AddEntry(histQCD, "QCD contribution", "f");
    // legend->AddEntry(histDY, "DY contribution", "f");
    // legend->AddEntry(histTTJets, "TTJets contribution", "f");
    // legend->AddEntry(histSinal, "Signal (x5000)", "l");
    // legend->AddEntry(histDados, "Data", "l");
    // legend->SetBorderSize(1);
    // legend->SetFillColor(0);
    // legend->Draw();
    // stack->GetXaxis()->SetTitle("BDT output");
    // stack->GetYaxis()->SetTitle("Arbitrary units");

    // Save
    canvas->Draw();
    canvas->SaveAs("bdt_output.png");

    bool interactive = false;

    if (interactive) {
        app.Run(true);
    }

}
