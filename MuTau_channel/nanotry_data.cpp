// nanotry_data.cpp
#include "ROOT/RDataFrame.hxx"
#include "TLorentzVector.h"
#include <cmath>
#include <iostream>
#include <string>
#include <vector>

using std::string;
using std::vector;

// Helpers
template <typename T>
inline T first_or(const vector<T>& v, T def) { return v.empty() ? def : v[0]; }

inline double acoplanarity(float phi1, float phi2) {
  double dphi = std::fabs(phi2 - phi1);
  if (dphi > M_PI) dphi -= 2.0 * M_PI;
  return std::fabs(dphi) / M_PI;
}

struct Kine { double mass, pt, rap, acop; };

inline Kine syst_kine(float mu_pt, float mu_eta, float mu_phi, float mu_E,
                      float t_pt,  float t_eta,  float t_phi,  float t_E) {
  TLorentzVector mu, tau, sys;
  mu.SetPtEtaPhiE(mu_pt, mu_eta, mu_phi, mu_E);
  tau.SetPtEtaPhiE(t_pt,  t_eta,  t_phi,  t_E);
  sys = mu + tau;
  return { sys.M(), sys.Pt(), sys.Rapidity(), acoplanarity(mu_phi, t_phi) };
}

inline int count_btags(const vector<int>& btags) {
  int n = 0; for (int b : btags) if (b > 0) ++n; return n; // treat >0 as tagged
}

