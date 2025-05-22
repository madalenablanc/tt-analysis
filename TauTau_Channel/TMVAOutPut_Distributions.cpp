#include <iostream>
#include <TFile.h>
#include <TH1D.h>
#include <TList.h>
#include "TTree.h"

using namespace std;

int main() {
   TFile *inputFile = new TFile("Output_TMVA_tautau_2018.root", "READ");
   TFile *outputFile1 = new TFile("Sinal_tautau_2018_afterTMVA.root", "RECREATE");
     
   TTree* tree_s = new TTree("tree","");


   double x1, x2, x3 ,x4 ,x5 ,x6, x7,x8 ,x9,x10;
   double I1_s, I2_s, I3_s, I4_s,I5_s;

   
   tree_s->Branch("BDT", &x1, "BDT/D");
   tree_s->Branch("MLP", &x2, "MLP/D");
   tree_s->Branch("Fisher", &x3, "Fisher/D");
   tree_s->Branch("PDERS", &x4, "PDERS/D");
   tree_s->Branch("Likelihood", &x5, "Likelihood/D");

   // Encontra os dois histogramas que deseja copiar
   TH1D *hist1 = (TH1D*) inputFile->Get("/dataset/Method_BDT/BDT/MVA_BDT_S");
   TH1D *hist2 = (TH1D*) inputFile->Get("/dataset/Method_MLP/MLP/MVA_MLP_S");
   TH1D *hist3 = (TH1D*) inputFile->Get("/dataset/Method_Fisher/Fisher/MVA_Fisher_S");
   TH1D *hist4 = (TH1D*) inputFile->Get("/dataset/Method_PDERS/PDERS/MVA_PDERS_S");
   TH1D *hist5 = (TH1D*) inputFile->Get("/dataset/Method_Likelihood/Likelihood/MVA_Likelihood_S");
   
   // Renomeia os histogramas antes de copiá-los para o novo arquivo

   I1_s = hist1 -> Integral();
   I2_s = hist2 -> Integral();
   I3_s = hist3 -> Integral();
   I4_s = hist4 -> Integral();
   I5_s = hist5 -> Integral();


   
   
   hist1->SetName("sinal_tautau_BDT");
   hist1-> SetTitle("Histogram of sinal_tautau_BDT");

   hist2->SetName("sinal_tautau_MLP");
   hist2-> SetTitle("Histogram of sinal_tautau_MLP");

   hist3->SetName("sinal_tautau_Fisher");
   hist3-> SetTitle("Histogram of sinal_tautau_Fisher");

   hist4->SetName("sinal_tautau_PDERS");
   hist4-> SetTitle("Histogram of sinal_tautau_PDERS");

   hist5->SetName("sinal_tautau_Likelihood");
   hist5-> SetTitle("Histogram of sinal_tautau_Likelihood");

   vector <double> BDT_s;
   vector <double> MLP_s;
   vector <double> Fisher_s;
   vector <double> PDERS_s;
   vector <double> Likelihood_s;

   cout << "Integral BDT :"<< I1_s<<endl; 

   for(int i = 0; i < hist1 -> GetNbinsX(); i++){

     for (int k = 0; k< (hist1 -> GetBinContent(i+1))*(hist1 -> GetBinWidth(i+1))*1003; k++){

       // double  eventos = (hist1 -> GetBinContent(i+1))*(hist1 -> GetBinWidth(i+1))/(I1_s)*1003;

      //cout<< "Número de eventos no bin "<<i<<" :"<<eventos<<endl;      


       BDT_s.push_back(hist1 -> GetBinCenter(i+1));

       // cout<<"Tamanho do vetor: "<<BDT_s.size()<<endl;
     }
   }

   for(int i = 0; i < hist2 -> GetNbinsX(); i++){

     for (int k = 0; k< (hist2 -> GetBinContent(i+1))*(hist2 -> GetBinWidth(i+1))*1003; k++){

    


       MLP_s.push_back(hist2 -> GetBinCenter(i+1));
     }
   }

   for(int i = 0; i < hist3 -> GetNbinsX(); i++){
	  
     for (int k = 0; k< (hist3 -> GetBinContent(i+1))*(hist3 -> GetBinWidth(i+1))*1003; k++){

        

       Fisher_s.push_back(hist3 -> GetBinCenter(i+1));
     }
   }

   for(int i = 0; i < hist4 -> GetNbinsX(); i++){
     
     for (int k = 0; k< (hist4 -> GetBinContent(i+1))*(hist4 -> GetBinWidth(i+1))*1003; k++){

       PDERS_s.push_back(hist4 -> GetBinCenter(i+1));
     }
   }

   for(int i = 0; i < hist5 -> GetNbinsX(); i++){
     
     for (int k = 0; k< (hist5 -> GetBinContent(i+1))*(hist5 -> GetBinWidth(i+1))*1003; k++){
       Likelihood_s.push_back(hist5 -> GetBinCenter(i+1));
     }
   }
     
     
   
   cout<<"Valor do tamanho do BDT_s:"<<BDT_s.size()<<endl;

   

   
   
   for (int i = 0; i < BDT_s.size(); i++) {
     if(i == 1000) break;

     

     x1 = BDT_s[i];
     x2 = MLP_s[i];
     x3 = Fisher_s[i];
     x4 = PDERS_s[i];
     x5 = Likelihood_s[i];
     tree_s->Fill();
   }

   tree_s -> Write();

   // Copia os dois histogramas selecionados para o novo arquivo
   hist1->Write();
   
   hist2->Write();

   hist3->Write();

   hist4->Write();

   hist5->Write();

   outputFile1->Close();

    TFile *outputFile2 = new TFile("Background_tautau_2018_afterTMVA.root", "RECREATE");

     TTree* tree_b = new TTree("tree","");

   tree_b->Branch("BDT", &x6, "BDT/D");
   tree_b->Branch("MLP", &x7, "MLP/D");
   tree_b->Branch("Fisher", &x8, "Fisher/D");
   tree_b->Branch("PDERS", &x9, "PDERS/D");
   tree_b->Branch("Likelihood", &x10, "Likelihood/D");
    


// Encontra os dois histogramas que deseja copiar
   TH1D *hist7 = (TH1D*) inputFile->Get("/dataset/Method_BDT/BDT/MVA_BDT_B");
   TH1D *hist8 = (TH1D*) inputFile->Get("/dataset/Method_MLP/MLP/MVA_MLP_B");
   TH1D *hist9 = (TH1D*) inputFile->Get("/dataset/Method_Fisher/Fisher/MVA_Fisher_B");
   TH1D *hist10 = (TH1D*) inputFile->Get("/dataset/Method_PDERS/PDERS/MVA_PDERS_B");
   TH1D *hist11 = (TH1D*) inputFile->Get("/dataset/Method_Likelihood/Likelihood/MVA_Likelihood_B");

   
   // Renomeia os histogramas antes de copiá-los para o novo arquivo


   hist7->SetName("background_tautau_BDT");
   hist7-> SetTitle("Histogram of background_tautau_BDT");

   hist8->SetName("background_tautau_MLP");
   hist8-> SetTitle("Histogram of background_tautau_MLP");

   hist9->SetName("background_tautau_Fisher");
   hist9-> SetTitle("Histogram of background_tautau_Fisher");

   hist10->SetName("background_tautau_PDERS");
   hist10-> SetTitle("Histogram of background_tautau_PDERS");

   hist11->SetName("background_tautau_Likelihood");
   hist11-> SetTitle("Histogram of background_tautau_Likelihood");

   vector <double> BDT_b;
   vector <double> MLP_b;
   vector <double> Fisher_b;
   vector <double> PDERS_b;
   vector <double> Likelihood_b;

   double I6_b, I7_b, I8_b, I9_b,I10_b;

   I6_b = hist7 -> Integral();
   I7_b = hist8 -> Integral();
   I8_b = hist9 -> Integral();
   I9_b = hist10 -> Integral();
   I10_b = hist11 -> Integral();



   for(int i = 0; i < hist7 -> GetNbinsX(); i++){

     for (int k = 0; k< (hist7 -> GetBinContent(i+1))*(hist7 -> GetBinWidth(i+1))*1196; k++){

       BDT_b.push_back(hist7 -> GetBinCenter(i+1));
     }
   }

   for(int i = 0; i < hist8 -> GetNbinsX(); i++){

     for (int k = 0; k< (hist8 -> GetBinContent(i+1))*(hist8 -> GetBinWidth(i+1))*1196; k++){

       MLP_b.push_back(hist8 -> GetBinCenter(i+1));
     }
   }

   for(int i = 0; i < hist9 -> GetNbinsX(); i++){
	  
     for (int k = 0; k< (hist9 -> GetBinContent(i+1))*(hist9 -> GetBinWidth(i+1))*1196; k++){

       Fisher_b.push_back(hist9 -> GetBinCenter(i+1));
     }
   }

   for(int i = 0; i < hist10 -> GetNbinsX(); i++){
     
     for (int k = 0; k< (hist10 -> GetBinContent(i+1))*(hist10 -> GetBinWidth(i+1))*1196; k++){

       PDERS_b.push_back(hist10 -> GetBinCenter(i+1));
     }
   }

   for(int i = 0; i < hist11 -> GetNbinsX(); i++){
     
     for (int k = 0; k< (hist11 -> GetBinContent(i+1))*(hist11 -> GetBinWidth(i+1))*1196; k++){
       Likelihood_b.push_back(hist11 -> GetBinCenter(i+1));
     }
   }
     
   cout<<"Valor do tamanho do BDT_b:"<<BDT_b.size()<<endl;
   
   for (int i = 0; i < BDT_b.size(); i++) {
     if(i == 1190) break;
     
     x6 = BDT_b[i];
     x7 = MLP_b[i];
     x8 = Fisher_b[i];
     x9 = PDERS_b[i];
     x10 = Likelihood_b[i];
     tree_b->Fill();
   }

 tree_b -> Write();

   
    // Copia os dois histogramas selecionados para o novo arquivo
   hist7->Write();
   hist8->Write();
   hist9->Write();
   hist10->Write();
   hist11->Write();
   
   // Fecha os arquivos
   inputFile->Close();
   outputFile2->Close();

   cout << "Os histogramas foram copiados com sucesso para o arquivo de saída!" << endl;

   return 0;
}

