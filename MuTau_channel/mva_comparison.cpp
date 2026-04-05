#include <TFile.h>
#include <TH1.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <TROOT.h>
#include <TStyle.h>
#include <THStack.h>
#include <TPad.h>
#include <TLine.h>
#include <TLatex.h>
#include <TApplication.h>
#include <TAxis.h>
#include <cmath>
#include <iostream>
#include <string>

TH1F* LoadHist(TFile* f, const char* histName, const char* newName, double scale = 1.0) {
    TH1F* h = (TH1F*)f->Get(histName);
    if (!h) {
        std::cerr << "WARNING: histogram " << histName << " not found in " << f->GetName() << std::endl;
        return nullptr;
    }
    TH1F* clone = (TH1F*)h->Clone(newName);
    clone->SetDirectory(nullptr);
    if (scale != 1.0) clone->Scale(scale);
    return clone;
}

void DrawMethod(TFile* fDY, TFile* fQCD, TFile* fTT, TFile* fSinal, TFile* fData,
                const char* histName, const char* xTitle, const char* outFile,
                TFile* fPostFit = nullptr) {

    TH1F* hDY    = LoadHist(fDY,    histName, "hDY",    1.004e-4);
    TH1F* hQCD   = LoadHist(fQCD,   histName, "hQCD");
    TH1F* hTT    = LoadHist(fTT,    histName, "hTT");
    TH1F* hSinal = LoadHist(fSinal, histName, "hSinal", 5000.0);
    TH1F* hData  = LoadHist(fData,  histName, "hData");

    if (!hDY || !hQCD || !hTT || !hSinal || !hData) {
        std::cerr << "Skipping " << histName << " due to missing histograms." << std::endl;
        return;
    }

    hDY->Rebin(4); hQCD->Rebin(4); hTT->Rebin(4);
    hSinal->Rebin(4); hData->Rebin(4);

    // ---- Total background ----
    TH1F* hBkg = (TH1F*)hDY->Clone("hBkg");
    hBkg->Add(hQCD);
    hBkg->Add(hTT);
    hBkg->SetDirectory(nullptr);

    // ---- Stat uncertainty band on total background ----
    TH1F* hBkgErr = (TH1F*)hBkg->Clone("hBkgErr");
    hBkgErr->SetDirectory(nullptr);
    hBkgErr->SetFillColor(kGray+2);
    hBkgErr->SetFillStyle(3354);
    hBkgErr->SetMarkerSize(0);
    for (int i = 1; i <= hBkg->GetNbinsX(); ++i)
        hBkgErr->SetBinError(i, sqrt(hBkg->GetBinContent(i)));

    // ---- Post-fit overlay (if provided) ----
    TH1F* hPostFit = nullptr;
    if (fPostFit) {
        std::string pfName = std::string("postfit_") + histName;
        hPostFit = LoadHist(fPostFit, pfName.c_str(), "hPostFit");
        if (hPostFit) {
            hPostFit->Rebin(4);
            hPostFit->SetLineColor(kBlue+1);
            hPostFit->SetLineWidth(2);
            hPostFit->SetLineStyle(2);
            hPostFit->SetFillStyle(0);
        }
    }

    // ---- S/sqrt(B) per bin ----
    TH1F* hSoSqrtB = (TH1F*)hSinal->Clone("hSoSqrtB");
    hSoSqrtB->SetDirectory(nullptr);
    hSoSqrtB->Reset();
    double integral_S = 0, integral_B = 0;
    for (int i = 1; i <= hBkg->GetNbinsX(); ++i) {
        double s = hSinal->GetBinContent(i) / 5000.0; // undo display scale
        double b = hBkg->GetBinContent(i);
        integral_S += s;
        integral_B += b;
        hSoSqrtB->SetBinContent(i, (b > 0) ? s / sqrt(b) : 0);
    }
    hSoSqrtB->SetLineColor(kMagenta+1);
    hSoSqrtB->SetLineWidth(2);
    hSoSqrtB->SetFillStyle(0);

    // ---- S/B per bin ----
    TH1F* hSoB = (TH1F*)hSinal->Clone("hSoB");
    hSoB->SetDirectory(nullptr);
    hSoB->Reset();
    for (int i = 1; i <= hBkg->GetNbinsX(); ++i) {
        double s = hSinal->GetBinContent(i) / 5000.0;
        double b = hBkg->GetBinContent(i);
        hSoB->SetBinContent(i, (b > 0) ? s / b : 0);
    }
    hSoB->SetLineColor(kOrange+1);
    hSoB->SetLineWidth(2);

    // ---- Print integrated significance ----
    double sig_integrated = (integral_B > 0) ? integral_S / sqrt(integral_B) : 0;
    std::cout << "\n===== " << histName << " =====" << std::endl;
    std::cout << "  Signal (unscaled integral): " << integral_S << std::endl;
    std::cout << "  Background integral:        " << integral_B << std::endl;
    std::cout << "  S/sqrt(B) integrated:       " << sig_integrated << std::endl;

    // ---- Data/MC ratio ----
    TH1F* hRatio = (TH1F*)hData->Clone("hRatio");
    hRatio->SetDirectory(nullptr);
    hRatio->Divide(hBkg);
    hRatio->SetMarkerStyle(20);
    hRatio->SetMarkerSize(0.8);
    hRatio->SetLineColor(kBlack);

    TH1F* hRatioErr = (TH1F*)hBkgErr->Clone("hRatioErr");
    hRatioErr->SetDirectory(nullptr);
    for (int i = 1; i <= hBkg->GetNbinsX(); ++i) {
        double b = hBkg->GetBinContent(i);
        hRatioErr->SetBinContent(i, 1.0);
        hRatioErr->SetBinError(i, (b > 0) ? hBkgErr->GetBinError(i) / b : 0);
    }

    // ============================================================
    // Canvas: 3 pads — main plot | ratio | S/sqrt(B)
    // ============================================================
    TCanvas* c = new TCanvas("c", xTitle, 800, 900);
    c->SetFillColor(0);

    // Top pad: main plot (60%)
    TPad* pad1 = new TPad("pad1", "pad1", 0, 0.38, 1, 1.0);
    pad1->SetBottomMargin(0.02);
    pad1->SetTopMargin(0.08);
    pad1->SetLogy();
    pad1->Draw();

    // Middle pad: data/MC ratio (22%)
    TPad* pad2 = new TPad("pad2", "pad2", 0, 0.18, 1, 0.38);
    pad2->SetTopMargin(0.02);
    pad2->SetBottomMargin(0.04);
    pad2->Draw();

    // Bottom pad: S/sqrt(B) (18%)
    TPad* pad3 = new TPad("pad3", "pad3", 0, 0.0, 1, 0.18);
    pad3->SetTopMargin(0.02);
    pad3->SetBottomMargin(0.35);
    pad3->Draw();

    // ---- Draw main plot ----
    pad1->cd();

    hDY->SetFillColor(kYellow);   hDY->SetLineColor(kYellow);
    hQCD->SetFillColor(kRed);     hQCD->SetLineColor(kRed);
    hTT->SetFillColor(kGreen);    hTT->SetLineColor(kGreen);
    hSinal->SetLineColor(kBlack); hSinal->SetLineWidth(3);
    hData->SetMarkerStyle(20);    hData->SetMarkerSize(0.8);

    THStack* stack = new THStack("stack", "");
    stack->Add(hDY);
    stack->Add(hQCD);
    stack->Add(hTT);
    stack->SetMinimum(0.01);
    stack->SetMaximum(50000);
    stack->Draw("histo");
    stack->GetXaxis()->SetLabelSize(0);
    stack->GetYaxis()->SetTitle("Events / bin");
    stack->GetYaxis()->SetTitleSize(0.06);
    stack->GetYaxis()->SetTitleOffset(0.8);

    hBkgErr->Draw("same E2");
    hSinal->Draw("histo same");
    if (hPostFit) hPostFit->Draw("histo same");
    hData->Draw("e same");

    TLegend* leg = new TLegend(0.65, 0.55, 0.92, 0.92);
    leg->SetBorderSize(1);
    leg->SetFillColor(0);
    leg->SetTextSize(0.045);
    leg->AddEntry(hQCD,    "QCD",            "f");
    leg->AddEntry(hDY,     "DY",             "f");
    leg->AddEntry(hTT,     "TTJets",         "f");
    leg->AddEntry(hBkgErr, "Stat. unc.",     "f");
    leg->AddEntry(hSinal,  "Signal (x5000)", "l");
    if (hPostFit) leg->AddEntry(hPostFit, "Post-fit", "l");
    leg->AddEntry(hData,   "Data",           "lep");
    leg->Draw();

    TLatex latex;
    latex.SetNDC();
    latex.SetTextSize(0.05);
    latex.DrawLatex(0.12, 0.93, "CMS #it{Preliminary}");
    latex.SetTextSize(0.04);
    latex.DrawLatex(0.60, 0.93, "59.7 fb^{-1} (13 TeV, 2018)");

    // ---- Draw ratio ----
    pad2->cd();
    hRatioErr->SetFillColor(kGray+1);
    hRatioErr->SetFillStyle(3354);
    hRatioErr->SetMarkerSize(0);
    hRatioErr->GetYaxis()->SetTitle("Data/MC");
    hRatioErr->GetYaxis()->SetTitleSize(0.13);
    hRatioErr->GetYaxis()->SetTitleOffset(0.35);
    hRatioErr->GetYaxis()->SetLabelSize(0.11);
    hRatioErr->GetYaxis()->SetRangeUser(0.0, 2.5);
    hRatioErr->GetYaxis()->SetNdivisions(505);
    hRatioErr->GetXaxis()->SetLabelSize(0);
    hRatioErr->Draw("E2");
    hRatio->Draw("e same");

    TLine* line = new TLine(hRatio->GetXaxis()->GetXmin(), 1.0,
                            hRatio->GetXaxis()->GetXmax(), 1.0);
    line->SetLineColor(kRed);
    line->SetLineStyle(2);
    line->Draw();

    // ---- Draw S/sqrt(B) ----
    pad3->cd();
    hSoSqrtB->GetXaxis()->SetTitle(xTitle);
    hSoSqrtB->GetXaxis()->SetTitleSize(0.18);
    hSoSqrtB->GetXaxis()->SetLabelSize(0.15);
    hSoSqrtB->GetYaxis()->SetTitle("S/#sqrt{B}");
    hSoSqrtB->GetYaxis()->SetTitleSize(0.14);
    hSoSqrtB->GetYaxis()->SetTitleOffset(0.35);
    hSoSqrtB->GetYaxis()->SetLabelSize(0.13);
    hSoSqrtB->GetYaxis()->SetNdivisions(504);
    hSoSqrtB->Draw("histo");

    c->SaveAs(outFile);
    std::cout << "  Saved " << outFile << std::endl;

    delete stack; delete leg; delete line;
    delete hBkg; delete hBkgErr; delete hRatio; delete hRatioErr;
    delete hSoSqrtB; delete hSoB; delete c;
    delete hDY; delete hQCD; delete hTT; delete hSinal; delete hData;
    if (hPostFit) delete hPostFit;
}

