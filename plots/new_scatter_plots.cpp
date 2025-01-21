#include <stdio.h>
#include <iostream>
#include <cmath>
#include "TApplication.h"
#include "TFile.h"
#include "TTree.h"
#include "TLeaf.h"
#include "TH1D.h"
#include <string>
#include <fstream>
#include <cstdlib>
#include <sstream>
#include "TCanvas.h"

#include <TPaveStats.h>
#include <TRandom3.h>
#include <TGaxis.h>
#include <THStack.h>

using namespace std;


void transpad(TTree *tree) {
   TCanvas *c1 = new TCanvas("c1","transparent pad",200,10,700,500);
   TPad *pad1 = new TPad("pad1","",0,0,1,1);
   TPad *pad2 = new TPad("pad2","",0,0,1,1);
   pad2->SetFillStyle(4000); //will be transparent
   pad1->Draw();
   pad1->cd();

   TH1F *h1 = new TH1F("h1","h1",100,-3,3);
   TH1F *h2 = new TH1F("h2","h2",100,-3,3);
//    TRandom r;
//    for (Int_t i=0;i<100000;i++) {
//       Double_t x1 = r.Gaus(-1,0.5);
//       Double_t x2 = r.Gaus(1,1.5);
//       if (i <1000) h1->Fill(x1);
//       h2->Fill(x2);
//   }

   double variable1, variable2;
tree->SetBranchAddress("tau0_phi", &variable1);
tree->SetBranchAddress("tau1_phi", &variable2);


   // Loop over events to fill histograms
for (int i = 0; i < tree->GetEntries(); ++i) {
    tree->GetEntry(i);
    h1->Fill(variable1);
    h2->Fill(variable2);
}


   h1->Draw();
   pad1->Update(); //this will force the generation of the "stats" box
   TPaveStats *ps1 = (TPaveStats*)h1->GetListOfFunctions()->FindObject("stats");
   ps1->SetX1NDC(0.4); ps1->SetX2NDC(0.6);
   pad1->Modified();
   c1->cd();
   
   //compute the pad range with suitable margins
   Double_t ymin = 0;
   Double_t ymax = 1000;
   Double_t dy = (ymax-ymin)/0.8; //10 per cent margins top and bottom
   Double_t xmin = -4;
   Double_t xmax = 4;
   Double_t dx = (xmax-xmin)/0.8; //10 per cent margins left and right
   //pad2->Range(xmin-0.1*dx,ymin-0.1*dy,xmax+0.1*dx,ymax+0.1*dy);
   pad2->Range(-4,4,0,1000);
   pad2->Draw();
   pad2->cd();
   h2->SetLineColor(kRed);
   h2->Draw("sames");
   pad2->Update();
   TPaveStats *ps2 = (TPaveStats*)h2->GetListOfFunctions()->FindObject("stats");
   ps2->SetX1NDC(0.65); ps2->SetX2NDC(0.85);
   ps2->SetTextColor(kRed);
   
   // draw axis on the right side of the pad
//    TGaxis *axis = new TGaxis(xmax,ymin,xmax,ymax,ymin,ymax,50510,"+L");
//    axis->SetLabelColor(kRed);
//    axis->Draw();

   c1->SaveAs("overlay_2.png");
}






void plot_vars(string var1, string var2, TTree *tree){

TCanvas *c1 = new TCanvas("c1", "Invariant Mass Plot", 800, 600);

TH1F *h1= new TH1F("tau0_pt",  "Tau+ pT; pT [GeV]; Events", 100, 0, 900);
TH1F *h2= new TH1F("tau1_pt", "Tau+ pT; pT [GeV]; Events", 100, 0, 900);


double variable1, variable2;
tree->SetBranchAddress("tau0_pt", &variable1);
tree->SetBranchAddress("tau1_pt", &variable2);

// Loop over events to fill histograms
for (int i = 0; i < tree->GetEntries(); ++i) {
    tree->GetEntry(i);
    h1->Fill(variable1);
    h2->Fill(variable2);
}

h1->SetLineColor(kRed); // Set color for the first histogram
h1->Draw();             // Draw first histogram
h2->SetLineColor(kBlue); // Set color for the second histogram
h2->Draw("SAME");        // Overlay second histogram

//c1->BuildLegend();      // Add legend
c1->SaveAs("overlay.png");
}




void hstack(TTree *tree){
    THStack *hs = new THStack("hs","Histograms");

    TH1F *h1= new TH1F("tau0_pt",  "Tau+ pT; pT [GeV]; Events", 100, -4, 4);
    TH1F *h2= new TH1F("tau1_pt", "Tau+ pT; pT [GeV]; Events", 100, -4, 4);


    double variable1, variable2;
    tree->SetBranchAddress("tau0_phi", &variable1);
    tree->SetBranchAddress("tau1_phi", &variable2);

    // Loop over events to fill histograms
    for (int i = 0; i < tree->GetEntries(); ++i) {
        tree->GetEntry(i);
        h1->Fill(variable1);
        h2->Fill(variable2);
    }

    h1->SetLineColor(kRed);
    hs->Add(h1);
    h2->SetLineColor(kBlue);
    hs->Add(h2);

    TCanvas *cst = new TCanvas("cst","stacked hists",10,10,700,700); 

    hs->Draw();

    cst->SaveAs("overlay_3.png");

}



int main(){

string input= "/eos/user/m/mblancco/samples_2018_tautau/fase0_2/ttJetscode_GammaGammaTauTau_SignalMC_SM_18UL_23k_NANOAODSIM_fase0_with_xi_and_deltaphi.root";

TFile *f = TFile::Open(input.c_str());


TTree *tree = (TTree*) f->Get("tree");

string line;
ifstream myfile ("variables.txt");
if (myfile.is_open())
{

while(getline(myfile, line)) {
    cout << line << endl;
}
myfile.close();
}

else cout << "Unable to open file"; 


//plot_vars("tau0_phi","tau1_phi",tree);
//transpad(tree);
hstack(tree);




return 0;
}

