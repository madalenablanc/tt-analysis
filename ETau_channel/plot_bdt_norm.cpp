#include <iostream>
#include <cmath>
#include <TFile.h>
#include <TH1.h>
#include <TCanvas.h>
#include <TApplication.h>


using namespace std;


int main() {

    TApplication app("app",NULL,NULL);

    // Abre o arquivo .root
    TFile *file = TFile::Open("Output_TMVA.root");

    // Obtém o histograma do arquivo
    TH1D *histogram_s = nullptr;
    TH1D *histogram_b = nullptr;
    TH1D *histogram_s_t = nullptr;
    TH1D *histogram_b_t = nullptr;

    file->GetObject("dataset/Method_BDT/BDT/MVA_BDT_S", histogram_s);
    file->GetObject("dataset/Method_BDT/BDT/MVA_BDT_B", histogram_b);
    file->GetObject("dataset/Method_BDT/BDT/MVA_BDT_Train_S", histogram_s_t);
    file->GetObject("dataset/Method_BDT/BDT/MVA_BDT_Train_B", histogram_b_t);
    // Define o valor para normalizar (substitua com seu valor desejado)
    double normValue_s = 8.89e-2*1000;
    double normValue_b = 104.0;

    histogram_s->Scale(normValue_s / histogram_s->Integral());
    histogram_b->Scale(normValue_b / histogram_b->Integral());
    histogram_s_t->Scale(normValue_s / histogram_s_t->Integral());
    histogram_b_t->Scale(normValue_b / histogram_b_t->Integral());

    histogram_s->Rebin(8);
    histogram_s_t->Rebin(8);
    histogram_b->Rebin(8);
    histogram_b_t->Rebin(8);

    histogram_s->SetStats(0);
    histogram_s_t->SetStats(0);
    histogram_b->SetStats(0);
    histogram_b_t->SetStats(0);

    histogram_b->SetLineWidth(0);
    histogram_b->SetFillStyle(3001);
    histogram_b->SetFillColor(kRed);

    histogram_s->SetLineWidth(3);
    histogram_s->SetLineColor(kBlack);
    histogram_s->SetMarkerStyle(0);

    histogram_s_t->SetLineWidth(2);
    histogram_s_t->SetMarkerStyle(20);
    histogram_s_t->SetLineColor(kBlack);
    histogram_b_t->SetMarkerColor(kBlue);
    histogram_b_t->SetLineWidth(2);
    histogram_b_t->SetMarkerStyle(20);
    histogram_b_t->SetMarkerColor(kRed);
    histogram_b_t->SetLineColor(kRed);

    histogram_s->GetXaxis()->SetTitle("Output BDT");
    histogram_s->GetYaxis()->SetTitle("Number of events");
    histogram_s->GetYaxis()->SetRangeUser(0,20);

    // Cria um canvas para desenhar o histograma
    TCanvas *canvas = new TCanvas("canvas", "Normalized Histogram", 800, 600);

    // Desenha o histograma
    histogram_s->Draw("HIST");
    histogram_b->Draw("HIST && same");
    histogram_s_t->Draw("e && same");
    histogram_b_t->Draw("e && same");


    app.Run(true);


    // Fecha o arquivo
    file->Close();

    return 0;
}

