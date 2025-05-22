#include <iostream>
#include <cmath>
#include "TFile.h"
#include "TBranch.h"
#include "TRandom3.h"
#include "TTree.h"
#include "TLeaf.h"

using namespace std;

int main(){

  TFile protons("/eos/cms/store/group/phys_smp/Exclusive_DiTau/proton_pool_2018/proton_pool_2018.root"); //File with PU protons
  TFile mc("/eos/user/m/mpisano/samples_2018_tautau/fase1/soma/Dados_fase1_tautau_total_2018.root"); //File to enrich

  double weight_corrector =1.*0.13 ; //weight of the sample * weight of PU1+

  TFile output("/eos/user/m/mpisano/samples_2018_tautau/fase1/soma/Dados_fase1-protons_2018_tautau.root","RECREATE","");
  TTree tree_out("tree","");

  double p11=0.08;  //probability to get 1 PU proton per arm;
  double p12=0.02;  //probability to get 1&2 PU protons in arms 1&2
  double p21=0.02;  //probability to get 2&1 PU protons in arms 1&2
  double p22=0.005;  //probability to get 2 PU proton per arm;

  //Single arm probability 2018: p0=58/(58+26+6+1)=58/91=0.64, p1=26/91=0.29, p2=6/91=0.07, so p(1+)=0.36.
  //So: p00=0.41, p(1+)0=p0(1+)=0.23 -> p(1+)(1+)=1-0.41-2*0.23 = 0.13.  p11=0.08, p12=p21=0.02, p22=0.005

  //Events will be divided in 4 sets according to the probability to get n&m PU protons.

  double frac11 = p11/(p11+p12+p21+p22);
  double frac12 = p12/(p11+p12+p21+p22);
  double frac21 = p21/(p11+p12+p21+p22);
  double frac22 = p22/(p11+p12+p21+p22);


  TTree* tree_pu = (TTree*) protons.Get("tree");
  TTree* tree_mc = (TTree*) mc.Get("tree");

  int n_protons = tree_pu->GetEntries();
  int n_mc = tree_mc->GetEntries();

  cout << "# of PU Protons contained in the file: " << n_protons << endl;
  cout << "# of events contained in the mc file: " << n_mc << endl;

  if(n_protons < 3*n_mc) {
    cout << "Sorry, I do not have enough protons :( I quit." << endl;
    return 1;
  }

  //declaration of auxiliary variables

  double entry1, entry2, entry3, entry4, entry5, entry6, entry7, entry8, entry9, entry10, entry11;
  double entry12, entry13, entry14, entry15, entry16, entry17, entry18, entry19, entry20, entry21, entry22;
  double  entry23, entry24, entry25, entry26, entry27, entry28, entry29, entry30, entry31, entry32, entry33, entry34, entry35;

  TRandom3 r;

  tree_out.Branch("tau0_id1", &entry1, "tau0_id1/I");
  tree_out.Branch("tau0_id2", &entry11, "tau0_id2/I");
  tree_out.Branch("tau0_id3", &entry34, "tau0_id3/I");
  tree_out.Branch("tau1_id1", &entry2, "tau1_id1/I");
  tree_out.Branch("tau1_id2", &entry3, "tau1_id2/I");
  tree_out.Branch("tau1_id3", &entry4, "tau1_id3/I");
  tree_out.Branch("tau0_pt", &entry5, "tau0_pt/D");
  tree_out.Branch("tau1_pt", &entry6, "tau1_pt/D");
  tree_out.Branch("tau0_charge", &entry7, "tau0_charge/D");
  tree_out.Branch("tau1_charge", &entry8, "tau1_charge/D");
  tree_out.Branch("tau0_eta", &entry9, "tau0_eta/D");
  tree_out.Branch("tau1_eta", &entry10, "tau1_eta/D");
  tree_out.Branch("tau_n", &entry12, "tau_n/I");
  tree_out.Branch("tau0_phi", &entry13, "tau0_phi/D");
  tree_out.Branch("tau1_phi", &entry14, "tau1_phi/D");
  tree_out.Branch("tau0_mass", &entry15, "tau0_mass/D");
  tree_out.Branch("tau1_mass", &entry16, "tau1_mass/D");
  tree_out.Branch("sist_mass", &entry17, "sist_mass/D");
  tree_out.Branch("sist_acop", &entry18, "sist_acop/D");
  tree_out.Branch("sist_pt", &entry19, "sist_pt/D");
  tree_out.Branch("sist_rap", &entry20, "sist_rap/D");
  tree_out.Branch("met_pt", &entry21, "met_pt/D");
  tree_out.Branch("met_phi", &entry22, "met_phi/D");
  tree_out.Branch("jet_pt", &entry23, "jet_pt/D");
  tree_out.Branch("jet_eta", &entry24, "jet_eta/D");
  tree_out.Branch("jet_phi", &entry25, "jet_phi/D");
  tree_out.Branch("jet_mass", &entry26, "jet_mass/D");
  tree_out.Branch("jet_btag", &entry27, "jet_btag/D");
  tree_out.Branch("weight", &entry28, "weight/D");
  tree_out.Branch("n_b_jet", &entry35, "N_b_jet/I");

  tree_out.Branch("generator_weight", &entry29, "generator_weight/D");
  tree_out.Branch("xi_arm1_1", &entry30, "xi_arm1_1/D");
  tree_out.Branch("xi_arm1_2", &entry31, "xi_arm1_2/D");
  tree_out.Branch("xi_arm2_1", &entry32, "xi_arm2_1/D");
  tree_out.Branch("xi_arm2_2", &entry33, "xi_arm2_2/D");

  int n_rndm=0;
  
  for (int i=0; i<n_mc; i++){

	if(i!=-1) cout << "I have enriched " << i << " events." << endl;

    int ev = tree_mc->GetEvent(i);
    double mc_frac = double (i)/ double (n_mc);

    //Reading && saving of all the variables contained in the MC file
    entry1 = tree_mc->GetLeaf("tau0_id1")->GetValue(0);
    entry11 = tree_mc->GetLeaf("tau0_id2")->GetValue(0);
    entry34 = tree_mc->GetLeaf("tau0_id3")->GetValue(0);
    entry2 = tree_mc->GetLeaf("tau_id1")->GetValue(0);
    entry3 = tree_mc->GetLeaf("tau_id2")->GetValue(0);
    entry4 = tree_mc->GetLeaf("tau_id3")->GetValue(0);
    entry5 = tree_mc->GetLeaf("tau0_pt")->GetValue(0);
    entry6 = tree_mc->GetLeaf("tau1_pt")->GetValue(0);
    entry7 = tree_mc->GetLeaf("tau0_charge")->GetValue(0);
    entry8 = tree_mc->GetLeaf("tau1_charge")->GetValue(0);
    entry9 = tree_mc->GetLeaf("tau0_eta")->GetValue(0);
    entry10 = tree_mc->GetLeaf("tau1_eta")->GetValue(0);
    entry12 = tree_mc->GetLeaf("tau_n")->GetValue(0);
    entry13 = tree_mc->GetLeaf("tau0_phi")->GetValue(0);
    entry14 = tree_mc->GetLeaf("tau1_phi")->GetValue(0);
    entry15 = tree_mc->GetLeaf("tau0_mass")->GetValue(0);
    entry16 = tree_mc->GetLeaf("tau1_mass")->GetValue(0);
    entry17 = tree_mc->GetLeaf("sist_mass")->GetValue(0);
    entry18 = tree_mc->GetLeaf("sist_acop")->GetValue(0);
    entry19 = tree_mc->GetLeaf("sist_pt")->GetValue(0);
    entry20 = tree_mc->GetLeaf("sist_rap")->GetValue(0);
    entry35 = tree_mc->GetLeaf("n_b_jet")->GetValue(0);

    entry21 = tree_mc->GetLeaf("met_pt")->GetValue(0);
    entry22 = tree_mc->GetLeaf("met_phi")->GetValue(0);
    entry23 = tree_mc->GetLeaf("jet_pt")->GetValue(0);
    entry24 = tree_mc->GetLeaf("jet_eta")->GetValue(0);
    entry25 = tree_mc->GetLeaf("jet_phi")->GetValue(0);
    entry26 = tree_mc->GetLeaf("jet_mass")->GetValue(0);
    entry27 = tree_mc->GetLeaf("jet_btag")->GetValue(0);
    entry28 = tree_mc->GetLeaf("weight")->GetValue(0)*0+weight_corrector;
    entry29 = tree_mc->GetLeaf("generator_weight")->GetValue(0);
    double xi_arm1_1 = -1;
    double xi_arm1_2 = -1;
    double xi_arm2_1 = -1;
    double xi_arm2_2 = -1;

    //Reading and saving of proton variables


      while(1>0){
      	tree_pu->GetEvent(n_rndm);
      	if(tree_pu->GetLeaf("proton_arm")->GetValue(0)==0) {xi_arm1_1 = tree_pu->GetLeaf("proton_xi")->GetValue(0);}
        if(tree_pu->GetLeaf("proton_arm")->GetValue(0)==1) {xi_arm2_1 = tree_pu->GetLeaf("proton_xi")->GetValue(0);}
        n_rndm++;
        if(xi_arm1_1!=-1 && xi_arm2_1!=-1) break;
      }

      if((mc_frac>frac11 && mc_frac < frac11+frac12)||(mc_frac > frac11+frac12+frac21)){ 

      while(1>0){
        n_rndm++;
	tree_pu->GetEvent(n_rndm);
        if(tree_pu->GetLeaf("proton_arm")->GetValue(0)==1) {xi_arm2_2 = tree_pu->GetLeaf("proton_xi")->GetValue(0); break;}
        }
      }

      if((mc_frac>frac11+frac12 && mc_frac < frac11+frac12+frac21) || (mc_frac > frac11+frac12+frac21)){

	while(1>0){
	n_rndm++;
        tree_pu->GetEvent(n_rndm);
        if(tree_pu->GetLeaf("proton_arm")->GetValue(0)==0) {xi_arm1_2 = tree_pu->GetLeaf("proton_xi")->GetValue(0); break;}
        }


       }

      // entry30 = histest xi in arm 1, entry31 = lowest xi in arm 1. entries32/33 = highest/lowest xi in arm 2.

      if(xi_arm1_1>=xi_arm1_2){
	entry30 = xi_arm1_1;
	entry31 = xi_arm1_2;
      } else {
	entry30 = xi_arm1_2;
	entry31 = xi_arm1_1;
      }

      if(xi_arm2_1>=xi_arm2_2){
	entry32 = xi_arm2_1;
	entry33 = xi_arm2_2;
      } else {
	entry32 = xi_arm2_2;
	entry33 = xi_arm2_1;
      }

      tree_out.Fill();

  }

  cout << "Enrichment processing ------ ended" << endl;

  output.Write();
  output.Close();

  return 0;
}
