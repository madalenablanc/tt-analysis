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

        // First section - RDataFrame approach
    auto start_rdf = high_resolution_clock::now();
    ROOT::RDataFrame rdf("Events", input.c_str());
    auto entries1 = rdf.Count();
    // Force execution by dereferencing the result
    std::cout << *entries1 << " entries passed all filters" << std::endl;
    auto stop_rdf = high_resolution_clock::now();
    auto duration_rdf = duration_cast<seconds>(stop_rdf - start_rdf);
    cout << "Duration RDataFrame: " << duration_rdf.count() << " seconds" << endl;

    // Second section - Traditional TTree approach
    TFile *f = TFile::Open(input.c_str());
    TTree *tree = (TTree*) f->Get("Events");
    TTree *tree_lumi = (TTree*) f->Get("LuminosityBlocks");

    auto start_tree = high_resolution_clock::now();
    int n_events = 0;
    for(int i = 0; i < tree->GetEntries(); i++) {
        tree->GetEntry(i);  // Actually get the entry
        n_events++;
    }
    cout << "n events: " << n_events << endl;
    auto stop_tree = high_resolution_clock::now();
    auto duration_tree = duration_cast<seconds>(stop_tree - start_tree);
    cout << "Duration TTree: " << duration_tree.count() << " seconds" << endl;

	return 0;
}
