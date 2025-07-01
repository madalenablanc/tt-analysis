#include <iostream>
#include <cmath>
#include "TApplication.h"
#include "THStack.h"
#include "TH1D.h"
#include "TCanvas.h"
#include "TFile.h"
#include "TLeaf.h"
#include "TTree.h"
#include "TPad.h"
#include "TLegend.h"
#include "TF1.h"
#include "TLatex.h"
#include "TASImage.h"
#include "TStyle.h"
#include "TPaveText.h"
#include "TLine.h"
#include "TSystem.h"

using namespace std;

// Function to create ratio plot
TH1D* CreateRatio(TH1D* data, THStack* mc_stack, const char* name) {
    // Get the total MC histogram
    TH1D* mc_total = (TH1D*)mc_stack->GetStack()->Last()->Clone();
    
    // Create ratio histogram
    TH1D* ratio = (TH1D*)data->Clone(name);
    ratio->Divide(mc_total);
    
    // Style the ratio plot
    ratio->SetMarkerStyle(20);
    ratio->SetMarkerSize(0.8);
    ratio->SetLineWidth(1);
    ratio->GetYaxis()->SetTitle("Data/Simulation");
    ratio->GetYaxis()->SetTitleSize(0.12);
    ratio->GetYaxis()->SetLabelSize(0.10);
    ratio->GetYaxis()->SetTitleOffset(0.5);
    ratio->GetYaxis()->SetRangeUser(0.0, 2.0);
    ratio->GetYaxis()->SetNdivisions(505);
    
    ratio->GetXaxis()->SetTitleSize(0.12);
    ratio->GetXaxis()->SetLabelSize(0.10);
    ratio->GetXaxis()->SetTitleOffset(1.0);
    
    return ratio;
}

// Function to create uncertainty band
TH1D* CreateUncertaintyBand(THStack* mc_stack, const char* name) {
    TH1D* mc_total = (TH1D*)mc_stack->GetStack()->Last()->Clone(name);
    
    // Set statistical uncertainties
    for (int i = 1; i <= mc_total->GetNbinsX(); ++i) {
        double content = mc_total->GetBinContent(i);
        double error = sqrt(content); // Poisson error
        mc_total->SetBinError(i, error);
    }
    
    mc_total->SetFillColor(kGray+1);
    mc_total->SetFillStyle(3354); // Hatched pattern
    mc_total->SetLineColor(kGray+1);
    
    return mc_total;
}

int coutBJets2(TTree* tree) {
    int btag=0;

		for(int j=0; j<tree->GetLeaf("Jet_btagDeepB")->GetLen(); j++){

			if(tree->GetLeaf("Jet_btagDeepB")->GetValue(j)>0.4506) btag++; //DeepCSV b+bb tag discriminator

		}
        return btag;

}


int countBJets(TTree* tree, double btagThreshold = 0.5) {
    int nBJets = 0;
    
    // Check if jet variables exist
    TLeaf* jet_pt_leaf = tree->GetLeaf("jet_pt");
    TLeaf* jet_btag_leaf = tree->GetLeaf("jet_btag");
    
    if(!jet_pt_leaf || !jet_btag_leaf) {
        return 0; // No jet info available
    }
    
    // Get number of jets in this event
    int nJets = jet_pt_leaf->GetLen(); // Length of the array
    
    for(int j = 0; j < nJets; j++) {
        double jet_pt = jet_pt_leaf->GetValue(j);
        double jet_btag = jet_btag_leaf->GetValue(j);
        
        // Apply basic jet selection and b-tagging
        if(jet_pt > 20.0 && jet_btag > btagThreshold) {  // Adjust thresholds as needed
            nBJets++;
        }
    }
    
    return nBJets;
}

