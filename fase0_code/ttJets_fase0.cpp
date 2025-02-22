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

string prefix_output = "ttJetscode_GammaGammaTauTau_SignalMC_SM_18UL_23k_NANOAODSIM_fase0_no_pileups";
//string prefix_output="ttJetsCode_GammaGammaTauTau_SignalMC_SM_18UL_23k_NANOAODSIM_fase0";


int k=0;

double weight = 1.0; //w= Numero monte carlo / Numero de 2018 , Numero de 2018 = luminosidade de 2018 * secção de 2018  


string output_tot = "/eos/user/m/mblancco/samples_2018_tautau/fase0_2/" + prefix_output + ".root";

// ifstream ifile;
// ifile.open("ttJets_2018_UL.txt");
// while(k<numero_linha) {
// 	ifile>>input;
// 	k++;
// }

double entry_1,entry_2,entry_3,entry_4,entry_5,entry_6,entry_7,entry_8,entry_9,entry_10,entry_11,entry_12,entry_13,entry_14,entry_15,entry_16,entry_17,entry_18,entry_19,entry_20,entry_21,entry_22
,entry_23,entry_24,entry_25,entry_26,entry_27,entry_28, entry_30, entry_31, entry_32, entry_33, entry_34, entry_35, entry_36, entry_37;
double proton_xi1, proton_xi2, p_invariant_mass, invariant_mass_tt_pair;
int entry_29;
double tau0_pt, tau1_pt;
double tau0_eta, tau1_eta;
double tau0_phi, tau1_phi;
double p_rapidity, tt_rapidity;

//contadores
int ntautau=0;
int nid=0;
int ncharge=0;
int npt=0;
int neta=0;

////////////////////////////////////////
TLorentzVector tau0;
TLorentzVector tau1;
TLorentzVector sistema;

//string total = prefix+input;
//input="/eos/cms/store/user/jjhollar/TauTau_NanoAOD_Madalena/GammaGammaTauTau_SignalMC_SM_18UL_23k_NANOAODSIM.root";
input="/eos/cms/store/user/jjhollar/TauTau_NanoAOD_Madalena/GammaGammaTauTau_SignalMC_SM_18UL_25k_NoPileupProtons_NANOAODSIM.root";
cout <<"Input file: " << input << endl;

TApplication app("app", NULL, NULL);
//cout<<"antes de abrir o ficheiro"<<endl;
//TFile *f = TFile::Open("root:://cms-xrd-global.cern.ch///store/mc/RunIISummer20UL18NanoAODv9/TTJets_TuneCP5_13TeV-amcatnloFXFX-pythia8/NANOAODSIM/106X_upgrade2018_realistic_v16_L1v1-v1/2520000/028FE21B-A107-4347-92C3-B533907C13DE.root");

TFile *f = TFile::Open(input.c_str());

TTree *tree = (TTree*) f->Get("Events");

///A siguente linha tem de ser considerada apenas pelo fundo QCD
TTree *tree_lumi = (TTree*) f->Get("LuminosityBlocks");
////////////////////////////////////////////////////////////////////


TFile output (output_tot.c_str(), "RECREATE", "");
TTree out("tree","");


