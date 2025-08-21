#include <stdio.h>
#include <iostream>
#include <cmath>
#include "TApplication.h"
#include "TFile.h"
#include "TTree.h"
#include "TLeaf.h"
#include "TH1D.h"
#include "TLorentzVector.h"
#include <string>
#include <fstream>
#include <cstdlib>
#include <sstream>

using namespace std;

int main(){

    string prefix = "root:://cms-xrd-global.cern.ch//";
    string input;

    string prefix_output = "Data_2018_UL_skimmed_MuTau_nano_";
    int k = 0;

    double weight = 1.;

    // int numero_linha;
    // cin >> numero_linha;

    // stringstream ss;
    // ss << numero_linha;

    // string out_put;
    // ss >> out_put;

    // // adjust the output path if you prefer a different folder
    string out_put="fase1total_2018";
    string output_tot = "/eos/user/m/mblancco/samples_2018_mutau/" + prefix_output + out_put + ".root";

    // ifstream ifile;
    // ifile.open("data_2018_UL.txt"); // change to a MuTau-specific list if you have one
    // while (k < numero_linha) {
    //     ifile >> input;
    //     k++;
    // }

    // entries
    double entry_1,entry_2,entry_3,entry_4,entry_5,entry_6,entry_7,entry_8,entry_9,entry_10,
           entry_11,entry_12,entry_13,entry_14,entry_15,entry_16,entry_17,entry_18,entry_19,entry_20,
           entry_21,entry_22,entry_23,entry_24,entry_25,entry_26,entry_27,entry_28, entry_30;
    int entry_29;

    // counters (mostly for printout)
    int nmutau=0, nid=0, ncharge=0, npt=0, neta=0;

    ////////////////////////////////////////
    TLorentzVector tau, muon, sistema;

    string total = "/eos/cms/store/user/jjhollar/TauTau_NanoAOD_Madalena/BackgroundSamples/MuTau/Dados_2018_UL_skimmed_MuTau_nano_fase0_merge.root";
    cout << "Input file: " << total << endl;

    TApplication app("app", NULL, NULL);

    TFile *f = TFile::Open(total.c_str());
    TTree *tree = (TTree*) f->Get("analyzer/ntp1");


    // (QCD-only use of LuminosityBlocks in your note — not used here)
    // TTree *tree_lumi = (TTree*) f->Get("LuminosityBlocks");

    TFile output(output_tot.c_str(), "RECREATE", "");
    TTree out("tree","");

    // Branches — μ and τ
    out.Branch("muon_id",   &entry_1 , "mu_id/D");
    out.Branch("tau_id1",   &entry_2 , "tau_id1/D"); // DeepTau VSjet
    out.Branch("tau_id2",   &entry_3 , "tau_id2/D"); // DeepTau VSe
    out.Branch("tau_id3",   &entry_4 , "tau_id3/D"); // DeepTau VSmu
    out.Branch("muon_pt",   &entry_5 , "mu_pt/D");
    out.Branch("tau_pt",    &entry_6 , "tau_pt/D");
    out.Branch("muon_charge",&entry_7, "mu_charge/D");
    out.Branch("tau_charge",&entry_8 , "tau_charge/D");
    out.Branch("muon_eta",  &entry_9 , "mu_eta/D");
    out.Branch("tau_eta",   &entry_10, "tau_eta/D");
    out.Branch("muon_n",    &entry_11, "mu_n/D");
    out.Branch("tau_n",     &entry_12, "tau_n/D");
    out.Branch("muon_phi",  &entry_13, "mu_phi/D");
    out.Branch("tau_phi",   &entry_14, "tau_phi/D");
    out.Branch("muon_mass", &entry_15, "mu_mass/D");
    out.Branch("tau_mass",  &entry_16, "tau_mass/D");

    // system & MET
    out.Branch("sist_mass", &entry_17, "sist_mass/D");
    out.Branch("sist_acop", &entry_18, "sist_acop/D");
    out.Branch("sist_pt",   &entry_19, "sist_pt/D");
    out.Branch("sist_rap",  &entry_20, "sist_rap/D");
    out.Branch("met_pt",    &entry_21, "met_pt/D");
    out.Branch("met_phi",   &entry_22, "met_phi/D");

    // jets
    out.Branch("jet_pt",    &entry_23, "jet_pt/D");
    out.Branch("jet_eta",   &entry_24, "jet_eta/D");
    out.Branch("jet_phi",   &entry_25, "jet_phi/D");
    out.Branch("jet_mass",  &entry_26, "jet_mass/D");
    out.Branch("jet_btag",  &entry_27, "jet_btag/D");
    out.Branch("weight",    &entry_28, "weight/D");
    out.Branch("n_b_jet",   &entry_29, "n_b_jet/I");
    out.Branch("generator_weight", &entry_30, "generator_weight/D");

    // TH1D histo("histo","histo", 1000, 0, 1000);

    const double deepB_wp = 0.4506; // same as your other code

    for (int i=0; i<tree->GetEntries(); i++) {

        tree->GetEvent(i);
        if (i%1000==0) cout << "progress: " << double(i)/tree->GetEntries()*100.0 << endl;

        // Trigger: single-muon
        if (tree->GetLeaf("HLT_IsoMu24")->GetValue(0) == 1) {

            // object presence + IDs (mirror your QCD selection)
            bool hasMuon = tree->GetLeaf("Muon_pt")->GetLen() > 0;
            bool hasTau  = tree->GetLeaf("Tau_pt")->GetLen()  > 0;

            if (hasMuon && hasTau &&
                tree->GetLeaf("Muon_mvaId")->GetValue(0)                >= 3 &&
                tree->GetLeaf("Tau_idDeepTau2017v2p1VSjet")->GetValue(0) >= 63 &&
                tree->GetLeaf("Tau_idDeepTau2017v2p1VSe")->GetValue(0)   >= 7 &&
                tree->GetLeaf("Tau_idDeepTau2017v2p1VSmu")->GetValue(0)  >= 1)
            {
                nmutau++;

                // read first muon and first tau
                entry_1  = tree->GetLeaf("Muon_mvaId")->GetValue(0);
                entry_2  = tree->GetLeaf("Tau_idDeepTau2017v2p1VSjet")->GetValue(0);
                entry_3  = tree->GetLeaf("Tau_idDeepTau2017v2p1VSe")->GetValue(0);
                entry_4  = tree->GetLeaf("Tau_idDeepTau2017v2p1VSmu")->GetValue(0);
                entry_5  = tree->GetLeaf("Muon_pt")->GetValue(0);
                entry_6  = tree->GetLeaf("Tau_pt")->GetValue(0);
                entry_7  = tree->GetLeaf("Muon_charge")->GetValue(0);
                entry_8  = tree->GetLeaf("Tau_charge")->GetValue(0);
                entry_9  = tree->GetLeaf("Muon_eta")->GetValue(0);
                entry_10 = tree->GetLeaf("Tau_eta")->GetValue(0);
                entry_11 = tree->GetLeaf("Muon_pt")->GetLen();
                entry_12 = tree->GetLeaf("Tau_pt")->GetLen();
                entry_13 = tree->GetLeaf("Muon_phi")->GetValue(0);
                entry_14 = tree->GetLeaf("Tau_phi")->GetValue(0);
                entry_15 = tree->GetLeaf("Muon_mass")->GetValue(0);
                entry_16 = tree->GetLeaf("Tau_mass")->GetValue(0);

                muon.SetPtEtaPhiM(entry_5, entry_9, entry_13, entry_15);
                tau .SetPtEtaPhiM(entry_6, entry_10, entry_14, entry_16);
                sistema = muon + tau;

                double deltaphi = fabs(entry_14 - entry_13);
                if (deltaphi > M_PI) deltaphi -= 2*M_PI;
                entry_18 = fabs(deltaphi)/M_PI;  // acoplanarity
                entry_17 = sistema.M();
                entry_19 = sistema.Pt();
                entry_20 = sistema.Rapidity();

                // MET (fixed ordering)
                entry_21 = tree->GetLeaf("MET_pt")->GetValue(0);
                entry_22 = tree->GetLeaf("MET_phi")->GetValue(0);

                // leading jet + b-tag counting
                entry_23 = tree->GetLeaf("Jet_pt")->GetLen()  > 0 ? tree->GetLeaf("Jet_pt")->GetValue(0)   : -99.;
                entry_24 = tree->GetLeaf("Jet_eta")->GetLen() > 0 ? tree->GetLeaf("Jet_eta")->GetValue(0)  : -99.;
                entry_25 = tree->GetLeaf("Jet_phi")->GetLen() > 0 ? tree->GetLeaf("Jet_phi")->GetValue(0)  : -99.;
                entry_26 = tree->GetLeaf("Jet_mass")->GetLen()> 0 ? tree->GetLeaf("Jet_mass")->GetValue(0) : -99.;
                entry_27 = tree->GetLeaf("Jet_btagDeepB")->GetLen()>0 ? tree->GetLeaf("Jet_btagDeepB")->GetValue(0) : -99.;

                int btag = 0;
                int njets = tree->GetLeaf("Jet_btagDeepB")->GetLen();
                for (int j=0; j<njets; ++j) {
                    if (tree->GetLeaf("Jet_btagDeepB")->GetValue(j) > deepB_wp) btag++;
                }
                entry_29 = btag;

                // weights (data)
                entry_28 = weight;   // 1.
                entry_30 = 1.;       // generator_weight placeholder for data

                // optional counters you had before (left as-is)
                nid++; ncharge++; npt++; neta++;

                out.Fill();
            }
        }
    }

    cout << nmutau << endl;
    cout << nid << endl;
    cout << ncharge << endl;
    cout << npt << endl;
    cout << neta << endl;
    cout << -11111111111111 << endl;

    output.Write();
    // app.Run(true);

    return 0;
}
