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

ifstream muon_neg18_lowTrig; 
ifstream muon_pos18_lowTrig; 
ifstream muon_18_highTrig; 
ifstream muon_IdISO;

void openFiles(){
 muon_neg18_lowTrig.open("../../emu_2018/muon_neg18_lowTrig.txt");
 muon_pos18_lowTrig.open("../../emu_2018/muon_pos18_lowTrig.txt");
 muon_18_highTrig.open("../../emu_2018/muon_18_highTrig.txt");
 muon_IdISO.open("../../emu_2018/muon_IdISO.txt");
}

//funções para calcular as correções

 double MuonTrigSF(double pt,double eta,double carga,double p_tot, int flag=0){


	double sf=0;

	muon_neg18_lowTrig.clear();
        muon_pos18_lowTrig.clear();
        muon_18_highTrig.clear();
	muon_neg18_lowTrig.seekg(0, std::ios::beg);
	muon_pos18_lowTrig.seekg(0, std::ios::beg);
        muon_18_highTrig.seekg(0, std::ios::beg);

        double eta_min, eta_max, pt_min, pt_max, corr, syst;

	if(pt<200. && carga<0.){

		while(muon_neg18_lowTrig >> eta_min >> eta_max >> pt_min >> pt_max >> corr >> syst){

			if(abs(eta)>eta_min && abs(eta)<=eta_max && pt>pt_min && pt<=pt_max) {if(flag==0) sf=corr; if(flag==1) sf=syst;};

		}

	}

	muon_neg18_lowTrig.clear();
        muon_pos18_lowTrig.clear();
        muon_18_highTrig.clear();
	muon_neg18_lowTrig.seekg(0, std::ios::beg);
        muon_pos18_lowTrig.seekg(0, std::ios::beg);
        muon_18_highTrig.seekg(0, std::ios::beg);

	if(pt<200. && carga>0.){

                while(muon_pos18_lowTrig >> eta_min >> eta_max >> pt_min >> pt_max >> corr >> syst){

                        if(abs(eta)>eta_min && abs(eta)<=eta_max && pt>pt_min && pt<=pt_max) {if(flag==0) sf=corr; if(flag==1) sf=syst;};

                }

        }

	muon_neg18_lowTrig.clear();
        muon_pos18_lowTrig.clear();
        muon_18_highTrig.clear();
	muon_neg18_lowTrig.seekg(0, std::ios::beg);
        muon_pos18_lowTrig.seekg(0, std::ios::beg);
        muon_18_highTrig.seekg(0, std::ios::beg);

	if(pt>200.){

                while(muon_18_highTrig >> eta_min >> eta_max >> pt_min >> pt_max >> corr >> syst){

                        if(abs(eta)>eta_min && abs(eta)<=eta_max && pt>pt_min && pt<=pt_max) {if(flag==0) sf=corr; if(flag==1) sf=syst;};

                }

        }

	muon_neg18_lowTrig.seekg(0, std::ios::beg);
        muon_pos18_lowTrig.seekg(0, std::ios::beg);
        muon_18_highTrig.seekg(0, std::ios::beg);

       if(abs(eta)>2.4 || pt<35.) sf=1.;

	return sf;
 }

double MuonIDISOSF(double pt,double eta,double carga,double p_tot, int flag=0){

	double sf=0;

	double eta_min, eta_max, pt_min, pt_max, corr, syst;

	muon_IdISO.clear();
        muon_IdISO.seekg(0, std::ios::beg);


        while(muon_IdISO >> eta_min >> eta_max >> pt_min >> pt_max >> corr >> syst){

              if(abs(eta)>eta_min && abs(eta)<=eta_max && pt>pt_min && pt<=pt_max) {if(flag==0) sf=corr; if(flag==1) sf=syst;}

        }


	if(abs(eta>2.4) || pt<35.) sf=1;

	return sf;


}

