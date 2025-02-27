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

using namespace std;

int main(){

	string prefix = "root:://cms-xrd-global.cern.ch//";

	string file_name = "ttJetscode_GammaGammaTauTau_SignalMC_SM_18UL_23k_NANOAODSIM_fase0_TEST";

	string output_tot = "/eos/user/m/mblancco/samples_2018_tautau/fase0_2/" + file_name + ".root";

    int ntautau=0;
    int nid=0;
    int ncharge=0;
    int npt=0;
    int neta=0;

    string input = "/store/data/Run2018A/Tau/NANOAOD/UL2018_MiniAODv2_NanoAODv9-v2/2810000/0075950B-3B06-844E-86D0-7900E3A78B52.root"

    //string input="/eos/cms/store/user/jjhollar/TauTau_NanoAOD_Madalena/GammaGammaTauTau_SignalMC_SM_18UL_23k_NANOAODSIM.root";
    cout <<"Input file: " << input << endl;

}