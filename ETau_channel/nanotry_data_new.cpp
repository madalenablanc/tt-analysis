// etau_skim_vec.cpp
// g++ -O2 -std=c++17 etau_skim_vec.cpp `root-config --cflags --libs` -o etau_skim_vec

#include <iostream>
#include <cmath>
#include <string>
#include <memory>

#include "TApplication.h"
#include "TFile.h"
#include "TTree.h"
#include "TTreeReader.h"
#include "TTreeReaderArray.h"
#include "TTreeReaderValue.h"
#include "TH1D.h"
#include "TLorentzVector.h"
#include "TMath.h"
#include "TVector2.h"

using std::cerr;
using std::cout;
using std::endl;
using std::string;

// Safe "leading element" getter for reader arrays
template <typename T>
inline T lead0(const TTreeReaderArray<T>& arr, T def = T{}) {
  return (arr.GetSize() > 0 ? arr[0] : def);
}

int main() {
  // --- I/O paths (your originals) ---
  const string out_put = "merged_new";
  const string output_tot =
      "/eos/user/m/mblancco/samples_2018_etau/fase1_etau/dados_fase1_skimmed_ETau_" + out_put + ".root";
  const string total =
      "/eos/cms/store/user/jjhollar/TauTau_NanoAOD_Madalena/BackgroundSamples/ETau/Dados_2018_UL_skimmed_ETau_nano_fase0_merge.root";

  cout << "Input : " << total << "\nOutput: " << output_tot << endl;

  TApplication app("app", nullptr, nullptr);

  // Open input
  TFile* f = TFile::Open(total.c_str(), "READ");
  if (!f || f->IsZombie()) {
    cerr << "ERROR: cannot open input file\n";
    return 1;
  }

  TTree* tree = static_cast<TTree*>(f->Get("analyzer/ntp1"));
  if (!tree) {
    cerr << "ERROR: cannot get analyzer/ntp1\n";
    return 2;
  }
  cout << "Opened tree with " << tree->GetEntries() << " entries\n";

  // --- Readers ---
  TTreeReader reader(tree);

  // electrons (vectors)
  TTreeReaderArray<float> e_pt(reader,     "e_pt");
  TTreeReaderArray<float> e_eta(reader,    "e_eta");
  TTreeReaderArray<float> e_phi(reader,    "e_phi");
  TTreeReaderArray<float> e_energy(reader, "e_energy");
  TTreeReaderArray<float> e_charge(reader, "e_charge");

  // taus (vectors)
  TTreeReaderArray<float> tau_pt(reader,     "tau_pt");
  TTreeReaderArray<float> tau_eta(reader,    "tau_eta");
  TTreeReaderArray<float> tau_phi(reader,    "tau_phi");
  TTreeReaderArray<float> tau_energy(reader, "tau_energy");
  TTreeReaderArray<int>   tau_charge(reader, "tau_charge");

  // jets (vectors)
  TTreeReaderArray<float> jet_pt(reader,   "jet_pt");
  TTreeReaderArray<float> jet_eta(reader,  "jet_eta");
  TTreeReaderArray<float> jet_phi(reader,  "jet_phi");
  TTreeReaderArray<float> jet_mass(reader, "jet_mass");
  TTreeReaderArray<int>   jet_btag(reader, "jet_btag");

  // MET (vectors)
  TTreeReaderArray<float> met_pt(reader,  "met_pt");
  TTreeReaderArray<float> met_phi(reader, "met_phi");

  // IDs (note: these are FLOAT in your file)
  TTreeReaderArray<float> e_id(reader,           "e_id");
  TTreeReaderArray<float> tau_id_full(reader,    "tau_id_full");
  TTreeReaderArray<float> tau_id_antimu(reader,  "tau_id_antimu");
  TTreeReaderArray<float> tau_id_antie(reader,   "tau_id_antie");
  TTreeReaderArray<float> tau_id_antij(reader,   "tau_id_antij");

  // Optional scalar weight
  std::unique_ptr<TTreeReaderValue<double>> weight_sm;
  if (tree->GetBranch("weight_sm")) {
    weight_sm = std::make_unique<TTreeReaderValue<double>>(reader, "weight_sm");
  }

  // --- Output file/tree (schema unchanged) ---
  TFile output(output_tot.c_str(), "RECREATE", "");
  TTree out("tree", "");

  double entry_1,entry_2,entry_3,entry_4,entry_5,entry_6,entry_7,entry_8,entry_9,entry_10,entry_11,entry_12,
         entry_13,entry_14,entry_15,entry_16,entry_17,entry_18,entry_19,entry_20,entry_21,entry_22,
         entry_23,entry_24,entry_25,entry_26,entry_27,entry_28, entry_30;
  int entry_29;

  out.Branch("eletron_id",      &entry_1 , "e_id/D");
  out.Branch("tau_id1",         &entry_2 , "tau_id1/D");
  out.Branch("tau_id2",         &entry_3 , "tau_id2/D");
  out.Branch("tau_id3",         &entry_4 , "tau_id3/D");
  out.Branch("eletron_pt",      &entry_5 , "e_pt/D");
  out.Branch("tau_pt",          &entry_6 , "tau_pt/D");
  out.Branch("eletron_charge",  &entry_7 , "e_charge/D");
  out.Branch("tau_charge",      &entry_8 , "tau_charge/D");
  out.Branch("eletron_eta",     &entry_9 , "e_eta/D");
  out.Branch("tau_eta",         &entry_10, "tau_eta/D");
  out.Branch("eletron_n",       &entry_11, "e_n/D");
  out.Branch("tau_n",           &entry_12, "tau_n/D");
  out.Branch("electron_phi",    &entry_13, "electron_phi/D");
  out.Branch("tau_phi",         &entry_14, "tau_phi/D");
  out.Branch("electron_mass",   &entry_15, "electron_mass/D");
  out.Branch("tau_mass",        &entry_16, "tau_mass/D");
  out.Branch("sist_mass",       &entry_17, "sist_mass/D");
  out.Branch("sist_acop",       &entry_18, "sist_acop/D");
  out.Branch("sist_pt",         &entry_19, "sist_pt/D");
  out.Branch("sist_rap",        &entry_20, "sist_rap/D");
  out.Branch("met_pt",          &entry_21, "met_pt/D");
  out.Branch("met_phi",         &entry_22, "met_phi/D");
  out.Branch("jet_pt",          &entry_23, "jet_pt/D");
  out.Branch("jet_eta",         &entry_24, "jet_eta/D");
  out.Branch("jet_phi",         &entry_25, "jet_phi/D");
  out.Branch("jet_mass",        &entry_26, "jet_mass/D");
  out.Branch("jet_btag",        &entry_27, "jet_btag/D");
  out.Branch("weight",          &entry_28, "weight/D");
  out.Branch("n_b_jet",         &entry_29, "n_b_jet/I");
  out.Branch("generator_weight",&entry_30, "generator_weight/D");

  TH1D h_m("histo","histo", 1000, 0, 1000);

  Long64_t c_all=0, c_eid=0, c_tauid=0, c_dr_eta=0, c_pt=0, c_q=0, c_written=0;
  TLorentzVector p4e, p4t, p4sys;

  // --- Event loop ---
  while (reader.Next()) {
    ++c_all;

    // Leading candidates
    const double e_pt0   = lead0(e_pt,     0.f);
    const double e_eta0  = lead0(e_eta,    0.f);
    const double e_phi0  = lead0(e_phi,    0.f);
    const double e_en0   = lead0(e_energy, 0.f);
    const double e_q0    = lead0(e_charge, 0.f);

    const double t_pt0   = lead0(tau_pt,     0.f);
    const double t_eta0  = lead0(tau_eta,    0.f);
    const double t_phi0  = lead0(tau_phi,    0.f);
    const double t_en0   = lead0(tau_energy, 0.f);
    const double t_q0    = static_cast<double>( lead0(tau_charge, 0) );

    // IDs: float in file; use >0 checks
    const float e_id0_f          = lead0(e_id,          0.f);
    const float tau_id_full0_f   = lead0(tau_id_full,   0.f);
    const float tau_id_antimu0_f = lead0(tau_id_antimu, 0.f);
    const float tau_id_antie0_f  = lead0(tau_id_antie,  0.f);
    const float tau_id_antij0_f  = lead0(tau_id_antij,  0.f);

    // 4-vectors
    p4e.SetPtEtaPhiE(e_pt0, e_eta0, e_phi0, e_en0);
    p4t.SetPtEtaPhiE(t_pt0, t_eta0, t_phi0, t_en0);

    // Selections
    const bool pass_eid    = (e_id0_f == 1);
    const bool pass_tauid  = (tau_id_full0_f>0.f && tau_id_antimu0_f>0.f &&
                              tau_id_antie0_f>0.f && tau_id_antij0_f>0.f);
    const bool pass_dr_eta = (p4e.Pt()>0 && p4t.Pt()>0 &&
                              p4e.DeltaR(p4t) > 0.4 &&
                              std::fabs(e_eta0) < 2.4 &&
                              std::fabs(t_eta0) < 2.4);
    const bool pass_pt     = (e_pt0>35. && t_pt0>100.);
    const bool pass_q      = (t_q0 * e_q0 < 0.0);

    if (pass_eid)   ++c_eid;   else continue;
    if (pass_tauid) ++c_tauid; else continue;
    if (pass_dr_eta)++c_dr_eta;else continue;
    if (pass_pt)    ++c_pt;    else continue;
    if (pass_q)     ++c_q;     else continue;

    // --- Fill outputs ---
    entry_1 = static_cast<double>(e_id0_f);
    entry_2 = static_cast<double>(tau_id_full0_f);
    entry_3 = static_cast<double>(tau_id_antimu0_f);
    entry_4 = static_cast<double>(tau_id_antij0_f);

    entry_5  = e_pt0;
    entry_6  = t_pt0;
    entry_7  = e_q0;
    entry_8  = t_q0;
    entry_9  = e_eta0;
    entry_10 = t_eta0;

    entry_11 = static_cast<double>( e_pt.GetSize() );
    entry_12 = static_cast<double>( tau_pt.GetSize() );

    entry_13 = e_phi0;
    entry_14 = t_phi0;

    entry_15 = p4e.M();
    entry_16 = p4t.M();

    p4sys = p4e + p4t;
    const double dphi = std::fabs(TVector2::Phi_mpi_pi(entry_14 - entry_13));
    entry_18 = dphi / TMath::Pi();   // acoplanarity
    entry_17 = p4sys.M();
    entry_19 = p4sys.Pt();
    entry_20 = p4sys.Rapidity();

    entry_21 = lead0(met_pt,  0.f);
    entry_22 = lead0(met_phi, 0.f);

    entry_23 = lead0(jet_pt,   0.f);
    entry_24 = lead0(jet_eta,  0.f);
    entry_25 = lead0(jet_phi,  0.f);
    entry_26 = lead0(jet_mass, 0.f);
    entry_27 = static_cast<double>( lead0(jet_btag, 0) );

    double w_sm_val = 1.0;
    if (weight_sm) w_sm_val = **weight_sm;
    entry_28 = w_sm_val;

    entry_29 = 0; // not provided
    entry_30 = 0; // not provided

    h_m.Fill(entry_17);
    out.Fill();
    ++c_written;
  }

  cout << "\nCutflow (events):"
       << "\n  All                    : " << c_all
       << "\n  e_id>0                 : " << c_eid
       << "\n  tau anti-IDs (all > 0) : " << c_tauid
       << "\n  \xCE\x94R>0.4 & |\xCE\xB7|<2.4       : " << c_dr_eta
       << "\n  pT cuts (e>35, \xCF\x84>100)  : " << c_pt
       << "\n  Opposite charge        : " << c_q
       << "\n  Written                : " << c_written
       << endl;

  output.Write();
  output.Close();
  f->Close();
  return 0;
}
