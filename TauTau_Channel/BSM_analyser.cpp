#include <iostream>
#include <vector>
#include <TFile.h>
#include <TTree.h>
#include <TApplication.h>
#include <TGraphErrors.h>
#include <TLeaf.h>
#include <TH1D.h>
#include <cmath>
#include <fstream>
#include <TF1.h>
#include "TCanvas.h"

using namespace std;

int main() {
    // Abre o arquivo ROOT
    TFile *file = TFile::Open("../grupo_francisco/ETau_sinal_SM_march.root");
    TGraphErrors gr;
    TGraphErrors ac;
    

    // Obtém a tree
    TTree *tree = (TTree*)file->Get("tree");

    // Define as variáveis que serão lidas da tree
    vector<double> *pesos;

    
    // Lê o número de entradas na tree
    int num_eventos = tree->GetEntries();
    vector <double> atau;

    for(int j = 1; j<101; j++){

      double C = -40 + 0.8*double(j-1);

      double a_tau = 2*1.777*C/(0.3*13042.8);

      cout << "valor do C: " << C << " , que corresponde ao valor a_tau: " << a_tau << endl;

      atau.push_back(a_tau);

      TH1D massa ("massa", "massa", 100, 0, 3000);

      double counter = 0;

      for(int i = 0;i<num_eventos;i++){

	tree -> GetEntry(i);

	double massa_invariante = tree-> GetLeaf("sist_mass")-> GetValue(0);
	
	vector<float>* weights_bsm_sf = (vector<float>*) tree->GetLeaf("weights_bsm_sf")->GetValuePointer();

	double weight_sm = tree-> GetLeaf("weight_sm")-> GetValue(0);

	counter = counter + (*weights_bsm_sf)[j]*weight_sm;


	  
	massa.Fill(massa_invariante,tree-> GetLeaf("weight_sm")-> GetValue(0) * (*weights_bsm_sf)[j]);
	
      }

      
      double media_massa = massa.GetMean();
      double erro_massa = massa.GetMeanError();

      gr.SetPoint(j-1, a_tau, media_massa);
      gr.SetPointError(j-1, 0, erro_massa);

      ac.SetPoint(j-1, a_tau, counter);
      ac.SetPointError(j-1,0,sqrt(counter));

    }




    

    TApplication Ap("Ap",NULL,NULL);

    TCanvas c1;
    gr.Draw("AP");

    TCanvas c2;
    ac.Draw("AP");
   


    // Fecha o arquivo ROOT
    file->Close();

     TGraphErrors sec;

        TF1 f("f", "[0] + [1]*x + [2]*x*x + [3]*x*x*x + [4]*x*x*x*x", -1,1);

    ifstream ifile;
    ifile.open("cross_section_bsm_last.txt");

    double seccao, numero;

    int i = 0;

    while(ifile >> numero >> seccao){


      sec.SetPoint(i,atau[i], seccao);

      i = i +1;
    }

    sec.SetMarkerStyle(21);

       f.SetParameter(0,0.008);
    f.SetParameter(1,0);
    f.SetParameter(2, 22);


    /* f.SetMinParameter(0,-0.1);
    f.SetMaxParameter(0,0.1);

    f.SetMinParameter(1,-1);
    f.SetMaxParameter(1,1);

    f.SetMinParameter(2,0.1);
    f.SetMaxParameter(2,0.4);*/


    TCanvas c3;
    sec.Draw("AP");
       sec.Fit("f");
    //Draw("SAME");
    Ap.Run("true");

    return 0;
}

