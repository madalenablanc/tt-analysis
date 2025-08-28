#include <iostream>
#include <fstream>
#include <string>
#include "TFile.h"
#include "TTree.h"

using namespace std;

int countEvents(string list_file, string tree_name="Events") {
    ifstream infile(list_file);
    string filename;
    Long64_t total_events = 0;

    string xrootd_prefix = "root://cms-xrd-global.cern.ch/";

    while (infile >> filename) {
        string fullpath = xrootd_prefix + filename;

        TFile *f = TFile::Open(fullpath.c_str());
        if (!f || f->IsZombie()) {
            cerr << "Could not open " << fullpath << endl;
            continue;
        }

        TTree *tree = (TTree*) f->Get(tree_name.c_str());
        if (!tree) {
            cerr << "Tree " << tree_name << " not found in " << fullpath << endl;
            f->Close();
            continue;
        }

        Long64_t nentries = tree->GetEntries();
        cout << filename << " : " << nentries << " events" << endl;
        total_events += nentries;

        f->Close();
    }

    cout << "Total events = " << total_events << endl;
    return 0;
}