// Print KS test p-values for overtraining check
// A p-value > 0.01 means no significant overtraining
void PrintKSTest(const char* tmvaFile, const char* method) {
    TFile* f = TFile::Open(tmvaFile);
    if (!f || f->IsZombie()) {
        std::cerr << "WARNING: cannot open " << tmvaFile << std::endl;
        return;
    }

    std::string base = std::string("/dataset/Method_") + method + "/" + method + "/";
    TH1* hS_train = (TH1*)f->Get((base + "MVA_" + method + "_S").c_str());
    TH1* hB_train = (TH1*)f->Get((base + "MVA_" + method + "_B").c_str());
    TH1* hS_test  = (TH1*)f->Get((base + "MVA_" + method + "_S_high").c_str());
    TH1* hB_test  = (TH1*)f->Get((base + "MVA_" + method + "_B_high").c_str());

    std::cout << "\n--- KS overtraining test: " << method << " ---" << std::endl;

    auto rebinToMatch = [](TH1* ref, TH1* h) -> TH1* {
        // Rebin h to have the same number of bins as ref
        int nRef = ref->GetNbinsX();
        int nH   = h->GetNbinsX();
        if (nRef == nH) return (TH1*)h->Clone("htmp");
        if (nH % nRef == 0) {
            TH1* cl = (TH1*)h->Clone("htmp");
            cl->SetDirectory(nullptr);
            cl->Rebin(nH / nRef);
            return cl;
        }
        // fallback: rebin ref to match h
        if (nH < nRef && nRef % nH == 0) {
            TH1* cl = (TH1*)ref->Clone("htmpref");
            cl->SetDirectory(nullptr);
            cl->Rebin(nRef / nH);
            return cl;
        }
        // arbitrary: just rebin h to nRef bins via clone+rebin closest factor
        TH1* cl = (TH1*)h->Clone("htmp");
        cl->SetDirectory(nullptr);
        cl->Rebin(nH / nRef);
        return cl;
    };

    if (!hS_train || !hS_test) {
        std::cout << "  Signal:     histograms not found" << std::endl;
    } else {
        TH1* hS_test_rb = rebinToMatch(hS_train, hS_test);
        double ks_s = hS_train->KolmogorovTest(hS_test_rb);
        std::cout << "  Signal KS p-value:     " << ks_s
                  << (ks_s < 0.01 ? "  *** OVERTRAINING SUSPECTED ***" : "  OK") << std::endl;
        delete hS_test_rb;
    }

    if (!hB_train || !hB_test) {
        std::cout << "  Background: histograms not found" << std::endl;
    } else {
        TH1* hB_test_rb = rebinToMatch(hB_train, hB_test);
        double ks_b = hB_train->KolmogorovTest(hB_test_rb);
        std::cout << "  Background KS p-value: " << ks_b
                  << (ks_b < 0.01 ? "  *** OVERTRAINING SUSPECTED ***" : "  OK") << std::endl;
        delete hB_test_rb;
    }

    f->Close();
}

