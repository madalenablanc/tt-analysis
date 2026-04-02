#include "TFile.h"
#include "TTree.h"
#include "TH1F.h"
#include "TLeaf.h"
#include "TString.h"
#include "TSystem.h"
#include "TMVA/Tools.h"
#include "TMVA/Reader.h"
#include <iostream>
#include <vector>

// Run BDT application on any sample
// Usage: root -l -b -q 'run_bdt_all.C("input.root", "output.root", "signal")'
// sampleType: "signal", "dy", "ttbar", "qcd", "data"

void run_bdt_all(TString inputFile, TString outputFile, TString sampleType)
{
    TMVA::Tools::Instance();

    // Determine sample properties
    bool useArrayXi   = (sampleType == "data" || sampleType == "qcd");
    bool useMuonPt    = (sampleType == "data" || sampleType == "qcd");

    std::cout << "=== BDT Application ===" << std::endl;
    std::cout << "Input:  " << inputFile << std::endl;
    std::cout << "Output: " << outputFile << std::endl;
    std::cout << "Sample: " << sampleType << std::endl;
    std::cout << "Array xi: " << useArrayXi << "  muon_pt: " << useMuonPt << std::endl;

    // Create TMVA Reader
    TMVA::Reader *reader = new TMVA::Reader("!Color:!Silent");

    // TMVA variables (must match training order exactly)
    Float_t var_rap, var_acop, var_pt, var_mupt, var_taupt;
    Float_t var_mass, var_massmatch, var_met, var_rapmatch;

    reader->AddVariable("sist_rap",       &var_rap);
    reader->AddVariable("acop",            &var_acop);
    reader->AddVariable("sist_pt",        &var_pt);
    reader->AddVariable("mu_pt",          &var_mupt);
    reader->AddVariable("tau_pt",         &var_taupt);
    reader->AddVariable("sist_mass",      &var_mass);
    reader->AddVariable("sist_mass - sqrt(13000.0*13000.0*xi_arm1_1*xi_arm2_1)", &var_massmatch);
    reader->AddVariable("met_pt",         &var_met);
    reader->AddVariable("sist_rap-0.5*log(xi_arm1_1/xi_arm2_1)", &var_rapmatch);

    // Book BDT method
    TString weightfile = "dataset/weights/TMVAClassification_BDT.weights.xml";
    reader->BookMVA("BDT method", weightfile);

    // Open input file
    TFile *input = TFile::Open(inputFile);
    if (!input || input->IsZombie()) {
        std::cout << "ERROR: could not open " << inputFile << std::endl;
        return;
    }

    TTree *tree = (TTree*)input->Get("tree");
    if (!tree) {
        std::cout << "ERROR: no tree found in " << inputFile << std::endl;
        return;
    }

    // Muon pT branch name differs between samples
    TString mupt_branch = useMuonPt ? "muon_pt" : "mu_pt";

    // For data/QCD: set up vector branches for proton arrays
    std::vector<int> *v_arm = nullptr;
    std::vector<float> *v_xi = nullptr;
    if (useArrayXi) {
        tree->SetBranchAddress("proton_multi_arm", &v_arm);
        tree->SetBranchAddress("proton_multi_xi",  &v_xi);
    }

    // Output histogram
    TH1F *histBdt = new TH1F("MVA_BDT", "MVA_BDT", 20, -0.8, 0.8);

    Long64_t nEntries = tree->GetEntries();
    std::cout << "Processing " << nEntries << " events..." << std::endl;

    int nPassed = 0;
    for (Long64_t i = 0; i < nEntries; i++) {
        if (i % 5000 == 0) std::cout << "  Event " << i << " / " << nEntries << std::endl;

        tree->GetEntry(i);

        // Read all variables via GetLeaf (avoids Float_t/Double_t type mismatch)
        double b_acop  = tree->GetLeaf("acop")->GetValue(0);
        double b_pt    = tree->GetLeaf("sist_pt")->GetValue(0);
        double b_mass  = tree->GetLeaf("sist_mass")->GetValue(0);
        double b_rap   = tree->GetLeaf("sist_rap")->GetValue(0);
        double b_met   = tree->GetLeaf("met_pt")->GetValue(0);
        double b_taupt = tree->GetLeaf("tau_pt")->GetValue(0);
        double b_mupt  = tree->GetLeaf(mupt_branch)->GetValue(0);
        double b_weight = tree->GetLeaf("weight")->GetValue(0);

        // Get xi values
        double xi1, xi2;
        if (useArrayXi) {
            xi1 = -1; xi2 = -1;
            if (v_arm && v_xi) {
                for (size_t j = 0; j < v_arm->size(); j++) {
                    if ((*v_arm)[j] == 0 && xi1 < 0) xi1 = (*v_xi)[j];
                    if ((*v_arm)[j] == 1 && xi2 < 0) xi2 = (*v_xi)[j];
                }
            }
            if (i == 0) {
                std::cout << "DEBUG: v_arm=" << v_arm << " v_xi=" << v_xi << std::endl;
                if (v_arm) std::cout << "DEBUG: v_arm size=" << v_arm->size() << std::endl;
                std::cout << "DEBUG: xi1=" << xi1 << " xi2=" << xi2 << std::endl;
            }
        } else {
            xi1 = tree->GetLeaf("xi_arm1_1")->GetValue(0);
            xi2 = tree->GetLeaf("xi_arm2_1")->GetValue(0);
        }

        // Skip events without protons on both arms
        if (xi1 <= 0 || xi2 <= 0) continue;
        nPassed++;

        // Fill TMVA variables
        var_acop    = b_acop;
        var_pt      = b_pt;
        var_mass    = b_mass;
        var_rap     = b_rap;
        var_met     = b_met;
        var_taupt   = b_taupt;
        var_mupt    = b_mupt;
        var_massmatch = b_mass - sqrt(13000.*13000.*xi1*xi2);
        var_rapmatch  = b_rap - 0.5*log(xi1/xi2);

        // Evaluate BDT
        double bdtScore = reader->EvaluateMVA("BDT method");
        histBdt->Fill(bdtScore, b_weight);
    }

    // Write output
    TFile *target = new TFile(outputFile, "RECREATE");
    histBdt->Write();
    target->Close();

    std::cout << "Events with protons on both arms: " << nPassed << " / " << nEntries << std::endl;
    std::cout << "BDT histogram entries: " << histBdt->GetEntries() << std::endl;
    std::cout << "Written to: " << outputFile << std::endl;

    delete reader;
    delete input;
}
