#include <iostream>
#include <TFile.h>
#include <TH1D.h>
#include <TList.h>

using namespace std;

int main() {
   TFile *inputFile = new TFile("Output_TMVA_tautau_2018.root", "READ");
   TFile *outputFile = new TFile("/eos/user/m/mpisano/tautau/CMSSW_10_2_13/src/HiggsAnalysis/CombinedLimit/Informacao_BDT_2018tautau_scalar_800_NWA.root", "RECREATE");
  
   
   if (!inputFile->IsOpen()) {
      cout << "Não foi possível abrir o arquivo de entrada!" << endl;
      return 1;
   }

   if (!outputFile->IsOpen()) {
      cout << "Não foi possível criar o arquivo de saída!" << endl;
      return 1;
   }

  
   // Obtém a lista de objetos no diretório
   //TList *list = inputFile->GetListOfKeys();

   // Encontra os dois histogramas que deseja copiar
   TH1D *hist1 = (TH1D*) inputFile->Get("/dataset/Method_BDT/BDT/MVA_BDT_S");
   TH1D *hist2 = (TH1D*) inputFile->Get("/dataset/Method_BDT/BDT/MVA_BDT_B");
   TH1D *hist3 = (TH1D*) inputFile->Get("/dataset/Method_BDT/BDT/MVA_BDT_B_high");

   
   // Renomeia os histogramas antes de copiá-los para o novo arquivo
   hist1->SetName("excl_tautau");
   hist1-> SetTitle("Histogram of excl_tautau__0");
   
   hist2->SetName("all_Bkg");
   hist2-> SetTitle("Histogram of all_Bkg__1");
   
   hist3 -> SetName("data_obs");
   hist3->SetTitle("Histogram of data_obs__0");

   hist1 -> Scale(260./hist1 -> Integral());
    hist3 -> Scale(0.0/hist3 -> Integral());
   hist2 -> Scale(104.0/hist2->Integral());
    
   hist1 -> Rebin(8);
   hist3 -> Rebin(8);
   hist2 -> Rebin(8);

   // Copia os dois histogramas selecionados para o novo arquivo
   hist1->Write();
   hist2->Write();
   hist3->Write();

   // Fecha os arquivos
   inputFile->Close();
   outputFile->Close();

   cout << "Os histogramas foram copiados com sucesso para o arquivo de saída!" << endl;

   return 0;
}