// Draw signal vs background separation from TMVA training output
// Reads MVA_BDT_S/B (train) and MVA_BDT_S_high/B_high (test) from the TMVA output file
void DrawTMVASeparation(const char* tmvaFile, const char* method,
                        const char* xTitle, const char* outFile) {

    TFile* f = TFile::Open(tmvaFile);
    if (!f || f->IsZombie()) {
        std::cerr << "WARNING: cannot open " << tmvaFile << std::endl;
        return;
    }

    // Paths inside TMVA output file
    std::string base = std::string("/dataset/Method_") + method + "/" + method + "/";
    TH1* hS_train = (TH1*)f->Get((base + "MVA_" + method + "_S").c_str());
    TH1* hB_train = (TH1*)f->Get((base + "MVA_" + method + "_B").c_str());
    // Test distributions (suffix _high = test events in TMVA naming)
    TH1* hS_test  = (TH1*)f->Get((base + "MVA_" + method + "_S_high").c_str());
    TH1* hB_test  = (TH1*)f->Get((base + "MVA_" + method + "_B_high").c_str());

    if (!hS_train || !hB_train) {
        std::cerr << "WARNING: signal/background histograms not found for " << method << std::endl;
        f->Close(); return;
    }

    TH1D* hSig  = (TH1D*)hS_train->Clone("hSig");
    TH1D* hBkg2 = (TH1D*)hB_train->Clone("hBkg2");
    hSig->SetDirectory(nullptr);
    hBkg2->SetDirectory(nullptr);

    // Normalize to unit area
    if (hSig->Integral()  > 0) hSig->Scale(1.0 / hSig->Integral());
    if (hBkg2->Integral() > 0) hBkg2->Scale(1.0 / hBkg2->Integral());

    hSig->SetLineColor(kBlue+1);  hSig->SetFillColor(kBlue-9);
    hSig->SetFillStyle(1001);     hSig->SetLineWidth(2);
    hBkg2->SetLineColor(kRed+1);  hBkg2->SetFillColor(kRed-9);
    hBkg2->SetFillStyle(1001);    hBkg2->SetLineWidth(2);

    TH1D* hSig_test  = nullptr;
    TH1D* hBkg_test  = nullptr;
    if (hS_test && hB_test) {
        hSig_test = (TH1D*)hS_test->Clone("hSig_test");
        hBkg_test = (TH1D*)hB_test->Clone("hBkg_test");
        hSig_test->SetDirectory(nullptr);
        hBkg_test->SetDirectory(nullptr);
        if (hSig_test->Integral() > 0) hSig_test->Scale(1.0 / hSig_test->Integral());
        if (hBkg_test->Integral() > 0) hBkg_test->Scale(1.0 / hBkg_test->Integral());
        hSig_test->SetMarkerColor(kBlue+1); hSig_test->SetMarkerStyle(20); hSig_test->SetMarkerSize(0.8);
        hBkg_test->SetMarkerColor(kRed+1);  hBkg_test->SetMarkerStyle(20); hBkg_test->SetMarkerSize(0.8);
        hSig_test->SetLineColor(kBlue+1);
        hBkg_test->SetLineColor(kRed+1);
    }

    double ymax = std::max(hSig->GetMaximum(), hBkg2->GetMaximum()) * 1.4;

    TCanvas* c = new TCanvas("cSep", xTitle, 800, 600);
    hBkg2->GetXaxis()->SetTitle(xTitle);
    hBkg2->GetYaxis()->SetTitle("Normalized");
    hBkg2->SetMaximum(ymax);
    hBkg2->Draw("histo");
    hSig->Draw("histo same");
    if (hSig_test) hSig_test->Draw("e same");
    if (hBkg_test) hBkg_test->Draw("e same");

    TLegend* leg = new TLegend(0.55, 0.72, 0.92, 0.92);
    leg->SetBorderSize(1); leg->SetFillColor(0); leg->SetTextSize(0.04);
    leg->AddEntry(hSig,  "Signal (train)",     "f");
    leg->AddEntry(hBkg2, "Background (train)", "f");
    if (hSig_test) leg->AddEntry(hSig_test, "Signal (test)",     "lep");
    if (hBkg_test) leg->AddEntry(hBkg_test, "Background (test)", "lep");
    leg->Draw();

    TLatex latex; latex.SetNDC(); latex.SetTextSize(0.04);
    latex.DrawLatex(0.12, 0.93, (std::string("TMVA: ") + method + " separation").c_str());

    c->SaveAs(outFile);
    std::cout << "Saved " << outFile << std::endl;

    delete hSig; delete hBkg2; delete c; delete leg;
    if (hSig_test) delete hSig_test;
    if (hBkg_test) delete hBkg_test;
    f->Close();
}

