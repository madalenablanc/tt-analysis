#include <iostream>
#include <cmath>
#include "TFile.h"
#include "TTree.h"
#include "TH1D.h"
#include "TLeaf.h"

using namespace std;

// MC normalization: Scale = (L x sigma) / Sum(generator_weights)
// DY weight branch: generator_weight x muon_SFs x 0.245 -> needs DY_SCALE
// ttbar weight branch: 0.15 x muon_SFs x 0.245 -> already normalized
const double DY_SCALE = 1.004e-4;       // L x sigma / Sum_w for DY
const double TTBAR_SCALE = 1.0;         // ttbar already normalized via 0.15 factor

// Helper: get leaf value trying multiple branch names (data/MC have different naming)
double GetLeafSafe(TTree* tree, std::initializer_list<const char*> names, double def = 0.0) {
    for (const char* name : names) {
        TLeaf* lf = tree->GetLeaf(name);
        if (lf) return lf->GetValue(0);
    }
    return def;
}

// Helper: load a MVA histogram from TMVA application outputs
TH1D* LoadBDTHist(const char* fileName, const char* newName, double scale = 1.0, const char* histName = "MVA_BDT") {
    TFile input(fileName);
    if (input.IsZombie()) {
        cerr << "WARNING: cannot open BDT file " << fileName << endl;
        return nullptr;
    }
    TH1* hist = dynamic_cast<TH1*>(input.Get(histName));
    if (!hist) {
        cerr << "WARNING: histogram 'MVA_BDT' missing in " << fileName << endl;
        return nullptr;
    }
    TH1D* clone = new TH1D(newName, hist->GetTitle(),
                           hist->GetNbinsX(),
                           hist->GetXaxis()->GetXmin(),
                           hist->GetXaxis()->GetXmax());
    clone->SetDirectory(nullptr);
    for (int b = 0; b <= hist->GetNbinsX() + 1; b++) {
        clone->SetBinContent(b, hist->GetBinContent(b));
        clone->SetBinError(b, hist->GetBinError(b));
    }
    if (!clone) {
        cerr << "WARNING: failed to clone histogram from " << fileName << endl;
        return nullptr;
    }
    clone->SetDirectory(nullptr);
    clone->Scale(scale);
    return clone;
}