int main(){

    TApplication app("app",NULL,NULL);

    // Open files
    TFile data("/eos/cms/store/user/jjhollar/TauTau_NanoAOD_Madalena/BackgroundSamples/TauhTauh/Dados_2018_UL_skimmed_TauTau_nano_fase1total-protons_2018.root");
    TFile dy("/eos/cms/store/user/jjhollar/TauTau_NanoAOD_Madalena/BackgroundSamples/TauhTauh/DY_2018_UL_skimmed_TauTau_nano_fase1total-protons_2018.root");
    TFile ttjets("/eos/cms/store/user/jjhollar/TauTau_NanoAOD_Madalena/BackgroundSamples/TauhTauh/ttJets_2018_UL_skimmed_TauTau_nano_fase1total-protons_2018.root");
    TFile qcd("/eos/cms/store/user/jjhollar/TauTau_NanoAOD_Madalena/BackgroundSamples/TauhTauh/QCD_2018_UL_skimmed_TauTau_nano_fase1total-protons_2018.root");
    TFile sinal("TauTau_sinal_PIC_july_2018.root");

    // Control Region parameters - MATCHED to reference plots exactly
    
    // DY Control Region parameters (plots tau_mass, range ~70-100 GeV from visual inspection)
    double min_aco_DY=0.05, max_aco_DY=0.25; int bin_aco_DY=5;
    double min_m_DY=70, max_m_DY=100; int bin_m_DY=5;  // tau_mass range from plot
    double min_r_DY=-2, max_r_DY=2; int bin_r_DY=5;
    double min_pt_DY=200, max_pt_DY=350; int bin_pt_DY=10;

    // QCD Control Region parameters (plots sist_mass, range ~400-1000 GeV from visual inspection)  
    double min_aco_QCD=0.8, max_aco_QCD=1.0; int bin_aco_QCD=6;
    double min_m_QCD=100, max_m_QCD=500; int bin_m_QCD=6;  // sist_mass range from plot
    double min_r_QCD=-2, max_r_QCD=2; int bin_r_QCD=10;
    double min_pt_QCD=0, max_pt_QCD=100; int bin_pt_QCD=5;

    // TT Control Region parameters (plots sist_mass, range ~400-1000 GeV from visual inspection)
    double min_aco_TT=0.5, max_aco_TT=1.0; int bin_aco_TT=10;
    double min_m_TT=400, max_m_TT=1000; int bin_m_TT=10;  // sist_mass range from plot
    double min_r_TT=-2, max_r_TT=2; int bin_r_TT=10;
    double min_pt_TT=0, max_pt_TT=240; int bin_pt_TT=5;

    // Create histograms
    TH1D h_aco_data_DY("h_aco_data_DY","",bin_aco_DY, min_aco_DY, max_aco_DY);
    TH1D h_aco_dy_DY("h_aco_dy_DY","",bin_aco_DY, min_aco_DY, max_aco_DY);
    TH1D h_aco_qcd_DY("h_aco_qcd_DY","",bin_aco_DY, min_aco_DY, max_aco_DY);
    TH1D h_aco_ttjets_DY("h_aco_ttjets_DY","",bin_aco_DY, min_aco_DY, max_aco_DY);

    TH1D h_m_data_DY("h_m_data_DY","",bin_m_DY, min_m_DY, max_m_DY);
    TH1D h_m_dy_DY("h_m_dy_DY","",bin_m_DY, min_m_DY, max_m_DY);
    TH1D h_m_qcd_DY("h_m_qcd_DY","",bin_m_DY, min_m_DY, max_m_DY);
    TH1D h_m_ttjets_DY("h_m_ttjets_DY","",bin_m_DY, min_m_DY, max_m_DY);

    TH1D h_r_data_DY("h_r_data_DY","",bin_r_DY, min_r_DY, max_r_DY);
    TH1D h_r_dy_DY("h_r_dy_DY","",bin_r_DY, min_r_DY, max_r_DY);
    TH1D h_r_qcd_DY("h_r_qcd_DY","",bin_r_DY, min_r_DY, max_r_DY);
    TH1D h_r_ttjets_DY("h_r_ttjets_DY","",bin_r_DY, min_r_DY, max_r_DY);

    TH1D h_pt_data_DY("h_pt_data_DY","",bin_pt_DY, min_pt_DY, max_pt_DY);
    TH1D h_pt_dy_DY("h_pt_dy_DY","",bin_pt_DY, min_pt_DY, max_pt_DY);
    TH1D h_pt_qcd_DY("h_pt_qcd_DY","",bin_pt_DY, min_pt_DY, max_pt_DY);
    TH1D h_pt_ttjets_DY("h_pt_ttjets_DY","",bin_pt_DY, min_pt_DY, max_pt_DY);

    // QCD CONTROL REGION HISTOGRAMS
    TH1D h_aco_data_QCD("h_aco_data_QCD","",bin_aco_QCD, min_aco_QCD, max_aco_QCD);
    TH1D h_aco_dy_QCD("h_aco_dy_QCD","",bin_aco_QCD, min_aco_QCD, max_aco_QCD);
    TH1D h_aco_qcd_QCD("h_aco_qcd_QCD","",bin_aco_QCD, min_aco_QCD, max_aco_QCD);
    TH1D h_aco_ttjets_QCD("h_aco_ttjets_QCD","",bin_aco_QCD, min_aco_QCD, max_aco_QCD);

    TH1D h_m_data_QCD("h_m_data_QCD","",bin_m_QCD, min_m_QCD, max_m_QCD);
    TH1D h_m_dy_QCD("h_m_dy_QCD","",bin_m_QCD, min_m_QCD, max_m_QCD);
    TH1D h_m_qcd_QCD("h_m_qcd_QCD","",bin_m_QCD, min_m_QCD, max_m_QCD);
    TH1D h_m_ttjets_QCD("h_m_ttjets_QCD","",bin_m_QCD, min_m_QCD, max_m_QCD);

    TH1D h_r_data_QCD("h_r_data_QCD","",bin_r_QCD, min_r_QCD, max_r_QCD);
    TH1D h_r_dy_QCD("h_r_dy_QCD","",bin_r_QCD, min_r_QCD, max_r_QCD);
    TH1D h_r_qcd_QCD("h_r_qcd_QCD","",bin_r_QCD, min_r_QCD, max_r_QCD);
    TH1D h_r_ttjets_QCD("h_r_ttjets_QCD","",bin_r_QCD, min_r_QCD, max_r_QCD);

    TH1D h_pt_data_QCD("h_pt_data_QCD","",bin_pt_QCD, min_pt_QCD, max_pt_QCD);
    TH1D h_pt_dy_QCD("h_pt_dy_QCD","",bin_pt_QCD, min_pt_QCD, max_pt_QCD);
    TH1D h_pt_qcd_QCD("h_pt_qcd_QCD","",bin_pt_QCD, min_pt_QCD, max_pt_QCD);
    TH1D h_pt_ttjets_QCD("h_pt_ttjets_QCD","",bin_pt_QCD, min_pt_QCD, max_pt_QCD);

    // TT CONTROL REGION HISTOGRAMS
    TH1D h_aco_data_TT("h_aco_data_TT","",bin_aco_TT, min_aco_TT, max_aco_TT);
    TH1D h_aco_dy_TT("h_aco_dy_TT","",bin_aco_TT, min_aco_TT, max_aco_TT);
    TH1D h_aco_qcd_TT("h_aco_qcd_TT","",bin_aco_TT, min_aco_TT, max_aco_TT);
    TH1D h_aco_ttjets_TT("h_aco_ttjets_TT","",bin_aco_TT, min_aco_TT, max_aco_TT);

    TH1D h_m_data_TT("h_m_data_TT","",bin_m_TT, min_m_TT, max_m_TT);
    TH1D h_m_dy_TT("h_m_dy_TT","",bin_m_TT, min_m_TT, max_m_TT);
    TH1D h_m_qcd_TT("h_m_qcd_TT","",bin_m_TT, min_m_TT, max_m_TT);
    TH1D h_m_ttjets_TT("h_m_ttjets_TT","",bin_m_TT, min_m_TT, max_m_TT);

    TH1D h_r_data_TT("h_r_data_TT","",bin_r_TT, min_r_TT, max_r_TT);
    TH1D h_r_dy_TT("h_r_dy_TT","",bin_r_TT, min_r_TT, max_r_TT);
    TH1D h_r_qcd_TT("h_r_qcd_TT","",bin_r_TT, min_r_TT, max_r_TT);
    TH1D h_r_ttjets_TT("h_r_ttjets_TT","",bin_r_TT, min_r_TT, max_r_TT);

    TH1D h_pt_data_TT("h_pt_data_TT","",bin_pt_TT, min_pt_TT, max_pt_TT);
    TH1D h_pt_dy_TT("h_pt_dy_TT","",bin_pt_TT, min_pt_TT, max_pt_TT);
    TH1D h_pt_qcd_TT("h_pt_qcd_TT","",bin_pt_TT, min_pt_TT, max_pt_TT);
    TH1D h_pt_ttjets_TT("h_pt_ttjets_TT","",bin_pt_TT, min_pt_TT, max_pt_TT);

    // Get trees
    TTree* tree_data = (TTree*) data.Get("tree");
    TTree* tree_dy = (TTree*) dy.Get("tree");
    TTree* tree_qcd = (TTree*) qcd.Get("tree");
    TTree* tree_ttjets = (TTree*) ttjets.Get("tree");

    int n_evt_data = tree_data->GetEntries();
    int n_evt_dy = tree_dy->GetEntries();
    int n_evt_qcd = tree_qcd->GetEntries();
    int n_evt_ttjets = tree_ttjets->GetEntries();

    // Fill histograms
    cout << "Filling histograms..." << endl;
    cout << "Filling histograms: DATA" << endl;

    // Fill data
    for(int i=0; i<n_evt_data; i++){
        tree_data->GetEvent(i);
        
        if(tree_data->GetLeaf("sist_mass")->GetValue(0) >= 0){

            double w_data = tree_data->GetLeaf("weight")->GetValue(0);
            
            double sist_acop=tree_data->GetLeaf("sist_acop")->GetValue(0);
            double sist_mass = tree_data->GetLeaf("sist_mass")->GetValue(0);
            double sist_rap= tree_data->GetLeaf("sist_rap")->GetValue(0);
            double sist_pt=tree_data->GetLeaf("sist_pt")->GetValue(0);
            int n_bjets = coutBJets2(tree_data);

            double delta_phi=fabs(tree_data->GetLeaf("tau1_phi")->GetValue(0)-tree_data->GetLeaf("tau0_phi")->GetValue(0));
            if (delta_phi > M_PI) delta_phi = 2*M_PI - delta_phi;
            double cos_delta_phi = cos(delta_phi);
            double cosh_delta_eta=cosh(tree_data->GetLeaf("tau1_eta")->GetValue(0)-tree_data->GetLeaf("tau0_eta")->GetValue(0));
            double tau0_pt=tree_data->GetLeaf("tau0_pt")->GetValue(0);
            double tau1_pt=tree_data->GetLeaf("tau1_pt")->GetValue(0);
            double tau_mass=sqrt(2*tau0_pt*tau1_pt*(cosh_delta_eta - cos_delta_phi));

            // Control region cuts exactly as defined in your reference
            bool isDY_CR = (tau_mass >= 40 && tau_mass <= 100) && (sist_acop < 0.3) && (n_bjets == 0);
            bool isQCD_CR = (tau_mass >= 100 && tau_mass <= 300) && (sist_acop > 0.8) && (n_bjets == 0) && (sist_pt < 75);
            bool isTT_CR = (tau_mass >= 200 && tau_mass <= 650) && (sist_acop > 0.5) && (n_bjets >= 1) && (sist_pt < 125);
            
            // Fill histograms - CRUCIAL: DY plots tau_mass, QCD/TT plot sist_mass
            if(isDY_CR) {
                h_aco_data_DY.Fill(sist_acop,w_data);
                h_m_data_DY.Fill(tau_mass,w_data);     // DY plots tau_mass
                h_r_data_DY.Fill(sist_rap,w_data);
                h_pt_data_DY.Fill(sist_pt,w_data);
            }
            if(isQCD_CR) {
                h_aco_data_QCD.Fill(sist_acop,w_data);
                h_m_data_QCD.Fill(sist_mass,w_data);   // QCD plots sist_mass
                h_r_data_QCD.Fill(sist_rap,w_data);
                h_pt_data_QCD.Fill(sist_pt,w_data);
            }
            if(isTT_CR) {
                h_aco_data_TT.Fill(sist_acop,w_data);
                h_m_data_TT.Fill(sist_mass,w_data);    // TT plots 
                h_r_data_TT.Fill(sist_rap,w_data);
                h_pt_data_TT.Fill(sist_pt,w_data);
            }
        }
    }

    cout << "Filling histograms: DY" << endl;

    // Fill DY MC
    for(int i=0; i<n_evt_dy; i++){
        tree_dy->GetEvent(i);
        if(tree_dy->GetLeaf("sist_mass")->GetValue(0) >= 0){
            double w_dy = tree_data->GetLeaf("weight")->GetValue(0);

            double sist_acop=tree_dy->GetLeaf("sist_acop")->GetValue(0);
            double sist_mass = tree_dy->GetLeaf("sist_mass")->GetValue(0);
            double sist_rap= tree_dy->GetLeaf("sist_rap")->GetValue(0);
            double sist_pt=tree_dy->GetLeaf("sist_pt")->GetValue(0);
            int n_bjets = countBJets(tree_dy, 0.4); 

            double delta_phi=fabs(tree_dy->GetLeaf("tau1_phi")->GetValue(0)-tree_dy->GetLeaf("tau0_phi")->GetValue(0));
            if (delta_phi > M_PI) delta_phi = 2*M_PI - delta_phi;
            double cos_delta_phi = cos(delta_phi);
            double cosh_delta_eta=cosh(tree_dy->GetLeaf("tau1_eta")->GetValue(0)-tree_dy->GetLeaf("tau0_eta")->GetValue(0));
            double tau0_pt=tree_dy->GetLeaf("tau0_pt")->GetValue(0);
            double tau1_pt=tree_dy->GetLeaf("tau1_pt")->GetValue(0);
            double tau_mass=sqrt(2*tau0_pt*tau1_pt*(cosh_delta_eta - cos_delta_phi));

            // Control region cuts exactly as defined in your reference
            bool isDY_CR = (tau_mass >= 40 && tau_mass <= 100) && (sist_acop < 0.3) && (n_bjets == 0);
            bool isQCD_CR = (tau_mass >= 100 && tau_mass <= 300) && (sist_acop > 0.8) && (n_bjets == 0) && (sist_pt < 75);
            bool isTT_CR = (tau_mass >= 200 && tau_mass <= 650) && (sist_acop > 0.5) && (n_bjets >= 1) && (sist_pt < 125);
            
            // Fill histograms - CRUCIAL: DY plots tau_mass, QCD/TT plot sist_mass
            if(isDY_CR) {
                h_aco_dy_DY.Fill(sist_acop,w_dy);
                h_m_dy_DY.Fill(tau_mass,w_dy);     // DY plots tau_mass
                h_r_dy_DY.Fill(sist_rap,w_dy);
                h_pt_dy_DY.Fill(sist_pt,w_dy);
            }
            if(isQCD_CR) {
                h_aco_dy_QCD.Fill(sist_acop,w_dy);
                h_m_dy_QCD.Fill(sist_mass,w_dy);   // QCD plots sist_mass
                h_r_dy_QCD.Fill(sist_rap,w_dy);
                h_pt_dy_QCD.Fill(sist_pt,w_dy);
            }
            if(isTT_CR) {
                h_aco_dy_TT.Fill(sist_acop,w_dy);
                h_m_dy_TT.Fill(sist_mass,w_dy);    // TT plots sist_mass
                h_r_dy_TT.Fill(sist_rap,w_dy);
                h_pt_dy_TT.Fill(sist_pt,w_dy);
            }
        }
    }

    cout << "Filling histograms: QCD" << endl;

    // Fill QCD MC
    for(int i=0; i<n_evt_qcd; i++){
        tree_qcd->GetEvent(i);
        if(tree_qcd->GetLeaf("sist_mass")->GetValue(0) >= 0){

            double w_qcd = tree_data->GetLeaf("weight")->GetValue(0);

            double sist_acop=tree_qcd->GetLeaf("sist_acop")->GetValue(0);
            double sist_mass = tree_qcd->GetLeaf("sist_mass")->GetValue(0);
            double sist_rap= tree_qcd->GetLeaf("sist_rap")->GetValue(0);
            double sist_pt=tree_qcd->GetLeaf("sist_pt")->GetValue(0);
            int n_bjets = countBJets(tree_qcd, 0.4); 
           
            double delta_phi=fabs(tree_qcd->GetLeaf("tau1_phi")->GetValue(0)-tree_qcd->GetLeaf("tau0_phi")->GetValue(0));
            if (delta_phi > M_PI) delta_phi = 2*M_PI - delta_phi;
            double cos_delta_phi = cos(delta_phi);
            double cosh_delta_eta=cosh(tree_qcd->GetLeaf("tau1_eta")->GetValue(0)-tree_qcd->GetLeaf("tau0_eta")->GetValue(0));
            double tau0_pt=tree_qcd->GetLeaf("tau0_pt")->GetValue(0);
            double tau1_pt=tree_qcd->GetLeaf("tau1_pt")->GetValue(0);
            double tau_mass=sqrt(2*tau0_pt*tau1_pt*(cosh_delta_eta - cos_delta_phi));

            // Control region cuts exactly as defined in your reference
            bool isDY_CR = (tau_mass >= 40 && tau_mass <= 100) && (sist_acop < 0.3) && (n_bjets == 0);
            bool isQCD_CR = (tau_mass >= 100 && tau_mass <= 300) && (sist_acop > 0.8) && (n_bjets == 0) && (sist_pt < 75);
            bool isTT_CR = (tau_mass >= 200 && tau_mass <= 650) && (sist_acop > 0.5) && (n_bjets >= 1) && (sist_pt < 125);
            
            // Fill histograms - CRUCIAL: DY plots tau_mass, QCD/TT plot sist_mass
            if(isDY_CR) {
                h_aco_qcd_DY.Fill(sist_acop,w_qcd);
                h_m_qcd_DY.Fill(tau_mass,w_qcd);     // DY plots tau_mass
                h_r_qcd_DY.Fill(sist_rap,w_qcd);
                h_pt_qcd_DY.Fill(sist_pt,w_qcd);
            }
            if(isQCD_CR) {
                h_aco_qcd_QCD.Fill(sist_acop,w_qcd);
                h_m_qcd_QCD.Fill(sist_mass,w_qcd);   // QCD plots sist_mass
                h_r_qcd_QCD.Fill(sist_rap,w_qcd);
                h_pt_qcd_QCD.Fill(sist_pt,w_qcd);
            }
            if(isTT_CR) {
                h_aco_qcd_TT.Fill(sist_acop,w_qcd);
                h_m_qcd_TT.Fill(sist_mass,w_qcd);    // TT plots sist_mass
                h_r_qcd_TT.Fill(sist_rap,w_qcd);
                h_pt_qcd_TT.Fill(sist_pt,w_qcd);
            }
        }
    }

    cout << "Filling histograms: TTJETS" << endl;

    // Fill ttjets MC
    for(int i=0; i<n_evt_ttjets; i++){
        tree_ttjets->GetEvent(i);
        if(tree_ttjets->GetLeaf("sist_mass")->GetValue(0) >= 0){

            double w_ttjets = tree_data->GetLeaf("weight")->GetValue(0);

            double sist_acop=tree_ttjets->GetLeaf("sist_acop")->GetValue(0);
            double sist_mass = tree_ttjets->GetLeaf("sist_mass")->GetValue(0);
            double sist_rap= tree_ttjets->GetLeaf("sist_rap")->GetValue(0);
            double sist_pt=tree_ttjets->GetLeaf("sist_pt")->GetValue(0);
            int n_bjets = countBJets(tree_ttjets, 0.4); 

            double delta_phi=fabs(tree_ttjets->GetLeaf("tau1_phi")->GetValue(0)-tree_ttjets->GetLeaf("tau0_phi")->GetValue(0));
            if (delta_phi > M_PI) delta_phi = 2*M_PI - delta_phi;
            double cos_delta_phi = cos(delta_phi);
            double cosh_delta_eta=cosh(tree_ttjets->GetLeaf("tau1_eta")->GetValue(0)-tree_ttjets->GetLeaf("tau0_eta")->GetValue(0));
            double tau0_pt=tree_ttjets->GetLeaf("tau0_pt")->GetValue(0);
            double tau1_pt=tree_ttjets->GetLeaf("tau1_pt")->GetValue(0);
            double tau_mass=sqrt(2*tau0_pt*tau1_pt*(cosh_delta_eta - cos_delta_phi));

            // Control region cuts exactly as defined in your reference
            bool isDY_CR = (tau_mass >= 40 && tau_mass <= 100) && (sist_acop < 0.3) && (n_bjets == 0);
            bool isQCD_CR = (tau_mass >= 100 && tau_mass <= 300) && (sist_acop > 0.8) && (n_bjets == 0) && (sist_pt < 75);
            bool isTT_CR = (tau_mass >= 200 && tau_mass <= 650) && (sist_acop > 0.5) && (n_bjets >= 1) && (sist_pt < 125);
            
            // Fill histograms - CRUCIAL: DY plots tau_mass, QCD/TT plot sist_mass
            if(isDY_CR) {
                h_aco_ttjets_DY.Fill(sist_acop,w_ttjets);
                h_m_ttjets_DY.Fill(tau_mass,w_ttjets);     // DY plots tau_mass
                h_r_ttjets_DY.Fill(sist_rap,w_ttjets);
                h_pt_ttjets_DY.Fill(sist_pt,w_ttjets);
            }
            if(isQCD_CR) {
                h_aco_ttjets_QCD.Fill(sist_acop,w_ttjets);
                h_m_ttjets_QCD.Fill(sist_mass,w_ttjets);   // QCD plots sist_mass
                h_r_ttjets_QCD.Fill(sist_rap,w_ttjets);
                h_pt_ttjets_QCD.Fill(sist_pt,w_ttjets);
            }
            if(isTT_CR) {
                h_aco_ttjets_TT.Fill(sist_acop,w_ttjets);
                h_m_ttjets_TT.Fill(sist_mass,w_ttjets);    // TT plots sist_mass
                h_r_ttjets_TT.Fill(sist_rap,w_ttjets);
                h_pt_ttjets_TT.Fill(sist_pt,w_ttjets);
            }
        }
    }

    cout << "Filling histograms: COMPLETE" << endl;

    cout << "DY CR - Data events: " << h_aco_data_DY.GetEntries() << endl;
    cout << "DY CR - DY MC events: " << h_aco_dy_DY.GetEntries() << endl;
    cout << "DY CR - QCD MC events: " << h_aco_qcd_DY.GetEntries() << endl;
    cout << "DY CR - ttjets MC events: " << h_aco_ttjets_DY.GetEntries() << endl;

    // Set colors DY CR
    h_aco_dy_DY.SetFillColor(kYellow); h_aco_dy_DY.SetLineWidth(0);
    h_aco_qcd_DY.SetFillColor(kRed); h_aco_qcd_DY.SetLineWidth(0);
    h_aco_ttjets_DY.SetFillColor(kGreen); h_aco_ttjets_DY.SetLineWidth(0);

    h_m_dy_DY.SetFillColor(kYellow); h_m_dy_DY.SetLineWidth(0);
    h_m_qcd_DY.SetFillColor(kRed); h_m_qcd_DY.SetLineWidth(0);
    h_m_ttjets_DY.SetFillColor(kGreen); h_m_ttjets_DY.SetLineWidth(0);

    h_r_dy_DY.SetFillColor(kYellow); h_r_dy_DY.SetLineWidth(0);
    h_r_qcd_DY.SetFillColor(kRed); h_r_qcd_DY.SetLineWidth(0);
    h_r_ttjets_DY.SetFillColor(kGreen); h_r_ttjets_DY.SetLineWidth(0);

    h_pt_dy_DY.SetFillColor(kYellow); h_pt_dy_DY.SetLineWidth(0);
    h_pt_qcd_DY.SetFillColor(kRed); h_pt_qcd_DY.SetLineWidth(0);
    h_pt_ttjets_DY.SetFillColor(kGreen); h_pt_ttjets_DY.SetLineWidth(0);

    // Data styling
    h_aco_data_DY.SetMarkerStyle(20); h_aco_data_DY.SetMarkerSize(0.8);
    h_m_data_DY.SetMarkerStyle(20); h_m_data_DY.SetMarkerSize(0.8);
    h_r_data_DY.SetMarkerStyle(20); h_r_data_DY.SetMarkerSize(0.8);
    h_pt_data_DY.SetMarkerStyle(20); h_pt_data_DY.SetMarkerSize(0.8);

    //QCD
    h_aco_dy_QCD.SetFillColor(kYellow); h_aco_dy_QCD.SetLineWidth(0);
    h_aco_qcd_QCD.SetFillColor(kRed); h_aco_qcd_QCD.SetLineWidth(0);
    h_aco_ttjets_QCD.SetFillColor(kGreen); h_aco_ttjets_QCD.SetLineWidth(0);

    h_m_dy_QCD.SetFillColor(kYellow); h_m_dy_QCD.SetLineWidth(0);
    h_m_qcd_QCD.SetFillColor(kRed); h_m_qcd_QCD.SetLineWidth(0);
    h_m_ttjets_QCD.SetFillColor(kGreen); h_m_ttjets_QCD.SetLineWidth(0);

    h_r_dy_QCD.SetFillColor(kYellow); h_r_dy_QCD.SetLineWidth(0);
    h_r_qcd_QCD.SetFillColor(kRed); h_r_qcd_QCD.SetLineWidth(0);
    h_r_ttjets_QCD.SetFillColor(kGreen); h_r_ttjets_QCD.SetLineWidth(0);

    h_pt_dy_QCD.SetFillColor(kYellow); h_pt_dy_QCD.SetLineWidth(0);
    h_pt_qcd_QCD.SetFillColor(kRed); h_pt_qcd_QCD.SetLineWidth(0);
    h_pt_ttjets_QCD.SetFillColor(kGreen); h_pt_ttjets_QCD.SetLineWidth(0);

    // Data styling
    h_aco_data_QCD.SetMarkerStyle(20); h_aco_data_QCD.SetMarkerSize(0.8);
    h_m_data_QCD.SetMarkerStyle(20); h_m_data_QCD.SetMarkerSize(0.8);
    h_r_data_QCD.SetMarkerStyle(20); h_r_data_QCD.SetMarkerSize(0.8);
    h_pt_data_QCD.SetMarkerStyle(20); h_pt_data_QCD.SetMarkerSize(0.8);

    //TT
    h_aco_dy_TT.SetFillColor(kYellow); h_aco_dy_TT.SetLineWidth(0);
    h_aco_qcd_TT.SetFillColor(kRed); h_aco_qcd_TT.SetLineWidth(0);
    h_aco_ttjets_TT.SetFillColor(kGreen); h_aco_ttjets_TT.SetLineWidth(0);

    h_m_dy_TT.SetFillColor(kYellow); h_m_dy_TT.SetLineWidth(0);
    h_m_qcd_TT.SetFillColor(kRed); h_m_qcd_TT.SetLineWidth(0);
    h_m_ttjets_TT.SetFillColor(kGreen); h_m_ttjets_TT.SetLineWidth(0);

    h_r_dy_TT.SetFillColor(kYellow); h_r_dy_TT.SetLineWidth(0);
    h_r_qcd_TT.SetFillColor(kRed); h_r_qcd_TT.SetLineWidth(0);
    h_r_ttjets_TT.SetFillColor(kGreen); h_r_ttjets_TT.SetLineWidth(0);

    h_pt_dy_TT.SetFillColor(kYellow); h_pt_dy_TT.SetLineWidth(0);
    h_pt_qcd_TT.SetFillColor(kRed); h_pt_qcd_TT.SetLineWidth(0);
    h_pt_ttjets_TT.SetFillColor(kGreen); h_pt_ttjets_TT.SetLineWidth(0);

    // Data styling
    h_aco_data_TT.SetMarkerStyle(20); h_aco_data_TT.SetMarkerSize(0.8);
    h_m_data_TT.SetMarkerStyle(20); h_m_data_TT.SetMarkerSize(0.8);
    h_r_data_TT.SetMarkerStyle(20); h_r_data_TT.SetMarkerSize(0.8);
    h_pt_data_TT.SetMarkerStyle(20); h_pt_data_TT.SetMarkerSize(0.8);

    cout << "Creating plots..." << endl;
    gSystem->Exec("mkdir -p output_plots");

    // ====================================
    // DY Control Region - Acoplanarity
    // ====================================
    TCanvas c1("c1", "", 700, 800);
    
    TPad *pad1 = new TPad("pad1", "pad1", 0, 0.3, 1, 1.0);
    pad1->SetBottomMargin(0.02);
    pad1->Draw();
    
    TPad *pad2 = new TPad("pad2", "pad2", 0, 0, 1, 0.3);
    pad2->SetTopMargin(0.05);
    pad2->SetBottomMargin(0.35);
    pad2->Draw();
    
    pad1->cd();
    
    THStack stack_aco_DY("stack_aco_DY", "");
    stack_aco_DY.Add(&h_aco_qcd_DY);     // Bottom (red)
    stack_aco_DY.Add(&h_aco_ttjets_DY);  // Middle (green)
    stack_aco_DY.Add(&h_aco_dy_DY);      // Top (yellow) - DY DOMINANT
    
    
    stack_aco_DY.Draw("hist");
    stack_aco_DY.GetYaxis()->SetTitle("Events");
    stack_aco_DY.GetYaxis()->SetTitleSize(0.06);
    stack_aco_DY.GetYaxis()->SetLabelSize(0.05);
    stack_aco_DY.GetYaxis()->SetTitleOffset(0.8);
    
    TH1D* unc_band_aco = CreateUncertaintyBand(&stack_aco_DY, "unc_aco");
    unc_band_aco->Draw("E2 same");
    
    h_aco_data_DY.Draw("E same");
    
    TLegend *leg1 = new TLegend(0.65, 0.55, 0.90, 0.85);
    leg1->SetBorderSize(0);
    leg1->SetFillStyle(0);
    leg1->AddEntry(&h_aco_data_DY, "Data (2018)", "lep");
    leg1->AddEntry(&h_aco_dy_DY, "Drell Yan", "f");        // Top of stack
    leg1->AddEntry(&h_aco_qcd_DY, "QCD (Data driven)", "f"); // Middle
    leg1->AddEntry(&h_aco_ttjets_DY, "t#bar{t}", "f");      // Bottom
    leg1->Draw();
    
    TLatex cms1;
    cms1.SetTextSize(0.06);
    cms1.SetTextFont(62);
    cms1.DrawLatexNDC(0.15, 0.85, "CMS-TOTEM Preliminary");
    
    TLatex lumi1;
    lumi1.SetTextSize(0.05);
    lumi1.DrawLatexNDC(0.65, 0.92, "54.9 fb^{-1} (13 TeV)");
    
    pad2->cd();
    
    TH1D* ratio_aco = CreateRatio(&h_aco_data_DY, &stack_aco_DY, "ratio_aco");
    ratio_aco->GetXaxis()->SetTitle("Acoplanarity of the central system");
    ratio_aco->Draw("E");
    
    TLine line_aco(min_aco_DY, 1.0, max_aco_DY, 1.0);
    line_aco.SetLineStyle(2);
    line_aco.SetLineColor(kRed);
    line_aco.Draw();
    
    c1.SaveAs("output_plots/DY_CR_acoplanarity.png");
    
    // ====================================
    // DY Control Region - Mass
    // ====================================
    TCanvas c2("c2", "", 700, 800);
    
    TPad *pad1_m = new TPad("pad1_m", "pad1_m", 0, 0.3, 1, 1.0);
    pad1_m->SetBottomMargin(0.02);
    pad1_m->Draw();
    
    TPad *pad2_m = new TPad("pad2_m", "pad2_m", 0, 0, 1, 0.3);
    pad2_m->SetTopMargin(0.05);
    pad2_m->SetBottomMargin(0.35);
    pad2_m->Draw();
    
    pad1_m->cd();
    
    THStack stack_m_DY("stack_m_DY", "");
    stack_m_DY.Add(&h_m_qcd_DY);        // Bottom (red)
    stack_m_DY.Add(&h_m_ttjets_DY);     // Middle (green)
    stack_m_DY.Add(&h_m_dy_DY);         // Top (yellow) - DY DOMINANT
    
    stack_m_DY.Draw("hist");
    stack_m_DY.GetYaxis()->SetTitle("Events");
    stack_m_DY.GetYaxis()->SetTitleSize(0.06);
    stack_m_DY.GetYaxis()->SetLabelSize(0.05);
    stack_m_DY.GetYaxis()->SetTitleOffset(0.8);
    
    TH1D* unc_band_m = CreateUncertaintyBand(&stack_m_DY, "unc_m");
    unc_band_m->Draw("E2 same");
    
    h_m_data_DY.Draw("E same");
    
    TLegend *leg2 = new TLegend(0.65, 0.55, 0.90, 0.85);
    leg2->SetBorderSize(0);
    leg2->SetFillStyle(0);
    leg2->AddEntry(&h_m_data_DY, "Data (2018)", "lep");
    leg2->AddEntry(&h_m_dy_DY, "Drell Yan", "f");
    leg2->AddEntry(&h_m_qcd_DY, "QCD (Data driven)", "f");
    leg2->AddEntry(&h_m_ttjets_DY, "t#bar{t}", "f");
    leg2->Draw();
    
    TLatex cms2;
    cms2.SetTextSize(0.06);
    cms2.SetTextFont(62);
    cms2.DrawLatexNDC(0.15, 0.85, "CMS-TOTEM Preliminary");
    
    TLatex lumi2;
    lumi2.SetTextSize(0.05);
    lumi2.DrawLatexNDC(0.65, 0.92, "54.9 fb^{-1} (13 TeV)");
    
    pad2_m->cd();
    
    TH1D* ratio_m = CreateRatio(&h_m_data_DY, &stack_m_DY, "ratio_m");
    ratio_m->GetXaxis()->SetTitle("Invariant mass of the central system [GeV]");
    ratio_m->Draw("E");
    
    TLine line_m(min_m_DY, 1.0, max_m_DY, 1.0);
    line_m.SetLineStyle(2);
    line_m.SetLineColor(kRed);
    line_m.Draw();
    
    c2.SaveAs("output_plots/DY_CR_mass.png");
    
    // ====================================
    // DY Control Region - Rapidity
    // ====================================
    TCanvas c3("c3", "", 700, 800);
    
    TPad *pad1_r = new TPad("pad1_r", "pad1_r", 0, 0.3, 1, 1.0);
    pad1_r->SetBottomMargin(0.02);
    pad1_r->Draw();
    
    TPad *pad2_r = new TPad("pad2_r", "pad2_r", 0, 0, 1, 0.3);
    pad2_r->SetTopMargin(0.05);
    pad2_r->SetBottomMargin(0.35);
    pad2_r->Draw();
    
    pad1_r->cd();
    
    THStack stack_r_DY("stack_r_DY", "");
    stack_r_DY.Add(&h_r_qcd_DY);        // Bottom (red)
    stack_r_DY.Add(&h_r_ttjets_DY);     // Middle (green)
    stack_r_DY.Add(&h_r_dy_DY);         // Top (yellow) - DY DOMINANT
            // Top (yellow) - DY DOMINANT
    
    stack_r_DY.Draw("hist");
    stack_r_DY.GetYaxis()->SetTitle("Events");
    stack_r_DY.GetYaxis()->SetTitleSize(0.06);
    stack_r_DY.GetYaxis()->SetLabelSize(0.05);
    stack_r_DY.GetYaxis()->SetTitleOffset(0.8);
    
    TH1D* unc_band_r = CreateUncertaintyBand(&stack_r_DY, "unc_r");
    unc_band_r->Draw("E2 same");
    
    h_r_data_DY.Draw("E same");
    
    TLegend *leg3 = new TLegend(0.65, 0.55, 0.90, 0.85);
    leg3->SetBorderSize(0);
    leg3->SetFillStyle(0);
    leg3->AddEntry(&h_r_data_DY, "Data (2018)", "lep");
    leg3->AddEntry(&h_r_dy_DY, "Drell Yan", "f");
    leg3->AddEntry(&h_r_qcd_DY, "QCD (Data driven)", "f");
    leg3->AddEntry(&h_r_ttjets_DY, "t#bar{t}", "f");
    leg3->Draw();
    
    TLatex cms3;
    cms3.SetTextSize(0.06);
    cms3.SetTextFont(62);
    cms3.DrawLatexNDC(0.15, 0.85, "CMS-TOTEM Preliminary");
    
    TLatex lumi3;
    lumi3.SetTextSize(0.05);
    lumi3.DrawLatexNDC(0.65, 0.92, "54.9 fb^{-1} (13 TeV)");
    
    pad2_r->cd();
    
    TH1D* ratio_r = CreateRatio(&h_r_data_DY, &stack_r_DY, "ratio_r");
    ratio_r->GetXaxis()->SetTitle("Rapidity of the central system");
    ratio_r->Draw("E");
    
    TLine line_r(min_r_DY, 1.0, max_r_DY, 1.0);
    line_r.SetLineStyle(2);
    line_r.SetLineColor(kRed);
    line_r.Draw();
    
    c3.SaveAs("output_plots/DY_CR_rapidity.png");
    
    // ====================================
    // DY Control Region - Transverse Momentum
    // ====================================
    TCanvas c4("c4", "", 700, 800);
    
    TPad *pad1_pt = new TPad("pad1_pt", "pad1_pt", 0, 0.3, 1, 1.0);
    pad1_pt->SetBottomMargin(0.02);
    pad1_pt->Draw();
    
    TPad *pad2_pt = new TPad("pad2_pt", "pad2_pt", 0, 0, 1, 0.3);
    pad2_pt->SetTopMargin(0.05);
    pad2_pt->SetBottomMargin(0.35);
    pad2_pt->Draw();
    
    pad1_pt->cd();
    
    THStack stack_pt_DY("stack_pt_DY", "");
    stack_pt_DY.Add(&h_pt_qcd_DY);      // Bottom (red)
    stack_pt_DY.Add(&h_pt_ttjets_DY);   // Middle (green)
    stack_pt_DY.Add(&h_pt_dy_DY);       // Top (yellow) - DY DOMINANT
    
    
    stack_pt_DY.Draw("hist");
    stack_pt_DY.GetYaxis()->SetTitle("Events");
    stack_pt_DY.GetYaxis()->SetTitleSize(0.06);
    stack_pt_DY.GetYaxis()->SetLabelSize(0.05);
    stack_pt_DY.GetYaxis()->SetTitleOffset(0.8);
    
    TH1D* unc_band_pt = CreateUncertaintyBand(&stack_pt_DY, "unc_pt");
    unc_band_pt->Draw("E2 same");
    
    h_pt_data_DY.Draw("E same");
    
    TLegend *leg4 = new TLegend(0.65, 0.55, 0.90, 0.85);
    leg4->SetBorderSize(0);
    leg4->SetFillStyle(0);
    leg4->AddEntry(&h_pt_data_DY, "Data (2018)", "lep");
    leg4->AddEntry(&h_pt_dy_DY, "Drell Yan", "f");
    leg4->AddEntry(&h_pt_qcd_DY, "QCD (Data driven)", "f");
    leg4->AddEntry(&h_pt_ttjets_DY, "t#bar{t}", "f");
    leg4->Draw();
    
    TLatex cms4;
    cms4.SetTextSize(0.06);
    cms4.SetTextFont(62);
    cms4.DrawLatexNDC(0.15, 0.85, "CMS-TOTEM Preliminary");
    
    TLatex lumi4;
    lumi4.SetTextSize(0.05);
    lumi4.DrawLatexNDC(0.65, 0.92, "54.9 fb^{-1} (13 TeV)");
    
    pad2_pt->cd();
    
    TH1D* ratio_pt = CreateRatio(&h_pt_data_DY, &stack_pt_DY, "ratio_pt");
    ratio_pt->GetXaxis()->SetTitle("Transverse momentum of the central system [GeV]");
    ratio_pt->Draw("E");
    
    TLine line_pt(min_pt_DY, 1.0, max_pt_DY, 1.0);
    line_pt.SetLineStyle(2);
    line_pt.SetLineColor(kRed);
    line_pt.Draw();
    
    c4.SaveAs("output_plots/DY_CR_pt.png");

    // ====================================
    // TT Control Region - Acoplanarity
    // ====================================
    TCanvas c5("c5", "", 700, 800);
    
    TPad *pad1_tt_aco = new TPad("pad1_tt_aco", "pad1_tt_aco", 0, 0.3, 1, 1.0);
    pad1_tt_aco->SetBottomMargin(0.02);
    pad1_tt_aco->Draw();
    
    TPad *pad2_tt_aco = new TPad("pad2_tt_aco", "pad2_tt_aco", 0, 0, 1, 0.3);
    pad2_tt_aco->SetTopMargin(0.05);
    pad2_tt_aco->SetBottomMargin(0.35);
    pad2_tt_aco->Draw();
    
    pad1_tt_aco->cd();
    
    THStack stack_aco_TT("stack_aco_TT", "");
    stack_aco_TT.Add(&h_aco_qcd_TT);     // Bottom (red)
    stack_aco_TT.Add(&h_aco_dy_TT);      // Middle (yellow)
    stack_aco_TT.Add(&h_aco_ttjets_TT);  // Top (green) - TT DOMINANT
    
    
    stack_aco_TT.Draw("hist");
    stack_aco_TT.GetYaxis()->SetTitle("Events");
    stack_aco_TT.GetYaxis()->SetTitleSize(0.06);
    stack_aco_TT.GetYaxis()->SetLabelSize(0.05);
    stack_aco_TT.GetYaxis()->SetTitleOffset(0.8);
    
    TH1D* unc_band_aco_tt = CreateUncertaintyBand(&stack_aco_TT, "unc_aco_tt");
    unc_band_aco_tt->Draw("E2 same");
    
    h_aco_data_TT.Draw("E same");
    
    TLegend *leg5 = new TLegend(0.65, 0.55, 0.90, 0.85);
    leg5->SetBorderSize(0);
    leg5->SetFillStyle(0);
    leg5->AddEntry(&h_aco_data_TT, "Data (2018)", "lep");
    leg5->AddEntry(&h_aco_ttjets_TT, "t#bar{t}", "f");        // Top of stack
    leg5->AddEntry(&h_aco_qcd_TT, "QCD (Data driven)", "f");  // Middle
    leg5->AddEntry(&h_aco_dy_TT, "Drell Yan", "f");          // Bottom
    leg5->Draw();
    
    TLatex cms5;
    cms5.SetTextSize(0.06);
    cms5.SetTextFont(62);
    cms5.DrawLatexNDC(0.15, 0.85, "CMS-TOTEM Preliminary");
    
    TLatex lumi5;
    lumi5.SetTextSize(0.05);
    lumi5.DrawLatexNDC(0.65, 0.92, "54.9 fb^{-1} (13 TeV)");
    
    pad2_tt_aco->cd();
    
    TH1D* ratio_aco_tt = CreateRatio(&h_aco_data_TT, &stack_aco_TT, "ratio_aco_tt");
    ratio_aco_tt->GetXaxis()->SetTitle("Acoplanarity of the central system");
    ratio_aco_tt->Draw("E");
    
    TLine line_aco_tt(min_aco_TT, 1.0, max_aco_TT, 1.0);
    line_aco_tt.SetLineStyle(2);
    line_aco_tt.SetLineColor(kRed);
    line_aco_tt.Draw();
    
    c5.SaveAs("output_plots/TT_CR_acoplanarity.png");
    
    // ====================================
    // TT Control Region - Mass
    // ====================================
    TCanvas c6("c6", "", 700, 800);
    
    TPad *pad1_tt_m = new TPad("pad1_tt_m", "pad1_tt_m", 0, 0.3, 1, 1.0);
    pad1_tt_m->SetBottomMargin(0.02);
    pad1_tt_m->Draw();
    
    TPad *pad2_tt_m = new TPad("pad2_tt_m", "pad2_tt_m", 0, 0, 1, 0.3);
    pad2_tt_m->SetTopMargin(0.05);
    pad2_tt_m->SetBottomMargin(0.35);
    pad2_tt_m->Draw();
    
    pad1_tt_m->cd();
    
    THStack stack_m_TT("stack_m_TT", "");
    stack_m_TT.Add(&h_m_qcd_TT);        // Bottom (red)
    stack_m_TT.Add(&h_m_dy_TT);         // Middle (yellow)
    stack_m_TT.Add(&h_m_ttjets_TT);     // Top (green) - TT DOMINANT
    
    
    stack_m_TT.Draw("hist");
    stack_m_TT.GetYaxis()->SetTitle("Events");
    stack_m_TT.GetYaxis()->SetTitleSize(0.06);
    stack_m_TT.GetYaxis()->SetLabelSize(0.05);
    stack_m_TT.GetYaxis()->SetTitleOffset(0.8);
    
    TH1D* unc_band_m_tt = CreateUncertaintyBand(&stack_m_TT, "unc_m_tt");
    unc_band_m_tt->Draw("E2 same");
    
    h_m_data_TT.Draw("E same");
    
    TLegend *leg6 = new TLegend(0.65, 0.55, 0.90, 0.85);
    leg6->SetBorderSize(0);
    leg6->SetFillStyle(0);
    leg6->AddEntry(&h_m_data_TT, "Data (2018)", "lep");
    leg6->AddEntry(&h_m_ttjets_TT, "t#bar{t}", "f");
    leg6->AddEntry(&h_m_qcd_TT, "QCD (Data driven)", "f");
    leg6->AddEntry(&h_m_dy_TT, "Drell Yan", "f");
    leg6->Draw();
    
    TLatex cms6;
    cms6.SetTextSize(0.06);
    cms6.SetTextFont(62);
    cms6.DrawLatexNDC(0.15, 0.85, "CMS-TOTEM Preliminary");
    
    TLatex lumi6;
    lumi6.SetTextSize(0.05);
    lumi6.DrawLatexNDC(0.65, 0.92, "54.9 fb^{-1} (13 TeV)");
    
    pad2_tt_m->cd();
    
    TH1D* ratio_m_tt = CreateRatio(&h_m_data_TT, &stack_m_TT, "ratio_m_tt");
    ratio_m_tt->GetXaxis()->SetTitle("Invariant mass of the central system [GeV]");
    ratio_m_tt->Draw("E");
    
    TLine line_m_tt(min_m_TT, 1.0, max_m_TT, 1.0);
    line_m_tt.SetLineStyle(2);
    line_m_tt.SetLineColor(kRed);
    line_m_tt.Draw();
    
    c6.SaveAs("output_plots/TT_CR_mass.png");
    
    // ====================================
    // TT Control Region - Rapidity
    // ====================================
    TCanvas c7("c7", "", 700, 800);
    
    TPad *pad1_tt_r = new TPad("pad1_tt_r", "pad1_tt_r", 0, 0.3, 1, 1.0);
    pad1_tt_r->SetBottomMargin(0.02);
    pad1_tt_r->Draw();
    
    TPad *pad2_tt_r = new TPad("pad2_tt_r", "pad2_tt_r", 0, 0, 1, 0.3);
    pad2_tt_r->SetTopMargin(0.05);
    pad2_tt_r->SetBottomMargin(0.35);
    pad2_tt_r->Draw();
    
    pad1_tt_r->cd();
    
    THStack stack_r_TT("stack_r_TT", "");
    stack_r_TT.Add(&h_r_qcd_TT);        // Bottom (red)
    stack_r_TT.Add(&h_r_dy_TT);         // Middle (yellow)
    stack_r_TT.Add(&h_r_ttjets_TT);     // Top (green) - TT DOMINANT
    
    
    stack_r_TT.Draw("hist");
    stack_r_TT.GetYaxis()->SetTitle("Events");
    stack_r_TT.GetYaxis()->SetTitleSize(0.06);
    stack_r_TT.GetYaxis()->SetLabelSize(0.05);
    stack_r_TT.GetYaxis()->SetTitleOffset(0.8);
    
    TH1D* unc_band_r_tt = CreateUncertaintyBand(&stack_r_TT, "unc_r_tt");
    unc_band_r_tt->Draw("E2 same");
    
    h_r_data_TT.Draw("E same");
    
    TLegend *leg7 = new TLegend(0.65, 0.55, 0.90, 0.85);
    leg7->SetBorderSize(0);
    leg7->SetFillStyle(0);
    leg7->AddEntry(&h_r_data_TT, "Data (2018)", "lep");
    leg7->AddEntry(&h_r_ttjets_TT, "t#bar{t}", "f");
    leg7->AddEntry(&h_r_qcd_TT, "QCD (Data driven)", "f");
    leg7->AddEntry(&h_r_dy_TT, "Drell Yan", "f");
    leg7->Draw();
    
    TLatex cms7;
    cms7.SetTextSize(0.06);
    cms7.SetTextFont(62);
    cms7.DrawLatexNDC(0.15, 0.85, "CMS-TOTEM Preliminary");
    
    TLatex lumi7;
    lumi7.SetTextSize(0.05);
    lumi7.DrawLatexNDC(0.65, 0.92, "54.9 fb^{-1} (13 TeV)");
    
    pad2_tt_r->cd();
    
    TH1D* ratio_r_tt = CreateRatio(&h_r_data_TT, &stack_r_TT, "ratio_r_tt");
    ratio_r_tt->GetXaxis()->SetTitle("Rapidity of the central system");
    ratio_r_tt->Draw("E");
    
    TLine line_r_tt(min_r_TT, 1.0, max_r_TT, 1.0);
    line_r_tt.SetLineStyle(2);
    line_r_tt.SetLineColor(kRed);
    line_r_tt.Draw();
    
    c7.SaveAs("output_plots/TT_CR_rapidity.png");
    
    // ====================================
    // TT Control Region - Transverse Momentum
    // ====================================
    TCanvas c8("c8", "", 700, 800);
    
    TPad *pad1_tt_pt = new TPad("pad1_tt_pt", "pad1_tt_pt", 0, 0.3, 1, 1.0);
    pad1_tt_pt->SetBottomMargin(0.02);
    pad1_tt_pt->Draw();
    
    TPad *pad2_tt_pt = new TPad("pad2_tt_pt", "pad2_tt_pt", 0, 0, 1, 0.3);
    pad2_tt_pt->SetTopMargin(0.05);
    pad2_tt_pt->SetBottomMargin(0.35);
    pad2_tt_pt->Draw();
    
    pad1_tt_pt->cd();
    
    THStack stack_pt_TT("stack_pt_TT", "");
    stack_pt_TT.Add(&h_pt_qcd_TT);      // Bottom (red)
    stack_pt_TT.Add(&h_pt_dy_TT);       // Middle (yellow)
    stack_pt_TT.Add(&h_pt_ttjets_TT);   // Top (green) - TT DOMINANT
    
    
    stack_pt_TT.Draw("hist");
    stack_pt_TT.GetYaxis()->SetTitle("Events");
    stack_pt_TT.GetYaxis()->SetTitleSize(0.06);
    stack_pt_TT.GetYaxis()->SetLabelSize(0.05);
    stack_pt_TT.GetYaxis()->SetTitleOffset(0.8);
    
    TH1D* unc_band_pt_tt = CreateUncertaintyBand(&stack_pt_TT, "unc_pt_tt");
    unc_band_pt_tt->Draw("E2 same");
    
    h_pt_data_TT.Draw("E same");
    
    TLegend *leg8 = new TLegend(0.65, 0.55, 0.90, 0.85);
    leg8->SetBorderSize(0);
    leg8->SetFillStyle(0);
    leg8->AddEntry(&h_pt_data_TT, "Data (2018)", "lep");
    leg8->AddEntry(&h_pt_ttjets_TT, "t#bar{t}", "f");
    leg8->AddEntry(&h_pt_qcd_TT, "QCD (Data driven)", "f");
    leg8->AddEntry(&h_pt_dy_TT, "Drell Yan", "f");
    leg8->Draw();
    
    TLatex cms8;
    cms8.SetTextSize(0.06);
    cms8.SetTextFont(62);
    cms8.DrawLatexNDC(0.15, 0.85, "CMS-TOTEM Preliminary");
    
    TLatex lumi8;
    lumi8.SetTextSize(0.05);
    lumi8.DrawLatexNDC(0.65, 0.92, "54.9 fb^{-1} (13 TeV)");
    
    pad2_tt_pt->cd();
    
    TH1D* ratio_pt_tt = CreateRatio(&h_pt_data_TT, &stack_pt_TT, "ratio_pt_tt");
    ratio_pt_tt->GetXaxis()->SetTitle("Transverse momentum of the central system [GeV]");
    ratio_pt_tt->Draw("E");
    
    TLine line_pt_tt(min_pt_TT, 1.0, max_pt_TT, 1.0);
    line_pt_tt.SetLineStyle(2);
    line_pt_tt.SetLineColor(kRed);
    line_pt_tt.Draw();
    
    c8.SaveAs("output_plots/TT_CR_pt.png");

    // ====================================
    // QCD Control Region - Acoplanarity  
    // ====================================
    TCanvas c9("c9", "", 700, 800);
    
    TPad *pad1_qcd_aco = new TPad("pad1_qcd_aco", "pad1_qcd_aco", 0, 0.3, 1, 1.0);
    pad1_qcd_aco->SetBottomMargin(0.02);
    pad1_qcd_aco->Draw();
    
    TPad *pad2_qcd_aco = new TPad("pad2_qcd_aco", "pad2_qcd_aco", 0, 0, 1, 0.3);
    pad2_qcd_aco->SetTopMargin(0.05);
    pad2_qcd_aco->SetBottomMargin(0.35);
    pad2_qcd_aco->Draw();
    
    pad1_qcd_aco->cd();
    
    THStack stack_aco_QCD("stack_aco_QCD", "");
    stack_aco_QCD.Add(&h_aco_dy_QCD);     // Middle (yellow)
    stack_aco_QCD.Add(&h_aco_ttjets_QCD); // Bottom (green)
   
    stack_aco_QCD.Add(&h_aco_qcd_QCD);    // Top (red) - QCD DOMINANT
    
    stack_aco_QCD.Draw("hist");
    stack_aco_QCD.GetYaxis()->SetTitle("Events");
    stack_aco_QCD.GetYaxis()->SetTitleSize(0.06);
    stack_aco_QCD.GetYaxis()->SetLabelSize(0.05);
    stack_aco_QCD.GetYaxis()->SetTitleOffset(0.8);
    
    TH1D* unc_band_aco_qcd = CreateUncertaintyBand(&stack_aco_QCD, "unc_aco_qcd");
    unc_band_aco_qcd->Draw("E2 same");
    
    h_aco_data_QCD.Draw("E same");
    
    TLegend *leg9 = new TLegend(0.65, 0.55, 0.90, 0.85);
    leg9->SetBorderSize(0);
    leg9->SetFillStyle(0);
    leg9->AddEntry(&h_aco_data_QCD, "Data (2018)", "lep");
    leg9->AddEntry(&h_aco_qcd_QCD, "QCD (Data driven)", "f"); // Top of stack
    leg9->AddEntry(&h_aco_dy_QCD, "Drell Yan", "f");         // Middle
    leg9->AddEntry(&h_aco_ttjets_QCD, "t#bar{t}", "f");      // Bottom
    leg9->Draw();
    
    TLatex cms9;
    cms9.SetTextSize(0.06);
    cms9.SetTextFont(62);
    cms9.DrawLatexNDC(0.15, 0.85, "CMS-TOTEM Preliminary");
    
    TLatex lumi9;
    lumi9.SetTextSize(0.05);
    lumi9.DrawLatexNDC(0.65, 0.92, "54.9 fb^{-1} (13 TeV)");
    
    pad2_qcd_aco->cd();
    
    TH1D* ratio_aco_qcd = CreateRatio(&h_aco_data_QCD, &stack_aco_QCD, "ratio_aco_qcd");
    ratio_aco_qcd->GetXaxis()->SetTitle("Acoplanarity of the central system");
    ratio_aco_qcd->Draw("E");
    
    TLine line_aco_qcd(min_aco_QCD, 1.0, max_aco_QCD, 1.0);
    line_aco_qcd.SetLineStyle(2);
    line_aco_qcd.SetLineColor(kRed);
    line_aco_qcd.Draw();
    
    c9.SaveAs("output_plots/QCD_CR_acoplanarity.png");
    
    // ====================================
    // QCD Control Region - Mass
    // ====================================
    TCanvas c10("c10", "", 700, 800);
    
    TPad *pad1_qcd_m = new TPad("pad1_qcd_m", "pad1_qcd_m", 0, 0.3, 1, 1.0);
    pad1_qcd_m->SetBottomMargin(0.02);
    pad1_qcd_m->Draw();
    
    TPad *pad2_qcd_m = new TPad("pad2_qcd_m", "pad2_qcd_m", 0, 0, 1, 0.3);
    pad2_qcd_m->SetTopMargin(0.05);
    pad2_qcd_m->SetBottomMargin(0.35);
    pad2_qcd_m->Draw();
    
    pad1_qcd_m->cd();
    
    THStack stack_m_QCD("stack_m_QCD", "");
    stack_m_QCD.Add(&h_m_dy_QCD);       // Middle (yellow)

    stack_m_QCD.Add(&h_m_ttjets_QCD);   // Bottom (green)
    stack_m_QCD.Add(&h_m_qcd_QCD);      // Top (red) - QCD DOMINANT
    
    stack_m_QCD.Draw("hist");
    stack_m_QCD.GetYaxis()->SetTitle("Events");
    stack_m_QCD.GetYaxis()->SetTitleSize(0.06);
    stack_m_QCD.GetYaxis()->SetLabelSize(0.05);
    stack_m_QCD.GetYaxis()->SetTitleOffset(0.8);
    
    TH1D* unc_band_m_qcd = CreateUncertaintyBand(&stack_m_QCD, "unc_m_qcd");
    unc_band_m_qcd->Draw("E2 same");
    
    h_m_data_QCD.Draw("E same");
    
    TLegend *leg10 = new TLegend(0.65, 0.55, 0.90, 0.85);
    leg10->SetBorderSize(0);
    leg10->SetFillStyle(0);
    leg10->AddEntry(&h_m_data_QCD, "Data (2018)", "lep");
    leg10->AddEntry(&h_m_qcd_QCD, "QCD (Data driven)", "f");
    leg10->AddEntry(&h_m_dy_QCD, "Drell Yan", "f");
    leg10->AddEntry(&h_m_ttjets_QCD, "t#bar{t}", "f");
    leg10->Draw();
    
    TLatex cms10;
    cms10.SetTextSize(0.06);
    cms10.SetTextFont(62);
    cms10.DrawLatexNDC(0.15, 0.85, "CMS-TOTEM Preliminary");
    
    TLatex lumi10;
    lumi10.SetTextSize(0.05);
    lumi10.DrawLatexNDC(0.65, 0.92, "54.9 fb^{-1} (13 TeV)");
    
    pad2_qcd_m->cd();
    
    TH1D* ratio_m_qcd = CreateRatio(&h_m_data_QCD, &stack_m_QCD, "ratio_m_qcd");
    ratio_m_qcd->GetXaxis()->SetTitle("Invariant mass of the central system [GeV]");
    ratio_m_qcd->Draw("E");
    
    TLine line_m_qcd(min_m_QCD, 1.0, max_m_QCD, 1.0);
    line_m_qcd.SetLineStyle(2);
    line_m_qcd.SetLineColor(kRed);
    line_m_qcd.Draw();
    
    c10.SaveAs("output_plots/QCD_CR_mass.png");
    
    // ====================================
    // QCD Control Region - Rapidity
    // ====================================
    TCanvas c11("c11", "", 700, 800);
    
    TPad *pad1_qcd_r = new TPad("pad1_qcd_r", "pad1_qcd_r", 0, 0.3, 1, 1.0);
    pad1_qcd_r->SetBottomMargin(0.02);
    pad1_qcd_r->Draw();
    
    TPad *pad2_qcd_r = new TPad("pad2_qcd_r", "pad2_qcd_r", 0, 0, 1, 0.3);
    pad2_qcd_r->SetTopMargin(0.05);
    pad2_qcd_r->SetBottomMargin(0.35);
    pad2_qcd_r->Draw();
    
    pad1_qcd_r->cd();
    
    THStack stack_r_QCD("stack_r_QCD", "");
    stack_r_QCD.Add(&h_r_dy_QCD);       // Middle (yellow)
    stack_r_QCD.Add(&h_r_ttjets_QCD);   // Bottom (green)
    
    stack_r_QCD.Add(&h_r_qcd_QCD);      // Top (red) - QCD DOMINANT
    
    stack_r_QCD.Draw("hist");
    stack_r_QCD.GetYaxis()->SetTitle("Events");
    stack_r_QCD.GetYaxis()->SetTitleSize(0.06);
    stack_r_QCD.GetYaxis()->SetLabelSize(0.05);
    stack_r_QCD.GetYaxis()->SetTitleOffset(0.8);
    
    TH1D* unc_band_r_qcd = CreateUncertaintyBand(&stack_r_QCD, "unc_r_qcd");
    unc_band_r_qcd->Draw("E2 same");
    
    h_r_data_QCD.Draw("E same");
    
    TLegend *leg11 = new TLegend(0.65, 0.55, 0.90, 0.85);
    leg11->SetBorderSize(0);
    leg11->SetFillStyle(0);
    leg11->AddEntry(&h_r_data_QCD, "Data (2018)", "lep");
    leg11->AddEntry(&h_r_qcd_QCD, "QCD (Data driven)", "f");
    leg11->AddEntry(&h_r_dy_QCD, "Drell Yan", "f");
    leg11->AddEntry(&h_r_ttjets_QCD, "t#bar{t}", "f");
    leg11->Draw();
    
    TLatex cms11;
    cms11.SetTextSize(0.06);
    cms11.SetTextFont(62);
    cms11.DrawLatexNDC(0.15, 0.85, "CMS-TOTEM Preliminary");
    
    TLatex lumi11;
    lumi11.SetTextSize(0.05);
    lumi11.DrawLatexNDC(0.65, 0.92, "54.9 fb^{-1} (13 TeV)");
    
    pad2_qcd_r->cd();
    
    TH1D* ratio_r_qcd = CreateRatio(&h_r_data_QCD, &stack_r_QCD, "ratio_r_qcd");
    ratio_r_qcd->GetXaxis()->SetTitle("Rapidity of the central system");
    ratio_r_qcd->Draw("E");
    
    TLine line_r_qcd(min_r_QCD, 1.0, max_r_QCD, 1.0);
    line_r_qcd.SetLineStyle(2);
    line_r_qcd.SetLineColor(kRed);
    line_r_qcd.Draw();
    
    c11.SaveAs("output_plots/QCD_CR_rapidity.png");
    
    // ====================================
    // QCD Control Region - Transverse Momentum
    // ====================================
    TCanvas c12("c12", "", 700, 800);
    
    TPad *pad1_qcd_pt = new TPad("pad1_qcd_pt", "pad1_qcd_pt", 0, 0.3, 1, 1.0);
    pad1_qcd_pt->SetBottomMargin(0.02);
    pad1_qcd_pt->Draw();
    
    TPad *pad2_qcd_pt = new TPad("pad2_qcd_pt", "pad2_qcd_pt", 0, 0, 1, 0.3);
    pad2_qcd_pt->SetTopMargin(0.05);
    pad2_qcd_pt->SetBottomMargin(0.35);
    pad2_qcd_pt->Draw();
    
    pad1_qcd_pt->cd();
    
    THStack stack_pt_QCD("stack_pt_QCD", "");
    stack_pt_QCD.Add(&h_pt_dy_QCD);      // Middle (yellow)
    stack_pt_QCD.Add(&h_pt_ttjets_QCD);  // Bottom (green)
    
    stack_pt_QCD.Add(&h_pt_qcd_QCD);     // Top (red) - QCD DOMINANT
    
    stack_pt_QCD.Draw("hist");
    stack_pt_QCD.GetYaxis()->SetTitle("Events");
    stack_pt_QCD.GetYaxis()->SetTitleSize(0.06);
    stack_pt_QCD.GetYaxis()->SetLabelSize(0.05);
    stack_pt_QCD.GetYaxis()->SetTitleOffset(0.8);
    
    TH1D* unc_band_pt_qcd = CreateUncertaintyBand(&stack_pt_QCD, "unc_pt_qcd");
    unc_band_pt_qcd->Draw("E2 same");
    
    h_pt_data_QCD.Draw("E same");
    
    TLegend *leg12 = new TLegend(0.65, 0.55, 0.90, 0.85);
    leg12->SetBorderSize(0);
    leg12->SetFillStyle(0);
    leg12->AddEntry(&h_pt_data_QCD, "Data (2018)", "lep");
    leg12->AddEntry(&h_pt_qcd_QCD, "QCD (Data driven)", "f");
    leg12->AddEntry(&h_pt_dy_QCD, "Drell Yan", "f");
    leg12->AddEntry(&h_pt_ttjets_QCD, "t#bar{t}", "f");
    leg12->Draw();
    
    TLatex cms12;
    cms12.SetTextSize(0.06);
    cms12.SetTextFont(62);
    cms12.DrawLatexNDC(0.15, 0.85, "CMS-TOTEM Preliminary");
    
    TLatex lumi12;
    lumi12.SetTextSize(0.05);
    lumi12.DrawLatexNDC(0.65, 0.92, "54.9 fb^{-1} (13 TeV)");
    
    pad2_qcd_pt->cd();
    
    TH1D* ratio_pt_qcd = CreateRatio(&h_pt_data_QCD, &stack_pt_QCD, "ratio_pt_qcd");
    ratio_pt_qcd->GetXaxis()->SetTitle("Transverse momentum of the central system [GeV]");
    ratio_pt_qcd->Draw("E");
    
    TLine line_pt_qcd(min_pt_QCD, 1.0, max_pt_QCD, 1.0);
    line_pt_qcd.SetLineStyle(2);
    line_pt_qcd.SetLineColor(kRed);
    line_pt_qcd.Draw();
    
    c12.SaveAs("output_plots/QCD_CR_pt.png");

    cout << "All control region plots created successfully!" << endl;
    cout << "Generated 12 plots with CORRECT stacking order:" << endl;
    cout << "  DY Control Region: DY (yellow) on top - DY dominates" << endl;
    cout << "  TT Control Region: TT (green) on top - TT dominates" << endl;
    cout << "  QCD Control Region: QCD (red) on top - QCD dominates" << endl;
    cout << "Check the output_plots/ directory for all generated plots." << endl;
    
    bool interactive = false;
    if (interactive) {
        app.Run(true);
    }

    return 0;
}