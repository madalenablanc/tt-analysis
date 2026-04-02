#include <iostream>
#include <vector>
#include <TFile.h>
#include <TH1D.h>
#include "TTree.h"

using namespace std;

int main() {
   TFile *inputFile = new TFile("TMVA_allBkg_Mutau_2018.root", "READ");

   // ===================== SIGNAL =====================
   TFile *outputFile1 = new TFile("Sinal_mutau_2018_afterTMVA.root", "RECREATE");

   TTree* tree_s = new TTree("tree","");
   double x1;
   tree_s->Branch("BDT", &x1, "BDT/D");

   // Get signal BDT histogram from TMVA output
   TH1D *hist_bdt_s = (TH1D*) inputFile->Get("/dataset/Method_BDT/BDT/MVA_BDT_S");

   double I_bdt_s = hist_bdt_s->Integral();
   cout << "Integral BDT signal: " << I_bdt_s << endl;

   hist_bdt_s->SetName("sinal_mutau_BDT");
   hist_bdt_s->SetTitle("Histogram of sinal_mutau_BDT");

   // Unfold histogram bins into pseudo-events
   vector<double> BDT_s;
   for (int i = 0; i < hist_bdt_s->GetNbinsX(); i++) {
      for (int k = 0; k < (hist_bdt_s->GetBinContent(i+1))*(hist_bdt_s->GetBinWidth(i+1))*1003; k++) {
         BDT_s.push_back(hist_bdt_s->GetBinCenter(i+1));
      }
   }

   cout << "BDT_s vector size: " << BDT_s.size() << endl;

   for (size_t i = 0; i < BDT_s.size(); i++) {
      if (i == 1000) break;
      x1 = BDT_s[i];
      tree_s->Fill();
   }

   tree_s->Write();
   hist_bdt_s->Write();
   outputFile1->Close();

   // ===================== BACKGROUND =====================
   TFile *outputFile2 = new TFile("Background_mutau_2018_afterTMVA.root", "RECREATE");

   TTree* tree_b = new TTree("tree","");
   double x2;
   tree_b->Branch("BDT", &x2, "BDT/D");

   // Get background BDT histogram from TMVA output
   TH1D *hist_bdt_b = (TH1D*) inputFile->Get("/dataset/Method_BDT/BDT/MVA_BDT_B");

   double I_bdt_b = hist_bdt_b->Integral();
   cout << "Integral BDT background: " << I_bdt_b << endl;

   hist_bdt_b->SetName("background_mutau_BDT");
   hist_bdt_b->SetTitle("Histogram of background_mutau_BDT");

   // Unfold histogram bins into pseudo-events
   vector<double> BDT_b;
   for (int i = 0; i < hist_bdt_b->GetNbinsX(); i++) {
      for (int k = 0; k < (hist_bdt_b->GetBinContent(i+1))*(hist_bdt_b->GetBinWidth(i+1))*1196; k++) {
         BDT_b.push_back(hist_bdt_b->GetBinCenter(i+1));
      }
   }

   cout << "BDT_b vector size: " << BDT_b.size() << endl;

   for (size_t i = 0; i < BDT_b.size(); i++) {
      if (i == 1190) break;
      x2 = BDT_b[i];
      tree_b->Fill();
   }

   tree_b->Write();
   hist_bdt_b->Write();
   outputFile2->Close();

   inputFile->Close();

   cout << "Histograms copied successfully to output files!" << endl;

   return 0;
}
