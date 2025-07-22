#include <stdio.h>
#include <iostream>
#include <cmath>
#include "TApplication.h"
#include "TFile.h"
#include "TTree.h"
#include "TH1D.h"
#include "TLorentzVector.h"
#include "ROOT/RDataFrame.hxx"
#include "ROOT/RVec.hxx"
#include <string>
#include <fstream>
#include <cstdlib>
#include <sstream>

using namespace std;
using namespace ROOT;
using RNode = ROOT::RDF::RNode;

int main(){
    // Enable implicit multi-threading
    ROOT::EnableImplicitMT();
    
    string prefix = "root:://cms-xrd-global.cern.ch//";
    string input;
    string prefix_output = "ttJets_2018_UL_skimmed_TauTau_nano";
    
    int k=0;
    double weight = 0.15;
    
    int numero_linha;
    cin >> numero_linha;
    
    stringstream ss;
    ss << numero_linha;
    string out_put;
    ss >> out_put;
    
    string output_tot = "/eos/user/m/mblancco/samples_2018_tautau/fase0_try_parallel/" + prefix_output + out_put + ".root";
    
    ifstream ifile;
    ifile.open("ttJets_2018_UL.txt");
    while(k < numero_linha) {
        ifile >> input;
        k++;
    }
    
    string total = prefix + input;
    cout << "Input file: " << total << endl;
    
    // Create RDataFrame
    RDataFrame df("Events", total);
    
    // Define the analysis chain with filters and transformations
    auto df_filtered = df
        // Filter: At least 2 taus
        .Filter("Tau_pt.size() >= 2", "At least 2 taus")
        
        // Filter: HLT trigger
        .Filter("HLT_DoubleMediumChargedIsoPFTauHPS40_Trk1_eta2p1_Reg == 1", "HLT trigger")
        
        // Define tau vectors
        .Define("tau0_vec", [](const RVec<float>& pt, const RVec<float>& eta, 
                              const RVec<float>& phi, const RVec<float>& mass) {
            TLorentzVector tau;
            tau.SetPtEtaPhiM(pt[0], eta[0], phi[0], mass[0]);
            return tau;
        }, {"Tau_pt", "Tau_eta", "Tau_phi", "Tau_mass"})
        
        .Define("tau1_vec", [](const RVec<float>& pt, const RVec<float>& eta, 
                              const RVec<float>& phi, const RVec<float>& mass) {
            TLorentzVector tau;
            tau.SetPtEtaPhiM(pt[1], eta[1], phi[1], mass[1]);
            return tau;
        }, {"Tau_pt", "Tau_eta", "Tau_phi", "Tau_mass"})
        
        // Define system variables
        .Define("sistema_vec", [](const TLorentzVector& tau0, const TLorentzVector& tau1) {
            return tau0 + tau1;
        }, {"tau0_vec", "tau1_vec"})
        
        .Define("sist_mass", [](const TLorentzVector& sistema) {
            return sistema.M();
        }, {"sistema_vec"})
        
        .Define("sist_pt", [](const TLorentzVector& sistema) {
            return sistema.Pt();
        }, {"sistema_vec"})
        
        .Define("sist_rap", [](const TLorentzVector& sistema) {
            return sistema.Rapidity();
        }, {"sistema_vec"})
        
        // Define acoplanarity
        .Define("sist_acop", [](const RVec<float>& phi) {
            double deltaphi = fabs(phi[1] - phi[0]);
            if(deltaphi > M_PI) {
                deltaphi = deltaphi - 2*M_PI;
            }
            return fabs(deltaphi)/M_PI;
        }, {"Tau_phi"})
        
        // Define b-jet counting
        .Define("n_b_jet", [](const RVec<float>& btag) {
            int btag_count = 0;
            for(const auto& b : btag) {
                if(b > 0.4506) btag_count++;
            }
            return btag_count;
        }, {"Jet_btagDeepB"})
        
        // Define weight
        .Define("weight", [weight]() { return weight; })
        
        // Define first jet variables (with safety checks)
        .Define("jet_pt", [](const RVec<float>& pt) {
            return pt.size() > 0 ? pt[0] : -999.0f;
        }, {"Jet_pt"})
        
        .Define("jet_eta", [](const RVec<float>& eta) {
            return eta.size() > 0 ? eta[0] : -999.0f;
        }, {"Jet_eta"})
        
        .Define("jet_phi", [](const RVec<float>& phi) {
            return phi.size() > 0 ? phi[0] : -999.0f;
        }, {"Jet_phi"})
        
        .Define("jet_mass", [](const RVec<float>& mass) {
            return mass.size() > 0 ? mass[0] : -999.0f;
        }, {"Jet_mass"})
        
        .Define("jet_btag", [](const RVec<float>& btag) {
            return btag.size() > 0 ? btag[0] : -999.0f;
        }, {"Jet_btagDeepB"});
    
    // Define columns to save
    vector<string> columns = {
        "Tau_idDeepTau2017v2p1VSjet", "Tau_idDeepTau2017v2p1VSe", "Tau_idDeepTau2017v2p1VSmu",
        "Tau_pt", "Tau_charge", "Tau_eta", "Tau_phi", "Tau_mass",
        "sist_mass", "sist_acop", "sist_pt", "sist_rap",
        "MET_pt", "MET_phi",
        "jet_pt", "jet_eta", "jet_phi", "jet_mass", "jet_btag",
        "weight", "n_b_jet", "Generator_weight",
        "Tau_decayMode", "Tau_genPartFlav"
    };
    
    // Save to file
    df_filtered.Snapshot("tree", output_tot, columns);
    
    // Print statistics
    auto neta = df_filtered.Count();
    cout << "Events processed: " << *neta << endl;
    cout << -11111111111111 << endl;
    
    return 0;
}