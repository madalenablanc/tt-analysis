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

ifstream ele_trig;
ifstream ele_idISO;
ifstream ele_RECO;

void openFiles(){
 ele_trig.open("../emu_2018/ele_trig.txt");
 ele_idISO.open("../emu_2018/ele_iso_id.txt");
 ele_RECO.open("../emu_2018/ele_reco.txt");
}

double EleTrig(double pt,double eta, int flag=0){

        double sf=0;

        double eta_min, eta_max, pt_min, pt_max, corr, syst;

        ele_trig.clear();
        ele_trig.seekg(0, std::ios::beg);


        while(ele_trig >> eta_min >> eta_max >> pt_min >> pt_max >> corr >> syst){

              if(eta>eta_min && eta<=eta_max && pt>pt_min && pt<=pt_max) {if(flag==0) sf=corr; if(flag==1) sf=syst;}

        }


	if(abs(eta>2.4) || pt<35.) sf=1;

        return sf;


}

double EleIDISO(double pt,double eta, int flag =0){

        double sf=0;

        double eta_min, eta_max, pt_min, pt_max, corr, syst;

        ele_idISO.clear();
        ele_idISO.seekg(0, std::ios::beg);


        while(ele_idISO >> eta_min >> eta_max >> pt_min >> pt_max >> corr >> syst){

              if(eta>eta_min && eta<=eta_max && pt>pt_min && pt<=pt_max) {if(flag==0) sf=corr; if(flag==1) sf=syst;}

        }


        if(abs(eta>2.4) || pt<35.) sf=1;

        return sf;


}

double EleReco(double pt, double eta, int flag=0){


	double sf=0;

        double eta_min, eta_max, pt_min, pt_max, corr, syst;

        ele_RECO.clear();
        ele_RECO.seekg(0, std::ios::beg);


        while(ele_RECO >> eta_min >> eta_max >> pt_min >> pt_max >> corr >> syst){

              if(eta>eta_min && eta<=eta_max && pt>pt_min && pt<=pt_max) {if(flag==0) sf=corr; if(flag==1) sf=syst;}

        }


        if(abs(eta>2.4) || pt<35.) sf=1;

        return sf;

}


