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
#include "TCanvas.h"


using namespace std;

int main(){

    // Declare variables
    double tau0_pt, tau1_pt;
    double tau0_eta, tau1_eta;
    double tau0_phi, tau1_phi;
    double proton_xi1 = -1, proton_xi2 = -1;
    double invariant_mass;

    // Input file
    string input = "/eos/cms/store/user/jjhollar/TauTau_NanoAOD_Madalena/GammaGammaTauTau_SignalMC_SM_18UL_23k_NANOAODSIM.root";
    cout << "Input file: " << input << endl;

    // Setup ROOT application
    TApplication app("app", NULL, NULL);
    TFile *f = TFile::Open(input.c_str());
    TTree *tree = (TTree*) f->Get("Events");

    // Output file and tree
    TFile output("output_with_protons.root", "RECREATE", "");
    TTree out("tree","Output Tree with Protons and Taus");

    // Output branches
    out.Branch("tau0_pt", &tau0_pt, "tau0_pt/D");
    out.Branch("tau1_pt", &tau1_pt, "tau1_pt/D");
    out.Branch("tau0_eta", &tau0_eta, "tau0_eta/D");
    out.Branch("tau1_eta", &tau1_eta, "tau1_eta/D");
    out.Branch("tau0_phi", &tau0_phi, "tau0_phi/D");
    out.Branch("tau1_phi", &tau1_phi, "tau1_phi/D");
    out.Branch("proton_xi1", &proton_xi1, "proton_xi1/D");
    out.Branch("proton_xi2", &proton_xi2, "proton_xi2/D");
    out.Branch("invariant_mass", &invariant_mass, "invariant_mass/D");

    // Loop over events
    for(int i = 0; i < tree->GetEntries(); i++){
        tree->GetEvent(i);

        // Check if there are at least two taus
        if(tree->GetLeaf("Tau_pt")->GetLen() < 2) continue;

        // Get tau kinematics
        tau0_pt = tree->GetLeaf("Tau_pt")->GetValue(0);
        tau1_pt = tree->GetLeaf("Tau_pt")->GetValue(1);
        tau0_eta = tree->GetLeaf("Tau_eta")->GetValue(0);
        tau1_eta = tree->GetLeaf("Tau_eta")->GetValue(1);
        tau0_phi = tree->GetLeaf("Tau_phi")->GetValue(0);
        tau1_phi = tree->GetLeaf("Tau_phi")->GetValue(1);

        // Reset proton variables
        proton_xi1 = -1;
        proton_xi2 = -1;

        // Loop over all protons in the event
        for (int j = 0; j < tree->GetLeaf("Proton_multiRP_xi")->GetLen(); j++) {
            int arm = tree->GetLeaf("Proton_multiRP_arm")->GetValue(j);
            double xi = tree->GetLeaf("Proton_multiRP_xi")->GetValue(j);

            if (arm == 0 && xi > proton_xi1) {
                // Update the largest xi for Arm 0
                proton_xi1 = xi;
            } else if (arm == 1 && xi > proton_xi2) {
                // Update the largest xi for Arm 1
                proton_xi2 = xi;
            }
        }

        // Check if we have valid protons from both arms
        if (proton_xi1 > 0 && proton_xi2 > 0) {
            double s = 13000 * 13000;  // Center-of-mass energy squared
            invariant_mass = sqrt(s * proton_xi1 * proton_xi2);
            out.Fill();
        }


        // Progress output
        if (i % 1000 == 0) cout << "Progress: " << i << "/" << tree->GetEntries() << endl;
    }

     TCanvas *c1 = new TCanvas("c1", "Invariant Mass Plot", 800, 600);
    out.Draw("invariant_mass");
    c1->SaveAs("invariant_mass.png");

    // Write output
    output.Write();

    return 0;
}
