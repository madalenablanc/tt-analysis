#include <ROOT/RDataFrame.hxx>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cmath>
#include "TFile.h"
#include "TH1D.h"
#include "TLorentzVector.h"

using namespace std;
using namespace ROOT;

// Helper function to build TLorentzVector with proper float types
TLorentzVector buildTLorentzVector(float pt, float eta, float phi, float mass) {
    TLorentzVector v;
    v.SetPtEtaPhiM(pt, eta, phi, mass);
    return v;
}

int main() {
    //enable or disable
    ROOT::EnableImplicitMT();

    // Read luminosity data
    ifstream ifilelum("dadosluminosidade.txt");
    int lum[447][101] = {0};
    vector<int> lumaux;
    int d;

    while (ifilelum >> d) {
        lumaux.push_back(d);
    }

    int o = -1;
    int z = 0;
    for (int i = 0; i < lumaux.size(); i++) {
        if (lumaux[i] > 50000) {
            o++;
            z = 0;
        }
        lum[o][z] = lumaux[i];
        z++;
    }

    // Input and output file paths
    string input = "root:://cms-xrd-global.cern.ch///store/data/Run2018A/Tau/NANOAOD/UL2018_MiniAODv2_NanoAODv9-v2/2810000/0075950B-3B06-844E-86D0-7900E3A78B52.root";
    string output_tot = "/eos/user/m/mblancco/samples_2018_tautau/background_test/QCD_2018_UL_skimmed_TauTau_nano.root";
    
    float weight = 1.0;

    // Create RDataFrame
    RDataFrame df("Events", input);
    
    // Get LuminosityBlocks tree separately for lumi checks
    TFile *f = TFile::Open(input.c_str());
    TTree *tree_lumi = (TTree*)f->Get("LuminosityBlocks");
    
    // Define filters and transformations
    auto df_filtered = df
        // Filter for events with at least 2 taus
        .Filter("Tau_pt.size() >= 2", "At least 2 taus")
        // Trigger requirement
        .Filter("HLT_DoubleMediumChargedIsoPFTauHPS35_Trk1_eta2p1_Reg == 1", "Double tau trigger")
        // Filter for run and luminosity blocks
        .Define("pass_lumi", [&lum](UInt_t run, UInt_t luminosityBlock) {
            int check = 0;
            int lumcheck = -1;
            
            // Find the run
            for (int j = 0; j < 447; j++) {
                if (lum[j][0] == run) {
                    check = 1;
                    lumcheck = j;
                    break;
                }
            }
            
            if (check == 0) return false;
            
            check = 0;
            // Check if luminosity block is in the accepted range
            for (int j = 1; j < 101; j += 2) {
                if (luminosityBlock > lum[lumcheck][j] && luminosityBlock < lum[lumcheck][j+1]) {
                    check = 1;
                    break;
                }
            }
            
            return check == 1;
        }, {"run", "luminosityBlock"})
        .Filter("pass_lumi", "Passes luminosity check")
        
        // Define new columns - use the same types as in the original NanoAOD (mostly float)
        .Define("tau0_id1", "Tau_idDeepTau2017v2p1VSjet[0]")
        .Define("tau0_id2", "Tau_idDeepTau2017v2p1VSe[0]")
        .Define("tau0_id3", "Tau_idDeepTau2017v2p1VSmu[0]")
        .Define("tau1_id1", "Tau_idDeepTau2017v2p1VSjet[1]")
        .Define("tau1_id2", "Tau_idDeepTau2017v2p1VSe[1]")
        .Define("tau1_id3", "Tau_idDeepTau2017v2p1VSmu[1]")
        .Define("tau0_pt", "Tau_pt[0]")
        .Define("tau1_pt", "Tau_pt[1]")
        .Define("tau0_charge", "Tau_charge[0]")
        .Define("tau1_charge", "Tau_charge[1]")
        .Define("tau0_eta", "Tau_eta[0]")
        .Define("tau1_eta", "Tau_eta[1]")
        .Define("tau_n", "(int)Tau_pt.size()")  // Convert to int
        .Define("tau0_phi", "Tau_phi[0]")
        .Define("tau1_phi", "Tau_phi[1]")
        .Define("tau0_mass", "Tau_mass[0]")
        .Define("tau1_mass", "Tau_mass[1]")
        
        // Build TLorentzVectors and calculate system variables
        // Use lambda functions with correct float parameter types
        .Define("tau0_tlv", [](float pt, float eta, float phi, float mass) {
            return buildTLorentzVector(pt, eta, phi, mass);
        }, {"tau0_pt", "tau0_eta", "tau0_phi", "tau0_mass"})
        
        .Define("tau1_tlv", [](float pt, float eta, float phi, float mass) {
            return buildTLorentzVector(pt, eta, phi, mass);
        }, {"tau1_pt", "tau1_eta", "tau1_phi", "tau1_mass"})
        
        .Define("system_tlv", [](const TLorentzVector &v1, const TLorentzVector &v2) {
            return v1 + v2;
        }, {"tau0_tlv", "tau1_tlv"})
        
        .Define("sist_mass", "system_tlv.M()")
        .Define("sist_pt", "system_tlv.Pt()")
        .Define("sist_rap", "system_tlv.Rapidity()")
        
        // Calculate acoplanarity - using float types
        .Define("delta_phi", [](float phi1, float phi2) {
            float deltaphi = fabs(phi2 - phi1);
            if (deltaphi > M_PI) {
                deltaphi = deltaphi - 2 * M_PI;
            }
            return deltaphi;
        }, {"tau0_phi", "tau1_phi"})
        
        .Define("sist_acop", "fabs(delta_phi) / M_PI")
        
        // Add MET and jet info
        .Define("met_pt", "MET_pt")
        .Define("met_phi", "MET_phi")
        .Define("jet_pt", "Jet_pt.size() > 0 ? Jet_pt[0] : -999.0f")
        .Define("jet_eta", "Jet_pt.size() > 0 ? Jet_eta[0] : -999.0f")
        .Define("jet_phi", "Jet_pt.size() > 0 ? Jet_phi[0] : -999.0f")
        .Define("jet_mass", "Jet_pt.size() > 0 ? Jet_mass[0] : -999.0f")
        .Define("jet_btag", "Jet_pt.size() > 0 ? Jet_btagDeepB[0] : -999.0f")
        
        // Count number of b-tagged jets
        .Define("n_b_jet", "(int)Sum(Jet_btagDeepB > 0.4506)")
        
        // Add weights
        .Define("weight", [weight]() { return weight; })
        .Define("generator_weight", []() { return -1.0f; });  // Use float for consistency

    // With multithreading disabled, we can directly use Range
    auto df_limited = df_filtered;
    
    // Declare the columns we want to save
    vector<string> columns = {
        "tau0_id1", "tau0_id2", "tau0_id3", "tau1_id1", "tau1_id2", "tau1_id3",
        "tau0_pt", "tau1_pt", "tau0_charge", "tau1_charge", "tau0_eta", "tau1_eta",
        "tau_n", "tau0_phi", "tau1_phi", "tau0_mass", "tau1_mass",
        "sist_mass", "sist_acop", "sist_pt", "sist_rap",
        "met_pt", "met_phi", "jet_pt", "jet_eta", "jet_phi", "jet_mass", "jet_btag",
        "weight", "n_b_jet", "generator_weight"
    };
    
    // Save the result to the output file
    df_limited.Snapshot("tree", output_tot, columns);
    
    // Print some statistics
    cout << "Number of events processed: " << *df_limited.Count() << endl;
    
    // Print counters (equivalents to the original code's counters)
    cout << "nid: " << *df_filtered.Count() << endl;
    cout << "ncharge: " << *df_filtered.Filter("tau0_charge * tau1_charge < 0").Count() << endl;
    cout << "npt: " << *df_filtered.Filter("tau0_pt > 0 && tau1_pt > 0").Count() << endl;
    cout << "neta: " << *df_filtered.Filter("abs(tau0_eta) < 2.1 && abs(tau1_eta) < 2.1").Count() << endl;
    
    return 0;
}