#include "ROOT/RDataFrame.hxx"
#include "TLorentzVector.h"
#include <cmath>
#include <iostream>
#include <string>
#include <vector>

using std::string;

// ---------------- helpers ----------------
inline double wrap_dphi(double dphi) {
  if (dphi > M_PI)  dphi -= 2.0 * M_PI;
  if (dphi < -M_PI) dphi += 2.0 * M_PI;
  return dphi;
}

inline double deltaR(double eta1, double phi1, double eta2, double phi2) {
  const double dEta = eta1 - eta2;
  const double dPhi = wrap_dphi(phi1 - phi2);
  return std::sqrt(dEta * dEta + dPhi * dPhi);
}

struct Kine { double mass, pt, rap, acop; };

inline Kine syst_kine(double e_pt, double e_eta, double e_phi, double e_m,
                      double t_pt, double t_eta, double t_phi, double t_m) {
  TLorentzVector e, tau, sys;
  e  .SetPtEtaPhiM(e_pt,  e_eta,  e_phi,  e_m);
  tau.SetPtEtaPhiM(t_pt,  t_eta,  t_phi,  t_m);
  sys = e + tau;

  double dphi = std::fabs(t_phi - e_phi);
  dphi = std::fabs(wrap_dphi(dphi));
  const double acop = dphi / M_PI;

  return { sys.M(), sys.Pt(), sys.Rapidity(), acop };
}

// --------------- main skim ---------------
void skim_etau_rdf(const string &infile,
                   const string &outfile,
                   const string &intree = "tree",
                   const string &outtree = "tree")
{
  ROOT::EnableImplicitMT();  // parallelize

  ROOT::RDataFrame df(intree, infile);

  // Selection (mirrors your QCD code):
  // e_id==1, tau_id1>64, tau_id2>8, tau_id3>2,
  // ΔR(e,τ)>0.4, |eta|<2.4, e_pt>35, tau_pt>100, opposite charge.
  auto df_sel = df
    .Filter([](double e_id){ return e_id == 1.0; }, {"e_id"}, "e_id==1")
    .Filter([](double t1, double t2, double t3){
        return (t1 > 64.) && (t2 > 8.) && (t3 > 2.);
      }, {"tau_id1","tau_id2","tau_id3"},
      "DeepTau WPs")
    .Filter([](double e_eta, double t_eta){
        return std::fabs(e_eta) < 2.4 && std::fabs(t_eta) < 2.4;
      }, {"e_eta","tau_eta"}, "|eta|<2.4")
    .Filter([](double e_pt, double t_pt){
        return (e_pt > 35.) && (t_pt > 100.);
      }, {"e_pt","tau_pt"}, "pT cuts")
    .Filter([](double e_q, double t_q){
        return (e_q * t_q) < 0.0;
      }, {"e_charge","tau_charge"}, "opposite charge")
    .Filter([](double e_eta, double e_phi, double t_eta, double t_phi){
        return deltaR(e_eta, e_phi, t_eta, t_phi) > 0.4;
      }, {"e_eta","electron_phi","tau_eta","tau_phi"}, "ΔR(e,τ)>0.4");

  // Derived system kinematics
  auto df_sys = df_sel
    .Define("syst_tuple", syst_kine,
            {"e_pt","e_eta","electron_phi","electron_mass",
             "tau_pt","tau_eta","tau_phi","tau_mass"})
    .Define("sist_mass", [](const Kine &k){ return k.mass; }, {"syst_tuple"})
    .Define("sist_pt",   [](const Kine &k){ return k.pt;   }, {"syst_tuple"})
    .Define("sist_rap",  [](const Kine &k){ return k.rap;  }, {"syst_tuple"})
    .Define("sist_acop", [](const Kine &k){ return k.acop; }, {"syst_tuple"})

    // Map to your output branch names (new columns where names differ)
    .Define("eletron_id",   [](double x){ return x; }, {"e_id"})
    .Define("eletron_pt",   [](double x){ return x; }, {"e_pt"})
    .Define("eletron_eta",  [](double x){ return x; }, {"e_eta"})
    .Define("eletron_charge",[](double x){ return x; }, {"e_charge"})
    .Define("eletron_n",    [](){ return 1.0; })   // scalar input ⇒ multiplicity = 1
    .Define("tau_n",        [](){ return 1.0; })
    .Define("weight",       [](){ return 1.0; })
    .Define("generator_weight", [](){ return 0.0; }); // matches your code (set to 0)

  // Columns to write (names & order follow your TTree::Branch list)
  const std::vector<std::string> cols = {
    "eletron_id",      // e_id
    "tau_id1",
    "tau_id2",
    "tau_id3",
    "eletron_pt",      // e_pt
    "tau_pt",
    "eletron_charge",  // e_charge
    "tau_charge",
    "eletron_eta",     // e_eta
    "tau_eta",
    "eletron_n",       // (1)
    "tau_n",           // (1)
    "electron_phi",
    "tau_phi",
    "electron_mass",
    "tau_mass",
    "sist_mass",
    "sist_acop",
    "sist_pt",
    "sist_rap",
    "met_pt",          // keep correct order: met_pt then met_phi
    "met_phi",
    "jet_pt",
    "jet_eta",
    "jet_phi",
    "jet_mass",
    "jet_btag",
    "weight",
    "n_b_jet",
    "generator_weight"
  };

  df_sys.Snapshot(outtree, outfile, cols);
  std::cout << "Wrote skim to: " << outfile << " (tree: " << outtree << ")\n";
}

int main(int argc, char **argv) {
  if (argc < 3) {
    std::cerr << "Usage: " << argv[0]
              << " input.root output.root [intree=tree] [outtree=tree]\n";
    return 1;
  }
  const string infile  = argv[1];
  const string outfile = argv[2];
  const string intree  = (argc > 3) ? argv[3] : "tree";
  const string outtree = (argc > 4) ? argv[4] : "tree";

  try {
    skim_etau_rdf(infile, outfile, intree, outtree);
  } catch (const std::exception &e) {
    std::cerr << "Error: " << e.what() << "\n";
    return 2;
  }
  return 0;
}
