#include <iostream>
#include <cmath>
#include <fstream>
#include "TFile.h"
#include "TTree.h"
#include "TApplication.h"
#include "TGraphErrors.h"
#include "TBranch.h"
#include <vector>
#include "TH1D.h"
#include "TCanvas.h"
#include "TLorentzVector.h"
#include "TH2F.h"
#include "TF1.h"

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

       cout << "val func.   pt= " << pt << " eta= " << eta << " carga= " << carga << " ptot= " << p_tot <<endl;

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

			cout << eta_min << " " << eta_max << " " << pt_min << " " << pt_max << " " << corr << " " << syst << endl;
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



 double entry_1, entry_2, entry_3, entry_4, entry_5, entry_6, entry_7, entry_8, entry_9, entry_10, entry_11, entry_12, entry_13, entry_14, entry_15;
 double entry_16, entry_17, entry_18, entry_19, entry_20, entry_21, entry_22, entry_23, entry_24, entry_25, entry_26, entry_27, entry_28, entry_29, entry_30;
 double entry_31, entry_32, entry_33, entry_34, entry_35, entry_36, entry_37, entry_38, entry_39, entry_40, entry_41, entry_42, entry_43, entry_44, entry_45;
 double entry_46, entry_47, entry_48, entry_49, entry_50, entry_51, entry_52, entry_53, entry_54, entry_55, entry_56, entry_57, entry_58, entry_59, entry_60;
 double entry_61, entry_62, entry_63, entry_64, entry_65, entry_66, entry_67, entry_68, entry_69, entry_71, entry_72, entry_75, entry_76, entry_77, entry_78, entry_79;

 vector<float> entry_86;

 vector <float>* tau_id_full;
 vector <float>* e_id;
 vector <int>* tau_charge;
 vector <float>* e_charge;
 vector <float>* tau_pt;
 vector <float>* e_pt;
 vector <float>* tau_eta;
 vector <float>* e_eta;
 vector <float>* tau_phi;
 vector <float>* e_phi;
 vector <float>* tau_energy;
 vector <float>* e_energy;
 vector <float>* met_pt;
 vector <float>* met_phi;
 vector <float>* xi;
 vector <float>* thy;
 vector <float>* thx;
 vector <float>* y;
 vector <float>* x;
 vector <float>* t;
 vector <float>* arm;
 vector <float>* Tempo;
 vector <float>* trackx1;
 vector <float>* trackx2;
 vector <float>* tracky1;
 vector <float>* tracky2;
 vector <float>* trackrpid1;
 vector <float>* trackrpid2;
 vector <float>* trackthx1;
 vector <float>* trackthx2;
 vector <float>* trackthy1;
 vector <float>* trackthy2;
 vector <float>* tau_id;
 vector <float>* tau_id_antimu;
 vector <float>* tau_id_antie;
 vector <float>* tau_id_antij;
 vector <float>* tau_decay;
 vector <float>* jet_pt;
 vector <float>* jet_eta;
 vector <float>* jet_phi;
 vector <float>* jet_mass;
 vector <float>* jet_btag;
 vector <float>* gen_proton_xi;
 vector <float>* M_e_met;
 vector <float>* Y;
 vector <float>* weight_sm;
 vector <float>* bsm_weights;

 TBranch* tau_id_fullB;
 TBranch* e_idB;
 TBranch* tau_chargeB;
 TBranch* e_chargeB;
 TBranch* tau_ptB;
 TBranch* e_ptB;
 TBranch* tau_etaB;
 TBranch* e_etaB;
 TBranch* tau_phiB;
 TBranch* e_phiB;
 TBranch* tau_energyB;
 TBranch* e_energyB;
 TBranch* met_ptB;
 TBranch* met_phiB;
 TBranch* M_e_metB;
 TBranch* xiB;
 TBranch* thyB;
 TBranch* thxB;
 TBranch* yB;
 TBranch* xB;
 TBranch* tB;
 TBranch* armB;
 TBranch* TempoB;
 TBranch* trackx1B;
 TBranch* trackx2B;
 TBranch* tracky1B;
 TBranch* tracky2B;
 TBranch* trackrpid1B;
 TBranch* trackrpid2B;
 TBranch* trackthx1B;
 TBranch* trackthx2B;
 TBranch* trackthy1B;
 TBranch* trackthy2B;
 TBranch* tau_idB;
 TBranch* tau_id_antimuB;
 TBranch* tau_id_antieB;
 TBranch* tau_id_antijB;
 TBranch* tau_decayB;
 TBranch* jet_ptB;
 TBranch* jet_etaB;
 TBranch* jet_phiB;
 TBranch* jet_massB;
 TBranch* jet_btagB;
 TBranch* gen_proton_xiB;
 TBranch* YB;
 TBranch* weight_smB;
 TBranch* bsm_weightsB;



 TApplication app("app", NULL, NULL);

 int main(){

 openFiles();

 //Inicialização proões (tratação dos erros estatísticos)

 TFile proton_sist_1("../../PIC_joao/reco_charactersitics_version1.root");
TGraphErrors* xi_sist_1 = (TGraphErrors*)proton_sist_1.Get("2018_TS1_TS2/multi rp-0/xi/g_systematics_vs_xi");
TF1 xi_sist_inter_1("xi_sist_inter_1","pol20" ,0,10);
xi_sist_inter_1.SetParLimits(0,-.8,0.1);
xi_sist_inter_1.SetParLimits(1,0.1,1.);
xi_sist_1->Fit("xi_sist_inter_1");
TGraphErrors* xi_sist_2 = (TGraphErrors*)proton_sist_1.Get("2018_TS1_TS2/multi rp-1/xi/g_systematics_vs_xi");
TF1 xi_sist_inter_2("xi_sist_inter_2","pol20",0,10);
xi_sist_inter_2.SetParLimits(0,-1,0.1);
xi_sist_inter_2.SetParLimits(1,0.1,1);
xi_sist_2->Fit("xi_sist_inter_2");

 TLorentzVector ele, tau, nu;

 TFile fundo ("/eos/user/m/mpisano/analyzer_mutau/CMSSW_12_5_0/src/ExclTauTau/ExclTauTau/test/scalar_1200_gamma137/Data_MuTauTriggers_2018C_BSM_scalar_1200_nov.root");
 TTree* ntp1 = (TTree*) fundo.Get("ntp1");
 TFile output ("./MuTau_sinal_BSM_2018_scalar_m1200.root", "RECREATE", "");
 double weight_sample =0.;
 double weight =1;


 TTree out("tree","");
 out.Branch("mu_pt", &entry_1 ,"mu_pt/D");
 out.Branch("tau_pt", &entry_2, "tau_pt/D");
 out.Branch("mu_charge", &entry_3, "mu_charge/D");
 out.Branch("tau_charge", &entry_4, "tau_charge/D");
 out.Branch("mu_eta", &entry_5, "mu_eta/D");
 out.Branch("tau_eta", &entry_6, "tau_eta/D");
 out.Branch("muon_phi", &entry_7, "muon_phi/D");
 out.Branch("tau_phi", &entry_8, "tau_phi/D");
 out.Branch("mu_id", &entry_9, "mu_id/D");
 out.Branch("met_phi", &entry_66, "met_phi/D"); //o met phi e met pt estão trocados
 out.Branch("met_pt", &entry_67, "met_pt/D");
 out.Branch("M_e_met", &entry_68, "M_e_met/D");
 out.Branch("tau_id_full", &entry_10, "tau_id_full/D");
 out.Branch("sist_acop", &entry_11, "sist_acop/D");
 out.Branch("sist_pt", &entry_12, "sist_pt/D");
 out.Branch("sist_mass", &entry_13, "sist_mass/D");
 out.Branch("xi_arm1_1", &entry_14, "xi_arm1_1/D");
 out.Branch("xi_arm2_1", &entry_15, "xi_arm2_1/D");
 out.Branch("thy1", &entry_16, "thy1/D"); 
 out.Branch("thy2", &entry_17, "thy2/D");
 out.Branch("thx1", &entry_18, "thx1/D");
 out.Branch("thx2", &entry_19, "thx2/D");
 out.Branch("y1", &entry_20, "y1/D");
 out.Branch("y2", &entry_21, "y2/D");
 out.Branch("x1", &entry_22, "x1/D");
 out.Branch("x2", &entry_23, "x2/D");
 out.Branch("t1", &entry_24, "t1/D");
 out.Branch("t2", &entry_25, "t2/D");
 out.Branch("arm1", &entry_26, "arm1/D");
 out.Branch("arm2", &entry_27, "arm2/D");
 out.Branch("time1", &entry_28, "time1/D");
 out.Branch("time2", &entry_29, "time2/D");
 out.Branch("trackx1_1", &entry_30, "trackx1_1/D");
 out.Branch("trackx1_2", &entry_31, "trackx1_2/D");
 out.Branch("trackx2_1", &entry_32, "trackx2_1/D");
 out.Branch("trackx2_2", &entry_33, "trackx2_2/D");
 out.Branch("tracky1_1", &entry_34, "tracky1_1/D");
 out.Branch("tracky1_2", &entry_35, "tracky1_2/D");
 out.Branch("tracky2_1", &entry_36, "tracky2_1/D");
 out.Branch("tracky2_2", &entry_37, "tracky2_2/D");
 out.Branch("trackrpid1_1", &entry_38, "trackrpid1_1/D");
 out.Branch("trackrpid1_2", &entry_39, "trackrpid1_2/D");
 out.Branch("trackrpid2_1", &entry_40, "trackrpid2_1/D");
 out.Branch("trackrpid2_2", &entry_41, "trackrpid2_2/D");
 out.Branch("trackthx1_1", &entry_42, "trackthx1_1/D");
 out.Branch("trackthx1_2", &entry_43, "trackthx1_2/D");
 out.Branch("trackthx2_1", &entry_44, "trackthx2_1/D");
 out.Branch("trackthx2_2", &entry_45, "trackthx2_2/D");
 out.Branch("trackthy1_1", &entry_46, "trackthy1_1/D");
 out.Branch("trackthy1_2", &entry_47, "trackthy1_2/D");
 out.Branch("trackthy2_1", &entry_48, "trackthy2_1/D");
 out.Branch("trackthy2_2", &entry_49, "trackthy2_2/D");
 out.Branch("tau_energy", &entry_50, "tau_energy/D");
 out.Branch("tau_id", &entry_51, "tau_id");
 out.Branch("tau_id_antimu", &entry_52, "tau_id_antimu/D");
 out.Branch("tau_id_antie", &entry_53, "tau_id_antie/D");
 out.Branch("tau_id_antij", &entry_54, "tau_id_antij/D");
 out.Branch("tau_decay", &entry_55, "tau_decay/D");
 out.Branch("jet_pt", &entry_56, "jet_pt/D");
 out.Branch("jet_eta", &entry_57, "jet_eta/D");
 out.Branch("jet_phi", &entry_58, "jet_phi/D");
 out.Branch("jet_mass", &entry_59, "jet_mass/D");
 out.Branch("jet_btag", &entry_60, "jet_btag/D");
 out.Branch("e_energy", &entry_61, "e_energy/D");
 out.Branch("n_tau", &entry_62, "n_tau/D"); 
 out.Branch("n_mu", &entry_63, "n_mu/D");
 out.Branch("n_b_jet", &entry_64, "n_b_jet/D");
 out.Branch("weight", &entry_65, "weight/D");
 out.Branch("sist_rap", &entry_69, "sist_rap/D");
 out.Branch("syst_mu_trig", &entry_71, "syst_mu_trig/D");
 out.Branch("syst_mu_idiso", &entry_72, "syst_mu_idiso/D");
 out.Branch("syst_mu_reco", &entry_75, "syst_mu_reco/D");
 out.Branch("xi_arm1_1_up", &entry_76, "xi_arm1_1_up/D");
 out.Branch("xi_arm1_1_dw", &entry_77, "xi_arm1_1_dw/D");
 out.Branch("xi_arm2_1_up", &entry_78, "xi_arm2_1_up/D");
 out.Branch("xi_arm2_1_dw", &entry_79, "xi_arm2_1_dw/D");
 out.Branch("weights_bsm_sf",&entry_86);

 TH1D histo_e_pt ("histo_e_pt", "histo_e_pt",50,0,500);
 TH1D histo_tau_pt ("histo_tau_pt", "histo_tau_pt", 50,0,500);
 TH1D histo_n_tau ("histo_n_tau", "histo_n_tau", 50,0,10);
 TH1D histo_n_eletron ("histo_n_eletron", "histo_n_eletron", 50,0,10);
 TH1D histo_acop ("histo_acop", "histo_acop", 50,0,1);
 TH1D histo_Pt ("histo_Pt", "histo_Pt", 50,0,1000);
 TH1D histo_Mt ("histo_Mt", "histo_Mt", 50,0,1000);
 TH1D histo_DR ("histo_DR", "histo_DR", 50,0,5);


 double n_id =0;
 double n_charge =0;
 double n_pt =0;
 double n_eta =0;
 double n_rec =0;
 double n_p_up_xi=0;
 double n_p_dw_xi=0;
 double n_p_rad=0;
 float PI = 3.14159265359;
 float phi_e = 0;
 float phi_tau = 0;
 float acop = 0;
 float Pt = 0;
 float Mt = 0;
 float eta_tau = 0;
 float eta_e =0;
 float m_e_met=0;
 float delta_phi =0;
 float delta_eta = 0;
 float angular_dist =0;
 int elet_n = 0;
 int tau_n =0;
 int jet_b_n=0;



for (int i = 0; i < ntp1 -> GetEntries(); i++){


	ntp1 -> GetEvent(i);
	ntp1 -> SetBranchAddress("mu_pt", &e_pt, &e_ptB);
	ntp1 -> SetBranchAddress("tau_pt", &tau_pt, &tau_ptB);
	ntp1 -> SetBranchAddress("mu_charge", &e_charge, &e_chargeB);
	ntp1 -> SetBranchAddress("tau_charge", &tau_charge, &tau_chargeB);
	ntp1 -> SetBranchAddress("mu_id", &e_id, &e_idB);
	ntp1 -> SetBranchAddress("tau_id_full", &tau_id_full, &tau_id_fullB);
	ntp1 -> SetBranchAddress ("tau_eta", &tau_eta, &tau_etaB);
	ntp1 -> SetBranchAddress ("mu_eta", &e_eta, &e_etaB);
	ntp1 -> SetBranchAddress ("tau_phi", &tau_phi, &tau_phiB);
	ntp1 -> SetBranchAddress ("mu_phi", &e_phi, &e_phiB);	
	ntp1 -> SetBranchAddress ("tau_energy", &tau_energy, &tau_energyB);
	ntp1 -> SetBranchAddress ("mu_energy", &e_energy, &e_energyB);
	ntp1 -> SetBranchAddress ("met_pt", &met_pt, &met_ptB);
	ntp1 -> SetBranchAddress ("met_phi", &met_phi, &met_phiB);
	ntp1 -> SetBranchAddress ("proton_xi", &xi, &xiB);  
	ntp1 -> SetBranchAddress ("proton_thy", &thy, &thyB); 
	ntp1 -> SetBranchAddress ("proton_thx", &thx, &thxB);
        ntp1 -> SetBranchAddress ("proton_y", &y, &yB);
        ntp1 -> SetBranchAddress ("proton_x", &x, &xB);
        ntp1 -> SetBranchAddress ("proton_t", &t, &tB);
        ntp1 -> SetBranchAddress ("proton_arm", &arm, &armB);
        ntp1 -> SetBranchAddress ("proton_time", &Tempo, &TempoB);
        ntp1 -> SetBranchAddress ("proton_trackx1", &trackx1, &trackx1B);
        ntp1 -> SetBranchAddress ("proton_trackx2", &trackx2, &trackx2B);
        ntp1 -> SetBranchAddress ("proton_tracky1", &tracky1, &tracky1B);
        ntp1 -> SetBranchAddress ("proton_tracky2", &tracky2, &tracky2B);
        ntp1 -> SetBranchAddress ("proton_trackrpid1", &trackrpid1, &trackrpid1B); 
        ntp1 -> SetBranchAddress ("proton_trackrpid2", &trackrpid2, &trackrpid2B);
        ntp1 -> SetBranchAddress ("proton_trackthx1", &trackthx1, &trackthx1B);
        ntp1 -> SetBranchAddress ("proton_trackthx2", &trackthx2, &trackthx2B);
        ntp1 -> SetBranchAddress ("proton_trackthy1", &trackthy1, &trackthy1B);
        ntp1 -> SetBranchAddress ("proton_trackthy2", &trackthy2, &trackthy2B);
        ntp1 -> SetBranchAddress ("tau_id", &tau_id, &tau_idB);
        ntp1 -> SetBranchAddress ("tau_id_antimu", &tau_id_antimu, &tau_id_antimuB);
        ntp1 -> SetBranchAddress ("tau_id_antie", &tau_id_antie, &tau_id_antieB);
        ntp1 -> SetBranchAddress ("tau_id_antij", &tau_id_antij, &tau_id_antijB);
        ntp1 -> SetBranchAddress ("tau_decay", &tau_decay, &tau_decayB);
        ntp1 -> SetBranchAddress ("jet_pt", &jet_pt, &jet_ptB);
        ntp1 -> SetBranchAddress ("jet_eta", &jet_eta, &jet_etaB);
        ntp1 -> SetBranchAddress ("jet_phi", &jet_phi, &jet_phiB);
        ntp1 -> SetBranchAddress ("jet_mass", &jet_mass, &jet_massB);
        ntp1 -> SetBranchAddress ("jet_btag", &jet_btag, &jet_btagB);
	ntp1 -> SetBranchAddress ("gen_proton_xi", &gen_proton_xi, &gen_proton_xiB);
        //ntp1 -> SetBranchAddress ("weight_sm", &weight_sm, &weight_smB);
	//ntp1 -> SetBranchAddress ("bsm_weights", &bsm_weights, &bsm_weightsB);

        if ((e_charge)->size()==0 || (tau_charge)->size()==0) continue;

        weight_sample = 54900.*1./(4000.*998.);

           	        phi_e = (*e_phi)[0];
                                        phi_tau = (*tau_phi)[0];

                                        if ((*e_phi)[0] < 0.0) {
                                                phi_e = (*e_phi)[0] + 2*PI;
                                                }

                                        if ((*tau_phi)[0] < 0.0) {
                                                phi_tau = (*tau_phi)[0] + 2*PI;
                                                }

                                        delta_phi =(abs((phi_e) - (phi_tau)));
	acop = (delta_phi /PI);

                         

	eta_e = (*e_eta)[0];
	eta_tau = (*tau_eta)[0];
	delta_eta = (abs((eta_e) - (eta_tau)));
	angular_dist = sqrt( ((delta_phi) * (delta_phi)) + ((delta_eta) * (delta_eta))); 
	histo_DR.Fill(angular_dist);

	if ((*tau_id_full)[0]>0.5 && (*e_id)[0] >0.5 && abs((*tau_eta)[0]) < 2.4 && abs((*e_eta)[0]) < 2.4 && angular_dist> 0.4){

        double mu_trig_sf = MuonTrigSF((*e_pt)[0], (*e_eta)[0], (*e_charge)[0], (*e_pt)[0]/sin(2.*atan(exp(-(*e_eta)[0]))));
	double mu_idiso_sf = MuonIDISOSF((*e_pt)[0], (*e_eta)[0], (*e_charge)[0], (*e_pt)[0]/sin(2.*atan(exp(-(*e_eta)[0]))));
	double mu_reco_sf = muRecoSF((*e_pt)[0], (*e_eta)[0]);

	n_id= n_id + weight_sample*mu_trig_sf*mu_idiso_sf*mu_reco_sf;
	
	if ((*tau_pt)[0] >100 && (*e_pt)[0] >35.0 ){
        n_pt = n_pt + weight_sample*mu_trig_sf*mu_idiso_sf*mu_reco_sf;

	if (float((*e_charge)[0]) *float((*tau_charge)[0]) <0.0){
	n_charge = n_charge + weight_sample*mu_trig_sf*mu_idiso_sf*mu_reco_sf;
	

	if (abs((*tau_eta)[0]) < 2.4 && abs((*e_eta)[0]) < 2.4) {
	n_eta = n_eta + weight_sample*mu_trig_sf*mu_idiso_sf*mu_reco_sf;

	double mu_trig_syst = MuonTrigSF((*e_pt)[0], (*e_eta)[0], (*e_charge)[0], (*e_pt)[0]/sin(2.*atan(exp(-(*e_eta)[0]))),1);
        double mu_idiso_syst = MuonIDISOSF((*e_pt)[0], (*e_eta)[0], (*e_charge)[0], (*e_pt)[0]/sin(2.*atan(exp(-(*e_eta)[0]))),1);
        double mu_reco_syst = muRecoSF((*e_pt)[0], (*e_eta)[0],1);

        entry_71 = mu_trig_syst;
	entry_72 = mu_idiso_syst;
        entry_75 = mu_reco_syst;

	if (xi -> size()!=2) continue;

        n_rec=n_rec+weight_sample*mu_trig_sf*mu_idiso_sf*mu_reco_sf;

	histo_e_pt.Fill((*e_pt)[0]);
	histo_tau_pt.Fill((*tau_pt)[0]);
	histo_n_tau.Fill(tau_pt->size());
	histo_n_eletron.Fill(e_pt->size());

	




	                	  if (acop <= 1.0){
                                                histo_acop.Fill(acop);
                                                }
                                          else {
                                                acop = 2 - acop;
                                                delta_phi = 2*PI - delta_phi;
                                                histo_acop.Fill(acop);
                                                }



	ele.SetPtEtaPhiE((*e_pt)[0],(*e_eta)[0],(*e_phi)[0],(*e_energy)[0]);
	tau.SetPtEtaPhiE((*tau_pt)[0],(*tau_eta)[0],(*tau_phi)[0],(*tau_energy)[0]);
	nu.SetPtEtaPhiE((*met_pt)[0],0,(*met_phi)[0],(*met_pt)[0]);
	Pt = (ele + tau).Pt();
	cout<<Pt<<endl;
	Mt = (ele + tau).M();
	m_e_met=(ele+nu).Mt();
	histo_Pt.Fill(Pt);
	histo_Mt.Fill(Mt);

	//cout << "tamanho bsm_weights: " << bsm_weights->size() << endl;

        //double sm_cs = (*bsm_weights)[51];

        //for(int p=0; p<102; p++){

        //        (*bsm_weights)[p]=(*bsm_weights)[p]/sm_cs;

        //}

	//entry_86=(*bsm_weights);
	

	//Proton corrections

	//FIDUCIAL CUTS

		    //UPPER XI

		    int find_era = 0;
		    double xangle=0;

		    double fraction = (double) i/ double(ntp1 -> GetEntries()) ;

		    if(fraction<=0.134) xangle = 130.;
		    if((fraction>=0.134 && fraction <=0.149)) xangle = 131.;
		    if((fraction>=0.149 && fraction <=0.166)) xangle = 132.;
		    if((fraction >= 0.166 && fraction<=0.185)) xangle = 133.;
		    if((fraction>0.185 && fraction<=0.205)) xangle = 134.;
                    if((fraction>=20.5 && fraction <=0.227)) xangle = 135.;
                    if((fraction>=0.227 && fraction <=0.251)) xangle = 136.;
                    if((fraction>= 0.251 && fraction<=0.277)) xangle = 137.;
		    if((fraction>0.277 && fraction<=0.303)) xangle = 138.;
                    if((fraction>=0.303 && fraction <=0.331)) xangle = 139.;
                    if((fraction>=0.331 && fraction <=0.362)) xangle = 140.;
                    if((fraction>= 0.362 && fraction<=0.39)) xangle = 141.;
		    if((fraction>=0.39 && fraction<=0.42)) xangle = 142.;
                    if((fraction>=0.42 && fraction <=0.45)) xangle = 143.;
                    if((fraction>=0.45 && fraction <=0.488)) xangle = 144.;
                    if((fraction>=0.488 && fraction<=0.521)) xangle = 145.;
                    if((fraction>=0.521 && fraction<0.556)) xangle = 146.;
                    if((fraction>=0.556 && fraction <=0.595)) xangle = 147.;
                    if((fraction>=0.595 && fraction <=0.633)) xangle = 148.;
                    if((fraction>=0.633 && fraction<=0.673)) xangle = 149.;
                    if((fraction>=0.673 && fraction<=0.713)) xangle = 150.;
                    if((fraction>=0.713 && fraction <=0.748)) xangle = 151.;
                    if((fraction>=0.748 && fraction <=0.783)) xangle = 152.;
                    if((fraction>=0.783 && fraction<=0.816)) xangle = 153.;
                    if((fraction>=0.816 && fraction<=0.849)) xangle = 154.;
                    if((fraction>=0.849 && fraction<=0.879)) xangle = 155.;
                    if((fraction>=0.879 && fraction<=0.901)) xangle = 156.;
                    if((fraction>=0.901 && fraction<=0.916)) xangle = 157.;
                    if((fraction>=0.916 && fraction<=0.924)) xangle = 158.;
                    if((fraction>=0.924 && fraction <=0.925)) xangle = 159.;
                    if((fraction>=0.925 && fraction <=1.00)) xangle = 160.;
		      
		    TF1 limit_fun_arm_0("limit_fun_arm_0", "-(8.44219E-07*[xangle]-0.000100957)+(([xi]<(0.000247185*[xangle]+0.101599))*-(1.40289E-05*[xangle]-0.00727237)+([xi]>=(0.000247185*[xangle]+0.101599))*-(0.000107811*[xangle]-0.0261867))*([xi]-(0.000247185*[xangle]+0.101599))",0,1);
		    TF1 limit_fun_arm_1("limit_fun_arm_1", "-(-4.74758E-07*[xangle]+3.0881E-05)+(([xi]<(0.000727859*[xangle]+0.0722653))*-(2.43968E-05*[xangle]-0.0085461)+([xi]>=(0.000727859*[xangle]+0.0722653))*-(7.19216E-05*[xangle]-0.0148267))*([xi]-(0.000727859*[xangle]+0.0722653))",0,1);

		    limit_fun_arm_0.SetParameter("xangle", xangle);
		    limit_fun_arm_1.SetParameter("xangle", xangle);
		    limit_fun_arm_0.SetParameter("xi", (*xi)[0]);
		    limit_fun_arm_1.SetParameter("xi", (*xi)[1]);

		    cout << "upper limit arm 0: " << -limit_fun_arm_0.Eval(0.5) << " upper limit arm 1: " << -limit_fun_arm_1.Eval(0.5) << endl;

		    if((*thx)[0]> -limit_fun_arm_0.Eval(0.5) || (*thx)[1]> -limit_fun_arm_1.Eval(0.5)) continue;

		   

		   cout << "#######################ehi" << endl;
		    n_p_up_xi=n_p_up_xi+weight_sample*mu_trig_sf*mu_idiso_sf*mu_reco_sf;

		    cout << "x1: " << (*trackx1)[0] << " x2: " << (*trackx2)[0] << " y1: "<< (*tracky1)[0] << " y2: " << (*tracky2)[0] << " xi_1: " << (*xi)[0] << " xi_2: " << (*xi)[1]<<endl;

		    //DOWN XI LIMIT

		   //Era A
                    if(fraction <=0.21) {

		      //NEAR 45

                      if((*trackx1)[0]<2.710 || (*trackx1)[0]>17.927) continue;
                      if((*tracky1)[0]<-11.589 || (*tracky1)[0]>3.698) continue;

		      //FAR 45

                      if((*trackx2)[0]<2.278 || (*trackx2)[0]>24.620) continue;
                      if((*tracky2)[0]<-10.898 || (*tracky2)[0]>4.398) continue;

		      //NEAR56

		      if((*trackx1)[1]<3.000 || (*trackx1)[1]>18.498) continue;
                      if((*tracky1)[1]<-11.298 || (*tracky1)[1]>4.098) continue;

		      //FAR56

		     if((*trackx2)[1]<2.420 || (*trackx2)[1]>25.045) continue;
                     if((*tracky2)[1]<-10.398 || (*tracky2)[1]>5.098) continue;

                    }


		   //Era B1
		    if(fraction >0.21 && fraction <= 0.29) {

		      //NEAR 45

                      if((*trackx1)[0]<2.850 || (*trackx1)[0]>17.927) continue;
                      if((*tracky1)[0]<-11.589 || (*tracky1)[0]>3.698) continue;

                      //FAR 45

                      if((*trackx2)[0]<2.420 || (*trackx2)[0]>24.620) continue;
                      if((*tracky2)[0]<-10.798 || (*tracky2)[0]>4.298) continue;

                      //NEAR56

                      if((*trackx1)[1]<3.000 || (*trackx1)[1]>18.070) continue;
                      if((*tracky1)[1]<-11.198 || (*tracky1)[1]>4.098) continue;

                      //FAR56

                     if((*trackx2)[1]<2.420 || (*trackx2)[1]>25.045) continue;
                     if((*tracky2)[1]<-10.398 || (*tracky2)[1]>5.098) continue;

		    }

		   //Era B2

		  if(fraction >0.29 && fraction<=0.37) {

		    //NEAR 45

                    if((*trackx1)[0]<2.562 || (*trackx1)[0]>17.640) continue;
                    if((*tracky1)[0]<-11.098 || (*tracky1)[0]>4.198) continue;

                   //FAR 45

                    if((*trackx2)[0]<2.135 || (*trackx2)[0]>24.620) continue;
                    if((*tracky2)[0]<-11.398 || (*tracky2)[0]>3.798) continue;

                   //NEAR56

                    if((*trackx1)[1]<3.000 || (*trackx1)[1]>17.931) continue;
                    if((*tracky1)[1]<-10.498 || (*tracky1)[1]>4.698) continue;

                   //FAR56

                    if((*trackx2)[1]<2.279 || (*trackx2)[1]>24.760) continue;
                    if((*tracky2)[1]<-10.598 || (*tracky2)[1]>4.498) continue;


                    }

		   //Era C

		    if(fraction >0.37 && fraction <= 0.50) {

                   //NEAR 45

                    if((*trackx1)[0]<2.564 || (*trackx1)[0]>17.930) continue;
                    if((*tracky1)[0]<-11.098 || (*tracky1)[0]>4.198) continue;

                   //FAR 45

                    if((*trackx2)[0]<2.278 || (*trackx2)[0]>24.620) continue;
                    if((*tracky2)[0]<-11.398 || (*tracky2)[0]>3.698) continue;

                   //NEAR56

                    if((*trackx1)[1]<3.000 || (*trackx1)[1]>17.931) continue;
                    if((*tracky1)[1]<-10.498 || (*tracky1)[1]>4.698) continue;

                   //FAR56

                    if((*trackx2)[1]<2.279 || (*trackx2)[1]>24.760) continue;
                    if((*tracky2)[1]<-10.598 || (*tracky2)[1]>4.398) continue;
		    }

		   //Era D1

                    if(fraction >0.50 && fraction<=0.77) {

			//NEAR 45

                        if((*trackx1)[0]<2.847 || (*trackx1)[0]>17.930) continue;
                        if((*tracky1)[0]<-11.098 || (*tracky1)[0]>4.098) continue;

                        //FAR 45

                        if((*trackx2)[0]<2.278 || (*trackx2)[0]>24.620) continue;
                        if((*tracky2)[0]<-11.398 || (*tracky2)[0]>3.698) continue;

                        //NEAR56

                        if((*trackx1)[1]<3.000 || (*trackx1)[1]>17.931) continue;
                        if((*tracky1)[1]<-10.498 || (*tracky1)[1]>4.698) continue;

                        //FAR56

                        if((*trackx2)[1]<2.279 || (*trackx2)[1]>24.760) continue;
                        if((*tracky2)[1]<-10.598 || (*tracky2)[1]>4.398) continue;
                    }

                    

		   //Era D2

                    if(fraction >0.77) {

			//NEAR 45

                        if((*trackx1)[0]<2.847 || (*trackx1)[0]>17.931) continue;
                        if((*tracky1)[0]<-11.598 || (*tracky1)[0]>4.498) continue;

                        //FAR 45

                        if((*trackx2)[0]<2.278 || (*trackx2)[0]>24.620) continue;
                        if((*tracky2)[0]<-11.598 || (*tracky2)[0]>3.398) continue;

                        //NEAR56

                        if((*trackx1)[1]<3.000 || (*trackx1)[1]>17.931) continue;
                        if((*tracky1)[1]<-9.998 || (*tracky1)[1]>4.698) continue;

                        //FAR56

                        if((*trackx2)[1]<2.279 || (*trackx2)[1]>24.760) continue;
                        if((*tracky2)[1]<-10.598 || (*tracky2)[1]>3.898) continue;


                    }

		    n_p_dw_xi=n_p_dw_xi+weight_sample*mu_trig_sf*mu_idiso_sf*mu_reco_sf;

		    //RADIATION DAMAGE

		    //Adicionar seletor de periodo


		    TFile file_multi("/eos/project-c/ctpps/subsystems/Pixel/RPixTracking/pixelEfficiencies_multiRP_reMiniAOD.root");
		    TFile file_rad("/eos/project-c/ctpps/subsystems/Pixel/RPixTracking/pixelEfficiencies_radiation_reMiniAOD.root");

		    weight=1.;

		    if(fraction<=0.21){
		      TH2F *raddamage56_match = (TH2F *)file_multi.Get("Pixel/2018/2018A/h56_220_2018A_all_2D");
		      TH2F *raddamage56 = (TH2F *)file_rad.Get("Pixel/2018/2018A/h56_210_2018A_all_2D");
		      weight=weight*raddamage56->GetBinContent(raddamage56->FindBin((*trackx1)[1],(*tracky1)[1]));
	              weight=weight*raddamage56_match->GetBinContent(raddamage56_match->FindBin((*trackx1)[1],(*tracky1)[1]));
		      TH2F *raddamage45_match = (TH2F *)file_multi.Get("Pixel/2018/2018A/h45_220_2018A_all_2D");
		      TH2F *raddamage45 = (TH2F *)file_rad.Get("Pixel/2018/2018A/h45_210_2018A_all_2D");
		      weight=weight*raddamage45->GetBinContent(raddamage45->FindBin((*trackx1)[0],(*tracky1)[0]));
		      weight=weight*raddamage45_match->GetBinContent(raddamage45_match->FindBin((*trackx1)[0],(*tracky1)[0]));
		    }

		    if(fraction>0.21 && fraction<0.29){
		      TH2F *raddamage56_match = (TH2F *)file_multi.Get("Pixel/2018/2018B1/h56_220_2018B1_all_2D");
		      TH2F *raddamage56 = (TH2F *)file_rad.Get("Pixel/2018/2018B1/h56_210_2018B1_all_2D");
		      weight=weight*raddamage56->GetBinContent(raddamage56->FindBin((*trackx1)[1],(*tracky1)[1]));
		      weight=weight*raddamage56_match->GetBinContent(raddamage56_match->FindBin((*trackx1)[1],(*tracky1)[1]));
		      TH2F *raddamage45_match = (TH2F *)file_multi.Get("Pixel/2018/2018B1/h45_220_2018B1_all_2D");
		      TH2F *raddamage45 = (TH2F *)file_rad.Get("Pixel/2018/2018B1/h45_210_2018B1_all_2D");
		      weight=weight*raddamage45->GetBinContent(raddamage45->FindBin((*trackx1)[0],(*tracky1)[0]));
		      weight=weight*raddamage45_match->GetBinContent(raddamage45_match->FindBin((*trackx1)[0],(*tracky1)[0]));
		    }

		   if(fraction > 0.29 && fraction <=0.37){
                      TH2F *raddamage56_match = (TH2F *)file_multi.Get("Pixel/2018/2018B2/h56_220_2018B2_all_2D");
		      TH2F *raddamage56 = (TH2F *)file_rad.Get("Pixel/2018/2018B2/h56_210_2018B2_all_2D");
                      weight=weight*raddamage56->GetBinContent(raddamage56->FindBin((*trackx1)[1],(*tracky1)[1]));
		      weight=weight*raddamage56_match->GetBinContent(raddamage56_match->FindBin((*trackx1)[1],(*tracky1)[1]));
                      TH2F *raddamage45_match = (TH2F *)file_multi.Get("Pixel/2018/2018B2/h45_220_2018B2_all_2D");
                      TH2F *raddamage45 = (TH2F *)file_rad.Get("Pixel/2018/2018B2/h45_210_2018B2_all_2D");
                      weight=weight*raddamage45->GetBinContent(raddamage45->FindBin((*trackx1)[0],(*tracky1)[0]));
	 	      weight=weight*raddamage45_match->GetBinContent(raddamage45_match->FindBin((*trackx1)[0],(*tracky1)[0]));
                    }

		    if(fraction>0.37 && fraction <=0.50){
		      TH2F *raddamage56_match = (TH2F *)file_multi.Get("Pixel/2018/2018C/h56_220_2018C_all_2D");
		      TH2F *raddamage56 = (TH2F *)file_rad.Get("Pixel/2018/2018C/h56_210_2018C_all_2D");
		      weight=weight*raddamage56->GetBinContent(raddamage56->FindBin((*trackx1)[1],(*tracky1)[1]));
		      weight=weight*raddamage56_match->GetBinContent(raddamage56_match->FindBin((*trackx1)[1],(*tracky1)[1]));
		      TH2F *raddamage45_match = (TH2F *)file_multi.Get("Pixel/2018/2018C/h45_220_2018C_all_2D");
		      TH2F *raddamage45 = (TH2F *)file_rad.Get("Pixel/2018/2018C/h45_210_2018C_all_2D");
		      weight=weight*raddamage45->GetBinContent(raddamage45->FindBin((*trackx1)[0],(*tracky1)[0]));
		      weight=weight*raddamage45_match->GetBinContent(raddamage45_match->FindBin((*trackx1)[0],(*tracky1)[0]));
		    }

		    if(fraction>0.50 && fraction <=0.77){
		      TH2F *raddamage56_match = (TH2F *)file_multi.Get("Pixel/2018/2018D1/h56_220_2018D1_all_2D");
		      TH2F *raddamage56 = (TH2F *)file_rad.Get("Pixel/2018/2018D1/h56_210_2018D1_all_2D");
		      weight=weight*raddamage56->GetBinContent(raddamage56->FindBin((*trackx1)[1],(*tracky1)[1]));
		      weight=weight*raddamage56_match->GetBinContent(raddamage56_match->FindBin((*trackx1)[1],(*tracky1)[1]));
		      TH2F *raddamage45_match = (TH2F *)file_multi.Get("Pixel/2018/2018D1/h45_220_2018D1_all_2D");
		      TH2F *raddamage45 = (TH2F *)file_rad.Get("Pixel/2018/2018D1/h45_210_2018D1_all_2D");
		      weight=weight*raddamage45->GetBinContent(raddamage45->FindBin((*trackx1)[0],(*tracky1)[0]));
		      weight=weight*raddamage45_match->GetBinContent(raddamage45_match->FindBin((*trackx1)[0],(*tracky1)[0]));
		    }


		    if(fraction>0.77){
		      TH2F *raddamage56_match = (TH2F *)file_multi.Get("Pixel/2018/2018D2/h56_220_2018D2_all_2D");
		      TH2F *raddamage56 = (TH2F *)file_rad.Get("Pixel/2018/2018D2/h56_210_2018D2_all_2D");
		      weight=weight*raddamage56->GetBinContent(raddamage56->FindBin((*trackx1)[1],(*tracky1)[1]));
		      weight=weight*raddamage56_match->GetBinContent(raddamage56_match->FindBin((*trackx1)[1],(*tracky1)[1]));
		      TH2F *raddamage45_match = (TH2F *)file_multi.Get("Pixel/2018/2018D2/h45_220_2018D2_all_2D");
		      TH2F *raddamage45 = (TH2F *)file_rad.Get("Pixel/2018/2018D2/h45_210_2018D2_all_2D");
		      weight=weight*raddamage45->GetBinContent(raddamage45->FindBin((*trackx1)[0],(*tracky1)[0]));
		      weight=weight*raddamage45_match->GetBinContent(raddamage45_match->FindBin((*trackx1)[0],(*tracky1)[0]));

		    }
	cout << "weight rad " << weight << endl;

	n_p_rad=n_p_rad+weight*weight_sample*mu_trig_sf*mu_idiso_sf*mu_reco_sf;


	///////////////////////////////////

	entry_1 = (*e_pt)[0];
	entry_2 = (*tau_pt)[0]; 
                                        entry_3 = (*e_charge)[0];
                                        entry_4 = (*tau_charge)[0];
                                        entry_5 = (*e_eta)[0];
                                        entry_6 = (*tau_eta)[0];
                                        entry_7 = (*e_phi)[0];
                                        entry_8 = (*tau_phi)[0];
                                        entry_9 = (*e_id)[0];
                                        entry_10 = (*tau_id_full)[0];
                                        entry_11 = acop;
                                        entry_12 = Pt;
                                        entry_13 = Mt;
	entry_66 = (*met_phi)[0];
	entry_67 = (*met_pt)[0];
	entry_68 = m_e_met;
	entry_69 = (ele+tau).Rapidity();

	    if (xi -> size()==2){
                                        entry_14 = (*xi)[0];
	entry_15 = (*xi)[1];
                                        entry_16 = (*thy)[0];
	entry_17 = (*thy)[1];
	entry_18 = (*thx)[0];
	entry_19 = (*thx)[1];
                                        entry_20 = (*y)[0];
	entry_21 = (*y)[1];
                                        entry_22 = (*x)[0];
	entry_23 = (*x)[1];
                                        entry_24 = (*t)[0];
	entry_25 = (*t)[1];
                                        entry_26 = (*arm)[0];
	entry_27 = (*arm)[1];
                                        entry_28 = (*Tempo)[0];
	entry_29 = (*Tempo)[1];
                                        entry_30 = (*trackx1)[0];
	entry_31 = (*trackx1)[1];
                                        entry_32 = (*trackx2)[0];
                                        entry_33 = (*trackx2)[1];
	entry_34 = (*tracky1)[0];
	entry_35 = (*tracky1)[1];
                                        entry_36 = (*tracky2)[0];
	entry_37 = (*tracky2)[1];
                                        entry_38 = (*trackrpid1)[0];
	entry_39 = (*trackrpid1)[1];
                                        entry_40 = (*trackrpid2)[0];
	entry_41 = (*trackrpid2)[1];
                                        entry_42 = (*trackthx1)[0];
	entry_43 = (*trackthx1)[1];
	entry_44 = (*trackthx2)[0];
                                        entry_45 = (*trackthx2)[1];
                                        entry_46 = (*trackthy1)[0];
	entry_47 = (*trackthy1)[1];
                                        entry_48 = (*trackthy2)[0];
	entry_49 = (*trackthy2)[1];
	}
	else {
	continue;
	}


	//if(gen_proton_xi->size()>=2){
	// entry_14 = (*gen_proton_xi)[0];
        //                                 entry_15 = (*gen_proton_xi)[1];
	//}

                                        entry_50 = (*tau_energy)[0];
                                        entry_51 = (*tau_id)[0];
                                        entry_52 = (*tau_id_antimu)[0];
                                        entry_53 = (*tau_id_antie)[0];
                                        entry_54 = (*tau_id_antij)[0];
                                        entry_55 = (*tau_decay)[0];
                                        entry_56 = (*jet_pt)[0];
                                        entry_57 = (*jet_eta)[0];
                                        entry_58 = (*jet_phi)[0];
                                        entry_59 = (*jet_mass)[0];



	 for (int j = 0; j<jet_btag -> size(); j++){
                                                if ((*jet_btag)[j]>0.5){
                                                        jet_b_n = jet_b_n +1;
                                                }
                                        }

                                        entry_60 = (*jet_btag)[0];
                                        entry_61 = (*e_energy)[0];


	 for (int j = 0; j<tau_id_full -> size(); j++){
                                                if ((*tau_id_full)[j]>0.5){
                                                        tau_n = tau_n + 1;
                                                }
                                        }
                                        entry_62 = tau_n;
	tau_n=0;

                                        for (int j = 0; j<e_id -> size(); j++){
                                                if ((*e_id)[j]>0.5){
                                                        elet_n = elet_n + 1;
                                                }
                                        }

	entry_63 = elet_n;
	entry_64 = jet_b_n;

	elet_n=0;
	jet_b_n=0;

	entry_65=weight_sample*weight*mu_trig_sf*mu_idiso_sf*mu_reco_sf;

        entry_76=entry_14+xi_sist_inter_1.Eval(entry_14);
        entry_77=entry_14-xi_sist_inter_1.Eval(entry_14);
        entry_78=entry_15+xi_sist_inter_2.Eval(entry_15);
        entry_79=entry_15-xi_sist_inter_2.Eval(entry_15);

	out.Fill();
                                        



	}	 	
	}	
	}
	}
	

	}
TCanvas c1;
histo_e_pt.Draw("histo");

TCanvas c2;
histo_tau_pt.Draw("histo");

TCanvas c3;
histo_n_tau.Draw("histo");

TCanvas c4;
histo_n_eletron.Draw("histo");

TCanvas c5;
histo_acop.Draw("histo");

TCanvas c6;
histo_Pt.Draw("histo");

TCanvas c7;
histo_Mt.Draw("histo");

TCanvas c8;
histo_DR.Draw("histo");

cout << "os acontecimentos já foram moltiplicados por o sample weight" << endl;

cout << "O número total de acontecimentos é "<<ntp1->GetEntries()*weight_sample <<endl;
cout << "O número de partículas com 1e e 1th é "<<n_id <<endl;
cout << "O n  mero de part  culas com tau pt e electron pt    "<<n_pt <<endl;
cout << "O número de partículas com o produto das cargas positivos é "<<n_charge <<endl;
cout << "O número de partículas com 2 prot reconstruidos é "<<n_rec <<endl;
cout << "O n  mero de part  culas com 2 prot reconstruidos que passam o up_xi cut é   " << n_p_up_xi << endl;
cout << "O n  mero de part  culas com 2 prot reconstruidos que passam o dw_xi cut      " << n_p_dw_xi << endl;
cout << "O n  mero de part  culas com 2 prot reconstruidos que passam o radiation damage cut       " << n_p_rad << endl;

output.Write();
app.Run(true);
return 0;
}

