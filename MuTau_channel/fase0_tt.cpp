#include <stdio.h>
#include <iostream>
#include <cmath>
#include "TApplication.h"
#include "TFile.h"
#include "TTree.h"
#include "TH1D.h"
#include "TLorentzVector.h"
#include "ROOT/RDataFrame.hxx"
#include "ROOT/RVec.hxx"
#include <string>
#include <fstream>
#include <cstdlib>
#include <sstream>


using namespace std;
using namespace ROOT;
using RNode = ROOT::RDF::RNode;

int main(){
    ROOT::EnableImplicitMT();
    
    string prefix = "root:://cms-xrd-global.cern.ch//";
    string input;
    string prefix_output = "ttJets_2018_UL_skimmed_MuTau_nano";

    int k=0;
    double weight = 1.;


    int numero_linha;
    cin >> numero_linha;
    
    stringstream ss;
    ss << numero_linha;
    string out_put;
    ss >> out_put;
    
    string output_tot = "/eos/user/m/mblancco/samples_2018_mutau/fase0_try_parallel/" + prefix_output + out_put + ".root";


    ifstream ifile;
    ifile.open("ttJets_2018_UL.txt");
    while(k < numero_linha) {
        ifile >> input;
        k++;
    }
    
    string total = prefix + input;
    cout << "Input file: " << total << endl;
    
    // Create RDataFrame
    RDataFrame df("Events", total);

    auto df_filtered = df

    // HLT trigger -> garante a presença de 1 muao
    .Filter("HLT_IsoMu24 ==1")

    //presenca de um muao e um tau hadronico
    .Filter("Muon_pt.size()>0 ")


}