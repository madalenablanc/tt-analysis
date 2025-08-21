#include "ROOT/RDataFrame.hxx"
#include "TLorentzVector.h"
#include <cmath>
#include <iostream>
#include <string>
#include <vector>

using std::string;
using std::vector;

inline double wrap_dphi(double dphi) {
  if (dphi >  M_PI) dphi -= 2.0 * M_PI;
  if (dphi < -M_PI) dphi += 2.0 * M_PI;
  return dphi;
}
inline double deltaR(double eta1, double phi1, double eta2, double phi2) {
  const double dEta = eta1 - eta2;
  const double dPhi = wrap_dphi(phi1 - phi2);
  return std::sqrt(dEta*dEta + dPhi*dPhi);
}
inline int count_btags(const vector<int>& btags) {
  int n=0; for (int b : btags) if (b > 0) ++n; return n; // treat >0 as tagged
}
struct Kine { double mass, pt, rap, acop; };
inline Kine syst_kine(double ept,double eeta,double ephi,double eE,
                      double tpt,double teta,double tphi,double tE) {
  TLorentzVector e, tau, sys;
  e  .SetPtEtaPhiE(ept, eeta, ephi, eE);
  tau.SetPtEtaPhiE(tpt, teta, tphi, tE);
  sys = e + tau;
  const double acop = std::fabs(wrap_dphi(tphi - ephi)) / M_PI;
  return { sys.M(), sys.Pt(), sys.Rapidity(), acop };
}

