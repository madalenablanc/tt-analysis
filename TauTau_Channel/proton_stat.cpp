#include <iostream>
#include <cmath>
#include "TApplication.h"
#include "TFile.h"
#include "TH1D.h"
#include "TTree.h"
#include "TLeaf.h"
#include "TCanvas.h"

using namespace std;

int main(){

	TApplication app("app", NULL, NULL);

	TH1D xi_1("xi_1","xi_1",10,0.02,0.15);
	TH1D xi_1_up("xi_1","xi_1",10,0.02,0.15);
	TH1D xi_1_down("xi_1","xi_1",10,0.02,0.15);
	TH1D ratio("ratio","ratio",100,1.,1.2);

	TFile f("TauTau_sinal_PIC_march_2018.root");
	TTree* tree = (TTree*) f.Get("tree");

	for(int i=0; i<tree->GetEntries(); i++){

		int o = tree->GetEvent(i);


		xi_1.Fill(tree->GetLeaf("xi_arm1_1")->GetValue(0));
		xi_1_up.Fill(tree->GetLeaf("xi_arm1_1_up")->GetValue(0));
		xi_1_down.Fill(tree->GetLeaf("xi_arm1_1_down")->GetValue(0));
		ratio.Fill(tree->GetLeaf("xi_arm1_1_up")->GetValue(0)/tree->GetLeaf("xi_arm1_1")->GetValue(0));

	}

	xi_1.SetLineColor(1);
	xi_1_up.SetLineColor(2);
	xi_1_down.SetLineColor(4);

	TCanvas c1;

	xi_1.Draw("E");
	xi_1_up.Draw("same && e");
	xi_1_down.Draw("same && e");

	TCanvas c2;

	ratio.Draw();

	app.Run("true");
	return 0;

}
