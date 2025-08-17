
// run with g++ -O2 -std=c++17 open_ttjets.cpp $(root-config --cflags --libs) -o open_ttjets
// [mblancco@lxplus964 MuTau_channel]$ ./open_ttjets 

#include <ROOT/RDataFrame.hxx>
#include <ROOT/RDFHelpers.hxx>   // contains AddProgressBar
#include <TLorentzVector.h>
#include <vector>
#include <array>
#include <string>
#include <fstream>
#include <cmath>

// Global SF tables
static std::vector<std::array<double,6>> sfNeg, sfPos, sfHigh, sfIdIso;

std::vector<std::array<double,6>> readSF(const std::string &path) {
    std::vector<std::array<double,6>> table;
    std::ifstream f(path);
    double e0,e1,p0,p1,corr,syst;
    while (f >> e0 >> e1 >> p0 >> p1 >> corr >> syst) {
        table.push_back({e0,e1,p0,p1,corr,syst});
    }
    return table;
}

void loadSF() {
    sfNeg  = readSF("/eos/user/m/mblancco/tau_analysis/POGCorrections/muon_neg18_lowTrig.txt");
    sfPos  = readSF("/eos/user/m/mblancco/tau_analysis/POGCorrections/muon_pos18_lowTrig.txt");
    sfHigh = readSF("/eos/user/m/mblancco/tau_analysis/POGCorrections/muon_18_highTrig.txt");
    sfIdIso= readSF("/eos/user/m/mblancco/tau_analysis/POGCorrections/muon_IdISO.txt");
}

// SF functions (unchanged; take doubles internally)
double MuonTrigSF(double pt, double eta, double charge, int flag=0) {
    if (pt < 35.0 || std::abs(eta) > 2.4) return 1.0;
    const auto &table = pt < 200.0 ? (charge < 0 ? sfNeg : sfPos) : sfHigh;
    double sf = 1.0;
    for (const auto &row : table) {
        if (std::abs(eta) > row[0] && std::abs(eta) <= row[1] &&
            pt > row[2] && pt <= row[3]) {
            sf = (flag == 0) ? row[4] : row[5];
            break;
        }
    }
    return sf;
}

double MuonIDISOSF(double pt, double eta, int flag=0) {
    if (pt < 35.0 || std::abs(eta) > 2.4) return 1.0;
    double sf = 1.0;
    for (const auto &row : sfIdIso) {
        if (std::abs(eta) > row[0] && std::abs(eta) <= row[1] &&
            pt > row[2] && pt <= row[3]) {
            sf = (flag == 0) ? row[4] : row[5];
            break;
        }
    }
    return sf;
}

double muRecoSF(double pt, double eta, int flag=0) {
    double sf = 1.0;
    double absEta = std::abs(eta);
    if (flag == 0) {
        if (absEta > 2.1 && pt < 200.0) return 0.999;
        if (pt > 200.0) {
            double p_tot = pt / std::sin(2.0 * std::atan(std::exp(-eta)));
            if (p_tot > 100 && p_tot < 150)  sf = absEta < 1.6 ? 0.9948 : 0.993;
            if (p_tot > 150 && p_tot < 200)  sf = absEta < 1.6 ? 0.9950 : 0.990;
            if (p_tot > 200 && p_tot < 300)  sf = absEta < 1.6 ? 0.994  : 0.988;
            if (p_tot > 300 && p_tot < 400)  sf = absEta < 1.6 ? 0.9914 : 0.981;
            if (p_tot > 400 && p_tot < 600)  sf = absEta < 1.6 ? 0.993  : 0.983;
            if (p_tot > 600 && p_tot < 1500) sf = absEta < 1.6 ? 0.991  : 0.978;
            if (p_tot > 1500 && p_tot < 3500) sf= absEta < 1.6 ? 1.0    : 0.98;
        }
    } else {
        if (pt < 200.0) {
            if (absEta < 0.9)        sf = 0.000382;
            else if (absEta < 1.2)   sf = 0.000522;
            else if (absEta < 2.1)   sf = 0.000831;
            else                     sf = 0.001724;
        } else {
            double p_tot = pt / std::sin(2.0 * std::atan(std::exp(-eta)));
            if (absEta < 1.6) {
                if (p_tot < 300)          sf = 0.001;
                else if (p_tot < 400)     sf = 0.0009;
                else if (p_tot < 600)     sf = 0.002;
                else if (p_tot < 1500)    sf = 0.004;
                else                      sf = 0.1;
            } else {
                if (p_tot < 300)          sf = 0.001;
                else if (p_tot < 400)     sf = 0.002;
                else if (p_tot < 600)     sf = 0.003;
                else if (p_tot < 1500)    sf = 0.006;
                else                      sf = 0.03;
            }
        }
    }
    return sf;
}

