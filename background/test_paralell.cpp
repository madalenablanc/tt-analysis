#include <ROOT/RDataFrame.hxx>
#include <stdio.h>
#include <iostream>
#include <cmath>
#include "TApplication.h"
#include "TFile.h"
#include "TTree.h"
#include "TLeaf.h"
#include "TH1D.h"
#include "TLorentzVector.h"
#include <string>
#include <fstream>
#include <cstdlib>
#include <sstream>
#include <chrono>

using namespace std;
using namespace std::chrono;

int main(){
    ROOT::EnableImplicitMT();

  //string prefix = "root:://cms-xrd-global.cern.ch//";
	// string input="/eos/user/m/mblancco/samples_2018_tautau/fase0/QCD_2018_UL_skimmed_TauTau_nano_.root";
    string input="root:://cms-xrd-global.cern.ch///store/data/Run2018A/Tau/NANOAOD/UL2018_MiniAODv2_NanoAODv9-v2/2810000/0075950B-3B06-844E-86D0-7900E3A78B52.root";

	string prefix_output = "QCD_fase1_PIC_skimmed_TauTau_2018";

    auto start2 = high_resolution_clock::now();

    ROOT::RDataFrame rdf("Events", input.c_str());

    auto entries1 = rdf.Count();
    std::cout << *entries1 << " entries passed all filters" << std::endl;

    TFile *f=TFile::Open(input.c_str());


    auto stop2 = high_resolution_clock::now();

    auto duration2 = duration_cast<microseconds>(stop2 - start2);

    // To get the value of duration use the count()
    // member function on the duration object
    cout <<"Duration; Parallel: "<< duration2.count() << endl;




    TTree *tree = (TTree*) f->Get("Events");
    TTree *tree_lumi = (TTree*) f->Get("LuminosityBlocks");
    
	int k=0;

	double weight = 1.0;
    auto start = high_resolution_clock::now();

    int n_events=0;

    for(int i=0;i<tree->GetEntries();i++){
        // int eventos=tree->GetEvent(i);
        // int eventoslum=tree_lumi->GetEvent(i);

        // int run=tree->GetLeaf("run")->GetValue(0);
        // int luminosity=tree->GetLeaf("luminosityBlock")->GetValue(0);

        // cout<<"run"<<run<<endl;
        // cout<<"lum"<<luminosity<<endl;
        // cout<<"//////////////////////"<<endl;
        n_events++;
    }    

    cout << "n events" << n_events << endl;
    auto stop = high_resolution_clock::now();

    auto duration = duration_cast<microseconds>(stop - start);

    // To get the value of duration use the count()
    // member function on the duration object
    cout <<"Duration: "<< duration.count() << endl;


	return 0;
}
