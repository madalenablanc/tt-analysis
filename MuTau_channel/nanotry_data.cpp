// nanotry_data.cpp
#include "ROOT/RDataFrame.hxx"
#include "TFile.h"
#include "TLorentzVector.h"
#include <cmath>
#include <iostream>
#include <string>
#include <vector>

using std::string;
using std::vector;

// ---------- helpers ----------
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
    int n = 0;
    for (int b : btags) if (b > 0) ++n;   // treat >0 as "tagged"
    return n;
}

// ---------- skimmer ----------
void skim_mutau_rdf(const string& infile,
                    const string& outfile,
                    const string& intree_path = "analyzer/ntp1",
                    const string& outtreename = "tree")
{
    // parallelize
    ROOT::EnableImplicitMT();

    // build RDF on nested tree path
    ROOT::RDataFrame df(intree_path, infile);

    // selection: at least one mu and tau
    auto df_sel = df
        .Filter([](const vector<float>& mu_pt, const vector<float>& tau_pt){
            return !mu_pt.empty() && !tau_pt.empty();
        }, {"mu_pt", "tau_pt"}, "has >=1 mu & >=1 tau")
        // leading muon id
        .Filter([](const vector<float>& mu_id){
            return !mu_id.empty() && mu_id[0] >= 3.f;
        }, {"mu_id"}, "mu_id >= 3 (leading)")
        // leading tau DeepTau IDs
        .Filter([](const vector<float>& aJ, const vector<float>& aE, const vector<float>& aMu){
            return !aJ.empty() && !aE.empty() && !aMu.empty()
                   && aJ[0] >= 63.f && aE[0] >= 7.f && aMu[0] >= 1.f;
        }, {"tau_id_antij","tau_id_antie","tau_id_antimu"},
           "DeepTau VSjet>=63, VSe>=7, VSmu>=1 (leading)");

    // scalarize leading objects (keep names close to your previous skims)
    auto df_lead = df_sel
        .Redefine("mu_id",     [](const vector<float>& v){ return first_or(v, -99.f); }, {"mu_id"})
        .Define("tau_id1",     [](const vector<float>& v){ return first_or(v, -99.f); }, {"tau_id_antij"}) // VSjet
        .Define("tau_id2",     [](const vector<float>& v){ return first_or(v, -99.f); }, {"tau_id_antie"}) // VSe
        .Define("tau_id3",     [](const vector<float>& v){ return first_or(v, -99.f); }, {"tau_id_antimu"})// VSmu
        .Redefine("mu_pt",     [](const vector<float>& v){ return first_or(v, -99.f); }, {"mu_pt"})
        .Redefine("tau_pt",      [](const vector<float>& v){ return first_or(v, -99.f); }, {"tau_pt"})
        .Redefine("mu_eta",    [](const vector<float>& v){ return first_or(v, -99.f); }, {"mu_eta"})
        .Redefine("tau_eta",     [](const vector<float>& v){ return first_or(v, -99.f); }, {"tau_eta"})
        .Redefine("mu_phi",    [](const vector<float>& v){ return first_or(v, -99.f); }, {"mu_phi"})
        .Redefine("tau_phi",     [](const vector<float>& v){ return first_or(v, -99.f); }, {"tau_phi"})
        .Redefine("mu_energy", [](const vector<float>& v){ return first_or(v, -99.f); }, {"mu_energy"})
        .Redefine("tau_energy",  [](const vector<float>& v){ return first_or(v, -99.f); }, {"tau_energy"})
        .Redefine("mu_charge", [](const vector<float>& v){ return first_or(v, -99.f); }, {"mu_charge"}) // file stores float
        .Redefine("tau_charge",  [](const vector<int>&   v){ return static_cast<double>(first_or(v, -99)); }, {"tau_charge"})
        .Define("mu_n",      [](const vector<float>& v){ return static_cast<double>(v.size()); }, {"mu_pt"})
        .Define("tau_n",       [](const vector<float>& v){ return static_cast<double>(v.size()); }, {"tau_pt"})
        // MET (vector<float>, typically size 1)
        .Redefine("met_pt",      [](const vector<float>& v){ return first_or(v, -99.f); }, {"met_pt"})
        .Redefine("met_phi",     [](const vector<float>& v){ return first_or(v, -99.f); }, {"met_phi"})
        // leading jet + b-tag multiplicity
        .Redefine("jet_pt",      [](const vector<float>& v){ return first_or(v, -99.f); }, {"jet_pt"})
        .Redefine("jet_eta",     [](const vector<float>& v){ return first_or(v, -99.f); }, {"jet_eta"})
        .Redefine("jet_phi",     [](const vector<float>& v){ return first_or(v, -99.f); }, {"jet_phi"})
        .Redefine("jet_mass",    [](const vector<float>& v){ return first_or(v, -99.f); }, {"jet_mass"})
        .Redefine("jet_btag",    [](const vector<int>&   v){ return static_cast<double>(first_or(v, 0)); }, {"jet_btag"})
        .Define("n_b_jet",     count_btags, {"jet_btag"});

    // system kinematics
    auto df_sys = df_lead
        .Define("syst_tuple",  syst_kine,
                {"mu_pt","mu_eta","mu_phi","mu_energy",
                 "tau_pt","tau_eta","tau_phi","tau_energy"})
        .Define("sist_mass", [](const Kine& k){ return k.mass; }, {"syst_tuple"})
        .Define("sist_pt",   [](const Kine& k){ return k.pt;   }, {"syst_tuple"})
        .Define("sist_rap",  [](const Kine& k){ return k.rap;  }, {"syst_tuple"})
        .Define("sist_acop", [](const Kine& k){ return k.acop; }, {"syst_tuple"})
        .Define("weight",    [](){ return 1.0; })
        .Define("generator_weight", [](){ return 1.0; });

    // columns to write (compact skim)
    const std::vector<string> cols = {
        "mu_id","tau_id1","tau_id2","tau_id3",
        "mu_pt","tau_pt","mu_charge","tau_charge",
        "mu_eta","tau_eta","mu_n","tau_n",
        "mu_phi","tau_phi","mu_energy","tau_energy",
        "sist_mass","sist_acop","sist_pt","sist_rap",
        "met_pt","met_phi",
        "jet_pt","jet_eta","jet_phi","jet_mass","jet_btag",
        "weight","n_b_jet","generator_weight"
    };

    df_sys.Snapshot(outtreename, outfile, cols);
    std::cout << "Wrote skim to: " << outfile << " (tree: " << outtreename << ")\n";
}

// ---------- main ----------
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