void skim_etau_rdf_fase0(const string& infile,
                         const string& outfile,
                         const string& intree_path = "analyzer/ntp1",
                         const string& outtreename = "tree")
{
  ROOT::EnableImplicitMT();

  ROOT::RDataFrame df(intree_path, infile);

  // Require at least one electron & one tau present
  auto df0 = df.Filter(
      [](const vector<float>& ept, const vector<float>& tpt){
        return !ept.empty() && !tpt.empty();
      }, {"e_pt","tau_pt"}, "has >=1 e & >=1 tau");

  // Apply selection on LEADING objects (index 0)
  auto df_sel = df0
    .Filter([](const vector<float>& e_id){
        return !e_id.empty() && e_id[0] == 1.0f;
      }, {"e_id"}, "e_id==1")
    .Filter([](const vector<float>& vsj,const vector<float>& vse,const vector<float>& vsmu){
        return !vsj.empty() && !vse.empty() && !vsmu.empty()
            && vsj[0] > 64.f && vse[0] > 8.f && vsmu[0] > 2.f;
      }, {"tau_id_antij","tau_id_antie","tau_id_antimu"}, "DeepTau WPs")
    .Filter([](const vector<float>& eeta,const vector<float>& teta){
        return std::fabs(eeta[0]) < 2.4 && std::fabs(teta[0]) < 2.4;
      }, {"e_eta","tau_eta"}, "|eta|<2.4")
    .Filter([](const vector<float>& ept,const vector<float>& tpt){
        return ept[0] > 35.f && tpt[0] > 100.f;
      }, {"e_pt","tau_pt"}, "pT cuts")
    .Filter([](const vector<float>& eq,const vector<int>& tq){
        return !eq.empty() && !tq.empty() && (eq[0] * tq[0] < 0.0f);
      }, {"e_charge","tau_charge"}, "opposite charge")
    .Filter([](const vector<float>& eeta,const vector<float>& ephi,
               const vector<float>& teta,const vector<float>& tphi){
        return deltaR(eeta[0], ephi[0], teta[0], tphi[0]) > 0.4;
      }, {"e_eta","e_phi","tau_eta","tau_phi"}, "ΔR(e,τ)>0.4");

  // Counts BEFORE redefining (use original vectors)
  auto df_counts = df_sel
    .Define("e_n",   [](const vector<float>& v){ return static_cast<double>(v.size()); }, {"e_pt"})
    .Define("tau_n", [](const vector<float>& v){ return static_cast<double>(v.size()); }, {"tau_pt"})
    .Define("n_b_jet", count_btags, {"jet_btag"});

  // Convert vectors to leading-value SCALARS (override names with Redefine)
  auto df_scalars = df_counts
    .Redefine("e_id",      [](const vector<float>& v){ return v.empty()?-99.0:double(v[0]); }, {"e_id"})
    .Redefine("e_pt",      [](const vector<float>& v){ return v.empty()?-99.0:double(v[0]); }, {"e_pt"})
    .Redefine("e_eta",     [](const vector<float>& v){ return v.empty()?-99.0:double(v[0]); }, {"e_eta"})
    .Define   ("electron_phi",[](const vector<float>& v){ return v.empty()?-99.0:double(v[0]); }, {"e_phi"})
    .Redefine("e_energy",  [](const vector<float>& v){ return v.empty()?-99.0:double(v[0]); }, {"e_energy"})
    .Redefine("e_charge",  [](const vector<float>& v){ return v.empty()?-99.0:double(v[0]); }, {"e_charge"})

    .Redefine("tau_pt",    [](const vector<float>& v){ return v.empty()?-99.0:double(v[0]); }, {"tau_pt"})
    .Redefine("tau_eta",   [](const vector<float>& v){ return v.empty()?-99.0:double(v[0]); }, {"tau_eta"})
    .Redefine("tau_phi",   [](const vector<float>& v){ return v.empty()?-99.0:double(v[0]); }, {"tau_phi"})
    .Redefine("tau_energy",[](const vector<float>& v){ return v.empty()?-99.0:double(v[0]); }, {"tau_energy"})
    .Redefine("tau_charge",[](const vector<int>&   v){ return v.empty()?-99.0:double(v[0]); }, {"tau_charge"})

    .Define("tau_id1",     [](const vector<float>& v){ return v.empty()?-99.0:double(v[0]); }, {"tau_id_antij"})
    .Define("tau_id2",     [](const vector<float>& v){ return v.empty()?-99.0:double(v[0]); }, {"tau_id_antie"})
    .Define("tau_id3",     [](const vector<float>& v){ return v.empty()?-99.0:double(v[0]); }, {"tau_id_antimu"})

    .Redefine("met_pt",    [](const vector<float>& v){ return v.empty()?-99.0:double(v[0]); }, {"met_pt"})
    .Redefine("met_phi",   [](const vector<float>& v){ return v.empty()?-99.0:double(v[0]); }, {"met_phi"})

    .Redefine("jet_pt",    [](const vector<float>& v){ return v.empty()?-99.0:double(v[0]); }, {"jet_pt"})
    .Redefine("jet_eta",   [](const vector<float>& v){ return v.empty()?-99.0:double(v[0]); }, {"jet_eta"})
    .Redefine("jet_phi",   [](const vector<float>& v){ return v.empty()?-99.0:double(v[0]); }, {"jet_phi"})
    .Redefine("jet_mass",  [](const vector<float>& v){ return v.empty()?-99.0:double(v[0]); }, {"jet_mass"})
    .Redefine("jet_btag",  [](const vector<int>&   v){ return v.empty()?  0.0:double(v[0]); }, {"jet_btag"})

    // Masses from (pt,eta,phi,E)
    .Define("electron_mass",
            [](double pt,double eta,double phi,double E){
              TLorentzVector p; p.SetPtEtaPhiE(pt,eta,phi,E); return p.M();
            }, {"e_pt","e_eta","electron_phi","e_energy"})
    .Define("tau_mass",
            [](double pt,double eta,double phi,double E){
              TLorentzVector p; p.SetPtEtaPhiE(pt,eta,phi,E); return p.M();
            }, {"tau_pt","tau_eta","tau_phi","tau_energy"})

    // System kinematics
    .Define("syst_tuple", syst_kine,
            {"e_pt","e_eta","electron_phi","e_energy",
             "tau_pt","tau_eta","tau_phi","tau_energy"})
    .Define("sist_mass", [](const Kine& k){ return k.mass; }, {"syst_tuple"})
    .Define("sist_pt",   [](const Kine& k){ return k.pt;   }, {"syst_tuple"})
    .Define("sist_rap",  [](const Kine& k){ return k.rap;  }, {"syst_tuple"})
    .Define("sist_acop", [](const Kine& k){ return k.acop; }, {"syst_tuple"})
    .Define("weight",           [](){ return 1.0; })
    .Define("generator_weight", [](){ return 0.0; });

  // Final column list — EXACT names your analyzer reads with GetLeaf(...)
  const std::vector<std::string> cols = {
    "e_id",
    "tau_id1","tau_id2","tau_id3",
    "e_pt","tau_pt",
    "e_charge","tau_charge",
    "e_eta","tau_eta",
    "e_n","tau_n",
    "electron_phi","tau_phi",
    "electron_mass","tau_mass",
    "sist_mass","sist_acop","sist_pt","sist_rap",
    "met_pt","met_phi",
    "jet_pt","jet_eta","jet_phi","jet_mass","jet_btag",
    "weight","n_b_jet","generator_weight"
  };

  df_scalars.Snapshot(outtreename, outfile, cols);
  std::cout << "Wrote skim to: " << outfile << " (tree: " << outtreename << ")\n";
}

int main(int argc, char** argv) {
  if (argc < 3) {
    std::cerr << "Usage: " << argv[0]
              << " input.root output.root [intree_path=analyzer/ntp1] [outtree_name=tree]\n";
    return 1;
  }
  const string infile      = argv[1];
  const string outfile     = argv[2];
  const string intree_path = (argc > 3) ? argv[3] : "analyzer/ntp1";
  const string outtree     = (argc > 4) ? argv[4] : "tree";

  try { skim_etau_rdf_fase0(infile, outfile, intree_path, outtree); }
  catch (const std::exception& e) { std::cerr << "Error: " << e.what() << "\n"; return 2; }
  return 0;
}