// Skimmer
void skim_mutau_rdf(const string& infile,
                    const string& outfile,
                    const string& intree_path = "analyzer/ntp1",
                    const string& outtreename = "tree")
{
  ROOT::EnableImplicitMT();
  ROOT::RDataFrame df(intree_path, infile);

  // Selection: ≥1 mu & ≥1 tau, then leading-object IDs
  auto df_sel = df
    .Filter([](const vector<float>& mu_pt, const vector<float>& tau_pt){
        return !mu_pt.empty() && !tau_pt.empty();
      }, {"mu_pt","tau_pt"})
    .Filter([](const vector<float>& mu_id){
        return !mu_id.empty() && mu_id[0] >= 3.f;
      }, {"mu_id"})
    .Filter([](const vector<float>& aJ, const vector<float>& aE, const vector<float>& aMu){
        return !aJ.empty() && !aE.empty() && !aMu.empty()
               && aJ[0] >= 63.f && aE[0] >= 7.f && aMu[0] >= 1.f;
      }, {"tau_id_antij","tau_id_antie","tau_id_antimu"});

  // Scalarize leading objects with NEW names (*_lead) to avoid collisions
  auto df_lead = df_sel
    .Define("muon_id",        [](const vector<float>& v){ return first_or(v, -99.f); }, {"mu_id"})
    .Define("tau_id1",        [](const vector<float>& v){ return first_or(v, -99.f); }, {"tau_id_antij"})
    .Define("tau_id2",        [](const vector<float>& v){ return first_or(v, -99.f); }, {"tau_id_antie"})
    .Define("tau_id3",        [](const vector<float>& v){ return first_or(v, -99.f); }, {"tau_id_antimu"})
    .Define("muon_pt",        [](const vector<float>& v){ return first_or(v, -99.f); }, {"mu_pt"})
    .Define("tau_pt_lead",    [](const vector<float>& v){ return first_or(v, -99.f); }, {"tau_pt"})
    .Define("muon_eta",       [](const vector<float>& v){ return first_or(v, -99.f); }, {"mu_eta"})
    .Define("tau_eta_lead",   [](const vector<float>& v){ return first_or(v, -99.f); }, {"tau_eta"})
    .Define("muon_phi",       [](const vector<float>& v){ return first_or(v, -99.f); }, {"mu_phi"})
    .Define("tau_phi_lead",   [](const vector<float>& v){ return first_or(v, -99.f); }, {"tau_phi"})
    .Define("muon_energy",    [](const vector<float>& v){ return first_or(v, -99.f); }, {"mu_energy"})
    .Define("tau_energy_lead",[](const vector<float>& v){ return first_or(v, -99.f); }, {"tau_energy"})
    .Define("muon_charge",    [](const vector<float>& v){ return first_or(v, -99.f); }, {"mu_charge"}) // float in file
    .Define("tau_charge_lead",[](const vector<int>&   v){ return static_cast<double>(first_or(v, -99)); }, {"tau_charge"})
    .Define("muon_n",         [](const vector<float>& v){ return static_cast<double>(v.size()); }, {"mu_pt"})
    .Define("tau_n",          [](const vector<float>& v){ return static_cast<double>(v.size()); }, {"tau_pt"})
    // MET (vector<float>, usually size 1) → scalars
    .Define("met_pt_lead",    [](const vector<float>& v){ return first_or(v, -99.f); }, {"met_pt"})
    .Define("met_phi_lead",   [](const vector<float>& v){ return first_or(v, -99.f); }, {"met_phi"})
    // leading jet + b-tag multiplicity
    .Define("jet_pt_lead",    [](const vector<float>& v){ return first_or(v, -99.f); }, {"jet_pt"})
    .Define("jet_eta_lead",   [](const vector<float>& v){ return first_or(v, -99.f); }, {"jet_eta"})
    .Define("jet_phi_lead",   [](const vector<float>& v){ return first_or(v, -99.f); }, {"jet_phi"})
    .Define("jet_mass_lead",  [](const vector<float>& v){ return first_or(v, -99.f); }, {"jet_mass"})
    .Define("jet_btag_lead",  [](const vector<int>&   v){ return static_cast<double>(first_or(v, 0)); }, {"jet_btag"})
    .Define("n_b_jet",        count_btags, {"jet_btag"});

  // System kinematics (μ + τ_lead)
  auto df_sys = df_lead
    .Define("syst_tuple",  syst_kine,
            {"muon_pt","muon_eta","muon_phi","muon_energy",
             "tau_pt_lead","tau_eta_lead","tau_phi_lead","tau_energy_lead"})
    .Define("sist_mass",   [](const Kine& k){ return k.mass; }, {"syst_tuple"})
    .Define("sist_pt",     [](const Kine& k){ return k.pt;   }, {"syst_tuple"})
    .Define("sist_rap",    [](const Kine& k){ return k.rap;  }, {"syst_tuple"})
    .Define("sist_acop",   [](const Kine& k){ return k.acop; }, {"syst_tuple"})
    .Define("weight",      [](){ return 1.0; })
    .Define("generator_weight", [](){ return 1.0; });

  // Columns to write (all names unique; no conflict with input)
  const std::vector<string> cols = {
    "muon_id","tau_id1","tau_id2","tau_id3",
    "muon_pt","tau_pt_lead","muon_charge","tau_charge_lead",
    "muon_eta","tau_eta_lead","muon_n","tau_n",
    "muon_phi","tau_phi_lead","muon_energy","tau_energy_lead",
    "sist_mass","sist_acop","sist_pt","sist_rap",
    "met_pt_lead","met_phi_lead",
    "jet_pt_lead","jet_eta_lead","jet_phi_lead","jet_mass_lead","jet_btag_lead",
    "weight","n_b_jet","generator_weight"
  };

  df_sys.Snapshot(outtreename, outfile, cols);
  std::cout << "Wrote skim to: " << outfile << " (tree: " << outtreename << ")\n";
}

int main(int argc, char** argv) {
  if (argc < 2) {
    std::cerr << "Usage: " << argv[0]
              << " input.root [output.root] [intree_path=analyzer/ntp1] [outtree_name=tree]\n";
    return 1;
  }
  const string infile      = argv[1];
  const string outfile     = (argc > 2) ? argv[2] : "skim_mutau_from_ntp1.root";
  const string intree_path = (argc > 3) ? argv[3] : "analyzer/ntp1";
  const string outtree     = (argc > 4) ? argv[4] : "tree";

  skim_mutau_rdf(infile, outfile, intree_path, outtree);
  return 0;
}
