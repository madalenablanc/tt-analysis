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
	string input;	//guarda a linha que vai ser aberta

	string prefix_output = "CEP_Jets250_2018_UL_skimmed_MuTau_nano_";

	int k=0;

	double weight = 1.;

	int numero_linha;	//Pedir ao utilizador o número da linha para analizar
	cin >> numero_linha;

	stringstream ss;	//converter int -> string
	ss << numero_linha;

	string out_put;
	ss>>out_put;

	string output_tot = "/eos/user/m/mpisano/samples_2018/" + prefix_output + out_put + ".root";	//ficheiro onde vão ficar guardadas as variáveis. localização implícita

	ifstream ifile;
	ifile.open("CEP_Jets250_UL.txt");
	while(k<numero_linha) {
		ifile>>input;
		k++;
	}

	double entry_1,entry_2,entry_3,entry_4,entry_5,entry_6,entry_7,entry_8,entry_9,entry_10,entry_11,entry_12,entry_13,entry_14,entry_15,entry_16,entry_17,entry_18,entry_19,entry_20,entry_21,entry_22
	,entry_23,entry_24,entry_25,entry_26,entry_27,entry_28, entry_30;
	int entry_29;

	//contadores
	int nmutau=0;
	int nid=0;
	int ncharge=0;
	int npt=0;
	int neta=0;

	////////////////////////////////////////
	TLorentzVector tau;
	TLorentzVector muon;
	TLorentzVector sistema;

	string total = prefix+input;
	cout <<"Input file: " << total << endl;

	TApplication app("app", NULL, NULL);
	//cout<<"antes de abrir o ficheiro"<<endl;
	//TFile *f = TFile::Open("root:://cms-xrd-global.cern.ch///store/mc/RunIISummer20UL18NanoAODv9/TTJets_TuneCP5_13TeV-amcatnloFXFX-pythia8/NANOAODSIM/106X_upgrade2018_realistic_v16_L1v1-v1/2520000/028FE21B-A107-4347-92C3-B533907C13DE.root");

	TFile *f = TFile::Open(total.c_str());	//abre o ficheiro (linha selecionada) com os dados

	TTree *tree = (TTree*) f->Get("Events");

	///A seguinte linha tem de ser considerada apenas pelo fundo QCD
	TTree *tree_lumi = (TTree*) f->Get("LuminosityBlocks");
	////////////////////////////////////////////////////////////////////


	TFile output (output_tot.c_str(), "RECREATE", "");
	TTree out("tree","");


out.Branch("muon_id", &entry_1 ,"mu_id/D");
out.Branch("tau_id1", &entry_2 ,"tau_id1/D");
out.Branch("tau_id2", &entry_3 ,"tau_id2/D");
out.Branch("tau_id3", &entry_4 ,"tau_id3/D");
out.Branch("muon_pt", &entry_5 ,"mu_pt/D");
out.Branch("tau_pt", &entry_6, "tau_pt/D");
out.Branch("muon_charge", &entry_7, "mu_charge/D");
out.Branch("tau_charge", &entry_8, "tau_charge/D");
out.Branch("muon_eta", &entry_9, "mu_eta/D");
out.Branch("tau_eta", &entry_10, "tau_eta/D");
out.Branch("muon_n", &entry_11, "mu_n/D");
out.Branch("tau_n", &entry_12, "tau_n/D");
out.Branch("muon_phi", &entry_13, "muon_phi/D");
out.Branch("tau_phi", &entry_14, "tau_phi/D");
out.Branch("muon_mass", &entry_15, "muon_mass/D");
out.Branch("tau_mass", &entry_16, "tau_mass/D");
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


	//TH1D histo("histo","histo", 1000, 0, 1000);		//criar histograma para o nº de eventos em relação à massa invariante (Mx)

	
	for(int i=0; i<tree->GetEntries(); i++){


		int eventos=tree->GetEvent(i);	//tirar as duas linhas abaixo porque vão buscar os mesmos valores que se encontram em baixo.
	
		if(i%1000==0) cout << "progress: " << double (i)/tree->GetEntries()*100 << endl;


		if( tree->GetLeaf("HLT_IsoMu24")->GetValue(0)==1){    //garante a presença de um muão

			if ( tree->GetLeaf("Muon_pt")->GetLen()>0 && tree->GetLeaf("Tau_pt")->GetLen()>0){ //garante a presençã de um muão e de um tau hadronico
				nmutau++;
				
										entry_1=tree->GetLeaf("Muon_mvaId")->GetValue(0);
										entry_2=tree->GetLeaf("Tau_idDeepTau2017v2p1VSjet")->GetValue(0);
										entry_3=tree->GetLeaf("Tau_idDeepTau2017v2p1VSe")->GetValue(0);
										entry_4=tree->GetLeaf("Tau_idDeepTau2017v2p1VSmu")->GetValue(0);
										entry_5=tree->GetLeaf("Muon_pt")->GetValue(0);
										entry_6=tree->GetLeaf("Tau_pt")->GetValue(0);
										entry_7=tree->GetLeaf("Muon_charge")->GetValue(0);
										entry_8=tree->GetLeaf("Tau_charge")->GetValue(0);
										entry_9=tree->GetLeaf("Muon_eta")->GetValue(0);
										entry_10=tree->GetLeaf("Tau_eta")->GetValue(0);
										entry_11=tree->GetLeaf("Muon_pt")->GetLen();
										entry_12=tree->GetLeaf("Tau_pt")->GetLen();
										entry_13=tree->GetLeaf("Muon_phi")->GetValue(0);
										entry_14=tree->GetLeaf("Tau_phi")->GetValue(0);
										entry_15=tree->GetLeaf("Muon_mass")->GetValue(0);
										entry_16=tree->GetLeaf("Tau_mass")->GetValue(0);
										
										muon.SetPtEtaPhiM(entry_5,entry_9,entry_13,entry_15);
										tau.SetPtEtaPhiM(entry_6,entry_10,entry_14,entry_16);
										sistema=tau+muon;
										double Acoplanarity,deltaphi;
										deltaphi=fabs(entry_14-entry_13);

										if(deltaphi>M_PI)
										{
										deltaphi=deltaphi-2*M_PI;
										}

										Acoplanarity=fabs(deltaphi)/M_PI;
										
										entry_17=sistema.M();
										entry_18=Acoplanarity;
										//cout<<entry_18<<endl;
										entry_19=sistema.Pt();
										entry_20=sistema.Rapidity();

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

										out.Fill();
					
				}
		}
				

		//if(i%1000==0) cout << "progress: " << double (i)/tree->GetEntries()*100 << endl; 	
					
	}
	cout<<"Eventos com muoes e taus: "<<nmutau<<endl;

	output.Write();
	//app.Run(true);

	return 0;


	}

