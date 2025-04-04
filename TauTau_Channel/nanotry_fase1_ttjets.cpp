#include <stdio.h>
#include <iostream>
#include <cmath>
#include "TApplication.h"
#include "TFile.h"
#include "TF1.h"
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

  //string prefix = "root:://cms-xrd-global.cern.ch//";
	string input;

	string prefix_output = "ttjets_fase1_PIC_skimmed_TauTau_2018_";

	int k=0;

	double weight = 0.15;

	int numero_linha;
	cin >> numero_linha;

	stringstream ss;
	ss << numero_linha;

	string out_put;
	ss>>out_put;

	string output_tot = "/eos/user/m/mpisano/samples_2018_tautau/fase1/" + prefix_output + out_put + ".root";

	ifstream ifile;
	ifile.open("ttJets_fase1_PIC.txt");
	while(k<numero_linha) {
		ifile>>input;
		k++;
	}

	double entry_1,entry_2,entry_3,entry_4,entry_5,entry_6,entry_7,entry_8,entry_9,entry_10,entry_11,entry_12,entry_13,entry_14,entry_15,entry_16,entry_17,entry_18,entry_19,entry_20,entry_21,entry_22
	  ,entry_23,entry_24,entry_25,entry_26,entry_27,entry_28, entry_30, entry_31;
	int entry_29;
	//contadores
	double nid=0;
	double ncharge=0;
	double npt=0;

	////////////////////////////////////////
	TLorentzVector tau0;
	TLorentzVector tau1;
	TLorentzVector sistema;

	string total = input;
	cout <<"Input file: " << total << endl;

	TApplication app("app", NULL, NULL);
	cout<<"joaonunes"<<endl;
	//TFile *f = TFile::Open("root:://cms-xrd-global.cern.ch///store/mc/RunIISummer20UL18NanoAODv9/TTJets_TuneCP5_13TeV-amcatnloFXFX-pythia8/NANOAODSIM/106X_upgrade2018_realistic_v16_L1v1-v1/2520000/028FE21B-A107-4347-92C3-B533907C13DE.root");

	TFile *f = TFile::Open(total.c_str());
	cout<<"joaotavares"<<endl;

	TTree *tree = (TTree*) f->Get("tree");

	///A siguente linha tem de ser considerada apenas pelo fundo QCD
	TTree *tree_lumi = (TTree*) f->Get("LuminosityBlocks");
	////////////////////////////////////////////////////////////////////


	TFile output (output_tot.c_str(), "RECREATE", "");
	TTree out("tree","");


	out.Branch("tau0_id1", &entry_1 ,"tau0_id1/D");
	out.Branch("tau0_id2", &entry_11 ,"tau0_id2/D");
	out.Branch("tau0_id3", &entry_31 ,"tau0_id3/D");
	out.Branch("tau1_id1", &entry_2 ,"tau_id1/D");
	out.Branch("tau1_id2", &entry_3 ,"tau_id2/D");
	out.Branch("tau1_id3", &entry_4 ,"tau_id3/D");
	out.Branch("tau0_pt", &entry_5 ,"tau0_pt/D");
	out.Branch("tau1_pt", &entry_6, "tau1_pt/D");
	out.Branch("tau0_charge", &entry_7, "tau0_charge/D");
	out.Branch("tau1_charge", &entry_8, "tau1_charge/D");
	out.Branch("tau0_eta", &entry_9, "tau0_eta/D");
	out.Branch("tau1_eta", &entry_10, "tau1_eta/D");
	out.Branch("tau_n", &entry_12, "tau_n/D");
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


	TH1D histo("histo","histo", 1000, 0, 1000);

	TFile id_sf("TauID_SF_pt_DeepTau2017v2p1VSjet_UL2018.root");
        TF1 *f_id_sf = (TF1*) id_sf.Get("VTight_cent");

	TFile tauenergy("TauES_dm_DeepTau2017v2p1VSjet_2018ReReco.root");
        TH1F *histtauenergy = (TH1F*) tauenergy.Get("tes");

	for(int i=0; i<tree->GetEntries(); i++){


		int eventos=tree->GetEvent(i);

		

		double k_e = 1;
		double tau_id_sf=f_id_sf->Eval(tree->GetLeaf("tau0_pt")->GetValue(0))*f_id_sf->Eval(tree->GetLeaf("tau1_pt")->GetValue(0));


		tau0.SetPtEtaPhiM(k*tree->GetLeaf("tau0_pt")->GetValue(0),tree->GetLeaf("tau0_eta")->GetValue(0),tree->GetLeaf("tau0_phi")->GetValue(0),tree->GetLeaf("tau0_mass")->GetValue(0));
		tau1.SetPtEtaPhiM(k*tree->GetLeaf("tau1_pt")->GetValue(0),tree->GetLeaf("tau1_eta")->GetValue(0),tree->GetLeaf("tau1_phi")->GetValue(0),tree->GetLeaf("tau1_mass")->GetValue(0));


		//double e_pt = tree->GetLeaf("Electron_pt")->GetValue(0);

	/* 	cout << "Numero de eletroes: " << tree->GetLeaf("Electron_pt")->GetLen() << endl;
		cout << "Energia do primeiro eletrao " << e_pt << endl ;
		cout << "ID eletrao: " << tree->GetLeaf("Electron_mvaFall17V2Iso_WP80")->GetValue(0) << endl << endl; */


		/* if(tree->GetLeaf("Electron_pt")->GetLen()>=1) histo.Fill(e_pt);

		   if(i%1000==0) cout << "progress: " << double (i)/tree->GetEntries()*100 << endl; */


		if(tree->GetLeaf("tau0_id1")->GetValue(0) >64 && tree->GetLeaf("tau1_id1")->GetValue(0)>64  && tree->GetLeaf("tau0_id2")->GetValue(0) > 8 && tree->GetLeaf("tau1_id2")->GetValue(0) > 8 && tree->GetLeaf("tau0_id3")->GetValue(0) > 2 && tree->GetLeaf("tau1_id3")->GetValue(0) > 2 && tau0.DeltaR(tau1)>0.4 && abs(tau0.Eta())<2.4 && abs(tau1.Eta())<2.4){
		  nid=nid+weight*tau_id_sf;

		  if(k_e*tree->GetLeaf("tau0_pt")->GetValue(0)>100. && k_e*tree->GetLeaf("tau1_pt")->GetValue(0)>100.){
		    npt=npt+weight*tau_id_sf;
		    
		    if(tree->GetLeaf("tau0_charge")->GetValue(0)*tree->GetLeaf("tau1_charge")->GetValue(0)<0.){
		      ncharge=ncharge+weight*tau_id_sf;
				
										
		      entry_1=tree->GetLeaf("tau0_id1")->GetValue(0);
		      entry_11=tree->GetLeaf("tau0_id2")->GetValue(0);
		      entry_31=tree->GetLeaf("tau0_id3")->GetValue(0);
		      entry_2=tree->GetLeaf("tau1_id1")->GetValue(0);
		      entry_3=tree->GetLeaf("tau1_id2")->GetValue(0);
		      entry_4=tree->GetLeaf("tau1_id3")->GetValue(0);
		      entry_5=k_e*tree->GetLeaf("tau0_pt")->GetValue(0);
		      entry_6=k_e*tree->GetLeaf("tau1_pt")->GetValue(0);
		      entry_7=tree->GetLeaf("tau0_charge")->GetValue(0);
		      entry_8=tree->GetLeaf("tau1_charge")->GetValue(0);
		      entry_9=tree->GetLeaf("tau0_eta")->GetValue(0);
		      entry_10=tree->GetLeaf("tau1_eta")->GetValue(0);	  
		      entry_12=tree->GetLeaf("tau_n")->GetValue(0);
		      entry_13=tree->GetLeaf("tau0_phi")->GetValue(0);
		      entry_14=tree->GetLeaf("tau1_phi")->GetValue(0);
		      entry_15=tree->GetLeaf("tau0_mass")->GetValue(0);
		      entry_16=tree->GetLeaf("tau1_mass")->GetValue(0);
										
		      tau0.SetPtEtaPhiM(k_e*entry_5,entry_9,entry_13,entry_15);
		      tau1.SetPtEtaPhiM(k_e*entry_6,entry_10,entry_14,entry_16);
		      sistema=tau0+tau1;
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

		      entry_21=tree->GetLeaf("met_phi")->GetValue(0); // Energia dos neutrinos, não medida pelo detetor
		      entry_22=tree->GetLeaf("met_pt")->GetValue(0);
										
		      entry_23=tree->GetLeaf("jet_pt")->GetValue(0);
		      entry_24=tree->GetLeaf("jet_eta")->GetValue(0);
		      entry_25=tree->GetLeaf("jet_phi")->GetValue(0);
		      entry_26=tree->GetLeaf("jet_mass")->GetValue(0);
		      entry_27=tree->GetLeaf("jet_btag")->GetValue(0);
		      entry_28=weight*tau_id_sf;
		      entry_29=tree->GetLeaf("n_b_jet")->GetValue(0);
		      entry_30=tree->GetLeaf("generator_weight")->GetValue(0);

		      out.Fill();
		    }
		  }					
		}
		
				

		//if(i%1000==0) cout << "progress: " << double (i)/tree->GetEntries()*100 << endl; 	
					
	}
	cout<<"Eventos com Taus e Taus: "<<nid<<endl;
	cout<<"Eventos com carga oposta tautau: "<<ncharge<<endl;
	cout<<"Eventos apos corte no momento dos taus e taus: "<<npt<<endl;
	//histo.Draw("");
	cout << nid << endl;
	cout << ncharge << endl;
	cout << npt << endl;
	cout << -11111111111111 << endl;

	output.Write();
	//app.Run(true);

	return 0;


	}