int main() {
    TH1::AddDirectory(kFALSE);
    gStyle->SetOptStat(0);

    TFile* fDY    = TFile::Open("TMVApp_DY.root");
    TFile* fQCD   = TFile::Open("TMVApp_QCD.root");
    TFile* fTT    = TFile::Open("TMVApp_ttjets.root");
    TFile* fSinal = TFile::Open("TMVApp_sinal.root");
    TFile* fData  = TFile::Open("TMVApp_data.root");

    if (!fDY || !fQCD || !fTT || !fSinal || !fData) {
        std::cerr << "ERROR: could not open one or more TMVApp files." << std::endl;
        return 1;
    }

    // Post-fit file from Combine FitDiagnostics (optional — pass nullptr to skip)
    // TFile* fPostFit = TFile::Open("fitDiagnostics.root");
    TFile* fPostFit = nullptr;

    DrawMethod(fDY, fQCD, fTT, fSinal, fData,
               "MVA_BDT",        "BDT output",        "mva_BDT.png",        fPostFit);

    DrawMethod(fDY, fQCD, fTT, fSinal, fData,
               "MVA_Likelihood", "Likelihood output",  "mva_Likelihood.png", fPostFit);

    DrawMethod(fDY, fQCD, fTT, fSinal, fData,
               "MVA_Fisher",     "Fisher output",      "mva_Fisher.png",     fPostFit);

    fDY->Close(); fQCD->Close(); fTT->Close(); fSinal->Close(); fData->Close();

    // KS overtraining tests
    PrintKSTest("TMVA_allBkg_Mutau_2018.root", "BDT");
    PrintKSTest("TMVA_allBkg_Mutau_2018.root", "Likelihood");
    PrintKSTest("TMVA_allBkg_Mutau_2018.root", "Fisher");

    // Signal vs background separation from TMVA training output
    DrawTMVASeparation("TMVA_allBkg_Mutau_2018.root", "BDT",
                       "BDT output", "tmva_separation_BDT.png");

    DrawTMVASeparation("TMVA_allBkg_Mutau_2018.root", "Likelihood",
                       "Likelihood output", "tmva_separation_Likelihood.png");

    DrawTMVASeparation("TMVA_allBkg_Mutau_2018.root", "Fisher",
                       "Fisher output", "tmva_separation_Fisher.png");

    return 0;
}
