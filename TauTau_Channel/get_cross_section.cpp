#include <iostream>
#include <cmath>
#include "TFile.h"
#include "TH1D.h"
#include <vector>
#include "TBranch.h"
#include "TTree.h"
#include <fstream>
#include "TApplication.h"
#include "TString.h"

using namespace std;

int main(){

	TApplication app("app",NULL, NULL);

	TFile f("/eos/cms/store/group/phys_smp/Exclusive_DiTau/miniaod/signal/GGToTauTau_Elastic_M_300GeV_filterXi_ktSmear_RunIISummer20UL18_TuneCP5_madgraphLO_BSM_Ctb20_reweight/miniAOD_1001.root");

	cout << "Starting programme..." << endl;

	TTree* tree = (TTree*) f.Get("Events");

	ofstream ofile;
	ofile.open("cross_sections_high_masses_july.txt");

	TH1D temp("temp","temp",100,0,100);

	int n_ev = tree->GetEntries();

	cout << "starting cycle..." << endl;

	for(int i=1; i<101; i++){

		TString selection = TString::Format("LHEEventProduct_externalLHEProducer__SIM.obj.weights_.wgt[%d]", i);
		tree->Draw("1>>temp",selection);

		TH1D histo;
		histo.Add(&temp);

		cout<< "A seccao eficaz do cenário " << i << "é" << histo.Integral()*1000./double (n_ev) << " fb" << endl;
		ofile <<  2*1.777*(-40. + 0.8*double(i-1))/(0.3*13042.8) << " " << histo.Integral()*1000./double (n_ev) << endl;

	}

	ofile.close();
	app.Run("true");
	return 0;

}
