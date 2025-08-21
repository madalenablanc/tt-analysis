#include <iostream>
#include <cmath>
#include <vector>
#include "TFile.h"
#include "TGraphErrors.h"
#include "TApplication.h"
#include "TTree.h"
#include "TLeaf.h"

using namespace std;


vector<double>* bsm_weights_sf;

TBranch* bsm_weights_sfB;

int main(){

	TApplication app("app", NULL, NULL, NULL);

	TFile f("ETau_sinal_SM_march.root");

	TTree* tree = (TTree*) f.Get("tree");

	TGraphErrors gr;

	double n_final[102];

	for(int i=0; i<102; i++) n_final[i]=0;

	for(int i=0; i<tree->GetEntries(); i++){


		tree->GetEvent(i);

		vector<float>* weights_bsm_sf = (vector<float>*) tree->GetLeaf("weights_bsm_sf")->GetValuePointer();
		double weight= tree->GetLeaf("weight_sm")->GetValue(0);


		for(int j=0; j<102; j++){

		cout << "j" << j << endl;

			n_final[j]=n_final[j]+weight*(*weights_bsm_sf)[j];

			cout << (tree->GetLeaf("weights_bsm_sf")->GetValue(j)) << endl;

		}



	}


	for(int j=1; j<102; j++){

               gr.SetPoint(j-1,j,n_final[j]);

        }

	gr.SetMarkerStyle(21);
	gr.Draw("");


        app.Run(true);
	return 0;
}