int main(){

        openFiles();

  //string prefix = "root:://cms-xrd-global.cern.ch//";
	string input;

	string prefix_output = "DY_fase1_francisco_skimmed_ETau_";

	int k=0;

	double weight = 1.;
	double weight_sample=1.81;

	int numero_linha;
	cin >> numero_linha;

	stringstream ss;
	ss << numero_linha;

	string out_put;
	ss>>out_put;

	string output_tot = "/eos/user/m/mpisano/samples_2018_etau/fase1_etau/" + prefix_output + out_put + ".root";

	ifstream ifile;
	ifile.open("DY_fase1_francisco.txt");
	while(k<numero_linha) {
		ifile>>input;
		k++;
	}

	double entry_1,entry_2,entry_3,entry_4,entry_5,entry_6,entry_7,entry_8,entry_9,entry_10,entry_11,entry_12,entry_13,entry_14,entry_15,entry_16,entry_17,entry_18,entry_19,entry_20,entry_21,entry_22
	,entry_23,entry_24,entry_25,entry_26,entry_27,entry_28, entry_30, entry_73, entry_74;
	int entry_29;
	//contadores
	int nid=0;
	int ncharge=0;
	int npt=0;

	////////////////////////////////////////
	TLorentzVector tau;
	TLorentzVector electron;
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
	//TTree *tree_lumi = (TTree*) f->Get("LuminosityBlocks");
	////////////////////////////////////////////////////////////////////


	TFile output (output_tot.c_str(), "RECREATE", "");
	TTree out("tree","");


out.Branch("eletron_id", &entry_1 ,"e_id/D");
out.Branch("tau_id1", &entry_2 ,"tau_id1/D");
out.Branch("tau_id2", &entry_3 ,"tau_id2/D");
out.Branch("tau_id3", &entry_4 ,"tau_id3/D");
out.Branch("eletron_pt", &entry_5 ,"e_pt/D");
out.Branch("tau_pt", &entry_6, "tau_pt/D");
out.Branch("eletron_charge", &entry_7, "e_charge/D");
out.Branch("tau_charge", &entry_8, "tau_charge/D");
out.Branch("eletron_eta", &entry_9, "e_eta/D");
out.Branch("tau_eta", &entry_10, "tau_eta/D");
out.Branch("eletron_n", &entry_11, "e_n/D");
out.Branch("tau_n", &entry_12, "tau_n/D");
out.Branch("electron_phi", &entry_13, "electron_phi/D");
out.Branch("tau_phi", &entry_14, "tau_phi/D");
out.Branch("electron_mass", &entry_15, "electron_mass/D");
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
 out.Branch("syst_e_trig", &entry_73, "syst_e_trig/D");
 out.Branch("syst_e_idiso", &entry_74, "syst_e_idiso/D");


	TH1D histo("histo","histo", 1000, 0, 1000);

	
	for(int i=0; i<tree->GetEntries(); i++){


		int eventos=tree->GetEvent(i);
		electron.SetPtEtaPhiM(tree->GetLeaf("e_pt")->GetValue(0),tree->GetLeaf("e_eta")->GetValue(0),tree->GetLeaf("electron_phi")->GetValue(0),tree->GetLeaf("electron_mass")->GetValue(0));
		tau.SetPtEtaPhiM(tree->GetLeaf("tau_pt")->GetValue(0),tree->GetLeaf("tau_eta")->GetValue(0),tree->GetLeaf("tau_phi")->GetValue(0),tree->GetLeaf("tau_mass")->GetValue(0));


		if(tree->GetLeaf("e_id")->GetValue(0)==1 && tree->GetLeaf("tau_id1")->GetValue(0)>=63 && tree->GetLeaf("tau_id2")->GetValue(0) >= 7 && tree->GetLeaf("tau_id3")->GetValue(0) >= 1 && electron.DeltaR(tau)>0.4 && abs(electron.Eta())<2.4 && abs(tau.Eta())<2.4){
		  
		  double ele_trig_sf= EleTrig(tree->GetLeaf("e_pt")->GetValue(0), tree->GetLeaf("e_eta")->GetValue(0));
	          double ele_idiso_sf= EleIDISO(tree->GetLeaf("e_pt")->GetValue(0), tree->GetLeaf("e_eta")->GetValue(0));
	          double ele_reco_sf = EleReco(tree->GetLeaf("e_pt")->GetValue(0), tree->GetLeaf("e_eta")->GetValue(0));

		  nid=nid+weight_sample*ele_trig_sf*ele_idiso_sf*ele_reco_sf;

		  if(tree->GetLeaf("e_pt")->GetValue(0)>35. && tree->GetLeaf("tau_pt")->GetValue(0)>100.){
		    npt = npt + weight_sample*ele_trig_sf*ele_idiso_sf*ele_reco_sf;
		    
		    if(tree->GetLeaf("tau_charge")->GetValue(0)*tree->GetLeaf("e_charge")->GetValue(0)<0.){
		      ncharge = ncharge + weight_sample*ele_trig_sf*ele_idiso_sf*ele_reco_sf;
				
                      double ele_trig_syst= EleTrig(tree->GetLeaf("e_pt")->GetValue(0), tree->GetLeaf("e_eta")->GetValue(0),1);
                      double ele_idiso_syst= EleIDISO(tree->GetLeaf("e_pt")->GetValue(0), tree->GetLeaf("e_eta")->GetValue(0),1);
                 
                      entry_73 = ele_trig_syst;
	              entry_74 = ele_idiso_syst;
										
		      entry_1=tree->GetLeaf("e_id")->GetValue(0);
		      entry_2=tree->GetLeaf("tau_id1")->GetValue(0);
		      entry_3=tree->GetLeaf("tau_id2")->GetValue(0);
		      entry_4=tree->GetLeaf("tau_id3")->GetValue(0);
		      entry_5=tree->GetLeaf("e_pt")->GetValue(0);
		      entry_6=tree->GetLeaf("tau_pt")->GetValue(0);
		      entry_7=tree->GetLeaf("e_charge")->GetValue(0);
		      entry_8=tree->GetLeaf("tau_charge")->GetValue(0);
		      entry_9=tree->GetLeaf("e_eta")->GetValue(0);
		      entry_10=tree->GetLeaf("tau_eta")->GetValue(0);
		      entry_11=tree->GetLeaf("e_pt")->GetLen();
		      entry_12=tree->GetLeaf("tau_pt")->GetLen();
		      entry_13=tree->GetLeaf("electron_phi")->GetValue(0);
		      entry_14=tree->GetLeaf("tau_phi")->GetValue(0);
		      entry_15=tree->GetLeaf("electron_mass")->GetValue(0);
		      entry_16=tree->GetLeaf("tau_mass")->GetValue(0);
										
		      electron.SetPtEtaPhiM(entry_5,entry_9,entry_13,entry_15);
		      tau.SetPtEtaPhiM(entry_6,entry_10,entry_14,entry_16);
		      sistema=tau+electron;
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

		      entry_21=tree->GetLeaf("met_phi")->GetValue(0);
		      entry_22=tree->GetLeaf("met_pt")->GetValue(0);
										
		      entry_23=tree->GetLeaf("jet_pt")->GetValue(0);
		      entry_24=tree->GetLeaf("jet_eta")->GetValue(0);
		      entry_25=tree->GetLeaf("jet_phi")->GetValue(0);
		      entry_26=tree->GetLeaf("jet_mass")->GetValue(0);
		      entry_27=tree->GetLeaf("jet_btag")->GetValue(0);
		      entry_28=weight_sample*weight*ele_trig_sf*ele_idiso_sf*ele_reco_sf;
		      entry_29=tree->GetLeaf("n_b_jet")->GetValue(0);
		      entry_30=tree->GetLeaf("generator_weight")->GetValue(0);

		      out.Fill();
		    }
		  }					
		}
		
				

		//if(i%1000==0) cout << "progress: " << double (i)/tree->GetEntries()*100 << endl; 	
					
	}
	cout<<"Eventos com eletroes e taus: "<<nid<<endl;
	cout<<"Eventos com carga oposta tau/eletrao: "<<ncharge<<endl;
	cout<<"Eventos apos corte no momento dos eletroes e taus: "<<npt<<endl;
	//histo.Draw("");
	cout << nid << endl;
	cout << ncharge << endl;
	cout << npt << endl;
	cout << -11111111111111 << endl;

	output.Write();
	//app.Run(true);

	return 0;


	}


