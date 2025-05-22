#include <iostream>

#include <cmath>

#include "TFile.h"

#include "TTree.h"

#include "TApplication.h"

#include "TGraphErrors.h"

#include "TBranch.h"

#include <vector>

#include "TH1D.h"

#include "TCanvas.h"

#include "TLorentzVector.h"

#include "TF1.h"

#include "TH1F.h"

#include "TH2F.h"



using namespace std;




double entry_1, entry_2, entry_3, entry_4, entry_5, entry_6, entry_7, entry_8, entry_9, entry_10, entry_11, entry_12, entry_13, entry_14, entry_15;

double entry_16, entry_17, entry_18, entry_19, entry_20, entry_21, entry_22, entry_23, entry_24, entry_25, entry_26, entry_27, entry_28, entry_29, entry_30;

double entry_31, entry_32, entry_33, entry_34, entry_35, entry_36, entry_37, entry_38, entry_39, entry_40, entry_41, entry_42, entry_43, entry_44, entry_45;

double entry_46, entry_47, entry_48, entry_49, entry_50, entry_51, entry_52, entry_53, entry_54, entry_55, entry_56, entry_57, entry_58, entry_59, entry_60;

double entry_61, entry_62, entry_63, entry_64, entry_65, entry_66, entry_67, entry_68, entry_69, entry_70,entry_71,entry_72,entry_73;

double entry_74, entry_75, entry_76, entry_77, entry_78, entry_79, entry_80, entry_81, entry_82, entry_83, entry_84, entry_85;

vector<float> entry_86;


vector <float>* tau_id_full;


vector <float>* tau_charge;

vector <float>* tau_pt;

vector <float>* tau_eta;

vector <float>* tau_phi;

vector <float>* tau_energy;

vector <float>* tau_energy_up;

vector <float>* tau_energy_dw;

vector <float>* met_pt;

vector <float>* met_phi;

vector <float>* xi;

vector <float>* xi_up;

vector <float>* xi_down;

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

vector <float>* tau_id_sf;

vector <float>* tau_id_sf_up;

vector <float>* tau_id_sf_dw;

//vector <float>* weight_sm;

//vector <float>* bsm_weights;


TBranch* tau_id_fullB;

TBranch* tau_idB;

TBranch* tau_chargeB;

TBranch* tau_ptB;

TBranch* tau_etaB;

TBranch* tau_phiB;

TBranch* tau_energyB;

TBranch* tau_energy_upB;

TBranch* tau_energy_dwB;

TBranch* met_ptB;

TBranch* met_phiB;

TBranch* M_e_metB;

TBranch* xiB;

TBranch* xi_upB;

TBranch* xi_downB;

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

TBranch* tau_id_sfB;

TBranch* tau_id_sf_upB;

TBranch* tau_id_sf_dwB;

//TBranch* weight_smB;

//TBranch* bsm_weightsB;



TApplication app("app", NULL, NULL);