int save_shapes(){

    // Prevent ROOT from auto-associating histograms with TFile directories
    // This avoids double-free crashes with stack-allocated histograms
    TH1::AddDirectory(kFALSE);

    // Open input files
    TFile dy("/eos/home-m/mblancco/samples_2018_mutau/DY_2018_UL_MuTau_nano_merged_pileup_protons.root");
    TFile ttjets("/eos/home-m/mblancco/samples_2018_mutau/ttjets_2018_UL_MuTau_nano_merged_pileup_protons.root");
    TFile qcd("/eos/home-m/mblancco/samples_2018_mutau/QCD_2018_UL_MuTau_nano_merged_proton_vars_new.root");
    TFile data("/eos/home-m/mblancco/samples_2018_mutau/Data_2018_UL_MuTau_nano_merged_proton_vars.root");
    TFile sinal("MuTau_sinal_SM_2018_july.root");

    // Output file
    TFile output("MuTau_shapes.root", "RECREATE", "");

    // Get trees
    TTree* tree_data   = (TTree*) data.Get("tree");
    TTree* tree_dy     = (TTree*) dy.Get("tree");
    TTree* tree_qcd    = (TTree*) qcd.Get("tree");
    TTree* tree_ttjets = (TTree*) ttjets.Get("tree");
    TTree* tree_sinal  = (TTree*) sinal.Get("tree");

    int n_evt_data   = tree_data->GetEntries();
    int n_evt_dy     = tree_dy->GetEntries();
    int n_evt_qcd    = tree_qcd->GetEntries();
    int n_evt_ttjets = tree_ttjets->GetEntries();
    int n_evt_sinal  = tree_sinal->GetEntries();

    // ---- Histogram definitions ----
    // Matching plot_m.cpp variables and binning

    // aco: acoplanarity
    int bin_aco = 20; double min_aco = 0.0, max_aco = 1.0;
    TH1D aco_data("aco_data","aco_data", bin_aco, min_aco, max_aco);
    TH1D aco_qcd("aco_qcd","aco_qcd", bin_aco, min_aco, max_aco);
    TH1D aco_dy("aco_dy","aco_dy", bin_aco, min_aco, max_aco);
    TH1D aco_ttjets("aco_ttjets","aco_ttjets", bin_aco, min_aco, max_aco);
    TH1D aco_sinal("aco_sinal","aco_sinal", bin_aco, min_aco, max_aco);

    // m: invariant mass
    int bin_m = 30; double min_m = 0, max_m = 1200;
    TH1D m_data("m_data","m_data", bin_m, min_m, max_m);
    TH1D m_qcd("m_qcd","m_qcd", bin_m, min_m, max_m);
    TH1D m_dy("m_dy","m_dy", bin_m, min_m, max_m);
    TH1D m_ttjets("m_ttjets","m_ttjets", bin_m, min_m, max_m);
    TH1D m_sinal("m_sinal","m_sinal", bin_m, min_m, max_m);

    // pt: system pT
    int bin_pt = 20; double min_pt = 0, max_pt = 600;
    TH1D pt_data("pt_data","pt_data", bin_pt, min_pt, max_pt);
    TH1D pt_qcd("pt_qcd","pt_qcd", bin_pt, min_pt, max_pt);
    TH1D pt_dy("pt_dy","pt_dy", bin_pt, min_pt, max_pt);
    TH1D pt_ttjets("pt_ttjets","pt_ttjets", bin_pt, min_pt, max_pt);
    TH1D pt_sinal("pt_sinal","pt_sinal", bin_pt, min_pt, max_pt);

    // ra: system rapidity
    int bin_ra = 10; double min_ra = -2.2, max_ra = 2.2;
    TH1D ra_data("ra_data","ra_data", bin_ra, min_ra, max_ra);
    TH1D ra_qcd("ra_qcd","ra_qcd", bin_ra, min_ra, max_ra);
    TH1D ra_dy("ra_dy","ra_dy", bin_ra, min_ra, max_ra);
    TH1D ra_ttjets("ra_ttjets","ra_ttjets", bin_ra, min_ra, max_ra);
    TH1D ra_sinal("ra_sinal","ra_sinal", bin_ra, min_ra, max_ra);

    // r: rapidity matching = sist_rap - 0.5*log(xi_arm1_1/xi_arm2_1)
    int bin_r = 10; double min_r = -2.2, max_r = 2.2;
    TH1D r_data("r_data","r_data", bin_r, min_r, max_r);
    TH1D r_qcd("r_qcd","r_qcd", bin_r, min_r, max_r);
    TH1D r_dy("r_dy","r_dy", bin_r, min_r, max_r);
    TH1D r_ttjets("r_ttjets","r_ttjets", bin_r, min_r, max_r);
    TH1D r_sinal("r_sinal","r_sinal", bin_r, min_r, max_r);

    // mm: mass difference = sist_mass - 13000*sqrt(xi_arm1_1*xi_arm2_1)
    int bin_mm = 20; double min_mm = -1500, max_mm = 200;
    TH1D mm_data("mm_data","mm_data", bin_mm, min_mm, max_mm);
    TH1D mm_qcd("mm_qcd","mm_qcd", bin_mm, min_mm, max_mm);
    TH1D mm_dy("mm_dy","mm_dy", bin_mm, min_mm, max_mm);
    TH1D mm_ttjets("mm_ttjets","mm_ttjets", bin_mm, min_mm, max_mm);
    TH1D mm_sinal("mm_sinal","mm_sinal", bin_mm, min_mm, max_mm);

    // tau: tau pT
    int bin_tau = 20; double min_tau = 100, max_tau = 500;
    TH1D tau_data("tau_data","tau_data", bin_tau, min_tau, max_tau);
    TH1D tau_qcd("tau_qcd","tau_qcd", bin_tau, min_tau, max_tau);
    TH1D tau_dy("tau_dy","tau_dy", bin_tau, min_tau, max_tau);
    TH1D tau_ttjets("tau_ttjets","tau_ttjets", bin_tau, min_tau, max_tau);
    TH1D tau_sinal("tau_sinal","tau_sinal", bin_tau, min_tau, max_tau);

    // met: missing transverse energy
    int bin_met = 20; double min_met = 0, max_met = 300;
    TH1D met_data("met_data","met_data", bin_met, min_met, max_met);
    TH1D met_qcd("met_qcd","met_qcd", bin_met, min_met, max_met);
    TH1D met_dy("met_dy","met_dy", bin_met, min_met, max_met);
    TH1D met_ttjets("met_ttjets","met_ttjets", bin_met, min_met, max_met);
    TH1D met_sinal("met_sinal","met_sinal", bin_met, min_met, max_met);

    // mupt: muon pT (BDT input variable)
    int bin_mupt = 20; double min_mupt = 0, max_mupt = 300;
    TH1D mupt_data("mupt_data","mupt_data", bin_mupt, min_mupt, max_mupt);
    TH1D mupt_qcd("mupt_qcd","mupt_qcd", bin_mupt, min_mupt, max_mupt);
    TH1D mupt_dy("mupt_dy","mupt_dy", bin_mupt, min_mupt, max_mupt);
    TH1D mupt_ttjets("mupt_ttjets","mupt_ttjets", bin_mupt, min_mupt, max_mupt);
    TH1D mupt_sinal("mupt_sinal","mupt_sinal", bin_mupt, min_mupt, max_mupt);

    // ---- Event loops ----

    // DATA: no weight, require protons on both arms
    // proton_multi_arm/xi are stored as std::vector branches - use SetBranchAddress
    {
        vector<int>   *v_arm = nullptr;
        vector<float> *v_xi  = nullptr;
        tree_data->SetBranchAddress("proton_multi_arm", &v_arm);
        tree_data->SetBranchAddress("proton_multi_xi",  &v_xi);

        cout << "Processing DATA (" << n_evt_data << " events)" << endl;
        for(int i = 0; i < n_evt_data; i++){
            tree_data->GetEvent(i);
            if(tree_data->GetLeaf("sist_mass")->GetValue(0) < 0) continue;

            double xi1 = -1, xi2 = -1;
            if(v_arm && v_xi){
                for(size_t j = 0; j < v_arm->size(); j++){
                    if((*v_arm)[j] == 0 && xi1 < 0) xi1 = (*v_xi)[j];
                    if((*v_arm)[j] == 1 && xi2 < 0) xi2 = (*v_xi)[j];
                }
            }
            if(xi1 <= 0 || xi2 <= 0) continue;

            aco_data.Fill(tree_data->GetLeaf("acop")->GetValue(0));
            m_data.Fill(tree_data->GetLeaf("sist_mass")->GetValue(0));
            pt_data.Fill(tree_data->GetLeaf("sist_pt")->GetValue(0));
            ra_data.Fill(tree_data->GetLeaf("sist_rap")->GetValue(0));
            tau_data.Fill(tree_data->GetLeaf("tau_pt")->GetValue(0));
            met_data.Fill(tree_data->GetLeaf("met_pt")->GetValue(0));
            mupt_data.Fill(GetLeafSafe(tree_data, {"mu_pt", "muon_pt"}));
            r_data.Fill(tree_data->GetLeaf("sist_rap")->GetValue(0) - 0.5*log(xi1/xi2));
            mm_data.Fill(tree_data->GetLeaf("sist_mass")->GetValue(0) - 13000.*sqrt(xi1*xi2));
        }
    }

    // QCD: no weight (data-driven), require protons on both arms
    {
        vector<int>   *v_arm = nullptr;
        vector<float> *v_xi  = nullptr;
        tree_qcd->SetBranchAddress("proton_multi_arm", &v_arm);
        tree_qcd->SetBranchAddress("proton_multi_xi",  &v_xi);

        cout << "Processing QCD (" << n_evt_qcd << " events)" << endl;
        for(int i = 0; i < n_evt_qcd; i++){
            tree_qcd->GetEvent(i);
            if(tree_qcd->GetLeaf("sist_mass")->GetValue(0) < 0) continue;

            double xi1 = -1, xi2 = -1;
            if(v_arm && v_xi){
                for(size_t j = 0; j < v_arm->size(); j++){
                    if((*v_arm)[j] == 0 && xi1 < 0) xi1 = (*v_xi)[j];
                    if((*v_arm)[j] == 1 && xi2 < 0) xi2 = (*v_xi)[j];
                }
            }
            if(xi1 <= 0 || xi2 <= 0) continue;

            aco_qcd.Fill(tree_qcd->GetLeaf("acop")->GetValue(0));
            m_qcd.Fill(tree_qcd->GetLeaf("sist_mass")->GetValue(0));
            pt_qcd.Fill(tree_qcd->GetLeaf("sist_pt")->GetValue(0));
            ra_qcd.Fill(tree_qcd->GetLeaf("sist_rap")->GetValue(0));
            tau_qcd.Fill(tree_qcd->GetLeaf("tau_pt")->GetValue(0));
            met_qcd.Fill(tree_qcd->GetLeaf("met_pt")->GetValue(0));
            mupt_qcd.Fill(GetLeafSafe(tree_qcd, {"mu_pt", "muon_pt"}));
            r_qcd.Fill(tree_qcd->GetLeaf("sist_rap")->GetValue(0) - 0.5*log(xi1/xi2));
            mm_qcd.Fill(tree_qcd->GetLeaf("sist_mass")->GetValue(0) - 13000.*sqrt(xi1*xi2));
        }
    }

    // DY: use weight branch, apply DY_SCALE after loop
    cout << "Processing DY (" << n_evt_dy << " events)" << endl;
    for(int i = 0; i < n_evt_dy; i++){
        tree_dy->GetEvent(i);
        if(tree_dy->GetLeaf("sist_mass")->GetValue(0) < 0) continue;

        double xi1 = tree_dy->GetLeaf("xi_arm1_1")->GetValue(0);
        double xi2 = tree_dy->GetLeaf("xi_arm2_1")->GetValue(0);
        if(xi1 <= 0 || xi2 <= 0) continue;  // require pileup protons on both arms

        double w = tree_dy->GetLeaf("weight")->GetValue(0);
        aco_dy.Fill(tree_dy->GetLeaf("acop")->GetValue(0), w);
        m_dy.Fill(tree_dy->GetLeaf("sist_mass")->GetValue(0), w);
        pt_dy.Fill(tree_dy->GetLeaf("sist_pt")->GetValue(0), w);
        ra_dy.Fill(tree_dy->GetLeaf("sist_rap")->GetValue(0), w);
        tau_dy.Fill(tree_dy->GetLeaf("tau_pt")->GetValue(0), w);
        met_dy.Fill(tree_dy->GetLeaf("met_pt")->GetValue(0), w);
        mupt_dy.Fill(GetLeafSafe(tree_dy, {"mu_pt", "muon_pt"}), w);

        r_dy.Fill(tree_dy->GetLeaf("sist_rap")->GetValue(0) - 0.5*log(xi1/xi2), w);
        mm_dy.Fill(tree_dy->GetLeaf("sist_mass")->GetValue(0) - 13000.*sqrt(xi1*xi2), w);
    }

    // ttbar: use weight branch, apply TTBAR_SCALE after loop
    cout << "Processing ttjets (" << n_evt_ttjets << " events)" << endl;
    for(int i = 0; i < n_evt_ttjets; i++){
        tree_ttjets->GetEvent(i);
        if(tree_ttjets->GetLeaf("sist_mass")->GetValue(0) < 0) continue;

        double xi1 = tree_ttjets->GetLeaf("xi_arm1_1")->GetValue(0);
        double xi2 = tree_ttjets->GetLeaf("xi_arm2_1")->GetValue(0);
        if(xi1 <= 0 || xi2 <= 0) continue;  // require pileup protons on both arms

        double w = tree_ttjets->GetLeaf("weight")->GetValue(0);
        aco_ttjets.Fill(tree_ttjets->GetLeaf("acop")->GetValue(0), w);
        m_ttjets.Fill(tree_ttjets->GetLeaf("sist_mass")->GetValue(0), w);
        pt_ttjets.Fill(tree_ttjets->GetLeaf("sist_pt")->GetValue(0), w);
        ra_ttjets.Fill(tree_ttjets->GetLeaf("sist_rap")->GetValue(0), w);
        tau_ttjets.Fill(tree_ttjets->GetLeaf("tau_pt")->GetValue(0), w);
        met_ttjets.Fill(tree_ttjets->GetLeaf("met_pt")->GetValue(0), w);
        mupt_ttjets.Fill(GetLeafSafe(tree_ttjets, {"mu_pt", "muon_pt"}), w);

        r_ttjets.Fill(tree_ttjets->GetLeaf("sist_rap")->GetValue(0) - 0.5*log(xi1/xi2), w);
        mm_ttjets.Fill(tree_ttjets->GetLeaf("sist_mass")->GetValue(0) - 13000.*sqrt(xi1*xi2), w);
    }

    // Signal: use weight branch (already contains proper normalization)
    cout << "Processing signal (" << n_evt_sinal << " events)" << endl;
    for(int i = 0; i < n_evt_sinal; i++){
        tree_sinal->GetEvent(i);
        if(tree_sinal->GetLeaf("sist_mass")->GetValue(0) < 0) continue;

        double xi1 = tree_sinal->GetLeaf("xi_arm1_1")->GetValue(0);
        double xi2 = tree_sinal->GetLeaf("xi_arm2_1")->GetValue(0);
        if(xi1 <= 0 || xi2 <= 0) continue;

        double w = tree_sinal->GetLeaf("weight")->GetValue(0);
        aco_sinal.Fill(tree_sinal->GetLeaf("acop")->GetValue(0), w);
        m_sinal.Fill(tree_sinal->GetLeaf("sist_mass")->GetValue(0), w);
        pt_sinal.Fill(tree_sinal->GetLeaf("sist_pt")->GetValue(0), w);
        ra_sinal.Fill(tree_sinal->GetLeaf("sist_rap")->GetValue(0), w);
        tau_sinal.Fill(tree_sinal->GetLeaf("tau_pt")->GetValue(0), w);
        met_sinal.Fill(tree_sinal->GetLeaf("met_pt")->GetValue(0), w);
        mupt_sinal.Fill(GetLeafSafe(tree_sinal, {"mu_pt", "muon_pt"}), w);

        r_sinal.Fill(tree_sinal->GetLeaf("sist_rap")->GetValue(0) - 0.5*log(xi1/xi2), w);
        mm_sinal.Fill(tree_sinal->GetLeaf("sist_mass")->GetValue(0) - 13000.*sqrt(xi1*xi2), w);
    }

    // ---- Apply MC scale factors ----
    aco_dy.Scale(DY_SCALE);    m_dy.Scale(DY_SCALE);    pt_dy.Scale(DY_SCALE);
    ra_dy.Scale(DY_SCALE);     r_dy.Scale(DY_SCALE);    mm_dy.Scale(DY_SCALE);
    tau_dy.Scale(DY_SCALE);    met_dy.Scale(DY_SCALE);    mupt_dy.Scale(DY_SCALE);

    aco_ttjets.Scale(TTBAR_SCALE);    m_ttjets.Scale(TTBAR_SCALE);    pt_ttjets.Scale(TTBAR_SCALE);
    ra_ttjets.Scale(TTBAR_SCALE);     r_ttjets.Scale(TTBAR_SCALE);    mm_ttjets.Scale(TTBAR_SCALE);
    tau_ttjets.Scale(TTBAR_SCALE);    met_ttjets.Scale(TTBAR_SCALE);  mupt_ttjets.Scale(TTBAR_SCALE);

    // ---- Print yields ----
    cout << endl << "=== Event yields ===" << endl;
    cout << "Data:   " << aco_data.GetEntries() << endl;
    cout << "QCD:    " << aco_qcd.GetEntries() << endl;
    cout << "DY:     " << aco_dy.GetEntries() << " (weighted: " << aco_dy.Integral() << ")" << endl;
    cout << "ttbar:  " << aco_ttjets.GetEntries() << " (weighted: " << aco_ttjets.Integral() << ")" << endl;
    cout << "Signal: " << aco_sinal.GetEntries() << " (weighted: " << aco_sinal.Integral() << ")" << endl;

    // ---- Write all histograms ----
    output.cd();

    aco_data.Write();  aco_qcd.Write();  aco_dy.Write();  aco_ttjets.Write();  aco_sinal.Write();
    m_data.Write();    m_qcd.Write();    m_dy.Write();    m_ttjets.Write();    m_sinal.Write();
    pt_data.Write();   pt_qcd.Write();   pt_dy.Write();   pt_ttjets.Write();   pt_sinal.Write();
    ra_data.Write();   ra_qcd.Write();   ra_dy.Write();   ra_ttjets.Write();   ra_sinal.Write();
    r_data.Write();    r_qcd.Write();    r_dy.Write();    r_ttjets.Write();    r_sinal.Write();
    mm_data.Write();   mm_qcd.Write();   mm_dy.Write();   mm_ttjets.Write();   mm_sinal.Write();
    tau_data.Write();  tau_qcd.Write();  tau_dy.Write();  tau_ttjets.Write();  tau_sinal.Write();
    met_data.Write();   met_qcd.Write();   met_dy.Write();   met_ttjets.Write();   met_sinal.Write();
    mupt_data.Write();  mupt_qcd.Write();  mupt_dy.Write();  mupt_ttjets.Write();  mupt_sinal.Write();

    // ---- BDT histograms ----
    TH1D* bdt_data   = LoadBDTHist("TMVApp_data.root",  "bdt_data");
    TH1D* bdt_qcd    = LoadBDTHist("TMVApp_QCD.root",   "bdt_qcd");
    TH1D* bdt_dy     = LoadBDTHist("TMVApp_DY.root",    "bdt_dy",     DY_SCALE);
    TH1D* bdt_ttjets = LoadBDTHist("TMVApp_ttjets.root","bdt_ttjets", TTBAR_SCALE);
    TH1D* bdt_sinal  = LoadBDTHist("TMVApp_sinal.root", "bdt_sinal");
    if (bdt_sinal) {
        cout << "bdt_sinal entries = " << bdt_sinal->GetEntries() << endl;
        cout << "bdt_sinal integral = " << bdt_sinal->Integral() << endl;
    }

    // ---- Likelihood histograms ----
    TH1D* lk_data   = LoadBDTHist("TMVApp_data.root",  "lk_data",   1.0,       "MVA_Likelihood");
    TH1D* lk_qcd    = LoadBDTHist("TMVApp_QCD.root",   "lk_qcd",    1.0,       "MVA_Likelihood");
    TH1D* lk_dy     = LoadBDTHist("TMVApp_DY.root",    "lk_dy",     DY_SCALE,  "MVA_Likelihood");
    TH1D* lk_ttjets = LoadBDTHist("TMVApp_ttjets.root","lk_ttjets", TTBAR_SCALE,"MVA_Likelihood");
    TH1D* lk_sinal  = LoadBDTHist("TMVApp_sinal.root", "lk_sinal",  1.0,       "MVA_Likelihood");

    // ---- Fisher histograms ----
    TH1D* fish_data   = LoadBDTHist("TMVApp_data.root",  "fish_data",   1.0,        "MVA_Fisher");
    TH1D* fish_qcd    = LoadBDTHist("TMVApp_QCD.root",   "fish_qcd",    1.0,        "MVA_Fisher");
    TH1D* fish_dy     = LoadBDTHist("TMVApp_DY.root",    "fish_dy",     DY_SCALE,   "MVA_Fisher");
    TH1D* fish_ttjets = LoadBDTHist("TMVApp_ttjets.root","fish_ttjets", TTBAR_SCALE,"MVA_Fisher");
    TH1D* fish_sinal  = LoadBDTHist("TMVApp_sinal.root", "fish_sinal",  1.0,        "MVA_Fisher");

    output.cd();
    auto writeIfExists = [&](TH1D* hist){
        if(hist){ output.cd(); hist->Write(); }
    };

    writeIfExists(bdt_data);   writeIfExists(bdt_qcd);   writeIfExists(bdt_dy);
    writeIfExists(bdt_ttjets); writeIfExists(bdt_sinal);

    writeIfExists(lk_data);    writeIfExists(lk_qcd);    writeIfExists(lk_dy);
    writeIfExists(lk_ttjets);  writeIfExists(lk_sinal);

    writeIfExists(fish_data);  writeIfExists(fish_qcd);  writeIfExists(fish_dy);
    writeIfExists(fish_ttjets);writeIfExists(fish_sinal);

    output.Close();
    cout << "Histograms saved to MuTau_shapes.root" << endl;

    return 0;
}
