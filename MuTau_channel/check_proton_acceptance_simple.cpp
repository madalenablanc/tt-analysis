/*
 * Simple C++ macro to check proton acceptance from data
 *
 * Usage in ROOT:
 *   root -l check_proton_acceptance_simple.cpp
 *
 * This calculates what fraction of events have at least 1 proton on each arm.
 */

#include <iostream>
#include "TFile.h"
#include "TTree.h"
#include "TLeaf.h"

void check_proton_acceptance_simple() {

    // Path to your data file with proton variables
    const char* data_file = "/eos/user/m/mblancco/samples_2018_mutau/Data_2018_UL_MuTau_nano_merged_proton_vars.root";

    std::cout << "========================================" << std::endl;
    std::cout << "Proton Acceptance Calculator" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "\nOpening: " << data_file << std::endl;

    TFile* f = TFile::Open(data_file);
    if (!f || f->IsZombie()) {
        std::cerr << "ERROR: Cannot open file!" << std::endl;
        return;
    }

    TTree* tree = (TTree*)f->Get("tree");
    if (!tree) {
        std::cerr << "ERROR: Cannot find tree!" << std::endl;
        return;
    }

    Long64_t total_events = tree->GetEntries();
    std::cout << "Total events: " << total_events << std::endl;

    // Counters
    Long64_t events_with_both_arms = 0;
    Long64_t no_protons = 0;
    Long64_t only_arm0 = 0;
    Long64_t only_arm1 = 0;
    Long64_t both_arms = 0;

    std::cout << "\nProcessing events..." << std::endl;

    // Loop over all events
    for (Long64_t i = 0; i < total_events; i++) {
        tree->GetEntry(i);

        // Progress
        if (i % 100000 == 0 && i > 0) {
            std::cout << "  Processed " << i << " / " << total_events
                      << " (" << (100.0 * i / total_events) << "%)" << std::endl;
        }

        // Get the proton arm information
        // Try different possible branch names
        TLeaf* leaf_arm = tree->GetLeaf("proton_multi_arm");
        if (!leaf_arm) leaf_arm = tree->GetLeaf("Proton_multiRP_arm");

        if (!leaf_arm) {
            if (i == 0) {
                std::cerr << "ERROR: Cannot find proton arm branch!" << std::endl;
                std::cout << "\nAvailable branches:" << std::endl;
                TObjArray* branches = tree->GetListOfBranches();
                for (int j = 0; j < branches->GetEntries(); j++) {
                    TBranch* br = (TBranch*)branches->At(j);
                    TString name = br->GetName();
                    if (name.Contains("proton", TString::kIgnoreCase)) {
                        std::cout << "  - " << name << std::endl;
                    }
                }
            }
            break;
        }

        // Check which arms have protons
        bool has_arm0 = false;
        bool has_arm1 = false;

        Int_t n_protons = leaf_arm->GetLen();
        for (Int_t j = 0; j < n_protons; j++) {
            Int_t arm = (Int_t)leaf_arm->GetValue(j);
            if (arm == 0) has_arm0 = true;
            if (arm == 1) has_arm1 = true;
        }

        // Count categories
        if (!has_arm0 && !has_arm1) {
            no_protons++;
        } else if (has_arm0 && !has_arm1) {
            only_arm0++;
        } else if (!has_arm0 && has_arm1) {
            only_arm1++;
        } else {
            both_arms++;
            events_with_both_arms++;
        }
    }

    // Calculate acceptance
    double acceptance = (double)events_with_both_arms / (double)total_events;

    // Print results
    std::cout << "\n========================================" << std::endl;
    std::cout << "RESULTS" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "\nTotal events analyzed: " << total_events << std::endl;
    std::cout << "Events with ≥1 proton on both arms: " << events_with_both_arms << std::endl;
    std::cout << "\nProton acceptance P = " << acceptance
              << " (" << (acceptance * 100.0) << "%)" << std::endl;

    std::cout << "\n--- Detailed Breakdown ---" << std::endl;
    std::cout << "No protons on either arm:     " << no_protons
              << " (" << (100.0 * no_protons / total_events) << "%)" << std::endl;
    std::cout << "Protons only on arm 0:        " << only_arm0
              << " (" << (100.0 * only_arm0 / total_events) << "%)" << std::endl;
    std::cout << "Protons only on arm 1:        " << only_arm1
              << " (" << (100.0 * only_arm1 / total_events) << "%)" << std::endl;
    std::cout << "Protons on BOTH arms:         " << both_arms
              << " (" << (100.0 * both_arms / total_events) << "%)" << std::endl;

    std::cout << "\n========================================" << std::endl;
    std::cout << "INTERPRETATION" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "\nThe measured proton acceptance is P = " << acceptance << std::endl;
    std::cout << "\nThis is the factor that should be used as FIXED_WEIGHT in merge_pp_mutau.py" << std::endl;

    if (fabs(acceptance - 0.13) < 0.03) {
        std::cout << "✓ Close to Matteo's value of 0.13" << std::endl;
    } else if (fabs(acceptance - 0.20) < 0.03) {
        std::cout << "✓ Close to Jonathan's estimate of ~0.20" << std::endl;
    } else {
        std::cout << "⚠ Different from both 0.13 and 0.20 - may depend on event selection" << std::endl;
    }

    std::cout << "\nNote: The exact value depends on triggers, cuts, and data period used." << std::endl;

    f->Close();
}
