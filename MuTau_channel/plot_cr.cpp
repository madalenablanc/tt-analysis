#include <iostream>
#include <cmath>
#include <vector>
#include "TApplication.h"
#include "THStack.h"
#include "TH1D.h"
#include "TCanvas.h"
#include "TFile.h"
#include "TLeaf.h"
#include "TTree.h"
#include "TPad.h"
#include "TLegend.h"
#include "TLatex.h"
#include "TStyle.h"
#include "TLine.h"
#include "TSystem.h"
#include "TColor.h"

using namespace std;

// Normalisation framework (thesis Section 3.5 / Appendix A.4.1):
//   w_DY     = DY_SCALE * event_weight * EPSILON_PROTON
//   w_ttjets = event_weight * EPSILON_PROTON   (0.15 already baked into event_weight)
//   w_QCD = w_data = 1.0 (real detector pileup already present, no acceptance correction)
const double DY_SCALE = 1.004e-4;
const double EPSILON_PROTON = 0.245;
const bool REQUIRE_DOUBLE_PROTON_TAG = true;

// Helper: return the value of the first existing leaf
double GetLeafSafe(TTree* tree,
                   std::initializer_list<const char*> names,
                   double def = 0.0)
{
    for (const char* name : names) {
        TLeaf* lf = tree->GetLeaf(name);
        if (lf) return lf->GetValue(0);
    }
    return def;
}

bool PassProtonSelection(TTree* tree,
                         bool has_protons,
                         const vector<int>* v_arm,
                         const vector<float>* v_xi)
{
    if (!REQUIRE_DOUBLE_PROTON_TAG || !has_protons) return true;
    if (!v_arm || !v_xi) return false;

    double xi1 = -1.0, xi2 = -1.0;
    for (size_t j = 0; j < v_arm->size(); ++j) {
        if ((*v_arm)[j] == 0 && xi1 < 0) xi1 = (*v_xi)[j];
        if ((*v_arm)[j] == 1 && xi2 < 0) xi2 = (*v_xi)[j];
    }
    return (xi1 > 0.0 && xi2 > 0.0);
}

bool PassMuTauBaseline(TTree* tree, bool require_os, bool require_ss)
{
    const double mu_eta  = GetLeafSafe(tree, {"mu_eta", "muon_eta"}, 999.0);
    const double tau_eta = GetLeafSafe(tree, {"tau_eta"}, 999.0);
    const double mu_ch   = GetLeafSafe(tree, {"mu_charge", "muon_charge"});
    const double tau_ch  = GetLeafSafe(tree, {"tau_charge"});

    const bool pass_eta = (std::fabs(mu_eta) < 2.4) && (std::fabs(tau_eta) < 2.4);
    const bool pass_os  = !require_os || (mu_ch * tau_ch < 0.0);
    const bool pass_ss  = !require_ss || (mu_ch * tau_ch > 0.0);

    return pass_eta && pass_os && pass_ss;
}

// Function to create ratio plot (style matches plot_shapes.py draw_and_save)
TH1D* CreateRatio(TH1D* data, THStack* mc_stack, const char* name) {
    TH1D* mc_total = (TH1D*)mc_stack->GetStack()->Last()->Clone();

    TH1D* ratio = (TH1D*)data->Clone(name);
    ratio->Divide(mc_total);

    ratio->SetTitle("");
    ratio->SetMarkerStyle(20);
    ratio->SetMarkerSize(0.8);
    ratio->SetLineColor(kBlack);
    ratio->GetYaxis()->SetTitle("Data / Bkg");
    ratio->GetYaxis()->SetTitleSize(0.12);
    ratio->GetYaxis()->SetLabelSize(0.10);
    ratio->GetYaxis()->SetTitleOffset(0.55);
    ratio->GetYaxis()->SetRangeUser(0.55, 1.45);
    ratio->GetYaxis()->SetNdivisions(505);

    ratio->GetXaxis()->SetTitleSize(0.10);
    ratio->GetXaxis()->SetLabelSize(0.10);
    ratio->GetXaxis()->SetTitleOffset(1.1);
    ratio->GetXaxis()->SetNdivisions(5);
    ratio->GetXaxis()->CenterTitle(true);
    ratio->GetXaxis()->SetTickLength(0.08);

    return ratio;
}

// Function to create uncertainty band (style matches plot_shapes.py h_mcsum)
TH1D* CreateUncertaintyBand(THStack* mc_stack, const char* name) {
    TH1D* mc_total = (TH1D*)mc_stack->GetStack()->Last()->Clone(name);

    for (int i = 1; i <= mc_total->GetNbinsX(); ++i) {
        double content = mc_total->GetBinContent(i);
        double error = sqrt(content);
        mc_total->SetBinError(i, error);
    }

    mc_total->SetFillColor(kGray+2);
    mc_total->SetFillStyle(3354);
    mc_total->SetMarkerSize(0);
    mc_total->SetLineWidth(0);

    return mc_total;
}

// CMS "Work in progress" + luminosity labels, matching plot_shapes.py add_standard_labels
void AddStandardLabels() {
    TLatex cms;
    cms.SetNDC(true);
    cms.SetTextFont(62);
    cms.SetTextSize(0.065);
    cms.DrawLatex(0.16, 0.93, "CMS");

    TLatex status;
    status.SetNDC(true);
    status.SetTextFont(52);
    status.SetTextSize(0.052);
    status.DrawLatex(0.16, 0.89, "Work in progress");

    TLatex lumi;
    lumi.SetNDC(true);
    lumi.SetTextFont(62);
    lumi.SetTextSize(0.052);
    lumi.SetTextAlign(31);
    lumi.DrawLatex(0.94, 0.93, "54.9 fb^{-1} (13 TeV)");
}

