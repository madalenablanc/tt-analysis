#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "TFile.h"
#include "TTree.h"
#include "TLorentzVector.h"

void check_protons(TTree *tree);

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Usage: %s <input ROOT file> <output ROOT file> [options]\n", argv[0]);
        return 1;
    }
    
    char *input_file = argv[1];
    char *output_file = argv[2];
    int apply_id_filter = 0, apply_charge_filter = 0, apply_pt_filter = 0, apply_eta_filter = 0;
    
    for (int i = 3; i < argc; i++) {
        if (strcmp(argv[i], "--apply_id_filter") == 0) apply_id_filter = 1;
        else if (strcmp(argv[i], "--apply_charge_filter") == 0) apply_charge_filter = 1;
        else if (strcmp(argv[i], "--apply_pt_filter") == 0) apply_pt_filter = 1;
        else if (strcmp(argv[i], "--apply_eta_filter") == 0) apply_eta_filter = 1;
    }
    
    TFile *file = TFile::Open(input_file, "READ");
    if (!file) {
        printf("Error opening file: %s\n", input_file);
        return 1;
    }
    
    TTree *tree = (TTree*) file->Get("Events");
    if (!tree) {
        printf("Error reading tree from file: %s\n", input_file);
        return 1;
    }
    
    // Create output file
    TFile output(output_file, "RECREATE");
    TTree out_tree("tree", "Processed Data");
    
    double tau0_pt, tau1_pt, tau0_eta, tau1_eta, tau0_phi, tau1_phi;
    int tau0_charge, tau1_charge, tau0_id, tau1_id;
    double invariant_mass_tt_pair;
    
    out_tree.Branch("tau0_pt", &tau0_pt, "tau0_pt/D");
    out_tree.Branch("tau1_pt", &tau1_pt, "tau1_pt/D");
    out_tree.Branch("tau0_eta", &tau0_eta, "tau0_eta/D");
    out_tree.Branch("tau1_eta", &tau1_eta, "tau1_eta/D");
    out_tree.Branch("tau0_phi", &tau0_phi, "tau0_phi/D");
    out_tree.Branch("tau1_phi", &tau1_phi, "tau1_phi/D");
    out_tree.Branch("tau0_charge", &tau0_charge, "tau0_charge/I");
    out_tree.Branch("tau1_charge", &tau1_charge, "tau1_charge/I");
    out_tree.Branch("tau0_id", &tau0_id, "tau0_id/I");
    out_tree.Branch("tau1_id", &tau1_id, "tau1_id/I");
    out_tree.Branch("invariant_mass_tt_pair", &invariant_mass_tt_pair, "invariant_mass_tt_pair/D");
    
    for (int i = 0; i < tree->GetEntries(); i++) {
        tree->GetEntry(i);
        
        tau0_pt = tree->GetLeaf("Tau_pt")->GetValue(0);
        tau1_pt = tree->GetLeaf("Tau_pt")->GetValue(1);
        tau0_eta = tree->GetLeaf("Tau_eta")->GetValue(0);
        tau1_eta = tree->GetLeaf("Tau_eta")->GetValue(1);
        tau0_phi = tree->GetLeaf("Tau_phi")->GetValue(0);
        tau1_phi = tree->GetLeaf("Tau_phi")->GetValue(1);
        tau0_charge = (int)tree->GetLeaf("Tau_charge")->GetValue(0);
        tau1_charge = (int)tree->GetLeaf("Tau_charge")->GetValue(1);
        tau0_id = (int)tree->GetLeaf("Tau_idDeepTau2017v2p1VSjet")->GetValue(0);
        tau1_id = (int)tree->GetLeaf("Tau_idDeepTau2017v2p1VSjet")->GetValue(1);
        
        double cosh_delta_eta = cosh(tau1_eta - tau0_eta);
        double delta_phi = fabs(tau1_phi - tau0_phi);
        if (delta_phi > M_PI) {
            delta_phi -= 2 * M_PI;
        }
        invariant_mass_tt_pair = sqrt(2 * tau0_pt * tau1_pt * (cosh_delta_eta - cos(delta_phi)));
        
        // Apply filters
        if (apply_id_filter && (tau0_id < 1 || tau1_id < 1)) continue;
        if (apply_charge_filter && (tau0_charge + tau1_charge != 0)) continue;
        if (apply_pt_filter && (tau0_pt < 20 || tau1_pt < 20)) continue;
        if (apply_eta_filter && (fabs(tau0_eta) > 2.3 || fabs(tau1_eta) > 2.3)) continue;
        
        out_tree.Fill();
    }
    
    output.Write();
    output.Close();
    file->Close();
    printf("Processing complete. Output saved to: %s\n", output_file);
    return 0;
}


void check_protons(TTree *tree){
    cout << "I just got executed!";
}