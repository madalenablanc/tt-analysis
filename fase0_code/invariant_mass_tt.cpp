#include <iostream>
#include "TFile.h"
#include "TTree.h"
#include "TH1D.h"
#include "TCanvas.h"
#include <cmath>

using namespace std;

int main() {
    // Open the ROOT file
    TFile *file = TFile::Open("/eos/cms/store/user/jjhollar/TauTau_NanoAOD_Madalena/GammaGammaTauTau_SignalMC_SM_18UL_23k_NANOAODSIM.root");
    if (!file || file->IsZombie()) {
        cout << "Error: Could not open the file!" << endl;
        return 1;
    }

    // Get the Events tree
    TTree *tree = (TTree*)file->Get("Events");
    if (!tree) {
        cout << "Error: Could not find the Events tree!" << endl;
        return 1;
    }

    // Declare variables to hold the tau kinematics
    double tau_pt[2], tau_eta[2], tau_phi[2];

    // Set the branch addresses
    tree->SetBranchAddress("Tau_pt", &tau_pt);
    tree->SetBranchAddress("Tau_eta", &tau_eta);
    tree->SetBranchAddress("Tau_phi", &tau_phi);

    // Create a histogram to store the invariant mass distribution
    TH1D *h_invariant_mass = new TH1D("h_invariant_mass", "Invariant Mass of Tau Pair;Mass (GeV);Events", 100, 0, 500);

    // Loop over events
    for (int i = 0; i < tree->GetEntries(); i++) {
        tree->GetEvent(i);

        // Get the tau kinematics from the arrays
        double tau0_pt = tau_pt[0];
        double tau1_pt = tau_pt[1];
        double tau0_eta = tau_eta[0];
        double tau1_eta = tau_eta[1];
        double tau0_phi = tau_phi[0];
        double tau1_phi = tau_phi[1];

        // Calculate the differences in azimuthal angle
        double delta_phi = fabs(tau0_phi - tau1_phi);
        if (delta_phi > M_PI) delta_phi -= 2 * M_PI;
        delta_phi = fabs(delta_phi);

        // Calculate cosh(delta_eta) directly using tau0_eta and tau1_eta
        double cosh_delta_eta = cosh(tau0_eta - tau1_eta);

        // Calculate the invariant mass using the transverse momenta formula
        double invariant_mass = sqrt(2 * tau0_pt * tau1_pt * (cosh_delta_eta - cos(delta_phi)));

        // Fill the histogram with the calculated invariant mass
        h_invariant_mass->Fill(invariant_mass);
    }

    // Save the histogram to an output file
    TFile *outputFile = new TFile("output_invariant_mass_tt.root", "RECREATE");
    h_invariant_mass->Write();
    outputFile->Close();

    // Create a canvas and draw the histogram
    TCanvas *c1 = new TCanvas("c1", "Tau Pair Invariant Mass Plot", 800, 600);
    h_invariant_mass->Draw();
    c1->SaveAs("invariant_mass_tt.png");

    // Close the input file
    file->Close();

    cout << "Invariant mass distribution saved to output_invariant_mass_tt.root and invariant_mass_tt.png" << endl;

    return 0;
}