int main(){

    TApplication app("app",NULL,NULL);
    gStyle->SetOptStat(0);

    // Open files
    TFile fdata  ("/eos/user/m/mblancco/samples_2018_mutau/final_samples/Data_2018_UL_MuTau_merged.root");
    TFile fdy    ("/eos/user/m/mblancco/samples_2018_mutau/final_samples/DY_2018_UL_MuTau_merged.root");
    TFile fqcd   ("/eos/user/m/mblancco/samples_2018_mutau/final_samples/QCD_2018_UL_MuTau_merged.root");
    TFile fttjets("/eos/user/m/mblancco/samples_2018_mutau/final_samples/ttjets_2018_UL_MuTau_merged.root");

    TTree* tree_data   = (TTree*) fdata  .Get("tree");
    TTree* tree_dy     = (TTree*) fdy    .Get("tree");
    TTree* tree_qcd    = (TTree*) fqcd   .Get("tree");
    TTree* tree_ttjets = (TTree*) fttjets.Get("tree");

    // Control Region parameters
    // DY CR
    double min_aco_DY=0.05, max_aco_DY=0.30; int bin_aco_DY=12;
    double min_m_DY=40,     max_m_DY=100;    int bin_m_DY=12;
    double min_pt_DY=150,   max_pt_DY=400;   int bin_pt_DY=12;
    double min_r_DY=-2,     max_r_DY=2;      int bin_r_DY=12;

    // TT CR
    double min_aco_TT=0.5,  max_aco_TT=1.0;  int bin_aco_TT=7;
    double min_m_TT=400,    max_m_TT=800;    int bin_m_TT=7;
    double min_pt_TT=0,     max_pt_TT=400;   int bin_pt_TT=10;
    double min_r_TT=-2,     max_r_TT=2;      int bin_r_TT=10;

    // QCD CR
    double min_aco_QCD=0.0, max_aco_QCD=0.8; int bin_aco_QCD=20;
    double min_m_QCD=100,   max_m_QCD=500;   int bin_m_QCD=16;
    double min_pt_QCD=0,    max_pt_QCD=500;  int bin_pt_QCD=13;
    double min_r_QCD=-2,    max_r_QCD=2;     int bin_r_QCD=20;

    // Create histograms - DY CR
    TH1D h_aco_data_DY("h_aco_data_DY","",bin_aco_DY, min_aco_DY, max_aco_DY);
    TH1D h_aco_dy_DY("h_aco_dy_DY","",bin_aco_DY, min_aco_DY, max_aco_DY);
    TH1D h_aco_qcd_DY("h_aco_qcd_DY","",bin_aco_DY, min_aco_DY, max_aco_DY);
    TH1D h_aco_ttjets_DY("h_aco_ttjets_DY","",bin_aco_DY, min_aco_DY, max_aco_DY);

    TH1D h_m_data_DY("h_m_data_DY","",bin_m_DY, min_m_DY, max_m_DY);
    TH1D h_m_dy_DY("h_m_dy_DY","",bin_m_DY, min_m_DY, max_m_DY);
    TH1D h_m_qcd_DY("h_m_qcd_DY","",bin_m_DY, min_m_DY, max_m_DY);
    TH1D h_m_ttjets_DY("h_m_ttjets_DY","",bin_m_DY, min_m_DY, max_m_DY);

    TH1D h_pt_data_DY("h_pt_data_DY","",bin_pt_DY, min_pt_DY, max_pt_DY);
    TH1D h_pt_dy_DY("h_pt_dy_DY","",bin_pt_DY, min_pt_DY, max_pt_DY);
    TH1D h_pt_qcd_DY("h_pt_qcd_DY","",bin_pt_DY, min_pt_DY, max_pt_DY);
    TH1D h_pt_ttjets_DY("h_pt_ttjets_DY","",bin_pt_DY, min_pt_DY, max_pt_DY);

    TH1D h_r_data_DY("h_r_data_DY","",bin_r_DY, min_r_DY, max_r_DY);
    TH1D h_r_dy_DY("h_r_dy_DY","",bin_r_DY, min_r_DY, max_r_DY);
    TH1D h_r_qcd_DY("h_r_qcd_DY","",bin_r_DY, min_r_DY, max_r_DY);
    TH1D h_r_ttjets_DY("h_r_ttjets_DY","",bin_r_DY, min_r_DY, max_r_DY);

    // TT CR
    TH1D h_aco_data_TT("h_aco_data_TT","",bin_aco_TT, min_aco_TT, max_aco_TT);
    TH1D h_aco_dy_TT("h_aco_dy_TT","",bin_aco_TT, min_aco_TT, max_aco_TT);
    TH1D h_aco_qcd_TT("h_aco_qcd_TT","",bin_aco_TT, min_aco_TT, max_aco_TT);
    TH1D h_aco_ttjets_TT("h_aco_ttjets_TT","",bin_aco_TT, min_aco_TT, max_aco_TT);

    TH1D h_m_data_TT("h_m_data_TT","",bin_m_TT, min_m_TT, max_m_TT);
    TH1D h_m_dy_TT("h_m_dy_TT","",bin_m_TT, min_m_TT, max_m_TT);
    TH1D h_m_qcd_TT("h_m_qcd_TT","",bin_m_TT, min_m_TT, max_m_TT);
    TH1D h_m_ttjets_TT("h_m_ttjets_TT","",bin_m_TT, min_m_TT, max_m_TT);

    TH1D h_pt_data_TT("h_pt_data_TT","",bin_pt_TT, min_pt_TT, max_pt_TT);
    TH1D h_pt_dy_TT("h_pt_dy_TT","",bin_pt_TT, min_pt_TT, max_pt_TT);
    TH1D h_pt_qcd_TT("h_pt_qcd_TT","",bin_pt_TT, min_pt_TT, max_pt_TT);
    TH1D h_pt_ttjets_TT("h_pt_ttjets_TT","",bin_pt_TT, min_pt_TT, max_pt_TT);

    TH1D h_r_data_TT("h_r_data_TT","",bin_r_TT, min_r_TT, max_r_TT);
    TH1D h_r_dy_TT("h_r_dy_TT","",bin_r_TT, min_r_TT, max_r_TT);
    TH1D h_r_qcd_TT("h_r_qcd_TT","",bin_r_TT, min_r_TT, max_r_TT);
    TH1D h_r_ttjets_TT("h_r_ttjets_TT","",bin_r_TT, min_r_TT, max_r_TT);

    // QCD CR (data points from OS data tree, "QCD" component from SS data-driven tree)
    TH1D h_aco_data_QCD("h_aco_data_QCD","",bin_aco_QCD, min_aco_QCD, max_aco_QCD);
    TH1D h_aco_dy_QCD("h_aco_dy_QCD","",bin_aco_QCD, min_aco_QCD, max_aco_QCD);
    TH1D h_aco_qcd_QCD("h_aco_qcd_QCD","",bin_aco_QCD, min_aco_QCD, max_aco_QCD);
    TH1D h_aco_ttjets_QCD("h_aco_ttjets_QCD","",bin_aco_QCD, min_aco_QCD, max_aco_QCD);

    TH1D h_m_data_QCD("h_m_data_QCD","",bin_m_QCD, min_m_QCD, max_m_QCD);
    TH1D h_m_dy_QCD("h_m_dy_QCD","",bin_m_QCD, min_m_QCD, max_m_QCD);
    TH1D h_m_qcd_QCD("h_m_qcd_QCD","",bin_m_QCD, min_m_QCD, max_m_QCD);
    TH1D h_m_ttjets_QCD("h_m_ttjets_QCD","",bin_m_QCD, min_m_QCD, max_m_QCD);

    TH1D h_pt_data_QCD("h_pt_data_QCD","",bin_pt_QCD, min_pt_QCD, max_pt_QCD);
    TH1D h_pt_dy_QCD("h_pt_dy_QCD","",bin_pt_QCD, min_pt_QCD, max_pt_QCD);
    TH1D h_pt_qcd_QCD("h_pt_qcd_QCD","",bin_pt_QCD, min_pt_QCD, max_pt_QCD);
    TH1D h_pt_ttjets_QCD("h_pt_ttjets_QCD","",bin_pt_QCD, min_pt_QCD, max_pt_QCD);

    TH1D h_r_data_QCD("h_r_data_QCD","",bin_r_QCD, min_r_QCD, max_r_QCD);
    TH1D h_r_dy_QCD("h_r_dy_QCD","",bin_r_QCD, min_r_QCD, max_r_QCD);
    TH1D h_r_qcd_QCD("h_r_qcd_QCD","",bin_r_QCD, min_r_QCD, max_r_QCD);
    TH1D h_r_ttjets_QCD("h_r_ttjets_QCD","",bin_r_QCD, min_r_QCD, max_r_QCD);

    // Fill histograms
    cout << "Filling histograms..." << endl;
    cout << "Filling histograms: DATA" << endl;

    // Fill data
    {
        vector<int>*   v_arm = nullptr;
        vector<float>* v_xi  = nullptr;
        bool has_p = (tree_data->GetBranch("proton_multi_arm") &&
                      tree_data->GetBranch("proton_multi_xi"));
        if (has_p) {
            tree_data->SetBranchAddress("proton_multi_arm", &v_arm);
            tree_data->SetBranchAddress("proton_multi_xi",  &v_xi);
        }

        int n_evt_data = tree_data->GetEntries();
        for(int i=0; i<n_evt_data; i++){
            tree_data->GetEntry(i);
            if (!PassProtonSelection(tree_data, has_p, v_arm, v_xi)) continue;
            if (!PassMuTauBaseline(tree_data, true, false)) continue;

            double sist_mass = GetLeafSafe(tree_data,{"sist_mass"},-1.0);
            if(sist_mass < 0) continue;

            double acop     = GetLeafSafe(tree_data,{"sist_acop","acop"});
            double sist_pt  = GetLeafSafe(tree_data,{"sist_pt"});
            double sist_rap = GetLeafSafe(tree_data,{"sist_rap"});
            int    n_b      = (int) GetLeafSafe(tree_data,{"n_b_jet"});
            double w_data   = 1.0;

            // muTau CR definitions:
            //   DY  : 40 <= M <= 100, a < 0.3, Nb = 0, OS
            //   TT  : M > 400,        a > 0.3, Nb >= 1, OS
            //   QCD : M > 100,        a < 0.8, Nb = 0, SS-driven background
            bool isDY_CR  = (sist_mass >= 40 && sist_mass <= 100) && (acop < 0.3) && (n_b == 0);
            bool isTT_CR  = (n_b >= 1) && (acop > 0.3) && (sist_mass > 400);
            bool isQCD_CR = (sist_mass > 100) && (n_b == 0) && (acop < 0.8);

            if(isDY_CR) {
                h_aco_data_DY.Fill(acop,w_data);
                h_m_data_DY.Fill(sist_mass,w_data);
                h_pt_data_DY.Fill(sist_pt,w_data);
                h_r_data_DY.Fill(sist_rap,w_data);
            }
            if(isTT_CR) {
                h_aco_data_TT.Fill(acop,w_data);
                h_m_data_TT.Fill(sist_mass,w_data);
                h_pt_data_TT.Fill(sist_pt,w_data);
                h_r_data_TT.Fill(sist_rap,w_data);
            }
            if(isQCD_CR) {
                h_aco_data_QCD.Fill(acop,w_data);
                h_m_data_QCD.Fill(sist_mass,w_data);
                h_pt_data_QCD.Fill(sist_pt,w_data);
                h_r_data_QCD.Fill(sist_rap,w_data);
            }
        }
    }

    cout << "Filling histograms: DY" << endl;

    // Fill DY MC
    {
        vector<int>*   v_arm = nullptr;
        vector<float>* v_xi  = nullptr;
        bool has_p = (tree_dy->GetBranch("proton_multi_arm") &&
                      tree_dy->GetBranch("proton_multi_xi"));
        if (has_p) {
            tree_dy->SetBranchAddress("proton_multi_arm", &v_arm);
            tree_dy->SetBranchAddress("proton_multi_xi",  &v_xi);
        }

        int n_evt_dy = tree_dy->GetEntries();
        for(int i=0; i<n_evt_dy; i++){
            tree_dy->GetEntry(i);
            if (!PassProtonSelection(tree_dy, has_p, v_arm, v_xi)) continue;
            if (!PassMuTauBaseline(tree_dy, true, false)) continue;

            double sist_mass = GetLeafSafe(tree_dy,{"sist_mass"},-1.0);
            if(sist_mass < 0) continue;

            double acop     = GetLeafSafe(tree_dy,{"sist_acop","acop"});
            double sist_pt  = GetLeafSafe(tree_dy,{"sist_pt"});
            double sist_rap = GetLeafSafe(tree_dy,{"sist_rap"});
            int    n_b      = (int) GetLeafSafe(tree_dy,{"n_b_jet"});
            double w_dy     = GetLeafSafe(tree_dy,{"event_weight","weight"},1.0) * DY_SCALE * EPSILON_PROTON;

            bool isDY_CR  = (sist_mass >= 40 && sist_mass <= 100) && (acop < 0.3) && (n_b == 0);
            bool isTT_CR  = (n_b >= 1) && (acop > 0.3) && (sist_mass > 400);
            bool isQCD_CR = (sist_mass > 100) && (n_b == 0) && (acop < 0.8);

            if(isDY_CR) {
                h_aco_dy_DY.Fill(acop,w_dy);
                h_m_dy_DY.Fill(sist_mass,w_dy);
                h_pt_dy_DY.Fill(sist_pt,w_dy);
                h_r_dy_DY.Fill(sist_rap,w_dy);
            }
            if(isTT_CR) {
                h_aco_dy_TT.Fill(acop,w_dy);
                h_m_dy_TT.Fill(sist_mass,w_dy);
                h_pt_dy_TT.Fill(sist_pt,w_dy);
                h_r_dy_TT.Fill(sist_rap,w_dy);
            }
            if(isQCD_CR) {
                h_aco_dy_QCD.Fill(acop,w_dy);
                h_m_dy_QCD.Fill(sist_mass,w_dy);
                h_pt_dy_QCD.Fill(sist_pt,w_dy);
                h_r_dy_QCD.Fill(sist_rap,w_dy);
            }
        }
    }

    cout << "Filling histograms: QCD" << endl;

    // Fill QCD (SS data-driven)
    {
        vector<int>*   v_arm = nullptr;
        vector<float>* v_xi  = nullptr;
        bool has_p = (tree_qcd->GetBranch("proton_multi_arm") &&
                      tree_qcd->GetBranch("proton_multi_xi"));
        if (has_p) {
            tree_qcd->SetBranchAddress("proton_multi_arm", &v_arm);
            tree_qcd->SetBranchAddress("proton_multi_xi",  &v_xi);
        }

        int n_evt_qcd = tree_qcd->GetEntries();
        for(int i=0; i<n_evt_qcd; i++){
            tree_qcd->GetEntry(i);
            if (!PassProtonSelection(tree_qcd, has_p, v_arm, v_xi)) continue;
            if (!PassMuTauBaseline(tree_qcd, false, true)) continue;

            double sist_mass = GetLeafSafe(tree_qcd,{"sist_mass"},-1.0);
            if(sist_mass < 0) continue;

            double acop     = GetLeafSafe(tree_qcd,{"sist_acop","acop"});
            double sist_pt  = GetLeafSafe(tree_qcd,{"sist_pt"});
            double sist_rap = GetLeafSafe(tree_qcd,{"sist_rap"});
            int    n_b      = (int) GetLeafSafe(tree_qcd,{"n_b_jet"});

            bool isDY_CR  = (sist_mass >= 40 && sist_mass <= 100) && (acop < 0.3) && (n_b == 0);
            bool isTT_CR  = (n_b >= 1) && (acop > 0.3) && (sist_mass > 400);
            bool isQCD_CR = (sist_mass > 100) && (n_b == 0) && (acop < 0.8);

            if(isDY_CR) {
                h_aco_qcd_DY.Fill(acop,1);
                h_m_qcd_DY.Fill(sist_mass,1);
                h_pt_qcd_DY.Fill(sist_pt,1);
                h_r_qcd_DY.Fill(sist_rap,1);
            }
            if(isTT_CR) {
                h_aco_qcd_TT.Fill(acop,1);
                h_m_qcd_TT.Fill(sist_mass,1);
                h_pt_qcd_TT.Fill(sist_pt,1);
                h_r_qcd_TT.Fill(sist_rap,1);
            }
            if(isQCD_CR) {
                h_aco_qcd_QCD.Fill(acop,1);
                h_m_qcd_QCD.Fill(sist_mass,1);
                h_pt_qcd_QCD.Fill(sist_pt,1);
                h_r_qcd_QCD.Fill(sist_rap,1);
            }
        }
    }

    cout << "Filling histograms: TTJETS" << endl;

    // Fill ttjets MC
    {
        vector<int>*   v_arm = nullptr;
        vector<float>* v_xi  = nullptr;
        bool has_p = (tree_ttjets->GetBranch("proton_multi_arm") &&
                      tree_ttjets->GetBranch("proton_multi_xi"));
        if (has_p) {
            tree_ttjets->SetBranchAddress("proton_multi_arm", &v_arm);
            tree_ttjets->SetBranchAddress("proton_multi_xi",  &v_xi);
        }

        int n_evt_ttjets = tree_ttjets->GetEntries();
        for(int i=0; i<n_evt_ttjets; i++){
            tree_ttjets->GetEntry(i);
            if (!PassProtonSelection(tree_ttjets, has_p, v_arm, v_xi)) continue;
            if (!PassMuTauBaseline(tree_ttjets, true, false)) continue;

            double sist_mass = GetLeafSafe(tree_ttjets,{"sist_mass"},-1.0);
            if(sist_mass < 0) continue;

            double acop     = GetLeafSafe(tree_ttjets,{"sist_acop","acop"});
            double sist_pt  = GetLeafSafe(tree_ttjets,{"sist_pt"});
            double sist_rap = GetLeafSafe(tree_ttjets,{"sist_rap"});
            int    n_b      = (int) GetLeafSafe(tree_ttjets,{"n_b_jet"});
            double w_ttjets = GetLeafSafe(tree_ttjets,{"event_weight","weight"},1.0) * EPSILON_PROTON;

            bool isDY_CR  = (sist_mass >= 40 && sist_mass <= 100) && (acop < 0.3) && (n_b == 0);
            bool isTT_CR  = (n_b >= 1) && (acop > 0.3) && (sist_mass > 400);
            bool isQCD_CR = (sist_mass > 100) && (n_b == 0) && (acop < 0.8);

            if(isDY_CR) {
                h_aco_ttjets_DY.Fill(acop,w_ttjets);
                h_m_ttjets_DY.Fill(sist_mass,w_ttjets);
                h_pt_ttjets_DY.Fill(sist_pt,w_ttjets);
                h_r_ttjets_DY.Fill(sist_rap,w_ttjets);
            }
            if(isTT_CR) {
                h_aco_ttjets_TT.Fill(acop,w_ttjets);
                h_m_ttjets_TT.Fill(sist_mass,w_ttjets);
                h_pt_ttjets_TT.Fill(sist_pt,w_ttjets);
                h_r_ttjets_TT.Fill(sist_rap,w_ttjets);
            }
            if(isQCD_CR) {
                h_aco_ttjets_QCD.Fill(acop,w_ttjets);
                h_m_ttjets_QCD.Fill(sist_mass,w_ttjets);
                h_pt_ttjets_QCD.Fill(sist_pt,w_ttjets);
                h_r_ttjets_QCD.Fill(sist_rap,w_ttjets);
            }
        }
    }

    cout << "Filling histograms: COMPLETE" << endl;

    cout << "DY CR - Data events: " << h_aco_data_DY.GetEntries() << endl;
    cout << "DY CR - DY MC events: " << h_aco_dy_DY.GetEntries() << endl;
    cout << "DY CR - QCD events: " << h_aco_qcd_DY.GetEntries() << endl;
    cout << "DY CR - ttjets MC events: " << h_aco_ttjets_DY.GetEntries() << endl;

    cout << "TT CR - Data events: " << h_aco_data_TT.GetEntries() << endl;
    cout << "TT CR - DY MC events: " << h_aco_dy_TT.GetEntries() << endl;
    cout << "TT CR - QCD events: " << h_aco_qcd_TT.GetEntries() << endl;
    cout << "TT CR - ttjets MC events: " << h_aco_ttjets_TT.GetEntries() << endl;

    cout << "QCD CR - Data events: " << h_aco_data_QCD.GetEntries() << endl;
    cout << "QCD CR - DY MC events: " << h_aco_dy_QCD.GetEntries() << endl;
    cout << "QCD CR - QCD events: " << h_aco_qcd_QCD.GetEntries() << endl;
    cout << "QCD CR - ttjets MC events: " << h_aco_ttjets_QCD.GetEntries() << endl;

    // Set colors DY CR
    h_aco_dy_DY.SetFillColor(TColor::GetColor("#ffd92f")); h_aco_dy_DY.SetLineColor(kBlack); h_aco_dy_DY.SetLineWidth(1);
    h_aco_qcd_DY.SetFillColor(TColor::GetColor("#fc8d62")); h_aco_qcd_DY.SetLineColor(kBlack); h_aco_qcd_DY.SetLineWidth(1);
    h_aco_ttjets_DY.SetFillColor(TColor::GetColor("#66c2a5")); h_aco_ttjets_DY.SetLineColor(kBlack); h_aco_ttjets_DY.SetLineWidth(1);

    h_m_dy_DY.SetFillColor(TColor::GetColor("#ffd92f")); h_m_dy_DY.SetLineColor(kBlack); h_m_dy_DY.SetLineWidth(1);
    h_m_qcd_DY.SetFillColor(TColor::GetColor("#fc8d62")); h_m_qcd_DY.SetLineColor(kBlack); h_m_qcd_DY.SetLineWidth(1);
    h_m_ttjets_DY.SetFillColor(TColor::GetColor("#66c2a5")); h_m_ttjets_DY.SetLineColor(kBlack); h_m_ttjets_DY.SetLineWidth(1);

    h_r_dy_DY.SetFillColor(TColor::GetColor("#ffd92f")); h_r_dy_DY.SetLineColor(kBlack); h_r_dy_DY.SetLineWidth(1);
    h_r_qcd_DY.SetFillColor(TColor::GetColor("#fc8d62")); h_r_qcd_DY.SetLineColor(kBlack); h_r_qcd_DY.SetLineWidth(1);
    h_r_ttjets_DY.SetFillColor(TColor::GetColor("#66c2a5")); h_r_ttjets_DY.SetLineColor(kBlack); h_r_ttjets_DY.SetLineWidth(1);

    h_pt_dy_DY.SetFillColor(TColor::GetColor("#ffd92f")); h_pt_dy_DY.SetLineColor(kBlack); h_pt_dy_DY.SetLineWidth(1);
    h_pt_qcd_DY.SetFillColor(TColor::GetColor("#fc8d62")); h_pt_qcd_DY.SetLineColor(kBlack); h_pt_qcd_DY.SetLineWidth(1);
    h_pt_ttjets_DY.SetFillColor(TColor::GetColor("#66c2a5")); h_pt_ttjets_DY.SetLineColor(kBlack); h_pt_ttjets_DY.SetLineWidth(1);

    h_aco_data_DY.SetMarkerStyle(20); h_aco_data_DY.SetMarkerSize(1.0);
    h_m_data_DY.SetMarkerStyle(20); h_m_data_DY.SetMarkerSize(1.0);
    h_r_data_DY.SetMarkerStyle(20); h_r_data_DY.SetMarkerSize(1.0);
    h_pt_data_DY.SetMarkerStyle(20); h_pt_data_DY.SetMarkerSize(1.0);

    // TT
    h_aco_dy_TT.SetFillColor(TColor::GetColor("#ffd92f")); h_aco_dy_TT.SetLineColor(kBlack); h_aco_dy_TT.SetLineWidth(1);
    h_aco_qcd_TT.SetFillColor(TColor::GetColor("#fc8d62")); h_aco_qcd_TT.SetLineColor(kBlack); h_aco_qcd_TT.SetLineWidth(1);
    h_aco_ttjets_TT.SetFillColor(TColor::GetColor("#66c2a5")); h_aco_ttjets_TT.SetLineColor(kBlack); h_aco_ttjets_TT.SetLineWidth(1);

    h_m_dy_TT.SetFillColor(TColor::GetColor("#ffd92f")); h_m_dy_TT.SetLineColor(kBlack); h_m_dy_TT.SetLineWidth(1);
    h_m_qcd_TT.SetFillColor(TColor::GetColor("#fc8d62")); h_m_qcd_TT.SetLineColor(kBlack); h_m_qcd_TT.SetLineWidth(1);
    h_m_ttjets_TT.SetFillColor(TColor::GetColor("#66c2a5")); h_m_ttjets_TT.SetLineColor(kBlack); h_m_ttjets_TT.SetLineWidth(1);

    h_r_dy_TT.SetFillColor(TColor::GetColor("#ffd92f")); h_r_dy_TT.SetLineColor(kBlack); h_r_dy_TT.SetLineWidth(1);
    h_r_qcd_TT.SetFillColor(TColor::GetColor("#fc8d62")); h_r_qcd_TT.SetLineColor(kBlack); h_r_qcd_TT.SetLineWidth(1);
    h_r_ttjets_TT.SetFillColor(TColor::GetColor("#66c2a5")); h_r_ttjets_TT.SetLineColor(kBlack); h_r_ttjets_TT.SetLineWidth(1);

    h_pt_dy_TT.SetFillColor(TColor::GetColor("#ffd92f")); h_pt_dy_TT.SetLineColor(kBlack); h_pt_dy_TT.SetLineWidth(1);
    h_pt_qcd_TT.SetFillColor(TColor::GetColor("#fc8d62")); h_pt_qcd_TT.SetLineColor(kBlack); h_pt_qcd_TT.SetLineWidth(1);
    h_pt_ttjets_TT.SetFillColor(TColor::GetColor("#66c2a5")); h_pt_ttjets_TT.SetLineColor(kBlack); h_pt_ttjets_TT.SetLineWidth(1);

    h_aco_data_TT.SetMarkerStyle(20); h_aco_data_TT.SetMarkerSize(1.0);
    h_m_data_TT.SetMarkerStyle(20); h_m_data_TT.SetMarkerSize(1.0);
    h_r_data_TT.SetMarkerStyle(20); h_r_data_TT.SetMarkerSize(1.0);
    h_pt_data_TT.SetMarkerStyle(20); h_pt_data_TT.SetMarkerSize(1.0);

    // QCD
    h_aco_dy_QCD.SetFillColor(TColor::GetColor("#ffd92f")); h_aco_dy_QCD.SetLineColor(kBlack); h_aco_dy_QCD.SetLineWidth(1);
    h_aco_qcd_QCD.SetFillColor(TColor::GetColor("#fc8d62")); h_aco_qcd_QCD.SetLineColor(kBlack); h_aco_qcd_QCD.SetLineWidth(1);
    h_aco_ttjets_QCD.SetFillColor(TColor::GetColor("#66c2a5")); h_aco_ttjets_QCD.SetLineColor(kBlack); h_aco_ttjets_QCD.SetLineWidth(1);

    h_m_dy_QCD.SetFillColor(TColor::GetColor("#ffd92f")); h_m_dy_QCD.SetLineColor(kBlack); h_m_dy_QCD.SetLineWidth(1);
    h_m_qcd_QCD.SetFillColor(TColor::GetColor("#fc8d62")); h_m_qcd_QCD.SetLineColor(kBlack); h_m_qcd_QCD.SetLineWidth(1);
    h_m_ttjets_QCD.SetFillColor(TColor::GetColor("#66c2a5")); h_m_ttjets_QCD.SetLineColor(kBlack); h_m_ttjets_QCD.SetLineWidth(1);

    h_r_dy_QCD.SetFillColor(TColor::GetColor("#ffd92f")); h_r_dy_QCD.SetLineColor(kBlack); h_r_dy_QCD.SetLineWidth(1);
    h_r_qcd_QCD.SetFillColor(TColor::GetColor("#fc8d62")); h_r_qcd_QCD.SetLineColor(kBlack); h_r_qcd_QCD.SetLineWidth(1);
    h_r_ttjets_QCD.SetFillColor(TColor::GetColor("#66c2a5")); h_r_ttjets_QCD.SetLineColor(kBlack); h_r_ttjets_QCD.SetLineWidth(1);

    h_pt_dy_QCD.SetFillColor(TColor::GetColor("#ffd92f")); h_pt_dy_QCD.SetLineColor(kBlack); h_pt_dy_QCD.SetLineWidth(1);
    h_pt_qcd_QCD.SetFillColor(TColor::GetColor("#fc8d62")); h_pt_qcd_QCD.SetLineColor(kBlack); h_pt_qcd_QCD.SetLineWidth(1);
    h_pt_ttjets_QCD.SetFillColor(TColor::GetColor("#66c2a5")); h_pt_ttjets_QCD.SetLineColor(kBlack); h_pt_ttjets_QCD.SetLineWidth(1);

    h_aco_data_QCD.SetMarkerStyle(20); h_aco_data_QCD.SetMarkerSize(1.0);
    h_m_data_QCD.SetMarkerStyle(20); h_m_data_QCD.SetMarkerSize(1.0);
    h_r_data_QCD.SetMarkerStyle(20); h_r_data_QCD.SetMarkerSize(1.0);
    h_pt_data_QCD.SetMarkerStyle(20); h_pt_data_QCD.SetMarkerSize(1.0);

    cout << "Creating plots..." << endl;
    gSystem->Exec("mkdir -p output_plots_cr");

    // ====================================
    // DY Control Region - Acoplanarity
    // ====================================
    TCanvas c1("c1", "", 700, 800);

    TPad *pad1 = new TPad("pad1", "pad1", 0, 0.3, 1, 1.0);
    pad1->SetBottomMargin(0.02);
    pad1->SetLeftMargin(0.16);
    pad1->SetRightMargin(0.06);
    pad1->SetTopMargin(0.12);
    pad1->Draw();

    TPad *pad2 = new TPad("pad2", "pad2", 0, 0, 1, 0.3);
    pad2->SetLeftMargin(0.16);
    pad2->SetRightMargin(0.06);
    pad2->SetTopMargin(0.02);
    pad2->SetBottomMargin(0.42);
    pad2->Draw();

    pad1->cd();

    THStack stack_aco_DY("stack_aco_DY", "");
    stack_aco_DY.Add(&h_aco_ttjets_DY);
    stack_aco_DY.Add(&h_aco_qcd_DY);
    stack_aco_DY.Add(&h_aco_dy_DY);

    stack_aco_DY.SetMaximum(std::max(stack_aco_DY.GetMaximum(), h_aco_data_DY.GetMaximum()) * 1.2);
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
    leg1->SetTextFont(42);
    leg1->SetTextSize(0.042);
    leg1->SetMargin(0.15);
    leg1->AddEntry(&h_aco_data_DY, "Data (2018)", "lep");
    leg1->AddEntry(&h_aco_dy_DY, "Drell Yan", "f");
    leg1->AddEntry(&h_aco_qcd_DY, "QCD (Data driven)", "f");
    leg1->AddEntry(&h_aco_ttjets_DY, "t#bar{t}", "f");
    leg1->Draw();

    AddStandardLabels();

    pad2->cd();

    TH1D* ratio_aco = CreateRatio(&h_aco_data_DY, &stack_aco_DY, "ratio_aco");
    ratio_aco->GetXaxis()->SetTitle("Acoplanarity of the central system");
    ratio_aco->Draw("E");

    TLine line_aco(min_aco_DY, 1.0, max_aco_DY, 1.0);
    line_aco.SetLineStyle(2);
    line_aco.SetLineColor(kGray+2);
    line_aco.Draw();

    c1.SaveAs("output_plots_cr/DY_CR_acoplanarity.png");

    // ====================================
    // DY Control Region - Mass
    // ====================================
    TCanvas c2("c2", "", 700, 800);

    TPad *pad1_m = new TPad("pad1_m", "pad1_m", 0, 0.3, 1, 1.0);
    pad1_m->SetBottomMargin(0.02);
    pad1_m->SetLeftMargin(0.16);
    pad1_m->SetRightMargin(0.06);
    pad1_m->SetTopMargin(0.12);
    pad1_m->Draw();

    TPad *pad2_m = new TPad("pad2_m", "pad2_m", 0, 0, 1, 0.3);
    pad2_m->SetLeftMargin(0.16);
    pad2_m->SetRightMargin(0.06);
    pad2_m->SetTopMargin(0.02);
    pad2_m->SetBottomMargin(0.42);
    pad2_m->Draw();

    pad1_m->cd();

    THStack stack_m_DY("stack_m_DY", "");
    stack_m_DY.Add(&h_m_ttjets_DY);
    stack_m_DY.Add(&h_m_qcd_DY);
    stack_m_DY.Add(&h_m_dy_DY);

    stack_m_DY.SetMaximum(std::max(stack_m_DY.GetMaximum(), h_m_data_DY.GetMaximum()) * 1.2);
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
    leg2->SetTextFont(42);
    leg2->SetTextSize(0.042);
    leg2->SetMargin(0.15);
    leg2->AddEntry(&h_m_data_DY, "Data (2018)", "lep");
    leg2->AddEntry(&h_m_dy_DY, "Drell Yan", "f");
    leg2->AddEntry(&h_m_qcd_DY, "QCD (Data driven)", "f");
    leg2->AddEntry(&h_m_ttjets_DY, "t#bar{t}", "f");
    leg2->Draw();

    AddStandardLabels();

    pad2_m->cd();

    TH1D* ratio_m = CreateRatio(&h_m_data_DY, &stack_m_DY, "ratio_m");
    ratio_m->GetXaxis()->SetTitle("Invariant mass of the central system [GeV]");
    ratio_m->Draw("E");

    TLine line_m(min_m_DY, 1.0, max_m_DY, 1.0);
    line_m.SetLineStyle(2);
    line_m.SetLineColor(kGray+2);
    line_m.Draw();

    c2.SaveAs("output_plots_cr/DY_CR_mass.png");

    // ====================================
    // DY Control Region - Rapidity
    // ====================================
    TCanvas c3("c3", "", 700, 800);

    TPad *pad1_r = new TPad("pad1_r", "pad1_r", 0, 0.3, 1, 1.0);
    pad1_r->SetBottomMargin(0.02);
    pad1_r->SetLeftMargin(0.16);
    pad1_r->SetRightMargin(0.06);
    pad1_r->SetTopMargin(0.12);
    pad1_r->Draw();

    TPad *pad2_r = new TPad("pad2_r", "pad2_r", 0, 0, 1, 0.3);
    pad2_r->SetLeftMargin(0.16);
    pad2_r->SetRightMargin(0.06);
    pad2_r->SetTopMargin(0.02);
    pad2_r->SetBottomMargin(0.42);
    pad2_r->Draw();

    pad1_r->cd();

    THStack stack_r_DY("stack_r_DY", "");
    stack_r_DY.Add(&h_r_ttjets_DY);
    stack_r_DY.Add(&h_r_qcd_DY);
    stack_r_DY.Add(&h_r_dy_DY);

    stack_r_DY.SetMaximum(std::max(stack_r_DY.GetMaximum(), h_r_data_DY.GetMaximum()) * 1.2);
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
    leg3->SetTextFont(42);
    leg3->SetTextSize(0.042);
    leg3->SetMargin(0.15);
    leg3->AddEntry(&h_r_data_DY, "Data (2018)", "lep");
    leg3->AddEntry(&h_r_dy_DY, "Drell Yan", "f");
    leg3->AddEntry(&h_r_qcd_DY, "QCD (Data driven)", "f");
    leg3->AddEntry(&h_r_ttjets_DY, "t#bar{t}", "f");
    leg3->Draw();

    AddStandardLabels();

    pad2_r->cd();

    TH1D* ratio_r = CreateRatio(&h_r_data_DY, &stack_r_DY, "ratio_r");
    ratio_r->GetXaxis()->SetTitle("Rapidity of the central system");
    ratio_r->Draw("E");

    TLine line_r(min_r_DY, 1.0, max_r_DY, 1.0);
    line_r.SetLineStyle(2);
    line_r.SetLineColor(kGray+2);
    line_r.Draw();

    c3.SaveAs("output_plots_cr/DY_CR_rapidity.png");

    // ====================================
    // DY Control Region - Transverse Momentum
    // ====================================
    TCanvas c4("c4", "", 700, 800);

    TPad *pad1_pt = new TPad("pad1_pt", "pad1_pt", 0, 0.3, 1, 1.0);
    pad1_pt->SetBottomMargin(0.02);
    pad1_pt->SetLeftMargin(0.16);
    pad1_pt->SetRightMargin(0.06);
    pad1_pt->SetTopMargin(0.12);
    pad1_pt->Draw();

    TPad *pad2_pt = new TPad("pad2_pt", "pad2_pt", 0, 0, 1, 0.3);
    pad2_pt->SetLeftMargin(0.16);
    pad2_pt->SetRightMargin(0.06);
    pad2_pt->SetTopMargin(0.02);
    pad2_pt->SetBottomMargin(0.42);
    pad2_pt->Draw();

    pad1_pt->cd();

    THStack stack_pt_DY("stack_pt_DY", "");
    stack_pt_DY.Add(&h_pt_ttjets_DY);
    stack_pt_DY.Add(&h_pt_qcd_DY);
    stack_pt_DY.Add(&h_pt_dy_DY);

    stack_pt_DY.SetMaximum(std::max(stack_pt_DY.GetMaximum(), h_pt_data_DY.GetMaximum()) * 1.2);
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
    leg4->SetTextFont(42);
    leg4->SetTextSize(0.042);
    leg4->SetMargin(0.15);
    leg4->AddEntry(&h_pt_data_DY, "Data (2018)", "lep");
    leg4->AddEntry(&h_pt_dy_DY, "Drell Yan", "f");
    leg4->AddEntry(&h_pt_qcd_DY, "QCD (Data driven)", "f");
    leg4->AddEntry(&h_pt_ttjets_DY, "t#bar{t}", "f");
    leg4->Draw();

    AddStandardLabels();

    pad2_pt->cd();

    TH1D* ratio_pt = CreateRatio(&h_pt_data_DY, &stack_pt_DY, "ratio_pt");
    ratio_pt->GetXaxis()->SetTitle("Transverse momentum of the central system [GeV]");
    ratio_pt->Draw("E");

    TLine line_pt(min_pt_DY, 1.0, max_pt_DY, 1.0);
    line_pt.SetLineStyle(2);
    line_pt.SetLineColor(kGray+2);
    line_pt.Draw();

    c4.SaveAs("output_plots_cr/DY_CR_pt.png");

    // ====================================
    // TT Control Region - Acoplanarity
    // ====================================
    TCanvas c5("c5", "", 700, 800);

    TPad *pad1_tt_aco = new TPad("pad1_tt_aco", "pad1_tt_aco", 0, 0.3, 1, 1.0);
    pad1_tt_aco->SetBottomMargin(0.02);
    pad1_tt_aco->SetLeftMargin(0.16);
    pad1_tt_aco->SetRightMargin(0.06);
    pad1_tt_aco->SetTopMargin(0.12);
    pad1_tt_aco->Draw();

    TPad *pad2_tt_aco = new TPad("pad2_tt_aco", "pad2_tt_aco", 0, 0, 1, 0.3);
    pad2_tt_aco->SetLeftMargin(0.16);
    pad2_tt_aco->SetRightMargin(0.06);
    pad2_tt_aco->SetTopMargin(0.02);
    pad2_tt_aco->SetBottomMargin(0.42);
    pad2_tt_aco->Draw();

    pad1_tt_aco->cd();

    THStack stack_aco_TT("stack_aco_TT", "");
    stack_aco_TT.Add(&h_aco_qcd_TT);
    stack_aco_TT.Add(&h_aco_dy_TT);
    stack_aco_TT.Add(&h_aco_ttjets_TT);

    stack_aco_TT.SetMaximum(std::max(stack_aco_TT.GetMaximum(), h_aco_data_TT.GetMaximum()) * 1.2);
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
    leg5->SetTextFont(42);
    leg5->SetTextSize(0.042);
    leg5->SetMargin(0.15);
    leg5->AddEntry(&h_aco_data_TT, "Data (2018)", "lep");
    leg5->AddEntry(&h_aco_ttjets_TT, "t#bar{t}", "f");
    leg5->AddEntry(&h_aco_qcd_TT, "QCD (Data driven)", "f");
    leg5->AddEntry(&h_aco_dy_TT, "Drell Yan", "f");
    leg5->Draw();

    AddStandardLabels();

    pad2_tt_aco->cd();

    TH1D* ratio_aco_tt = CreateRatio(&h_aco_data_TT, &stack_aco_TT, "ratio_aco_tt");
    ratio_aco_tt->GetXaxis()->SetTitle("Acoplanarity of the central system");
    ratio_aco_tt->Draw("E");

    TLine line_aco_tt(min_aco_TT, 1.0, max_aco_TT, 1.0);
    line_aco_tt.SetLineStyle(2);
    line_aco_tt.SetLineColor(kGray+2);
    line_aco_tt.Draw();

    c5.SaveAs("output_plots_cr/TT_CR_acoplanarity.png");

    // ====================================
    // TT Control Region - Mass
    // ====================================
    TCanvas c6("c6", "", 700, 800);

    TPad *pad1_tt_m = new TPad("pad1_tt_m", "pad1_tt_m", 0, 0.3, 1, 1.0);
    pad1_tt_m->SetBottomMargin(0.02);
    pad1_tt_m->SetLeftMargin(0.16);
    pad1_tt_m->SetRightMargin(0.06);
    pad1_tt_m->SetTopMargin(0.12);
    pad1_tt_m->Draw();

    TPad *pad2_tt_m = new TPad("pad2_tt_m", "pad2_tt_m", 0, 0, 1, 0.3);
    pad2_tt_m->SetLeftMargin(0.16);
    pad2_tt_m->SetRightMargin(0.06);
    pad2_tt_m->SetTopMargin(0.02);
    pad2_tt_m->SetBottomMargin(0.42);
    pad2_tt_m->Draw();

    pad1_tt_m->cd();

    THStack stack_m_TT("stack_m_TT", "");
    stack_m_TT.Add(&h_m_qcd_TT);
    stack_m_TT.Add(&h_m_dy_TT);
    stack_m_TT.Add(&h_m_ttjets_TT);

    stack_m_TT.SetMaximum(std::max(stack_m_TT.GetMaximum(), h_m_data_TT.GetMaximum()) * 1.2);
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
    leg6->SetTextFont(42);
    leg6->SetTextSize(0.042);
    leg6->SetMargin(0.15);
    leg6->AddEntry(&h_m_data_TT, "Data (2018)", "lep");
    leg6->AddEntry(&h_m_ttjets_TT, "t#bar{t}", "f");
    leg6->AddEntry(&h_m_qcd_TT, "QCD (Data driven)", "f");
    leg6->AddEntry(&h_m_dy_TT, "Drell Yan", "f");
    leg6->Draw();

    AddStandardLabels();

    pad2_tt_m->cd();

    TH1D* ratio_m_tt = CreateRatio(&h_m_data_TT, &stack_m_TT, "ratio_m_tt");
    ratio_m_tt->GetXaxis()->SetTitle("Invariant mass of the central system [GeV]");
    ratio_m_tt->Draw("E");

    TLine line_m_tt(min_m_TT, 1.0, max_m_TT, 1.0);
    line_m_tt.SetLineStyle(2);
    line_m_tt.SetLineColor(kGray+2);
    line_m_tt.Draw();

    c6.SaveAs("output_plots_cr/TT_CR_mass.png");

    // ====================================
    // TT Control Region - Rapidity
    // ====================================
    TCanvas c7("c7", "", 700, 800);

    TPad *pad1_tt_r = new TPad("pad1_tt_r", "pad1_tt_r", 0, 0.3, 1, 1.0);
    pad1_tt_r->SetBottomMargin(0.02);
    pad1_tt_r->SetLeftMargin(0.16);
    pad1_tt_r->SetRightMargin(0.06);
    pad1_tt_r->SetTopMargin(0.12);
    pad1_tt_r->Draw();

    TPad *pad2_tt_r = new TPad("pad2_tt_r", "pad2_tt_r", 0, 0, 1, 0.3);
    pad2_tt_r->SetLeftMargin(0.16);
    pad2_tt_r->SetRightMargin(0.06);
    pad2_tt_r->SetTopMargin(0.02);
    pad2_tt_r->SetBottomMargin(0.42);
    pad2_tt_r->Draw();

    pad1_tt_r->cd();

    THStack stack_r_TT("stack_r_TT", "");
    stack_r_TT.Add(&h_r_qcd_TT);
    stack_r_TT.Add(&h_r_dy_TT);
    stack_r_TT.Add(&h_r_ttjets_TT);

    stack_r_TT.SetMaximum(std::max(stack_r_TT.GetMaximum(), h_r_data_TT.GetMaximum()) * 1.2);
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
    leg7->SetTextFont(42);
    leg7->SetTextSize(0.042);
    leg7->SetMargin(0.15);
    leg7->AddEntry(&h_r_data_TT, "Data (2018)", "lep");
    leg7->AddEntry(&h_r_ttjets_TT, "t#bar{t}", "f");
    leg7->AddEntry(&h_r_qcd_TT, "QCD (Data driven)", "f");
    leg7->AddEntry(&h_r_dy_TT, "Drell Yan", "f");
    leg7->Draw();

    AddStandardLabels();

    pad2_tt_r->cd();

    TH1D* ratio_r_tt = CreateRatio(&h_r_data_TT, &stack_r_TT, "ratio_r_tt");
    ratio_r_tt->GetXaxis()->SetTitle("Rapidity of the central system");
    ratio_r_tt->Draw("E");

    TLine line_r_tt(min_r_TT, 1.0, max_r_TT, 1.0);
    line_r_tt.SetLineStyle(2);
    line_r_tt.SetLineColor(kGray+2);
    line_r_tt.Draw();

    c7.SaveAs("output_plots_cr/TT_CR_rapidity.png");

    // ====================================
    // TT Control Region - Transverse Momentum
    // ====================================
    TCanvas c8("c8", "", 700, 800);

    TPad *pad1_tt_pt = new TPad("pad1_tt_pt", "pad1_tt_pt", 0, 0.3, 1, 1.0);
    pad1_tt_pt->SetBottomMargin(0.02);
    pad1_tt_pt->SetLeftMargin(0.16);
    pad1_tt_pt->SetRightMargin(0.06);
    pad1_tt_pt->SetTopMargin(0.12);
    pad1_tt_pt->Draw();

    TPad *pad2_tt_pt = new TPad("pad2_tt_pt", "pad2_tt_pt", 0, 0, 1, 0.3);
    pad2_tt_pt->SetLeftMargin(0.16);
    pad2_tt_pt->SetRightMargin(0.06);
    pad2_tt_pt->SetTopMargin(0.02);
    pad2_tt_pt->SetBottomMargin(0.42);
    pad2_tt_pt->Draw();

    pad1_tt_pt->cd();

    THStack stack_pt_TT("stack_pt_TT", "");
    stack_pt_TT.Add(&h_pt_qcd_TT);
    stack_pt_TT.Add(&h_pt_dy_TT);
    stack_pt_TT.Add(&h_pt_ttjets_TT);

    stack_pt_TT.SetMaximum(std::max(stack_pt_TT.GetMaximum(), h_pt_data_TT.GetMaximum()) * 1.2);
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
    leg8->SetTextFont(42);
    leg8->SetTextSize(0.042);
    leg8->SetMargin(0.15);
    leg8->AddEntry(&h_pt_data_TT, "Data (2018)", "lep");
    leg8->AddEntry(&h_pt_ttjets_TT, "t#bar{t}", "f");
    leg8->AddEntry(&h_pt_qcd_TT, "QCD (Data driven)", "f");
    leg8->AddEntry(&h_pt_dy_TT, "Drell Yan", "f");
    leg8->Draw();

    AddStandardLabels();

    pad2_tt_pt->cd();

    TH1D* ratio_pt_tt = CreateRatio(&h_pt_data_TT, &stack_pt_TT, "ratio_pt_tt");
    ratio_pt_tt->GetXaxis()->SetTitle("Transverse momentum of the central system [GeV]");
    ratio_pt_tt->Draw("E");

    TLine line_pt_tt(min_pt_TT, 1.0, max_pt_TT, 1.0);
    line_pt_tt.SetLineStyle(2);
    line_pt_tt.SetLineColor(kGray+2);
    line_pt_tt.Draw();

    c8.SaveAs("output_plots_cr/TT_CR_pt.png");

    // ====================================
    // QCD Control Region - Acoplanarity
    // ====================================
    TCanvas c9("c9", "", 700, 800);

    TPad *pad1_qcd_aco = new TPad("pad1_qcd_aco", "pad1_qcd_aco", 0, 0.3, 1, 1.0);
    pad1_qcd_aco->SetBottomMargin(0.02);
    pad1_qcd_aco->SetLeftMargin(0.16);
    pad1_qcd_aco->SetRightMargin(0.06);
    pad1_qcd_aco->SetTopMargin(0.12);
    pad1_qcd_aco->Draw();

    TPad *pad2_qcd_aco = new TPad("pad2_qcd_aco", "pad2_qcd_aco", 0, 0, 1, 0.3);
    pad2_qcd_aco->SetLeftMargin(0.16);
    pad2_qcd_aco->SetRightMargin(0.06);
    pad2_qcd_aco->SetTopMargin(0.02);
    pad2_qcd_aco->SetBottomMargin(0.42);
    pad2_qcd_aco->Draw();

    pad1_qcd_aco->cd();

    THStack stack_aco_QCD("stack_aco_QCD", "");
    stack_aco_QCD.Add(&h_aco_ttjets_QCD);
    stack_aco_QCD.Add(&h_aco_dy_QCD);
    stack_aco_QCD.Add(&h_aco_qcd_QCD);

    stack_aco_QCD.SetMaximum(std::max(stack_aco_QCD.GetMaximum(), h_aco_data_QCD.GetMaximum()) * 1.2);
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
    leg9->SetTextFont(42);
    leg9->SetTextSize(0.042);
    leg9->SetMargin(0.15);
    leg9->AddEntry(&h_aco_data_QCD, "Data (2018)", "lep");
    leg9->AddEntry(&h_aco_qcd_QCD, "QCD (Data driven)", "f");
    leg9->AddEntry(&h_aco_dy_QCD, "Drell Yan", "f");
    leg9->AddEntry(&h_aco_ttjets_QCD, "t#bar{t}", "f");
    leg9->Draw();

    AddStandardLabels();

    pad2_qcd_aco->cd();

    TH1D* ratio_aco_qcd = CreateRatio(&h_aco_data_QCD, &stack_aco_QCD, "ratio_aco_qcd");
    ratio_aco_qcd->GetXaxis()->SetTitle("Acoplanarity of the central system");
    ratio_aco_qcd->Draw("E");

    TLine line_aco_qcd(min_aco_QCD, 1.0, max_aco_QCD, 1.0);
    line_aco_qcd.SetLineStyle(2);
    line_aco_qcd.SetLineColor(kGray+2);
    line_aco_qcd.Draw();

    c9.SaveAs("output_plots_cr/QCD_CR_acoplanarity.png");

    // ====================================
    // QCD Control Region - Mass
    // ====================================
    TCanvas c10("c10", "", 700, 800);

    TPad *pad1_qcd_m = new TPad("pad1_qcd_m", "pad1_qcd_m", 0, 0.3, 1, 1.0);
    pad1_qcd_m->SetBottomMargin(0.02);
    pad1_qcd_m->SetLeftMargin(0.16);
    pad1_qcd_m->SetRightMargin(0.06);
    pad1_qcd_m->SetTopMargin(0.12);
    pad1_qcd_m->Draw();

    TPad *pad2_qcd_m = new TPad("pad2_qcd_m", "pad2_qcd_m", 0, 0, 1, 0.3);
    pad2_qcd_m->SetLeftMargin(0.16);
    pad2_qcd_m->SetRightMargin(0.06);
    pad2_qcd_m->SetTopMargin(0.02);
    pad2_qcd_m->SetBottomMargin(0.42);
    pad2_qcd_m->Draw();

    pad1_qcd_m->cd();

    THStack stack_m_QCD("stack_m_QCD", "");
    stack_m_QCD.Add(&h_m_ttjets_QCD);
    stack_m_QCD.Add(&h_m_dy_QCD);
    stack_m_QCD.Add(&h_m_qcd_QCD);

    stack_m_QCD.SetMaximum(std::max(stack_m_QCD.GetMaximum(), h_m_data_QCD.GetMaximum()) * 1.2);
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
    leg10->SetTextFont(42);
    leg10->SetTextSize(0.042);
    leg10->SetMargin(0.15);
    leg10->AddEntry(&h_m_data_QCD, "Data (2018)", "lep");
    leg10->AddEntry(&h_m_qcd_QCD, "QCD (Data driven)", "f");
    leg10->AddEntry(&h_m_dy_QCD, "Drell Yan", "f");
    leg10->AddEntry(&h_m_ttjets_QCD, "t#bar{t}", "f");
    leg10->Draw();

    AddStandardLabels();

    pad2_qcd_m->cd();

    TH1D* ratio_m_qcd = CreateRatio(&h_m_data_QCD, &stack_m_QCD, "ratio_m_qcd");
    ratio_m_qcd->GetXaxis()->SetTitle("Invariant mass of the central system [GeV]");
    ratio_m_qcd->Draw("E");

    TLine line_m_qcd(min_m_QCD, 1.0, max_m_QCD, 1.0);
    line_m_qcd.SetLineStyle(2);
    line_m_qcd.SetLineColor(kGray+2);
    line_m_qcd.Draw();

    c10.SaveAs("output_plots_cr/QCD_CR_mass.png");

    // ====================================
    // QCD Control Region - Rapidity
    // ====================================
    TCanvas c11("c11", "", 700, 800);

    TPad *pad1_qcd_r = new TPad("pad1_qcd_r", "pad1_qcd_r", 0, 0.3, 1, 1.0);
    pad1_qcd_r->SetBottomMargin(0.02);
    pad1_qcd_r->SetLeftMargin(0.16);
    pad1_qcd_r->SetRightMargin(0.06);
    pad1_qcd_r->SetTopMargin(0.12);
    pad1_qcd_r->Draw();

    TPad *pad2_qcd_r = new TPad("pad2_qcd_r", "pad2_qcd_r", 0, 0, 1, 0.3);
    pad2_qcd_r->SetLeftMargin(0.16);
    pad2_qcd_r->SetRightMargin(0.06);
    pad2_qcd_r->SetTopMargin(0.02);
    pad2_qcd_r->SetBottomMargin(0.42);
    pad2_qcd_r->Draw();

    pad1_qcd_r->cd();

    THStack stack_r_QCD("stack_r_QCD", "");
    stack_r_QCD.Add(&h_r_ttjets_QCD);
    stack_r_QCD.Add(&h_r_dy_QCD);
    stack_r_QCD.Add(&h_r_qcd_QCD);

    stack_r_QCD.SetMaximum(std::max(stack_r_QCD.GetMaximum(), h_r_data_QCD.GetMaximum()) * 1.2);
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
    leg11->SetTextFont(42);
    leg11->SetTextSize(0.042);
    leg11->SetMargin(0.15);
    leg11->AddEntry(&h_r_data_QCD, "Data (2018)", "lep");
    leg11->AddEntry(&h_r_qcd_QCD, "QCD (Data driven)", "f");
    leg11->AddEntry(&h_r_dy_QCD, "Drell Yan", "f");
    leg11->AddEntry(&h_r_ttjets_QCD, "t#bar{t}", "f");
    leg11->Draw();

    AddStandardLabels();

    pad2_qcd_r->cd();

    TH1D* ratio_r_qcd = CreateRatio(&h_r_data_QCD, &stack_r_QCD, "ratio_r_qcd");
    ratio_r_qcd->GetXaxis()->SetTitle("Rapidity of the central system");
    ratio_r_qcd->Draw("E");

    TLine line_r_qcd(min_r_QCD, 1.0, max_r_QCD, 1.0);
    line_r_qcd.SetLineStyle(2);
    line_r_qcd.SetLineColor(kGray+2);
    line_r_qcd.Draw();

    c11.SaveAs("output_plots_cr/QCD_CR_rapidity.png");

    // ====================================
    // QCD Control Region - Transverse Momentum
    // ====================================
    TCanvas c12("c12", "", 700, 800);

    TPad *pad1_qcd_pt = new TPad("pad1_qcd_pt", "pad1_qcd_pt", 0, 0.3, 1, 1.0);
    pad1_qcd_pt->SetBottomMargin(0.02);
    pad1_qcd_pt->SetLeftMargin(0.16);
    pad1_qcd_pt->SetRightMargin(0.06);
    pad1_qcd_pt->SetTopMargin(0.12);
    pad1_qcd_pt->Draw();

    TPad *pad2_qcd_pt = new TPad("pad2_qcd_pt", "pad2_qcd_pt", 0, 0, 1, 0.3);
    pad2_qcd_pt->SetLeftMargin(0.16);
    pad2_qcd_pt->SetRightMargin(0.06);
    pad2_qcd_pt->SetTopMargin(0.02);
    pad2_qcd_pt->SetBottomMargin(0.42);
    pad2_qcd_pt->Draw();

    pad1_qcd_pt->cd();

    THStack stack_pt_QCD("stack_pt_QCD", "");
    stack_pt_QCD.Add(&h_pt_ttjets_QCD);
    stack_pt_QCD.Add(&h_pt_dy_QCD);
    stack_pt_QCD.Add(&h_pt_qcd_QCD);

    stack_pt_QCD.SetMaximum(std::max(stack_pt_QCD.GetMaximum(), h_pt_data_QCD.GetMaximum()) * 1.2);
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
    leg12->SetTextFont(42);
    leg12->SetTextSize(0.042);
    leg12->SetMargin(0.15);
    leg12->AddEntry(&h_pt_data_QCD, "Data (2018)", "lep");
    leg12->AddEntry(&h_pt_qcd_QCD, "QCD (Data driven)", "f");
    leg12->AddEntry(&h_pt_dy_QCD, "Drell Yan", "f");
    leg12->AddEntry(&h_pt_ttjets_QCD, "t#bar{t}", "f");
    leg12->Draw();

    AddStandardLabels();

    pad2_qcd_pt->cd();

    TH1D* ratio_pt_qcd = CreateRatio(&h_pt_data_QCD, &stack_pt_QCD, "ratio_pt_qcd");
    ratio_pt_qcd->GetXaxis()->SetTitle("Transverse momentum of the central system [GeV]");
    ratio_pt_qcd->Draw("E");

    TLine line_pt_qcd(min_pt_QCD, 1.0, max_pt_QCD, 1.0);
    line_pt_qcd.SetLineStyle(2);
    line_pt_qcd.SetLineColor(kGray+2);
    line_pt_qcd.Draw();

    c12.SaveAs("output_plots_cr/QCD_CR_pt.png");

    cout << "All control region plots created successfully!" << endl;
    cout << "Generated 12 plots:" << endl;
    cout << "  DY Control Region: DY (yellow) on top - DY dominates" << endl;
    cout << "  TT Control Region: TT (green) on top - TT dominates" << endl;
    cout << "  QCD Control Region: QCD (red) on top - QCD dominates" << endl;
    cout << "Check the output_plots_cr/ directory for all generated plots." << endl;

    bool interactive = false;
    if (interactive) {
        app.Run(true);
    }

    return 0;
}