int main(){



TLorentzVector tau0, tau1, nu;


TFile fundo ("/eos/user/m/mpisano/analyser/CMSSW_12_5_0/src/ExclTauTau/ExclTauTau/test/Data_TauPairTriggers_2018_UL_sgn_BSM_scalar_M400_NWA_nov23.root");

TTree* ntp1 = (TTree*) fundo.Get("ntp1");

TFile output ("./TauTau_sinal_PIC_scalar_400_NWA_nov_2018.root", "RECREATE", "");

//double weight_sample =0.000190;

double weight_sample=0;

double weight = 1.;

 double fraction;

 double n_final = 0;

TTree out("tree","");

out.Branch("tau0_pt", &entry_1 ,"tau0_pt/D");

out.Branch("tau1_pt", &entry_2, "tau1_pt/D");

out.Branch("tau0_charge", &entry_3, "tau0_charge/D");

out.Branch("tau1_charge", &entry_4, "tau1_charge/D");

out.Branch("tau0_eta", &entry_5, "tau0_eta/D");

out.Branch("tau1_eta", &entry_6, "tau1_eta/D");

out.Branch("tau0_phi", &entry_7, "tau0_phi/D");

out.Branch("tau1_phi", &entry_8, "tau1_phi/D");

out.Branch("tau0_id_full", &entry_9, "tau0_id_full/D");

out.Branch("met_phi", &entry_66, "met_phi/D"); //o met phi e met pt estão trocados

out.Branch("met_pt", &entry_67, "met_pt/D");

out.Branch("M_e_met", &entry_68, "M_e_met/D");

out.Branch("tau1_id_full", &entry_10, "tau1_id_full/D");

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

out.Branch("tau0_energy", &entry_50, "tau_energy/D");

 out.Branch("tau0_id", &entry_51, "tau_id");

 out.Branch("tau0_id_antimu", &entry_52, "tau_id_antimu/D");

 out.Branch("tau0_id_antie", &entry_53, "tau_id_antie/D");

 out.Branch("tau0_id_antij", &entry_54, "tau_id_antij/D");

 out.Branch("tau1_id_antimu", &entry_70, "tau_id_antimu/D");

 out.Branch("tau1_id_antie", &entry_71, "tau_id_antie/D");

 out.Branch("tau1_id_antij", &entry_72, "tau_id_antij/D");

 out.Branch("tau0_decay", &entry_55, "tau_decay/D");
 
 out.Branch("tau1_decay", &entry_73, "tau_decay/D");

 out.Branch("jet_pt", &entry_56, "jet_pt/D");

 out.Branch("jet_eta", &entry_57, "jet_eta/D");

 out.Branch("jet_phi", &entry_58, "jet_phi/D");

 out.Branch("jet_mass", &entry_59, "jet_mass/D");

 out.Branch("jet_btag", &entry_60, "jet_btag/D");

 out.Branch("tau1_energy", &entry_61, "e_energy/D");

 out.Branch("n_tau", &entry_62, "n_tau/D"); 

out.Branch("tau1_id", &entry_63, "tau1_id/D");

out.Branch("n_b_jet", &entry_64, "n_b_jet/D");

out.Branch("weight", &entry_65, "weight/D");

out.Branch("sist_rap", &entry_69, "sist_rap/D");

out.Branch("tau_id_sf", &entry_74, "tau_id_sf/D");

out.Branch("tau_id_sf_up", &entry_75, "tau_id_sf_up/D");

out.Branch("tau_id_sf_dw", &entry_76, "tau_id_sf_dw/D");

out.Branch("tau_energy_up", &entry_77, "tau_energy_up/D");

out.Branch("tau_energy_dw", &entry_78, "tau_energy_dw/D");

out.Branch("xi_arm1_1_up", &entry_79, "xi_arm1_1_up/D");

out.Branch("xi_arm1_1_dw", &entry_80, "xi_arm1_1_down/D");

out.Branch("xi_arm2_1_up", &entry_81, "xi_arm2_1_up/D");

out.Branch("xi_arm2_1_dw", &entry_82, "xi_arm2_1_down/D");

out.Branch("tau_trigger_sf", &entry_83, "tau_trigger_sf/D");

out.Branch("tau_trigger_sf_up", &entry_84, "tau_trigger_sf_up/D");

out.Branch("tau_trigger_sf_dw", &entry_85, "tau_trigger_sf_dw/D");

//out.Branch("weights_bsm_sf",&entry_86);




double n_id =0;

double n_charge =0;

double n_pt =0;

double n_eta =0;

double n_rec =0;

double n_p_up_xi=0;

double n_p_dw_xi=0;

double n_p_rad=0;

float PI = 3.14159265359;

float phi_tau0 = 0;

float phi_tau1 = 0;

float acop = 0;

float Pt = 0;

float Mt = 0;

float eta_tau0 = 0;

float eta_tau1 =0;

float m_e_met=0;

float delta_phi =0;

float delta_eta = 0;

float angular_dist =0;
 
int tau_n =0;

int jet_b_n=0;
 
 double k =0.0;

TFile proton_sist_1("../PIC_joao/reco_charactersitics_version1.root");
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


//xi_sist_2->Draw("AP");
//xi_sist_2->GetXaxis()->SetRangeUser(0.02,0.15);
//xi_sist_inter_2.Draw("same");
//app.Run(true);

TFile file_multi("/eos/project-c/ctpps/subsystems/Pixel/RPixTracking/pixelEfficiencies_multiRP_reMiniAOD.root");
TFile file_rad("/eos/project-c/ctpps/subsystems/Pixel/RPixTracking/pixelEfficiencies_radiation_reMiniAOD.root");
TFile id_sf("TauID_SF_pt_DeepTau2017v2p1VSjet_UL2018.root");

TF1 *f_id_sf = (TF1*) id_sf.Get("VTight_cent");
TF1 *f_id_sf_up = (TF1*) id_sf.Get("VTight_up");
TF1 *f_id_sf_down = (TF1*) id_sf.Get("VTight_down");

TFile trigger_tau_sf("2018UL_tauTriggerEff_DeepTau2017v2p1.root");
TH1F *trig_tau_sf = (TH1F*) trigger_tau_sf.Get("sf_ditau_VTight_dmall_fitted");


for (int i = 0; i < ntp1 -> GetEntries(); i++){


  fraction = double(i)/double((ntp1 -> GetEntries()));

  
	 ntp1 -> GetEvent(i);

	 ntp1 -> SetBranchAddress("tau_pt", &tau_pt, &tau_ptB);

	 ntp1 -> SetBranchAddress("tau_charge", &tau_charge, &tau_chargeB);

	 ntp1 -> SetBranchAddress("tau_id_full", &tau_id_full, &tau_id_fullB);

	 ntp1 -> SetBranchAddress ("tau_eta", &tau_eta, &tau_etaB);

	 ntp1 -> SetBranchAddress ("tau_phi", &tau_phi, &tau_phiB);	

	 ntp1 -> SetBranchAddress ("tau_energy", &tau_energy, &tau_energyB);

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


	 if  ((tau_charge)->size()<=1) continue;


         weight_sample = 54900.*(1.)/(4000.*1000.);

	 phi_tau0 = (*tau_phi)[0];

	 phi_tau1 = (*tau_phi)[1];


	 if ((*tau_phi)[0] < 0.0) {

	 phi_tau0 = (*tau_phi)[0] + 2*PI;

	 }


	 if ((*tau_phi)[1] < 0.0) {

	 phi_tau1 = (*tau_phi)[1] + 2*PI;

	 }


	 delta_phi =(abs((phi_tau0) - (phi_tau1)));

	 acop = (delta_phi /PI);


	 


	 eta_tau0 = (*tau_eta)[0];

	 eta_tau1 = (*tau_eta)[1];

	 delta_eta = (abs((eta_tau0) - (eta_tau1)));

	 angular_dist = sqrt( ((delta_phi) * (delta_phi)) + ((delta_eta) * (delta_eta))); 



	 TFile trigger_tau_sf("2018UL_tauTriggerEff_DeepTau2017v2p1.root");
	 TH1F *trig_tau_sf = (TH1F*) trigger_tau_sf.Get("sf_ditau_VTight_dmall_fitted");

	 double tau_id_sf=f_id_sf->Eval((*tau_pt)[0])*f_id_sf->Eval((*tau_pt)[1])*trig_tau_sf->GetBinContent(trig_tau_sf->FindBin((*tau_pt)[0]))*trig_tau_sf->GetBinContent(trig_tau_sf->FindBin((*tau_pt)[1]));

	 entry_74=f_id_sf->Eval((*tau_pt)[0]);
	 entry_75=f_id_sf_up->Eval((*tau_pt)[0]);
	 entry_76=f_id_sf_down->Eval((*tau_pt)[0]);

	 entry_83=trig_tau_sf->GetBinContent(trig_tau_sf->FindBin((*tau_pt)[0]));
	 entry_84=trig_tau_sf->GetBinContent(trig_tau_sf->FindBin((*tau_pt)[0]))+trig_tau_sf->GetBinErrorUp(trig_tau_sf->FindBin((*tau_pt)[0]));
	 entry_85=trig_tau_sf->GetBinContent(trig_tau_sf->FindBin((*tau_pt)[0]))-trig_tau_sf->GetBinErrorLow(trig_tau_sf->FindBin((*tau_pt)[0]));


	 if ((*tau_id_full)[0]>0.5 && (*tau_id_full)[1] >0.5 && abs((*tau_eta)[0]) < 2.3 && abs((*tau_eta)[1]) < 2.3 && angular_dist> 0.5 && (*tau_decay)[0]!=5 && (*tau_decay)[1]!=5 && (*tau_decay)[0]!=6 && (*tau_decay)[1]!=6){

	 n_id= n_id + 1*weight_sample*tau_id_sf;

	 TFile tauenergy("TauES_dm_DeepTau2017v2p1VSjet_2018ReReco.root");

	 TH1F *histtauenergy = (TH1F*) tauenergy.Get("tes");

	 k = histtauenergy -> GetBinContent(histtauenergy -> FindBin((*tau_decay)[0]));

	 

	 if (k*(*tau_pt)[0] >100 && k*(*tau_pt)[1] >100.0 ){

	 n_pt = n_pt + 1*weight_sample*tau_id_sf;


	 if (float((*tau_charge)[0]) *float((*tau_charge)[1]) <0.0){

	 n_charge = n_charge + 1*weight_sample*tau_id_sf;

	 



	 if (abs((*tau_eta)[0]) < 2.4 && abs((*tau_eta)[1]) < 2.4) {

	 n_eta = n_eta + 1*weight_sample*tau_id_sf;

	 /* histo_e_pt.Fill((*e_pt)[0]);

	 histo_tau_pt.Fill((*tau_pt)[0]);

	 histo_n_tau.Fill(tau_pt->size());

	 histo_n_eletron.Fill(e_pt->size());*/


	 



	 

	 if (acop <= 1.0){

	   // histo_acop.Fill(acop);

	 }

	 else {

	 acop = 2 - acop;

	 delta_phi = 2*PI - delta_phi;

	 // histo_acop.Fill(acop);

	 }


	//cout << "tamanho bsm_weights: " << bsm_weights->size() << endl;

        //double sm_cs = (*bsm_weights)[51];

        //for(int p=0; p<102; p++){

          //      (*bsm_weights)[p]=(*bsm_weights)[p]/sm_cs;

        //}

	//entry_86=(*bsm_weights);



	 tau0.SetPtEtaPhiE(k*(*tau_pt)[0],(*tau_eta)[0],(*tau_phi)[0],k*(*tau_energy)[0]);

	 tau1.SetPtEtaPhiE(k*(*tau_pt)[1],(*tau_eta)[1],(*tau_phi)[1],k*(*tau_energy)[1]);

	 nu.SetPtEtaPhiE((*met_pt)[0],0,(*met_phi)[0],(*met_pt)[0]);

	 Pt = (tau0 + tau1).Pt();

	 cout<<Pt<<endl;

	 Mt = (tau0 + tau1).M();

	 //m_e_met=(ele+nu).Mt();

	 //histo_Pt.Fill(Pt);

	 //histo_Mt.Fill(Mt);

	 


	 if (xi -> size()!=2) continue;


	 n_rec=n_rec+1*weight_sample*tau_id_sf;


	 entry_1 = k*(*tau_pt)[0];

	 entry_2 = k*(*tau_pt)[1]; 

	 entry_3 = (*tau_charge)[0];

	 entry_4 = (*tau_charge)[1];

	 entry_5 = (*tau_eta)[0];

	 entry_6 = (*tau_eta)[1];

	 entry_7 = (*tau_phi)[0];

	 entry_8 = (*tau_phi)[1];

	 entry_9 = (*tau_id_full)[0];

	 entry_10 = (*tau_id_full)[1];

	 entry_11 = acop;

	 entry_12 = Pt;

	 entry_13 = Mt;

	 entry_66 = (*met_phi)[0];

	 entry_67 = (*met_pt)[0];

	 entry_68 = m_e_met;

	 entry_69 = (tau0+tau1).Rapidity();


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


	 entry_50 = k*(*tau_energy)[0];

	 entry_51 = (*tau_id)[0];

	 entry_52 = (*tau_id_antimu)[0];

	 entry_53 = (*tau_id_antie)[0];

	 entry_54 = (*tau_id_antij)[0];

	 entry_55 = (*tau_decay)[0];

	 entry_61 = k*(*tau_energy)[1];

	 entry_63 = (*tau_id)[1];

	 entry_70 = (*tau_id_antimu)[1];

	 entry_71 = (*tau_id_antie)[1];

	 entry_72 = (*tau_id_antij)[1];

	 entry_73 = (*tau_decay)[1];

	 entry_56 = (*jet_pt)[0];

	 entry_57 = (*jet_eta)[0];

	 entry_58 = (*jet_phi)[0];

	 entry_59 = (*jet_mass)[0];


//UPPER XI


		    double xangle=0;

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


		    TF1 limit_fun_arm_0("limit_fun_arm_0","-(8.44219E-07*[xangle]-0.000100957) + (([xi]<(0.000247185*[xangle]+0.101599))*-(1.40289E-05*[xangle]-0.00727237)+([xi]>=(0.000247185*[xangle]+0.101599))*-(0.000107811*[xangle]-0.0261867))*([xi]-(0.000247185*[xangle]+0.101599))",0,1);

		     

		  TF1 limit_fun_arm_1("limit_fun_arm_1","-(-4.74758E-07*[xangle]+3.0881E-05)+ (([xi]<(0.000727859*[xangle]+0.0722653))*-(2.43968E-05*[xangle]-0.0085461)+ ([xi]>=(0.000727859*[xangle]+0.0722653))*-(7.19216E-05*[xangle]-0.0148267))*([xi]-(0.000727859*[xangle]+0.0722653)) ",0,1);



		    limit_fun_arm_0.SetParameter("xangle", xangle);

		    limit_fun_arm_1.SetParameter("xangle", xangle);

		    limit_fun_arm_0.SetParameter("xi", (*xi)[0]);

		    limit_fun_arm_1.SetParameter("xi", (*xi)[1]);


		    cout << "upper limit arm 0: " << -limit_fun_arm_0.Eval(0.5) << " upper limit arm 1: " << -limit_fun_arm_1.Eval(0.5) << endl;


		    if((*thx)[0]> -limit_fun_arm_0.Eval(0.5) || (*thx)[1]> -limit_fun_arm_1.Eval(0.5)) continue;


	      

	    

	   

		    n_p_up_xi=n_p_up_xi+1*weight_sample*tau_id_sf;


		   

		  





	 

	 // Down xi cuts


	 if(fraction <= 0.21){

	   if((*trackx1)[0] <= 2.71 || (*trackx1)[0] >= 17.927 || (*tracky1)[0] <= -11.598 || (*tracky1)[0] >= 3.698 || (*trackx2)[0] <= 2.278 || (*trackx2)[0] >= 24.62 || (*tracky2)[0] <= -10.898 || (*tracky2)[0] >= 4.398) continue;

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

   n_p_dw_xi=n_p_dw_xi+ 1*weight_sample*tau_id_sf;



	 


	 //Radiation Damage

	  //Adicionar seletor de periodo
	 

	//Ficheiros declarados fora do for.
	 //TFile file_multi("/eos/project-c/ctpps/subsystems/Pixel/RPixTracking/pixelEfficiencies_multiRP_reMiniAOD.root");
	 //TFile file_rad("/eos/project-c/ctpps/subsystems/Pixel/RPixTracking/pixelEfficiencies_radiation_reMiniAOD.root");

	 //EraA - 2018A
	 
	    if(fraction<=0.21){

	      //braço56

	      TH2F *raddamage56_multi = (TH2F *)file_multi.Get("Pixel/2018/2018A/h56_220_2018A_all_2D");
	      TH2F *raddamage56_rad = (TH2F *)file_rad.Get("Pixel/2018/2018A/h56_210_2018A_all_2D");

	      weight=weight*raddamage56_rad->GetBinContent(raddamage56_rad->FindBin((*trackx1)[1],(*tracky1)[1]))*raddamage56_multi -> GetBinContent(raddamage56_multi->FindBin((*trackx1)[1],(*tracky1)[1])) ;

	      TH2F *raddamage45_multi = (TH2F *)file_multi.Get("Pixel/2018/2018A/h45_220_2018A_all_2D");
	      TH2F *raddamage45_rad = (TH2F *)file_rad.Get("Pixel/2018/2018A/h45_210_2018A_all_2D");

	      weight=weight*raddamage45_rad->GetBinContent(raddamage45_rad->FindBin((*trackx1)[0],(*tracky1)[0]))*raddamage45_multi -> GetBinContent(raddamage45_multi->FindBin((*trackx1)[0],(*tracky1)[0])) ;

	     

	    }


	    if(fraction>0.21 && fraction<=0.29){

	     TH2F *raddamage56_multi = (TH2F *)file_multi.Get("Pixel/2018/2018B1/h56_220_2018B1_all_2D");
	      TH2F *raddamage56_rad = (TH2F *)file_rad.Get("Pixel/2018/2018B1/h56_210_2018B1_all_2D");

	      weight=weight*raddamage56_rad->GetBinContent(raddamage56_rad->FindBin((*trackx1)[1],(*tracky1)[1]))*raddamage56_multi -> GetBinContent(raddamage56_multi->FindBin((*trackx1)[1],(*tracky1)[1])) ;

	      TH2F *raddamage45_multi = (TH2F *)file_multi.Get("Pixel/2018/2018B1/h45_220_2018B1_all_2D");
	      TH2F *raddamage45_rad = (TH2F *)file_rad.Get("Pixel/2018/2018B1/h45_210_2018B1_all_2D");

	      weight=weight*raddamage45_rad->GetBinContent(raddamage45_rad->FindBin((*trackx1)[0],(*tracky1)[0]))*raddamage45_multi -> GetBinContent(raddamage45_multi->FindBin((*trackx1)[0],(*tracky1)[0])) ;
	    }


	  if(fraction > 0.29 && fraction <=0.37){

              TH2F *raddamage56_multi = (TH2F *)file_multi.Get("Pixel/2018/2018B2/h56_220_2018B2_all_2D");
	      TH2F *raddamage56_rad = (TH2F *)file_rad.Get("Pixel/2018/2018B2/h56_210_2018B2_all_2D");

	      weight=weight*raddamage56_rad->GetBinContent(raddamage56_rad->FindBin((*trackx1)[1],(*tracky1)[1]))*raddamage56_multi -> GetBinContent(raddamage56_multi->FindBin((*trackx1)[1],(*tracky1)[1])) ;

	      TH2F *raddamage45_multi = (TH2F *)file_multi.Get("Pixel/2018/2018B2/h45_220_2018B2_all_2D");
	      TH2F *raddamage45_rad = (TH2F *)file_rad.Get("Pixel/2018/2018B2/h45_210_2018B2_all_2D");

	      weight=weight*raddamage45_rad->GetBinContent(raddamage45_rad->FindBin((*trackx1)[0],(*tracky1)[0]))*raddamage45_multi -> GetBinContent(raddamage45_multi->FindBin((*trackx1)[0],(*tracky1)[0])) ;

                    }


	    if(fraction>0.37 && fraction <=0.50){

	      TH2F *raddamage56_multi = (TH2F *)file_multi.Get("Pixel/2018/2018C/h56_220_2018C_all_2D");
	      TH2F *raddamage56_rad = (TH2F *)file_rad.Get("Pixel/2018/2018C/h56_210_2018C_all_2D");

	      weight=weight*raddamage56_rad->GetBinContent(raddamage56_rad->FindBin((*trackx1)[1],(*tracky1)[1]))*raddamage56_multi -> GetBinContent(raddamage56_multi->FindBin((*trackx1)[1],(*tracky1)[1])) ;

	      TH2F *raddamage45_multi = (TH2F *)file_multi.Get("Pixel/2018/2018C/h45_220_2018C_all_2D");
	      TH2F *raddamage45_rad = (TH2F *)file_rad.Get("Pixel/2018/2018C/h45_210_2018C_all_2D");

	      weight=weight*raddamage45_rad->GetBinContent(raddamage45_rad->FindBin((*trackx1)[0],(*tracky1)[0]))*raddamage45_multi -> GetBinContent(raddamage45_multi->FindBin((*trackx1)[0],(*tracky1)[0])) ;


	    }


	    if(fraction>0.50 && fraction <=0.77){

	      TH2F *raddamage56_multi = (TH2F *)file_multi.Get("Pixel/2018/2018D1/h56_220_2018D1_all_2D");
	      TH2F *raddamage56_rad = (TH2F *)file_rad.Get("Pixel/2018/2018D1/h56_210_2018D1_all_2D");

	      weight=weight*raddamage56_rad->GetBinContent(raddamage56_rad->FindBin((*trackx1)[1],(*tracky1)[1]))*raddamage56_multi -> GetBinContent(raddamage56_multi->FindBin((*trackx1)[1],(*tracky1)[1])) ;

	      TH2F *raddamage45_multi = (TH2F *)file_multi.Get("Pixel/2018/2018D1/h45_220_2018D1_all_2D");
	      TH2F *raddamage45_rad = (TH2F *)file_rad.Get("Pixel/2018/2018D1/h45_210_2018D1_all_2D");

	      weight=weight*raddamage45_rad->GetBinContent(raddamage45_rad->FindBin((*trackx1)[0],(*tracky1)[0]))*raddamage45_multi -> GetBinContent(raddamage45_multi->FindBin((*trackx1)[0],(*tracky1)[0])) ;

	    }



	    if(fraction>0.77 && fraction <=1.00){

	      TH2F *raddamage56_multi = (TH2F *)file_multi.Get("Pixel/2018/2018D2/h56_220_2018D2_all_2D");
	      TH2F *raddamage56_rad = (TH2F *)file_rad.Get("Pixel/2018/2018D2/h56_210_2018D2_all_2D");

	      weight=weight*raddamage56_rad->GetBinContent(raddamage56_rad->FindBin((*trackx1)[1],(*tracky1)[1]))*raddamage56_multi -> GetBinContent(raddamage56_multi->FindBin((*trackx1)[1],(*tracky1)[1])) ;

	      TH2F *raddamage45_multi = (TH2F *)file_multi.Get("Pixel/2018/2018D2/h45_220_2018D2_all_2D");
	      TH2F *raddamage45_rad = (TH2F *)file_rad.Get("Pixel/2018/2018D2/h45_210_2018D2_all_2D");

	      weight=weight*raddamage45_rad->GetBinContent(raddamage45_rad->FindBin((*trackx1)[0],(*tracky1)[0]))*raddamage45_multi -> GetBinContent(raddamage45_multi->FindBin((*trackx1)[0],(*tracky1)[0])) ;

	    }



	  
	  cout << "weight " <<  weight << endl;

	    n_final=n_final+weight*weight_sample*tau_id_sf;


	 for (int j = 0; j<jet_btag -> size(); j++){

		  if ((*jet_btag)[j]>0.5){

		  jet_b_n = jet_b_n +1;

		  }

		  }


		  entry_60 = (*jet_btag)[0];

		  //  entry_61 = (*e_energy)[0];


		  
		  for (int j = 0; j<tau_id_full -> size(); j++){

		    if ((*tau_id_full)[j]>0.5){

		      tau_n = tau_n + 1;

		    }

		  }

		  entry_62 = tau_n;

		  tau_n=0;

		  /*
		  for (int j = 0; j<e_id -> size(); j++){

		    if ((*e_id)[j]>0.5){

		      elet_n = elet_n + 1;

		    }

		  }


		  entry_63 = elet_n;*/

		  entry_64 = jet_b_n;


		  // elet_n=0;

		  jet_b_n=0;


		  entry_65=weight_sample*weight*tau_id_sf;

		 entry_79=entry_14+xi_sist_inter_1.Eval(entry_14);
		 entry_80=entry_14-xi_sist_inter_1.Eval(entry_14);
		 entry_81=entry_15+xi_sist_inter_2.Eval(entry_15);
                 entry_82=entry_15-xi_sist_inter_2.Eval(entry_15);

		 weight=1.;


		  out.Fill();

				    




	 }	 	

	 }	

	 }

	 }

				    


}
/*
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

*/
 cout<< "Os números finais vêm já multiplicados pelo weight sample!!!"<<endl;
 
cout << "O número total de acontecimentos é "<<ntp1->GetEntries() <<endl;

cout << "O número de partículas com 2th é "<<n_id <<endl;

cout << "O número de partículas com 2tau pt "<<n_pt<<endl;

cout << "O número de partículas com o produto das cargas negativo é "<<n_charge <<endl;

cout << "O número de partículas com 2 prot reconstruidos é "<<n_rec <<endl;
 
 cout<< "O número de partículas com 2 protões reconstruídos que passam o up_xi cut   "<< n_p_up_xi << endl;

cout << "O número de partículas com 2 protões reconstruidos que passam o dw_xi cut   " << n_p_dw_xi << endl;

cout << "O número de partículas com 2 protões reconstruidos que passam o radiation damage cut   " << n_final << endl;



output.Write();

app.Run(true);

return 0;

}