double muRecoSF(double pt, double eta, int flag=0){

	double sf=1;

	if(abs(eta)>2.1 && pt<200 && flag==0) sf=0.999;
	if(pt>200 && flag==0){

		double p_tot = pt/sin(2*atan(exp(-eta)));
		if(p_tot>100 && p_tot < 150 && abs(eta)<1.6) sf=0.9948;
		if(p_tot>100 && p_tot < 150 && abs(eta)>1.6) sf=0.993;
		if(p_tot>150 && p_tot < 200 && abs(eta)<1.6) sf=0.9950;
                if(p_tot>150 && p_tot < 200 && abs(eta)>1.6) sf=0.990;
		if(p_tot>200 && p_tot < 300 && abs(eta)<1.6) sf=0.994;
                if(p_tot>200 && p_tot < 300 && abs(eta)>1.6) sf=0.988;
		if(p_tot>300 && p_tot < 400 && abs(eta)<1.6) sf=0.9914;
                if(p_tot>300 && p_tot < 400 && abs(eta)>1.6) sf=0.981;
		if(p_tot>400 && p_tot < 600 && abs(eta)<1.6) sf=0.993;
                if(p_tot>400 && p_tot < 600 && abs(eta)>1.6) sf=0.983;
		if(p_tot>600 && p_tot < 1500 && abs(eta)<1.6) sf=0.991;
                if(p_tot>600 && p_tot < 1500 && abs(eta)>1.6) sf=0.978;
		if(p_tot>1500 && p_tot < 3500 && abs(eta)<1.6) sf=1.;
                if(p_tot>1500 && p_tot < 3500 && abs(eta)>1.6) sf=0.98;

	}

	if(pt<200. && flag==1){

		if(abs(eta)<0.9) sf =0.000382;
		if(abs(eta)>=0.9 && abs(eta)<1.2) sf=0.000522;
		if(abs(eta)>=1.2 && abs(eta)<2.1) sf=0.000831;
		if(abs(eta)>=2.1) sf = 0.001724;

	}

	if(pt>200. && flag==1){

		double p_tot = pt/sin(2*atan(exp(-eta)));

                if(abs(eta)<1.6 && p_tot<300.) sf =0.001;
                if(abs(eta)<1.6 && p_tot>=300 && p_tot<400) sf=0.0009;
                if(abs(eta)<1.6 && p_tot>=400 && p_tot<600) sf=0.002;
                if(abs(eta)<1.6 && p_tot>=600 && p_tot<1500) sf = 0.004;
		if(abs(eta)<1.6 && p_tot>=1500) sf = 0.1;

		if(abs(eta)>1.6 && p_tot<300.) sf =0.001;
                if(abs(eta)>1.6 && p_tot>=300 && p_tot<400) sf=0.002;
                if(abs(eta)>1.6 && p_tot>=400 && p_tot<600) sf=0.003;
                if(abs(eta)>1.6 && p_tot>=600 && p_tot<1500) sf = 0.006;
                if(abs(eta)>1.6 && p_tot>=1500) sf = 0.03;

        }



	return sf;
}