out.Branch("tau0_id1", &entry_1 ,"tau0_id1/D");
out.Branch("tau0_id2", &entry_31 ,"tau0_id2/D");
out.Branch("tau0_id3", &entry_12 ,"tau0_id3/D");
out.Branch("tau1_id1", &entry_2 ,"tau1_id1/D");
out.Branch("tau1_id2", &entry_3 ,"tau1_id2/D");
out.Branch("tau1_id3", &entry_4 ,"tau1_id3/D");
out.Branch("tau0_pt", &entry_5 ,"tau0_pt/D");
out.Branch("tau1_pt", &entry_6, "tau1_pt/D");
out.Branch("tau0_charge", &entry_7, "tau0_charge/D");
out.Branch("tau1_charge", &entry_8, "tau1_charge/D");
out.Branch("tau0_eta", &entry_9, "tau0_eta/D");
out.Branch("tau1_eta", &entry_10, "tau1_eta/D");
out.Branch("tau_n", &entry_11, "tau_n/D");
out.Branch("tau0_phi", &entry_13, "tau0_phi/D");
out.Branch("tau1_phi", &entry_14, "tau1_phi/D");
out.Branch("tau0_mass", &entry_15, "tau0_mass/D");
out.Branch("tau1_mass", &entry_16, "tau1_mass/D");
out.Branch("sist_mass", &entry_17, "sist_mass/D");
out.Branch("sist_acop", &entry_18, "sist_acop/D");
out.Branch("sist_pt", &entry_19, "sist_pt/D");
out.Branch("sist_rap", &entry_20, "sist_rap/D");
out.Branch("met_pt", &entry_21, "met_pt/D");
out.Branch("met_phi", &entry_22, "met_phi/D");
out.Branch("jet_pt", &entry_23, "jet_pt/D");
out.Branch("jet_eta", &entry_24, "jet_eta/D");
out.Branch("jet_phi", &entry_25, "jet_phi/D");
out.Branch("jet_mass", &entry_26, "jet_mass/D");
out.Branch("jet_btag", &entry_27, "jet_btag/D");
out.Branch("weight", &entry_28, "weight/D");
out.Branch("n_b_jet",&entry_29, "n_b_jet/I");
out.Branch("generator_weight", &entry_30, "generator_weight/D");
out.Branch("tau0_decay",&entry_32,"tau0_decay/D");
out.Branch("tau1_decay",&entry_33,"tau1_decay/D");
out.Branch("tau0_genmatch",&entry_34,"tau0_genmatch/D");
out.Branch("tau1_genmatch",&entry_35,"tau1_genmatch/D");
out.Branch("proton_xi_multi_rp", &entry_36, "proton_xi_multi_rp/D");
out.Branch("delta_phi", &entry_37,"delta_phi/D");
out.Branch("proton_xi1", &proton_xi1, "proton_xi1/D");
out.Branch("proton_xi2", &proton_xi2, "proton_xi2/D");
out.Branch("p_invariant_mass", &p_invariant_mass, "p_invariant_mass/D");
out.Branch("invariant_mass_tt_pair", &invariant_mass_tt_pair, "invariant_mass_tt_pair/D");
out.Branch("p_rapidity", &p_rapidity, "p_rapidity/D");
out.Branch("tt_rapidity", &tt_rapidity, "tt_rapidity/D");

TH1D histo("histo","histo", 1000, 0, 1000);


