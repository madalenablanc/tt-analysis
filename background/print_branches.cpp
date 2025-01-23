#include <iostream>
#include <TFile.h>
#include <TTree.h>
#include <TObjArray.h>
#include <TBranch.h>
#include <TCanvas.h>
#include <fstream>

using namespace std;

int main(int argc, char** argv) {


    // Open the ROOT file
    // const char* filename = "/eos/user/m/mblancco/samples_2018_tautau/fase0_2/ttJetscode_GammaGammaTauTau_SignalMC_SM_18UL_23k_NANOAODSIM_fase0_with_xi_and_deltaphi.root";
    const char* filename="root:://cms-xrd-global.cern.ch///store/mc/RunIISummer20UL18NanoAODv9/TTJets_TuneCP5_13TeV-amcatnloFXFX-pythia8/NANOAODSIM/106X_upgrade2018_realistic_v16_L1v1-v1/2520000/028FE21B-A107-4347-92C3-B533907C13DE.root";
    TFile* file = TFile::Open(filename);

    // Check if the file was successfully opened
    if (!file || file->IsZombie()) {
        std::cerr << "Error: Could not open file " << filename << std::endl;
        return 1;
    }
    std::cout << "Successfully opened file: " << filename << std::endl;

    // Get the TTree (you may need to replace "tree_name" with the actual tree name)
    TTree* tree = (TTree*)file->Get("Events");
    if (!tree) {
        std::cerr << "Error: Could not find TTree in file " << filename << std::endl;
        file->Close();
        return 1;
    }
    std::cout << "Successfully loaded TTree: " << tree->GetName() << std::endl;

    // Print the number of entries in the tree
    std::cout << "Number of entries in the tree: " << tree->GetEntries() << std::endl;


    ofstream myfile;
    myfile.open ("branches_fase0_2.txt");

    // Print branch names
    std::cout << "Branches in the tree:" << std::endl;
    TObjArray* branches = tree->GetListOfBranches();
    for (int i = 0; i < branches->GetEntries(); ++i) {
        std::cout << "- " << branches->At(i)->GetName() << std::endl;
        myfile << branches->At(i)->GetName()<<"\n";
    }

    
    
    myfile.close();

    //prints
    //  TCanvas* canvas = new TCanvas("canvas", "Basic Plots", 800, 600);
    // tree->Draw("tau0_id1");
    // canvas->SaveAs("plot1.png");

    // Close the ROOT file
    file->Close();
    std::cout << "File closed successfully." << std::endl;

    return 0;
}