int main() {
        ROOT::EnableImplicitMT();  // parallelise automatically
    
        // load scale factors
        loadSF();
    
        // build list of ROOT files to process
        std::vector<std::string> files;
        std::ifstream list("/eos/user/m/mblancco/samples_2018_mutau/samples_ttjets_skimmed.txt");
        std::string fname;
        while (std::getline(list,fname)) {
            if(!fname.empty())
                files.push_back("/eos/user/m/mblancco/samples_2018_mutau/" + fname);
        }
    
        // define the initial dataframe
        ROOT::RDataFrame rdf("tree", files);
    
        // apply filters and compute new columns (types match your input tree)
        auto df = rdf
            .Define("dR", [](float mu_eta, float mu_phi, float tau_eta, float tau_phi) {
                float dphi = mu_phi - tau_phi;
                if(dphi > M_PI) dphi -= 2.f*M_PI;
                if(dphi < -M_PI) dphi += 2.f*M_PI;
                return std::sqrt((mu_eta - tau_eta)*(mu_eta - tau_eta) + dphi*dphi);
            }, {"mu_eta","mu_phi","tau_eta","tau_phi"})
            .Filter("mu_id > 3 && tau_id1 > 63 && tau_id2 > 7 && tau_id3 > 1","id_cuts")
            .Filter("dR > 0.4f && abs(mu_eta) < 2.4f && abs(tau_eta) < 2.4f","eta_cuts")
            .Filter("mu_pt > 35.0f && tau_pt > 100.0f","pt_cuts")
            .Filter("mu_charge * tau_charge < 0","charge_cuts")
            // define SFs using float/int arguments to match branch types
            .Define("mu_trig_sf",  [](float pt,float eta,int charge){ return MuonTrigSF(pt,eta,charge,0); }, {"mu_pt","mu_eta","mu_charge"})
            .Define("mu_idiso_sf", [](float pt,float eta){ return MuonIDISOSF(pt,eta,0); }, {"mu_pt","mu_eta"})
            .Define("mu_reco_sf",  [](float pt,float eta){ return muRecoSF(pt,eta,0); }, {"mu_pt","mu_eta"})
            .Define("mu_trig_syst",  [](float pt,float eta,int charge){ return MuonTrigSF(pt,eta,charge,1); }, {"mu_pt","mu_eta","mu_charge"})
            .Define("mu_idiso_syst", [](float pt,float eta){ return MuonIDISOSF(pt,eta,1); }, {"mu_pt","mu_eta"})
            .Define("mu_reco_syst",  [](float pt,float eta){ return muRecoSF(pt,eta,1); }, {"mu_pt","mu_eta"})
            .Define("event_weight", "0.15 * mu_trig_sf * mu_idiso_sf * mu_reco_sf")
            // build 4‑vectors and new system variables
            .Define("mu4vec",   [](float pt,float eta,float phi,float m){ TLorentzVector v; v.SetPtEtaPhiM(pt,eta,phi,m); return v; }, {"mu_pt","mu_eta","mu_phi","mu_mass"})
            .Define("tau4vec",  [](float pt,float eta,float phi,float m){ TLorentzVector v; v.SetPtEtaPhiM(pt,eta,phi,m); return v; }, {"tau_pt","tau_eta","tau_phi","tau_mass"})
            .Define("sistema_vec", [](const TLorentzVector &t,const TLorentzVector &m){ return t + m; }, {"tau4vec","mu4vec"})
            .Define("calc_sist_mass","sistema_vec.M()")
            .Define("calc_sist_pt",  "sistema_vec.Pt()")
            .Define("calc_sist_rap", "sistema_vec.Rapidity()")
            .Define("calc_acop", [](float mu_phi,float tau_phi){
                float dphi = std::fabs(mu_phi - tau_phi);
                if(dphi > M_PI) dphi -= 2.f*M_PI;
                return std::fabs(dphi) / static_cast<float>(M_PI);
            }, {"mu_phi","tau_phi"});
    
        // attach a progress bar to df
        ROOT::RDF::Experimental::AddProgressBar(df);
    
        auto report = df.Report();
        

        // trigger computation and write output
        df.Snapshot("tree_out",
                    "/eos/user/m/mblancco/samples_2018_mutau/ficheiros_fase1/ttjets_parallel_with_bar.root",
                    {
                        "mu_id","tau_id1","tau_id2","tau_id3",
                        "mu_pt","tau_pt","mu_charge","tau_charge",
                        "mu_eta","tau_eta","mu_n","tau_n",
                        "mu_phi","tau_phi","mu_mass","tau_mass",
                        "calc_sist_mass","calc_acop","calc_sist_pt","calc_sist_rap",
                        "met_pt","met_phi","jet_pt","jet_eta","jet_phi",
                        "jet_mass","jet_btag","weight","n_b_jet",
                        "generator_weight","mu_trig_sf","mu_idiso_sf","mu_reco_sf",
                        "mu_trig_syst","mu_idiso_syst","mu_reco_syst","event_weight"
                    });

                    report->Print();

    
        return 0;
    }