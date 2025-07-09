#include <iostream>
#include "TFile.h"
#include "TLeaf.h"

#include "TTree.h"
#include "TRandom3.h"

using namespace std;

int main() {
  TFile protons("/eos/cms/store/group/phys_smp/Exclusive_DiTau/proton_pool_2018/proton_pool_2018.root");
  TFile mc("/eos/user/j/jjhollar/TauTauBackup/samples_2018_tautau/fase1/soma/Dados_fase1_tautau_total_2018.root");

  double weight_corrector = 1.0 * 0.13;

  TFile output("/eos/user/m/mblancco/samples_2018_tautau/fase1/soma/Dados_fase1-protons_2018_tautau.root", "RECREATE", "");
  TTree tree_out("tree", "");

  double p11 = 0.08, p12 = 0.02, p21 = 0.02, p22 = 0.005;
  double total = p11 + p12 + p21 + p22;
  double frac11 = p11 / total, frac12 = p12 / total, frac21 = p21 / total;

  TTree* tree_pu = (TTree*)protons.Get("tree");
  TTree* tree_mc = (TTree*)mc.Get("tree");

  int n_protons = tree_pu->GetEntries();
  int n_mc = tree_mc->GetEntries();
  if (n_protons < 3 * n_mc) {
    cout << "Not enough protons. Exiting.\n";
    return 1;
  }

  // Declare input variables
  double tau0_id1, tau0_id2, tau0_id3;
  double tau1_id1, tau1_id2, tau1_id3;
  double tau0_pt, tau1_pt, tau0_charge, tau1_charge;
  double tau0_eta, tau1_eta, tau0_phi, tau1_phi;
  double tau0_mass, tau1_mass, sist_mass, sist_acop, sist_pt, sist_rap;
  double met_pt, met_phi, jet_pt, jet_eta, jet_phi, jet_mass, jet_btag;
  double weight, generator_weight;
  double tau_n;
  int n_b_jet;

  // Set branch addresses
  tree_mc->SetBranchAddress("tau0_id1", &tau0_id1);
  tree_mc->SetBranchAddress("tau0_id2", &tau0_id2);
  tree_mc->SetBranchAddress("tau0_id3", &tau0_id3);
  tree_mc->SetBranchAddress("tau1_id1", &tau1_id1);
  tree_mc->SetBranchAddress("tau1_id2", &tau1_id2);
  tree_mc->SetBranchAddress("tau1_id3", &tau1_id3);
  tree_mc->SetBranchAddress("tau0_pt", &tau0_pt);
  tree_mc->SetBranchAddress("tau1_pt", &tau1_pt);
  tree_mc->SetBranchAddress("tau0_charge", &tau0_charge);
  tree_mc->SetBranchAddress("tau1_charge", &tau1_charge);
  tree_mc->SetBranchAddress("tau0_eta", &tau0_eta);
  tree_mc->SetBranchAddress("tau1_eta", &tau1_eta);
  tree_mc->SetBranchAddress("tau_n", &tau_n);
  tree_mc->SetBranchAddress("tau0_phi", &tau0_phi);
  tree_mc->SetBranchAddress("tau1_phi", &tau1_phi);
  tree_mc->SetBranchAddress("tau0_mass", &tau0_mass);
  tree_mc->SetBranchAddress("tau1_mass", &tau1_mass);
  tree_mc->SetBranchAddress("sist_mass", &sist_mass);
  tree_mc->SetBranchAddress("sist_acop", &sist_acop);
  tree_mc->SetBranchAddress("sist_pt", &sist_pt);
  tree_mc->SetBranchAddress("sist_rap", &sist_rap);
  tree_mc->SetBranchAddress("met_pt", &met_pt);
  tree_mc->SetBranchAddress("met_phi", &met_phi);
  tree_mc->SetBranchAddress("jet_pt", &jet_pt);
  tree_mc->SetBranchAddress("jet_eta", &jet_eta);
  tree_mc->SetBranchAddress("jet_phi", &jet_phi);
  tree_mc->SetBranchAddress("jet_mass", &jet_mass);
  tree_mc->SetBranchAddress("jet_btag", &jet_btag);
  tree_mc->SetBranchAddress("weight", &weight);
  tree_mc->SetBranchAddress("generator_weight", &generator_weight);
  tree_mc->SetBranchAddress("n_b_jet", &n_b_jet);

  // Declare proton variables
  double xi_arm1_1, xi_arm1_2, xi_arm2_1, xi_arm2_2;

  // Output branches
  tree_out.Branch("tau0_id1", &tau0_id1, "tau0_id1/I");
  tree_out.Branch("tau0_id2", &tau0_id2, "tau0_id2/I");
  tree_out.Branch("tau0_id3", &tau0_id3, "tau0_id3/I");
  tree_out.Branch("tau1_id1", &tau1_id1, "tau1_id1/I");
  tree_out.Branch("tau1_id2", &tau1_id2, "tau1_id2/I");
  tree_out.Branch("tau1_id3", &tau1_id3, "tau1_id3/I");
  tree_out.Branch("tau0_pt", &tau0_pt, "tau0_pt/D");
  tree_out.Branch("tau1_pt", &tau1_pt, "tau1_pt/D");
  tree_out.Branch("tau0_charge", &tau0_charge, "tau0_charge/D");
  tree_out.Branch("tau1_charge", &tau1_charge, "tau1_charge/D");
  tree_out.Branch("tau0_eta", &tau0_eta, "tau0_eta/D");
  tree_out.Branch("tau1_eta", &tau1_eta, "tau1_eta/D");
  tree_out.Branch("tau_n", &tau_n, "tau_n/I");
  tree_out.Branch("tau0_phi", &tau0_phi, "tau0_phi/D");
  tree_out.Branch("tau1_phi", &tau1_phi, "tau1_phi/D");
  tree_out.Branch("tau0_mass", &tau0_mass, "tau0_mass/D");
  tree_out.Branch("tau1_mass", &tau1_mass, "tau1_mass/D");
  tree_out.Branch("sist_mass", &sist_mass, "sist_mass/D");
  tree_out.Branch("sist_acop", &sist_acop, "sist_acop/D");
  tree_out.Branch("sist_pt", &sist_pt, "sist_pt/D");
  tree_out.Branch("sist_rap", &sist_rap, "sist_rap/D");
  tree_out.Branch("met_pt", &met_pt, "met_pt/D");
  tree_out.Branch("met_phi", &met_phi, "met_phi/D");
  tree_out.Branch("jet_pt", &jet_pt, "jet_pt/D");
  tree_out.Branch("jet_eta", &jet_eta, "jet_eta/D");
  tree_out.Branch("jet_phi", &jet_phi, "jet_phi/D");
  tree_out.Branch("jet_mass", &jet_mass, "jet_mass/D");
  tree_out.Branch("jet_btag", &jet_btag, "jet_btag/D");
  tree_out.Branch("weight", &weight_corrector, "weight/D");
  tree_out.Branch("generator_weight", &generator_weight, "generator_weight/D");
  tree_out.Branch("n_b_jet", &n_b_jet, "n_b_jet/I");
  tree_out.Branch("xi_arm1_1", &xi_arm1_1, "xi_arm1_1/D");
  tree_out.Branch("xi_arm1_2", &xi_arm1_2, "xi_arm1_2/D");
  tree_out.Branch("xi_arm2_1", &xi_arm2_1, "xi_arm2_1/D");
  tree_out.Branch("xi_arm2_2", &xi_arm2_2, "xi_arm2_2/D");

  TRandom3 r;
  int n_rndm = 0;

  for (int i = 0; i < n_mc; i++) {
    tree_mc->GetEntry(i);

    double mc_frac = double(i) / n_mc;
    xi_arm1_1 = xi_arm1_2 = xi_arm2_1 = xi_arm2_2 = -1;

    while (true) {
      tree_pu->GetEntry(n_rndm++);
      int arm = tree_pu->GetLeaf("proton_arm")->GetValue();
      double xi = tree_pu->GetLeaf("proton_xi")->GetValue();
      if (arm == 0 && xi_arm1_1 < 0) xi_arm1_1 = xi;
      if (arm == 1 && xi_arm2_1 < 0) xi_arm2_1 = xi;
      if (xi_arm1_1 >= 0 && xi_arm2_1 >= 0) break;
    }

    if (mc_frac > frac11 && mc_frac < frac11 + frac12 || mc_frac > frac11 + frac12 + frac21) {
      while (true) {
        tree_pu->GetEntry(n_rndm++);
        if (tree_pu->GetLeaf("proton_arm")->GetValue() == 1) {
          xi_arm2_2 = tree_pu->GetLeaf("proton_xi")->GetValue();
          break;
        }
      }
    }

    if (mc_frac > frac11 + frac12 && mc_frac < frac11 + frac12 + frac21 || mc_frac > frac11 + frac12 + frac21) {
      while (true) {
        tree_pu->GetEntry(n_rndm++);
        if (tree_pu->GetLeaf("proton_arm")->GetValue() == 0) {
          xi_arm1_2 = tree_pu->GetLeaf("proton_xi")->GetValue();
          break;
        }
      }
    }

    tree_out.Fill();
  }

  cout << "Enrichment completed.\n";
  output.Write();
  output.Close();
  return 0;
}