for(int i=0; i<tree->GetEntries(); i++){


	int eventos=tree->GetEvent(i);
	
	cout << "Progress: " << i << "/" <<tree->GetEntries() << endl;

	if(tree->GetLeaf("Tau_pt") -> GetLen() < 2) continue;
	
	tau0.SetPtEtaPhiM(tree->GetLeaf("Tau_pt")->GetValue(0),tree->GetLeaf("Tau_eta")->GetValue(0),tree->GetLeaf("Tau_phi")->GetValue(0),tree->GetLeaf("Tau_mass")->GetValue(0));
	tau1.SetPtEtaPhiM(tree->GetLeaf("Tau_pt")->GetValue(1),tree->GetLeaf("Tau_eta")->GetValue(1),tree->GetLeaf("Tau_phi")->GetValue(1),tree->GetLeaf("Tau_mass")->GetValue(1));

	//double e_pt = tree->GetLeaf("Electron_pt")->GetValue(0);

/* 	cout << "Numero de eletroes: " << tree->GetLeaf("Electron_pt")->GetLen() << endl;
	cout << "Energia do primeiro eletrao " << e_pt << endl ;
	cout << "ID eletrao: " << tree->GetLeaf("Electron_mvaFall17V2Iso_WP80")->GetValue(0) << endl << endl; */


	/* if(tree->GetLeaf("Electron_pt")->GetLen()>=1) histo.Fill(e_pt);

	if(i%1000==0) cout << "progress: " << double (i)/tree->GetEntries()*100 << endl; */

	// apply trigger
	if( tree->GetLeaf("HLT_DoubleMediumChargedIsoPFTauHPS40_Trk1_eta2p1_Reg")->GetValue(0)==1){

				
									
		entry_1=tree->GetLeaf("Tau_idDeepTau2017v2p1VSjet")->GetValue(0);
		entry_31=tree->GetLeaf("Tau_idDeepTau2017v2p1VSe")->GetValue(0);
		entry_12=tree->GetLeaf("Tau_idDeepTau2017v2p1VSmu")->GetValue(0);
		entry_2=tree->GetLeaf("Tau_idDeepTau2017v2p1VSjet")->GetValue(1);
		entry_3=tree->GetLeaf("Tau_idDeepTau2017v2p1VSe")->GetValue(1);
		entry_4=tree->GetLeaf("Tau_idDeepTau2017v2p1VSmu")->GetValue(1);



		
		entry_5=tree->GetLeaf("Tau_pt")->GetValue(0);
		entry_6=tree->GetLeaf("Tau_pt")->GetValue(1);

		
		entry_7=tree->GetLeaf("Tau_charge")->GetValue(0);
		entry_8=tree->GetLeaf("Tau_charge")->GetValue(1);

		
		entry_9=tree->GetLeaf("Tau_eta")->GetValue(0);
		entry_10=tree->GetLeaf("Tau_eta")->GetValue(1);

		
		
		entry_11=tree->GetLeaf("Tau_pt")->GetLen();

		
		entry_13=tree->GetLeaf("Tau_phi")->GetValue(0);
		entry_14=tree->GetLeaf("Tau_phi")->GetValue(1);

		
		entry_15=tree->GetLeaf("Tau_mass")->GetValue(0);
		entry_16=tree->GetLeaf("Tau_mass")->GetValue(1);
									
		tau0.SetPtEtaPhiM(entry_5,entry_9,entry_13,entry_15);
		tau1.SetPtEtaPhiM(entry_6,entry_10,entry_14,entry_16);
		sistema=tau0 + tau1;
		double Acoplanarity,deltaphi,deltaeta,invmasstt;


		
		deltaphi=fabs(entry_14-entry_13);

		if(deltaphi>M_PI)
			{
		deltaphi=deltaphi-2*M_PI;
			}

		Acoplanarity=fabs(deltaphi)/M_PI;

		entry_37=deltaphi;
									
		entry_17=sistema.M();
		entry_18=Acoplanarity;
		//cout<<entry_18<<endl;
		entry_19=sistema.Pt();
		entry_20=sistema.Rapidity();

		double cosh_delta_eta = cosh(entry_10- entry_9);

		invariant_mass_tt_pair=sqrt(2 * entry_5 * entry_6 * (cosh_delta_eta - cos(deltaphi)));
		// mx=sqrt(2pt1pt2 (cosh(delta eta - cos(delta phi))))




		entry_21=tree->GetLeaf("MET_phi")->GetValue(0);
		entry_22=tree->GetLeaf("MET_pt")->GetValue(0);
									
		entry_23=tree->GetLeaf("Jet_pt")->GetValue(0);
		entry_24=tree->GetLeaf("Jet_eta")->GetValue(0);
		entry_25=tree->GetLeaf("Jet_phi")->GetValue(0);
		entry_26=tree->GetLeaf("Jet_mass")->GetValue(0);
		entry_27=tree->GetLeaf("Jet_btagDeepB")->GetValue(0);
		entry_28=weight;

		int btag=0;

		for(int j=0; j<tree->GetLeaf("Jet_btagDeepB")->GetLen(); j++){

			if(tree->GetLeaf("Jet_btagDeepB")->GetValue(j)>0.4506) btag++;

		}

		entry_29=btag;
		entry_30=tree->GetLeaf("Generator_weight")->GetValue(0);

		entry_32=tree->GetLeaf("Tau_decayMode")->GetValue(0);
		entry_33=tree->GetLeaf("Tau_decayMode")->GetValue(1);
		entry_34=tree->GetLeaf("Tau_genPartFlav")->GetValue(0);
		entry_35=tree->GetLeaf("Tau_genPartFlav")->GetValue(1);

		entry_36=tree->GetLeaf("Proton_multiRP_xi")->GetValue(0);


				//calculations for tau
		TLorentzVector tau_pair = tau0 + tau1;
		// double invariant_mass_tt_pair = tau_pair.M(); // Invariant mass

		double rapidity_pair = tau_pair.Rapidity(); // rapidity

		tt_rapidity=rapidity_pair;

		//for proton ivnvariant mass and rapidity


		// if(tree->GetLeaf("Tau_pt")->GetLen() < 2) continue;

		tau0_pt = tree->GetLeaf("Tau_pt")->GetValue(0);
		tau1_pt = tree->GetLeaf("Tau_pt")->GetValue(1);
		tau0_eta = tree->GetLeaf("Tau_eta")->GetValue(0);
		tau1_eta = tree->GetLeaf("Tau_eta")->GetValue(1);
		tau0_phi = tree->GetLeaf("Tau_phi")->GetValue(0);
		tau1_phi = tree->GetLeaf("Tau_phi")->GetValue(1);

		// Initialize variables for protons
		double proton_xi1 = -1; 
		double proton_xi2 = -1; 


// Multi-RP proton selection: Loop over multi-RP protons
for (int j = 0; j < tree->GetLeaf("Proton_multiRP_xi")->GetLen(); j++) {
    int arm = tree->GetLeaf("Proton_multiRP_arm")->GetValue(j);  // Arm of the proton (0 or 1)
    double xi = tree->GetLeaf("Proton_multiRP_xi")->GetValue(j); // Fractional momentum loss (xi)


    // Select the largest xi for each arm
    if (arm == 0 && xi > proton_xi1) {
        proton_xi1 = xi; // Assign to Arm 0
    } else if (arm == 1 && xi > proton_xi2) {
        proton_xi2 = xi; // Assign to Arm 1
    }
}

if (proton_xi1 > 0 && proton_xi2 > 0) {
    double s = 13000 * 13000;  // Center-of-mass energy squared (13 TeV collisions)
    double invariant_mass = sqrt(s * proton_xi1 * proton_xi2);
    double proton_rapidity = 0.5 * log(proton_xi1 / proton_xi2);


	p_rapidity=proton_rapidity;
    p_invariant_mass=invariant_mass;

	cout << "P inv mass: " << p_invariant_mass ;

    std::cout << "Invariant Mass: " << invariant_mass
              << ", Rapidity: " << p_rapidity << std::endl;
		neta++;
		out.Fill();
}

	
		


		// neta++;
		// out.Fill();
							
		}
	
			

	//if(i%1000==0) cout << "progress: " << double (i)/tree->GetEntries()*100 << endl; 	
				
}
//cout<<"Eventos com eletroes e taus: "<<neletau<<endl;
//cout<<"Eventos apos id de eletroes e taus: "<<nid<<endl;
//cout<<"Eventos apos carga <0: "<<ncharge<<endl;
//cout<<"Eventos apos momento eletroes e taus: "<<npt<<endl;
//cout<<"Eventos apos angulo eta: "<<neta<<endl;
//histo.Draw("");
//cout << neletau <<endl;
cout << nid << endl;
cout << ncharge << endl;
cout << npt << endl;
cout << neta << endl;
cout << -11111111111111 << endl;

output.Write();
//app.Run(true);

return 0;


}


