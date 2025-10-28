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

int main() { // plots bdt output

    TApplication app("app",NULL,NULL);

    // Abrir os arquivos contendo os histogramas
    TFile *fileDY = TFile::Open("TMVApp_DY.root");
    TFile *fileQCD = TFile::Open("TMVApp_QCD.root");
    TFile *fileTTJets = TFile::Open("TMVApp_ttjets.root");
    TFile *fileSinal = TFile::Open("TMVApp_sinal.root");
    TFile *fileDados = TFile::Open("TMVApp_dados.root");

    // Extrair os histogramas dos arquivos
    TH1F *histDY = (TH1F*)fileDY->Get("MVA_BDT");
    TH1F *histQCD = (TH1F*)fileQCD->Get("MVA_BDT");
    TH1F *histTTJets = (TH1F*)fileTTJets->Get("MVA_BDT");
    TH1F *histSinal = (TH1F*)fileSinal->Get("MVA_BDT");
    TH1F *histDados = (TH1F*)fileDados->Get("MVA_BDT");

    TH1F sum_bkg;

    histDY->Rebin(4); // Reagrupa em 20 bins (80/4 = 20)
    histQCD->Rebin(4);
    histTTJets->Rebin(4);
    histSinal->Rebin(4);
    histDados->Rebin(4);

    // Adjust scaling factors for MuTau channel - using approximate values
    // These should be adjusted based on actual cross-sections and luminosity
    histDY->Scale(1.81/histDY->Integral()); // DY weight
    histQCD->Scale(1.0/histQCD->Integral()); // QCD weight
    histTTJets->Scale(0.15/histTTJets->Integral()); // TTJets weight
    histSinal->Scale(8.89e-2*5000/histSinal->Integral()); // Signal scaled by 5000
    //    histDados->Scale(0.13);

    sum_bkg=*histDY;
    sum_bkg.Add(histQCD);
    sum_bkg.Add(histTTJets);
    sum_bkg.SetFillColor(kGray+1);
    sum_bkg.SetFillStyle(3354);

    for (int i = 1; i <= sum_bkg.GetNbinsX(); ++i) {
       double content = sum_bkg.GetBinContent(i);
       double error = sqrt(content);
       sum_bkg.SetBinError(i, error);
    }


    // Criar um TCanvas para desenhar o histograma
    TCanvas *canvas = new TCanvas("canvas", "Stacked Histogram", 800, 600);

    // Criar um TStack para sobrepor os histogramas
    THStack *stack = new THStack("stack", "Stacked Histogram");
    stack->SetMinimum(0.01);
    stack->SetMaximum(5000);

    // Definir cores para os histogramas
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

    //histSinal->Scale(100);

    // Adicionar os histogramas ao TStack
    stack->Add(histDY);
    stack->Add(histQCD);
    stack->Add(histTTJets);

    // Desenhar o TStack
    // For log scale, must be > 0


    canvas->SetLogy();

    stack->Draw("histo"); // "nostack" para sobrepor
    histSinal->Draw("histo && same");
    histDados->Draw("e && same");
    sum_bkg.Draw("same && E2");

    // Criar a legenda
    TLegend *legend = new TLegend(0.7, 0.7, 0.9, 0.9);
    legend->AddEntry(histQCD, "QCD contribution", "f");
    legend->AddEntry(histDY, "DY contribution", "f");
    legend->AddEntry(histTTJets, "TTJets contribution", "f");
    legend->AddEntry(histSinal, "Signal (x5000)", "l");
    legend->AddEntry(histDados, "Data", "l");
    legend->SetBorderSize(1);
    legend->SetFillColor(0);
    legend->Draw();
    stack->GetXaxis()->SetTitle("BDT output");
    stack->GetYaxis()->SetTitle("Arbitrary units");

    // Mostrar o canvas
    canvas->Draw();
    canvas->SaveAs("bdt_output_mutau.png");

    bool interactive = false;

    if (interactive) {
        app.Run(true);
    }


}
