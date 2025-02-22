#include <ROOT/RDataFrame.hxx>
#include <TFile.h>
#include <TLorentzVector.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>

using namespace std;

// Function to load luminosity data from a file
vector<vector<int>> loadLuminosityData(const string& filename) {
    ifstream ifilelum(filename);
    vector<int> lumaux;
    int d;
    while (ifilelum >> d) {
        lumaux.push_back(d);
    }
    vector<vector<int>> lum(447, vector<int>(101, 0));
    int o = -1, z = 0;
    for (size_t i = 0; i < lumaux.size(); ++i) {
        if (lumaux[i] > 50000) {
            o++; z = 0;
        }
        lum[o][z] = lumaux[i];
        z++;
    }
    return lum;
}

// Function to check if an event passes the luminosity selection
bool passLuminositySelection(unsigned int run, unsigned int luminosityBlock, const vector<vector<int>>& lum) {
    for (const auto& l : lum) {
        if (l[0] == static_cast<int>(run)) {
            for (size_t j = 1; j < l.size(); j += 2) {
                if (luminosityBlock > static_cast<unsigned int>(l[j]) && luminosityBlock < static_cast<unsigned int>(l[j + 1])) {
                    return true;
                }
            }
        }
    }
    return false;
}

// Function to process each event
bool processEvent(unsigned int run, unsigned int luminosityBlock,
                  const ROOT::RVec<float>& Tau_pt, const ROOT::RVec<float>& Tau_eta,
                  const ROOT::RVec<float>& Tau_phi, const ROOT::RVec<float>& Tau_mass,
                  const ROOT::RVec<int>& Tau_charge, const ROOT::RVec<int>& Tau_idDeepTau2017v2p1VSjet,
                  const ROOT::RVec<int>& Tau_idDeepTau2017v2p1VSe, const ROOT::RVec<int>& Tau_idDeepTau2017v2p1VSmu,
                  float MET_pt, float MET_phi, const ROOT::RVec<float>& Jet_pt,
                  const ROOT::RVec<float>& Jet_eta, const ROOT::RVec<float>& Jet_phi,
                  const ROOT::RVec<float>& Jet_mass, const ROOT::RVec<float>& Jet_btagDeepB,
                  const vector<vector<int>>& lum) {
    if (!passLuminositySelection(run, luminosityBlock, lum)) return false;
    if (Tau_pt.size() < 2) return false;
    TLorentzVector tau0, tau1;
    tau0.SetPtEtaPhiM(Tau_pt[0], Tau_eta[0], Tau_phi[0], Tau_mass[0]);
    tau1.SetPtEtaPhiM(Tau_pt[1], Tau_eta[1], Tau_phi[1], Tau_mass[1]);
    TLorentzVector system = tau0 + tau1;
    double deltaPhi = fabs(tau1.Phi() - tau0.Phi());
    if (deltaPhi > M_PI) deltaPhi -= 2 * M_PI;
    double acoplanarity = fabs(deltaPhi) / M_PI;
    int btagCount = 0;
    for (const auto& btag : Jet_btagDeepB) {
        if (btag > 0.4506) btagCount++;
    }
    return true;
}

int main() {
    ROOT::EnableImplicitMT();
    auto lum = loadLuminosityData("dadosluminosidade.txt");
    string inputFileName = "root://cms-xrd-global.cern.ch//store/mc/RunIISummer20UL18NanoAODv9/TTJets_TuneCP5_13TeV-amcatnloFXFX-pythia8/NANOAODSIM/106X_upgrade2018_realistic_v16_L1v1-v1/2520000/028FE21B-A107-4347-92C3-B533907C13DE.root";
    string outputFileName = "output.root";
    const Long64_t nEvents = 20000;
    ROOT::RDataFrame df("Events", inputFileName);
    auto df_limited = df.Range(0, nEvents);
    auto filtered = df_limited.Filter([&](unsigned int run, unsigned int luminosityBlock,
                                          const ROOT::RVec<float>& Tau_pt, const ROOT::RVec<float>& Tau_eta,
                                          const ROOT::RVec<float>& Tau_phi, const ROOT::RVec<float>& Tau_mass,
                                          const ROOT::RVec<int>& Tau_charge, const ROOT::RVec<int>& Tau_idDeepTau2017v2p1VSjet,
                                          const ROOT::RVec<int>& Tau_idDeepTau2017v2p1VSe, const ROOT::RVec<int>& Tau_idDeepTau2017v2p1VSmu,
                                          float MET_pt, float MET_phi, const ROOT::RVec<float>& Jet_pt,
                                          const ROOT::RVec<float>& Jet_eta, const ROOT::RVec<float>& Jet_phi,
                                          const ROOT::RVec<float>& Jet_mass, const ROOT::RVec<float>& Jet_btagDeepB) {
        return processEvent(run, luminosityBlock, Tau_pt, Tau_eta, Tau_phi, Tau_mass,
                            Tau_charge, Tau_idDeepTau2017v2p1VSjet, Tau_idDeepTau2017v2p1VSe,
                            Tau_idDeepTau2017v2p1VSmu, MET_pt, MET_phi, Jet_pt, Jet_eta,
                            Jet_phi, Jet_mass, Jet_btagDeepB, lum);
    }, {"run", "luminosityBlock", "Tau_pt", "Tau_eta", "Tau_phi", "Tau_mass", "Tau_charge",
        "Tau_idDeepTau2017v2p1VSjet", "Tau_idDeepTau2017v2p1VSe", "Tau_idDeepTau2017v2p1VSmu",
        "MET_pt", "MET_phi", "Jet_pt", "Jet_eta", "Jet_phi", "Jet_mass", "Jet_btagDeepB"});
    auto result = filtered.Snapshot("tree", outputFileName);
    return 0;
}