int main(){

openFiles();

TApplication app("app", NULL, NULL);

int k = 0;

int numero_linha;	//Pedir ao utilizador o número da linha para analizar
cin >> numero_linha;

stringstream ss;	//converter int -> string
ss << numero_linha;

string out_put;
ss>>out_put;

string input;
string endereco = "/eos/user/m/mpisano/samples_2018/";

string file_output = "DY_2018_UL_skimmed_MuTau_nano_cortes_" + out_put + ".root";
string endereco_out = "./ficheiros_fase1/";

ifstream ifile;
ifile.open("DY_reading.txt");
while(k<numero_linha) {
    ifile>>input;
    k++;
}

TFile *f = TFile::Open((endereco + input).c_str());

double entry_1,entry_2,entry_3,entry_4,entry_5,entry_6,entry_7,entry_8,entry_9,entry_10,entry_11,entry_12,entry_13,entry_14,entry_15,entry_16,entry_17,entry_18,entry_19,entry_20,entry_21,entry_22
,entry_23,entry_24,entry_25,entry_26,entry_27,entry_28, entry_30, entry_71, entry_72, entry_75;
int entry_29;

//contadores
double nmutau=0;
double nideta=0;
double ncharge=0;
double npt=0;

double weight_sample = 1.81;
double weight=1;

////////////////////////////////////////
TLorentzVector tau;
TLorentzVector muon;
TLorentzVector sistema;

TTree *tree = (TTree*) f->Get("tree");

TFile output ((endereco_out + file_output).c_str(), "RECREATE", "");
TTree out("tree_out","");

//alterar para mus
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
out.Branch("syst_mu_trig", &entry_71, "syst_mu_trig/D");
out.Branch("syst_mu_idiso", &entry_72, "syst_mu_idiso/D");
out.Branch("syst_mu_reco", &entry_75, "syst_mu_reco/D");


for(int i=0; i<tree->GetEntries(); i++){

int eventos=tree->GetEvent(i);	//tirar as duas linhas abaixo porque vão buscar os mesmos valores que se encontram em baixo.



if(tree->GetLeaf("met_phi")->GetLen()>0 && tree->GetLeaf("jet_phi")->GetLen()>0){ //
    
    entry_5=tree->GetLeaf("mu_pt")->GetValue(0);
    entry_6=tree->GetLeaf("tau_pt")->GetValue(0);
    entry_9=tree->GetLeaf("mu_eta")->GetValue(0);
    entry_10=tree->GetLeaf("tau_eta")->GetValue(0);
    entry_13=tree->GetLeaf("muon_phi")->GetValue(0);
    entry_14=tree->GetLeaf("tau_phi")->GetValue(0);
    entry_15=tree->GetLeaf("muon_mass")->GetValue(0);
    entry_16=tree->GetLeaf("tau_mass")->GetValue(0);
    
    muon.SetPtEtaPhiM(entry_5,entry_9,entry_13,entry_15);
    tau.SetPtEtaPhiM(entry_6,entry_10,entry_14,entry_16);

    double R=tau.DeltaR(muon);

    //Verificar o Id////////////
    if(tree->GetLeaf("mu_id")->GetValue(0)>3 && tree->GetLeaf("tau_id1")->GetValue(0)>63 && tree->GetLeaf("tau_id2")->GetValue(0) > 7 && tree->GetLeaf("tau_id3")->GetValue(0) > 1 && R>0.4 && fabs(tree->GetLeaf("tau_eta")->GetValue(0))<2.4 && tree->GetLeaf("mu_eta")->GetValue(0)<2.4){

	double mu_trig_sf = MuonTrigSF(tree->GetLeaf("mu_pt")->GetValue(0), tree->GetLeaf("mu_eta")->GetValue(0), tree->GetLeaf("mu_charge")->GetValue(0), tree->GetLeaf("mu_pt")->GetValue(0)/sin(2.*atan(exp(-tree->GetLeaf("mu_eta")->GetValue(0)))));
	double mu_idiso_sf = MuonIDISOSF(tree->GetLeaf("mu_pt")->GetValue(0), tree->GetLeaf("mu_eta")->GetValue(0), tree->GetLeaf("mu_charge")->GetValue(0), tree->GetLeaf("mu_pt")->GetValue(0)/sin(2.*atan(exp(-tree->GetLeaf("mu_eta")->GetValue(0)))));
	double mu_reco_sf = muRecoSF(tree->GetLeaf("mu_pt")->GetValue(0), tree->GetLeaf("mu_eta")->GetValue(0));

        nideta=nideta+weight_sample*mu_trig_sf*mu_idiso_sf*mu_reco_sf;
        
        if(tree->GetLeaf("mu_pt")->GetValue(0)>35. && tree->GetLeaf("tau_pt")->GetValue(0)>100.){
            npt=npt+weight_sample*mu_trig_sf*mu_idiso_sf*mu_reco_sf;

            if(tree->GetLeaf("tau_charge")->GetValue(0)*tree->GetLeaf("mu_charge")->GetValue(0)<0.){
                ncharge=ncharge+weight_sample*mu_trig_sf*mu_idiso_sf*mu_reco_sf;
		
		double mu_trig_syst = MuonTrigSF(tree->GetLeaf("mu_pt")->GetValue(0), tree->GetLeaf("mu_eta")->GetValue(0), tree->GetLeaf("mu_charge")->GetValue(0), tree->GetLeaf("mu_pt")->GetValue(0)/sin(2.*atan(exp(-tree->GetLeaf("mu_eta")->GetValue(0)))),1);
                double mu_idiso_syst = MuonIDISOSF(tree->GetLeaf("mu_pt")->GetValue(0), tree->GetLeaf("mu_eta")->GetValue(0), tree->GetLeaf("mu_charge")->GetValue(0), tree->GetLeaf("mu_pt")->GetValue(0)/sin(2.*atan(exp(-tree->GetLeaf("mu_eta")->GetValue(0)))),1);
		double mu_reco_syst = muRecoSF(tree->GetLeaf("mu_pt")->GetValue(0), tree->GetLeaf("mu_eta")->GetValue(0),1);
       
		entry_71=mu_trig_syst;
		entry_72=mu_idiso_syst;
		entry_75=mu_reco_syst;
 
                entry_1=tree->GetLeaf("mu_id")->GetValue(0);
                entry_2=tree->GetLeaf("tau_id1")->GetValue(0);
                entry_3=tree->GetLeaf("tau_id2")->GetValue(0);
                entry_4=tree->GetLeaf("tau_id3")->GetValue(0);
                entry_5=tree->GetLeaf("mu_pt")->GetValue(0);
                entry_6=tree->GetLeaf("tau_pt")->GetValue(0);
                entry_7=tree->GetLeaf("mu_charge")->GetValue(0);
                entry_8=tree->GetLeaf("tau_charge")->GetValue(0);
                entry_9=tree->GetLeaf("mu_eta")->GetValue(0);
                entry_10=tree->GetLeaf("tau_eta")->GetValue(0);
                entry_11=tree->GetLeaf("mu_pt")->GetLen();
                entry_12=tree->GetLeaf("tau_pt")->GetLen();
                entry_13=tree->GetLeaf("muon_phi")->GetValue(0);
                entry_14=tree->GetLeaf("tau_phi")->GetValue(0);
                entry_15=tree->GetLeaf("muon_mass")->GetValue(0);
                entry_16=tree->GetLeaf("tau_mass")->GetValue(0);
		
                
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

                entry_21=tree->GetLeaf("met_phi")->GetValue(0);
                entry_22=tree->GetLeaf("met_pt")->GetValue(0);
                
                entry_23=tree->GetLeaf("jet_pt")->GetValue(0);
                entry_24=tree->GetLeaf("jet_eta")->GetValue(0);
                entry_25=tree->GetLeaf("jet_phi")->GetValue(0);
                entry_26=tree->GetLeaf("jet_mass")->GetValue(0);
                entry_27=tree->GetLeaf("jet_btag")->GetValue(0);

                entry_28= weight_sample*weight*mu_trig_sf*mu_idiso_sf*mu_reco_sf;;

                entry_29=tree->GetLeaf("n_b_jet")->GetValue(0);
                entry_30=tree->GetLeaf("generator_weight")->GetValue(0);

                out.Fill();
                }
            }
        }
    }
    }

            

    //if(i%1000==0) cout << "progress: " << double (i)/tree->GetEntries()*100 << endl; 	
                

cout<<"Eventos com muoes e taus: "<<nmutau<<endl;
cout<<"Eventos apos id de muoes e taus e controlo de etas: "<<nideta<<endl;
cout<<"Eventos apos momento muoes e taus: "<<npt<<endl;
cout<<"Eventos apos carga <3: "<<ncharge<<endl;
//histo.Draw("");
//cout << nmutau <<endl;
// cout << nid << endl;
// cout << ncharge << endl;
// cout << npt << endl;
// cout << neta << endl;

output.Write();
//app.Run(true);

return 0;


}
