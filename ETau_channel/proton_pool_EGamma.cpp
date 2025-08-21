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
	string input;

	string prefix_output = "proton_pool_EGAMMA_2018_UL_fromQCD_nano_";

	int k=0;

	double weight = 1.;

	int numero_linha;
	cin >> numero_linha;

	stringstream ss;
	ss << numero_linha;

	string out_put;
	ss>>out_put;

	string output_tot = "/eos/cms/store/group/phys_smp/Exclusive_DiTau/proton_pool_2018/" + prefix_output + out_put + ".root";

	ifstream ifile;
	ifile.open("QCD_2018_UL.txt");
	while(k<numero_linha) {
		ifile>>input;
		k++;
	}

	double entry_1,entry_2,entry_3,entry_4,entry_5,entry_6,entry_7,entry_8,entry_9,entry_10,entry_11,entry_12,entry_13,entry_14,entry_15,entry_16,entry_17,entry_18,entry_19,entry_20,entry_21,entry_22
	,entry_23,entry_24,entry_25,entry_26,entry_27,entry_28, entry_30;
	int entry_29;

	string total = prefix+input;
	cout <<"Input file: " << total << endl;

	TApplication app("app", NULL, NULL);
	//cout<<"antes de abrir o ficheiro"<<endl;
	//TFile *f = TFile::Open("root:://cms-xrd-global.cern.ch///store/mc/RunIISummer20UL18NanoAODv9/TTJets_TuneCP5_13TeV-amcatnloFXFX-pythia8/NANOAODSIM/106X_upgrade2018_realistic_v16_L1v1-v1/2520000/028FE21B-A107-4347-92C3-B533907C13DE.root");

	TFile *f = TFile::Open(total.c_str());

	TTree *tree = (TTree*) f->Get("Events");



	TFile output (output_tot.c_str(), "RECREATE", "");
	TTree out("tree","");


 	out.Branch("proton_xi", &entry_1, "proton_xi/D");
 	out.Branch("proton_arm", &entry_2, "proton_arm/D");
 	out.Branch("proton_thx", &entry_3, "proton_thx/D");
 	out.Branch("proton_thy", &entry_4, "proton_thy/D");
	out.Branch("n_pu", &entry_5, "n_pu/D");


	for(int i=0; i<tree->GetEntries(); i++){

		int eventos=tree->GetEvent(i);

		//if(i%1000==0) cout << "Progress: " <<double (i)/double (tree->GetEntries()) << endl;

		if( (tree->GetLeaf("HLT_Ele32_WPTight_Gsf")->GetValue(0)==1) ){


		    entry_1=tree->GetLeaf("Proton_multiRP_xi")->GetValue(0);
		    entry_2=tree->GetLeaf("Proton_multiRP_arm")->GetValue(0);
		    entry_3=tree->GetLeaf("Proton_multiRP_thetaX")->GetValue(0);
		    entry_4=tree->GetLeaf("Proton_multiRP_thetaY")->GetValue(0);

		   //if(entry_1<0.001) continue;

		    entry_5=tree->GetLeaf("Proton_multiRP_xi")->GetLen();


		    out.Fill();

		}
	}
	output.Write();

	return 0;


}


