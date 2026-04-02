#include "TFile.h"
#include "TTree.h"
#include "TLeaf.h"
#include <iostream>

// Merge DY, ttbar, QCD into a single background file for TMVA training.
// Harmonizes branch names to match what TMVAClassification.C expects:
//   sist_acop, sist_pt, sist_mass, sist_rap, met_pt, mu_pt, tau_pt,
//   xi_arm1_1, xi_arm2_1, weight
//
// Usage: root -l -b -q merge_backgrounds.C

void merge_backgrounds()
{
    // Input files
    TFile *f_dy     = TFile::Open("/eos/home-m/mblancco/samples_2018_mutau/DY_2018_UL_MuTau_nano_merged_pileup_protons.root");
    TFile *f_ttbar  = TFile::Open("/eos/home-m/mblancco/samples_2018_mutau/ttjets_2018_UL_MuTau_nano_merged_pileup_protons.root");
    TFile *f_qcd    = TFile::Open("/eos/home-m/mblancco/samples_2018_mutau/QCD_2018_UL_MuTau_nano_merged_pileup_protons.root");

    TTree *t_dy     = (TTree*)f_dy->Get("tree");
    TTree *t_ttbar  = (TTree*)f_ttbar->Get("tree");
    TTree *t_qcd    = (TTree*)f_qcd->Get("tree");

    // Output file
    TFile *fout = new TFile("background_total-protons_syst.root", "RECREATE");
    TTree *tout = new TTree("tree", "merged background");

    // Output branches (uniform names)
    Double_t o_acop, o_pt, o_mass, o_rap, o_met, o_mupt, o_taupt;
    Double_t o_xi1, o_xi2, o_weight;

    tout->Branch("acop",       &o_acop,  "acop/D");
    tout->Branch("sist_pt",    &o_pt,    "sist_pt/D");
    tout->Branch("sist_mass",  &o_mass,  "sist_mass/D");
    tout->Branch("sist_rap",   &o_rap,   "sist_rap/D");
    tout->Branch("met_pt",     &o_met,   "met_pt/D");
    tout->Branch("mu_pt",      &o_mupt,  "mu_pt/D");
    tout->Branch("tau_pt",     &o_taupt, "tau_pt/D");
    tout->Branch("xi_arm1_1",  &o_xi1,   "xi_arm1_1/D");
    tout->Branch("xi_arm2_1",  &o_xi2,   "xi_arm2_1/D");
    tout->Branch("weight",     &o_weight, "weight/D");

    int nfilled = 0;

    // Helper: copy one input tree into the output tree, renaming acop -> sist_acop
    auto copyTree = [&](TTree *tin, const char *label) {
        Long64_t n = tin->GetEntries();
        std::cout << "Processing " << label << " (" << n << " events)..." << std::endl;
        int count = 0;
        for (Long64_t i = 0; i < n; i++) {
            tin->GetEntry(i);
            o_acop   = tin->GetLeaf("acop")->GetValue(0);
            o_pt     = tin->GetLeaf("sist_pt")->GetValue(0);
            o_mass   = tin->GetLeaf("sist_mass")->GetValue(0);
            o_rap    = tin->GetLeaf("sist_rap")->GetValue(0);
            o_met    = tin->GetLeaf("met_pt")->GetValue(0);
            o_mupt   = tin->GetLeaf("mu_pt")->GetValue(0);
            o_taupt  = tin->GetLeaf("tau_pt")->GetValue(0);
            o_xi1    = tin->GetLeaf("xi_arm1_1")->GetValue(0);
            o_xi2    = tin->GetLeaf("xi_arm2_1")->GetValue(0);
            o_weight = tin->GetLeaf("weight")->GetValue(0);
            tout->Fill();
            count++;
        }
        std::cout << "  " << label << ": " << count << " events copied" << std::endl;
        nfilled += count;
    };

    copyTree(t_dy,    "DY");
    copyTree(t_ttbar, "ttbar");
    copyTree(t_qcd,   "QCD");

    // Write and close
    fout->cd();
    tout->Write();
    fout->Close();

    std::cout << std::endl;
    std::cout << "Total background events: " << nfilled << std::endl;
    std::cout << "Written to: background_total-protons_syst.root" << std::endl;
}
