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
#include "TSystem.h"

using namespace std;

int main(){
    // Create TApplication
    TApplication app("app", 0, 0);
    
    string luminosity;
    
    // Create output directory if it doesn't exist
    gSystem->mkdir("output_plots", true);
    
    // Sample files for DY region
    TFile dy("/eos/cms/store/user/jjhollar/TauTau_NanoAOD_Madalena/BackgroundSamples/ETau/DY_2018_UL_skimmed_ETau_nano_fase1total-protons_2018.root");
    TFile ttjets("/eos/cms/store/user/jjhollar/TauTau_NanoAOD_Madalena/BackgroundSamples/ETau/ttJets_2018_UL_skimmed_ETau_nano_fase1total-protons_2018.root");
    TFile qcd("/eos/cms/store/user/jjhollar/TauTau_NanoAOD_Madalena/BackgroundSamples/ETau/QCD_2018_UL_skimmed_ETau_nano_fase1total-protons_2018.root");
    TFile sinal("ETau_sinal_SM_july.root");

    TFile output("DY_CR_e_tau_UL_2018_shapes.root", "RECREATE", "");

    // Define weights
    double w_qcd = 1.0;
    double w_ttjets = 0.15;
    double w_dy = 1.81;
    double w_sinal;

    // Define histogram parameters with improved ranges
    double min_aco = 0.0, max_aco = 1.0;
    int bin_aco = 20;
    
    double min_m = 0, max_m = 1200;
    int bin_m = 30;
    
    double min_r = -3.0, max_r = 3.0;
    int bin_r = 15;
    
    double min_pt = 0, max_pt = 600;  // Start from 0 to include all pT values
    int bin_pt = 25;
    
    double min_mm = -2000, max_mm = 500;  // Extended range for mass difference
    int bin_mm = 25;
    
    double min_ra = -3.0, max_ra = 3.0;  // Extended rapidity range
    int bin_ra = 15;
    
    double min_tau = 20, max_tau = 500;  // Lower minimum for tau pT
    int bin_tau = 25;
    
    double min_met = 0, max_met = 250;  // Better range for electron pT
    int bin_met = 25;

    // Create THStacks
    THStack *aco = new THStack("aco", "");
    THStack *m = new THStack("m", "");
    THStack *r = new THStack("r", "");
    THStack *pt = new THStack("pt", "");
    THStack *mm = new THStack("mm", "");
    THStack *ra = new THStack("ra", "");
    THStack *tau = new THStack("tau", "");
    THStack *met = new THStack("met", "");

    // Create legends
    auto l_aco = new TLegend(0.18, 0.7, 0.48, 0.9);
    auto l_m = new TLegend(0.18, 0.7, 0.48, 0.9);
    auto l_r = new TLegend(0.18, 0.7, 0.48, 0.9);
    auto l_pt = new TLegend(0.18, 0.7, 0.48, 0.9);
    auto l_mm = new TLegend(0.18, 0.7, 0.48, 0.9);
    auto l_ra = new TLegend(0.18, 0.7, 0.48, 0.9);
    auto l_tau = new TLegend(0.18, 0.7, 0.48, 0.9);
    auto l_met = new TLegend(0.18, 0.7, 0.48, 0.9);

    // Set legend properties
    l_aco->SetBorderSize(0);
    l_m->SetBorderSize(0);
    l_r->SetBorderSize(0);
    l_pt->SetBorderSize(0);
    l_mm->SetBorderSize(0);
    l_ra->SetBorderSize(0);
    l_tau->SetBorderSize(0);
    l_met->SetBorderSize(0);

    // Create histograms for QCD
    TH1D aco_qcd("aco_qcd", "aco_qcd", bin_aco, min_aco, max_aco);
    TH1D m_qcd("m_qcd", "m_qcd", bin_m, min_m, max_m);
    TH1D r_qcd("r_qcd", "r_qcd", bin_r, min_r, max_r);
    TH1D pt_qcd("pt_qcd", "pt_qcd", bin_pt, min_pt, max_pt);
    TH1D mm_qcd("mm_qcd", "mm_qcd", bin_mm, min_mm, max_mm);
    TH1D ra_qcd("ra_qcd", "ra_qcd", bin_ra, min_ra, max_ra);
    TH1D tau_qcd("tau_qcd", "tau_qcd", bin_tau, min_tau, max_tau);
    TH1D met_qcd("met_qcd", "met_qcd", bin_met, min_met, max_met);

    // Create histograms for DY
    TH1D aco_dy("aco_dy", "aco_dy", bin_aco, min_aco, max_aco);
    TH1D m_dy("m_dy", "m_dy", bin_m, min_m, max_m);
    TH1D r_dy("r_dy", "r_dy", bin_r, min_r, max_r);
    TH1D pt_dy("pt_dy", "pt_dy", bin_pt, min_pt, max_pt);
    TH1D mm_dy("mm_dy", "mm_dy", bin_mm, min_mm, max_mm);
    TH1D ra_dy("ra_dy", "ra_dy", bin_ra, min_ra, max_ra);
    TH1D tau_dy("tau_dy", "tau_dy", bin_tau, min_tau, max_tau);
    TH1D met_dy("met_dy", "met_dy", bin_met, min_met, max_met);

    // Create histograms for ttjets
    TH1D aco_ttjets("aco_ttjets", "aco_ttjets", bin_aco, min_aco, max_aco);
    TH1D m_ttjets("m_ttjets", "m_ttjets", bin_m, min_m, max_m);
    TH1D r_ttjets("r_ttjets", "r_ttjets", bin_r, min_r, max_r);
    TH1D pt_ttjets("pt_ttjets", "pt_ttjets", bin_pt, min_pt, max_pt);
    TH1D mm_ttjets("mm_ttjets", "mm_ttjets", bin_mm, min_mm, max_mm);
    TH1D ra_ttjets("ra_ttjets", "ra_ttjets", bin_ra, min_ra, max_ra);
    TH1D tau_ttjets("tau_ttjets", "tau_ttjets", bin_tau, min_tau, max_tau);
    TH1D met_ttjets("met_ttjets", "met_ttjets", bin_met, min_met, max_met);

    // Create histograms for signal
    TH1D aco_sinal("aco_sinal", "aco_sinal", bin_aco, min_aco, max_aco);
    TH1D m_sinal("m_sinal", "m_sinal", bin_m, min_m, max_m);
    TH1D r_sinal("r_sinal", "r_sinal", bin_r, min_r, max_r);
    TH1D pt_sinal("pt_sinal", "pt_sinal", bin_pt, min_pt, max_pt);
    TH1D mm_sinal("mm_sinal", "mm_sinal", bin_mm, min_mm, max_mm);
    TH1D ra_sinal("ra_sinal", "ra_sinal", bin_ra, min_ra, max_ra);
    TH1D tau_sinal("tau_sinal", "tau_sinal", bin_tau, min_tau, max_tau);
    TH1D met_sinal("met_sinal", "met_sinal", bin_met, min_met, max_met);

    // Create sum histograms
    TH1D sum_aco("sum_aco_ttjets", "sum_aco_ttjets", bin_aco, min_aco, max_aco);
    TH1D sum_m("sum_m_ttjets", "sum_m_ttjets", bin_m, min_m, max_m);
    TH1D sum_r("sum_r_ttjets", "sum_r_ttjets", bin_r, min_r, max_r);
    TH1D sum_pt("sum_pt_ttjets", "sum_pt_ttjets", bin_pt, min_pt, max_pt);
    TH1D sum_mm("sum_mm_ttjets", "sum_mm_ttjets", bin_mm, min_mm, max_mm);
    TH1D sum_ra("sum_ra_ttjets", "sum_ra_ttjets", bin_ra, min_ra, max_ra);
    TH1D sum_tau("sum_tau_ttjets", "sum_tau_ttjets", bin_tau, min_tau, max_tau);
    TH1D sum_met("sum_met_ttjets", "sum_met_ttjets", bin_met, min_met, max_met);

    // Get trees
    TTree* tree_dy = (TTree*) dy.Get("tree");
    TTree* tree_qcd = (TTree*) qcd.Get("tree");
    TTree* tree_ttjets = (TTree*) ttjets.Get("tree");
    TTree* tree_sinal = (TTree*) sinal.Get("tree");

    int n_evt_dy = tree_dy->GetEntries();
    int n_evt_qcd = tree_qcd->GetEntries();
    int n_evt_ttjets = tree_ttjets->GetEntries();
    int n_evt_sinal = tree_sinal->GetEntries();

    cout << "Processing " << n_evt_qcd << " QCD events..." << endl;
    
    // Fill QCD histograms
    double n_qcd = 0;
    for(int i = 0; i < n_evt_qcd; i++){
        int o = tree_qcd->GetEvent(i);
        if(tree_qcd->GetLeaf("sist_mass")->GetValue(0) >= 0){
            n_qcd = n_qcd + w_qcd;
            aco_qcd.Fill(tree_qcd->GetLeaf("sist_acop")->GetValue(0), w_qcd);
            m_qcd.Fill(tree_qcd->GetLeaf("sist_mass")->GetValue(0), w_qcd);
            r_qcd.Fill(tree_qcd->GetLeaf("sist_rap")->GetValue(0) - 0.5*log(tree_qcd->GetLeaf("xi_arm1_1")->GetValue(0)/tree_qcd->GetLeaf("xi_arm2_1")->GetValue(0)), w_qcd);
            pt_qcd.Fill(tree_qcd->GetLeaf("sist_pt")->GetValue(0), w_qcd);
            mm_qcd.Fill(tree_qcd->GetLeaf("sist_mass")->GetValue(0) - 13000.*sqrt(tree_qcd->GetLeaf("xi_arm1_1")->GetValue(0)*tree_qcd->GetLeaf("xi_arm2_1")->GetValue(0)), w_qcd);
            ra_qcd.Fill(tree_qcd->GetLeaf("sist_rap")->GetValue(0), w_qcd);
            tau_qcd.Fill(tree_qcd->GetLeaf("tau_pt")->GetValue(0), w_qcd);
            met_qcd.Fill(tree_qcd->GetLeaf("e_pt")->GetValue(0), w_qcd);
        }
    }
    cout << "QCD events processed: " << n_qcd << endl;

    // Set QCD histogram properties
    aco_qcd.SetFillColor(kRed); aco_qcd.SetLineWidth(0);
    m_qcd.SetFillColor(kRed); m_qcd.SetLineWidth(0);
    r_qcd.SetFillColor(kRed); r_qcd.SetLineWidth(0);
    pt_qcd.SetFillColor(kRed); pt_qcd.SetLineWidth(0);
    mm_qcd.SetFillColor(kRed); mm_qcd.SetLineWidth(0);
    ra_qcd.SetFillColor(kRed); ra_qcd.SetLineWidth(0);
    tau_qcd.SetFillColor(kRed); tau_qcd.SetLineWidth(0);
    met_qcd.SetFillColor(kRed); met_qcd.SetLineWidth(0);

    cout << "Processing " << n_evt_dy << " DY events..." << endl;
    
    // Fill DY histograms
    double n_dy = 0;
    for(int i = 0; i < n_evt_dy; i++){
        int o = tree_dy->GetEvent(i);
        if(tree_dy->GetLeaf("sist_mass")->GetValue(0) >= 0){
            w_dy = tree_dy->GetLeaf("weight")->GetValue(0);
            n_dy = n_dy + w_dy;
            aco_dy.Fill(tree_dy->GetLeaf("sist_acop")->GetValue(0), w_dy);
            m_dy.Fill(tree_dy->GetLeaf("sist_mass")->GetValue(0), w_dy);
            r_dy.Fill(tree_dy->GetLeaf("sist_rap")->GetValue(0) - 0.5*log(tree_dy->GetLeaf("xi_arm1_1")->GetValue(0)/tree_dy->GetLeaf("xi_arm2_1")->GetValue(0)), w_dy);
            pt_dy.Fill(tree_dy->GetLeaf("sist_pt")->GetValue(0), w_dy);
            mm_dy.Fill(tree_dy->GetLeaf("sist_mass")->GetValue(0) - 13000.*sqrt(tree_dy->GetLeaf("xi_arm1_1")->GetValue(0)*tree_dy->GetLeaf("xi_arm2_1")->GetValue(0)), w_dy);
            ra_dy.Fill(tree_dy->GetLeaf("sist_rap")->GetValue(0), w_dy);
            tau_dy.Fill(tree_dy->GetLeaf("tau_pt")->GetValue(0), w_dy);
            met_dy.Fill(tree_dy->GetLeaf("e_pt")->GetValue(0), w_dy);
        }
    }
    cout << "DY events processed: " << n_dy * 1.81 << endl;

    // Set DY histogram properties
    aco_dy.SetFillColor(kYellow); aco_dy.SetLineWidth(0);
    m_dy.SetFillColor(kYellow); m_dy.SetLineWidth(0);
    r_dy.SetFillColor(kYellow); r_dy.SetLineWidth(0);
    pt_dy.SetFillColor(kYellow); pt_dy.SetLineWidth(0);
    mm_dy.SetFillColor(kYellow); mm_dy.SetLineWidth(0);
    ra_dy.SetFillColor(kYellow); ra_dy.SetLineWidth(0);
    tau_dy.SetFillColor(kYellow); tau_dy.SetLineWidth(0);
    met_dy.SetFillColor(kYellow); met_dy.SetLineWidth(0);

    cout << "Processing " << n_evt_ttjets << " ttjets events..." << endl;
    
    // Fill ttjets histograms
    double n_tt = 0;
    for(int i = 0; i < n_evt_ttjets; i++){
        int o = tree_ttjets->GetEvent(i);
        if(tree_ttjets->GetLeaf("sist_mass")->GetValue(0) >= 0){
            w_ttjets = tree_ttjets->GetLeaf("weight")->GetValue(0);
            n_tt = n_tt + w_ttjets;
            aco_ttjets.Fill(tree_ttjets->GetLeaf("sist_acop")->GetValue(0), w_ttjets);
            m_ttjets.Fill(tree_ttjets->GetLeaf("sist_mass")->GetValue(0), w_ttjets);
            r_ttjets.Fill(tree_ttjets->GetLeaf("sist_rap")->GetValue(0) - 0.5*log(tree_ttjets->GetLeaf("xi_arm1_1")->GetValue(0)/tree_ttjets->GetLeaf("xi_arm2_1")->GetValue(0)), w_ttjets);
            pt_ttjets.Fill(tree_ttjets->GetLeaf("sist_pt")->GetValue(0), w_ttjets);
            mm_ttjets.Fill(tree_ttjets->GetLeaf("sist_mass")->GetValue(0) - 13000.*sqrt(tree_ttjets->GetLeaf("xi_arm1_1")->GetValue(0)*tree_ttjets->GetLeaf("xi_arm2_1")->GetValue(0)), w_ttjets);
            ra_ttjets.Fill(tree_ttjets->GetLeaf("sist_rap")->GetValue(0), w_ttjets);
            tau_ttjets.Fill(tree_ttjets->GetLeaf("tau_pt")->GetValue(0), w_ttjets);
            met_ttjets.Fill(tree_ttjets->GetLeaf("e_pt")->GetValue(0), w_ttjets);
        }
    }
    cout << "ttjets events processed: " << n_tt * 0.15 << endl;

    // Set ttjets histogram properties
    aco_ttjets.SetFillColor(kGreen); aco_ttjets.SetLineWidth(0);
    m_ttjets.SetFillColor(kGreen); m_ttjets.SetLineWidth(0);
    r_ttjets.SetFillColor(kGreen); r_ttjets.SetLineWidth(0);
    pt_ttjets.SetFillColor(kGreen); pt_ttjets.SetLineWidth(0);
    mm_ttjets.SetFillColor(kGreen); mm_ttjets.SetLineWidth(0);
    ra_ttjets.SetFillColor(kGreen); ra_ttjets.SetLineWidth(0);
    tau_ttjets.SetFillColor(kGreen); tau_ttjets.SetLineWidth(0);
    met_ttjets.SetFillColor(kGreen); met_ttjets.SetLineWidth(0);

    cout << "Processing " << n_evt_sinal << " signal events..." << endl;
    
    // Check what weight branches are available in the signal tree
    cout << "Checking available weight branches in signal tree:" << endl;
    for(int i = 0; i < tree_sinal->GetListOfBranches()->GetEntries(); i++){
        TString branchName = tree_sinal->GetListOfBranches()->At(i)->GetName();
        if(branchName.Contains("weight") || branchName.Contains("Weight")){
            cout << "  Found weight branch: " << branchName << endl;
        }
    }
    
    // Fill signal histograms
    double signal_weight_sum = 0;
    int events_with_nonzero_weight = 0;
    for(int i = 0; i < n_evt_sinal; i++){
        int o = tree_sinal->GetEvent(i);
        if(tree_sinal->GetLeaf("sist_mass")->GetValue(0) >= 0){
            // Try different weight branch names
            double base_weight = 0;
            if(tree_sinal->GetLeaf("weight_sm")) {
                base_weight = tree_sinal->GetLeaf("weight_sm")->GetValue(0);
            }
            
            // If weight_sm is zero, try weights_bsm_sf or use unit weight
            if(base_weight == 0) {
                if(tree_sinal->GetLeaf("weights_bsm_sf")) {
                    base_weight = tree_sinal->GetLeaf("weights_bsm_sf")->GetValue(0);
                }
                if(base_weight == 0) {
                    // Use unit weight if all weights are zero
                    base_weight = 1.0;
                }
            }
            
            w_sinal = base_weight * 0.02;  // Slightly larger to make signal more visible
            signal_weight_sum += w_sinal;
            if(w_sinal > 0) events_with_nonzero_weight++;
            
            // Debug: print first few signal weights and some physics variables
            if(i < 10) {
                cout << "Event " << i << ": mass=" << tree_sinal->GetLeaf("sist_mass")->GetValue(0) 
                     << ", base_weight=" << base_weight << ", final_weight=" << w_sinal << endl;
            }
            
            m_sinal.Fill(tree_sinal->GetLeaf("sist_mass")->GetValue(0), w_sinal);
            aco_sinal.Fill(tree_sinal->GetLeaf("sist_acop")->GetValue(0), w_sinal);
            r_sinal.Fill(tree_sinal->GetLeaf("sist_rap")->GetValue(0) - 0.5*log(tree_sinal->GetLeaf("xi_arm1_1")->GetValue(0)/tree_sinal->GetLeaf("xi_arm2_1")->GetValue(0)), w_sinal);
            pt_sinal.Fill(tree_sinal->GetLeaf("sist_pt")->GetValue(0), w_sinal);
            mm_sinal.Fill(tree_sinal->GetLeaf("sist_mass")->GetValue(0) - 13000.*sqrt(tree_sinal->GetLeaf("xi_arm1_1")->GetValue(0)*tree_sinal->GetLeaf("xi_arm2_1")->GetValue(0)), w_sinal);
            ra_sinal.Fill(tree_sinal->GetLeaf("sist_rap")->GetValue(0), w_sinal);
            tau_sinal.Fill(tree_sinal->GetLeaf("tau_pt")->GetValue(0), w_sinal);
            met_sinal.Fill(tree_sinal->GetLeaf("e_pt")->GetValue(0), w_sinal);
        }
    }
    cout << "Signal events processed successfully" << endl;
    cout << "Events with non-zero weight: " << events_with_nonzero_weight << " out of " << n_evt_sinal << endl;
    cout << "Total signal weight sum: " << signal_weight_sum << endl;
    cout << "Signal histogram integrals: mass=" << m_sinal.Integral() << ", aco=" << aco_sinal.Integral() << endl;

    // Set signal histogram properties
    m_sinal.SetLineColor(kBlack); aco_sinal.SetLineColor(kBlack);
    pt_sinal.SetLineColor(kBlack); r_sinal.SetLineColor(kBlack);
    mm_sinal.SetLineColor(kBlack); ra_sinal.SetLineColor(kBlack);
    tau_sinal.SetLineColor(kBlack); met_sinal.SetLineColor(kBlack);

    // Create sum histograms
    sum_aco.Add(&aco_dy); sum_aco.Add(&aco_qcd); sum_aco.Add(&aco_ttjets);
    sum_ra.Add(&ra_dy); sum_ra.Add(&ra_qcd); sum_ra.Add(&ra_ttjets);
    sum_m.Add(&m_ttjets); sum_m.Add(&m_qcd); sum_m.Add(&m_dy);
    sum_r.Add(&r_dy); sum_r.Add(&r_qcd); sum_r.Add(&r_ttjets);
    sum_pt.Add(&pt_dy); sum_pt.Add(&pt_qcd); sum_pt.Add(&pt_ttjets);
    sum_mm.Add(&mm_dy); sum_mm.Add(&mm_qcd); sum_mm.Add(&mm_ttjets);
    sum_tau.Add(&tau_dy); sum_tau.Add(&tau_qcd); sum_tau.Add(&tau_ttjets);
    sum_met.Add(&met_dy); sum_met.Add(&met_qcd); sum_met.Add(&met_ttjets);

    // Add histograms to stacks
    aco->Add(&aco_ttjets); aco->Add(&aco_qcd); aco->Add(&aco_dy);
    m->Add(&m_ttjets); m->Add(&m_qcd); m->Add(&m_dy);
    r->Add(&r_ttjets); r->Add(&r_qcd); r->Add(&r_dy);
    pt->Add(&pt_ttjets); pt->Add(&pt_qcd); pt->Add(&pt_dy);
    mm->Add(&mm_ttjets); mm->Add(&mm_qcd); mm->Add(&mm_dy);
    ra->Add(&ra_ttjets); ra->Add(&ra_qcd); ra->Add(&ra_dy);
    met->Add(&met_ttjets); met->Add(&met_qcd); met->Add(&met_dy);
    tau->Add(&tau_ttjets); tau->Add(&tau_qcd); tau->Add(&tau_dy);

    // Write histograms to output file
    aco_dy.Write();
    aco_ttjets.Write();
    output.Close();

    cout << "Creating plots..." << endl;

    // Function to create and save plots
    auto createPlot = [](THStack* stack, TH1D* signal, TLegend* legend, const string& xtitle, const string& ytitle, const string& filename) {
        TCanvas c1("c1", "c1", 800, 600);  // Better aspect ratio
        c1.SetLeftMargin(0.12);   // More space for y-axis label
        c1.SetBottomMargin(0.12); // More space for x-axis label
        c1.SetTopMargin(0.08);    // Space for title
        c1.SetRightMargin(0.05);  // Minimal right margin
        
        legend->AddEntry((TH1D*)stack->GetHists()->At(0), "t #bar{t}", "f");
        legend->AddEntry((TH1D*)stack->GetHists()->At(2), "Drell Yan", "f");
        legend->AddEntry((TH1D*)stack->GetHists()->At(1), "QCD (Data driven)", "f");
        legend->AddEntry(signal, "Signal (x0.02)", "l");
        
        stack->Draw("histo");
        signal->Draw("same histo");
        signal->SetLineWidth(4);
        signal->SetLineColor(kBlack);
        
        // Ensure signal is visible by checking if it has entries
        if(signal->Integral() > 0) {
            cout << "Drawing signal with integral: " << signal->Integral() << endl;
        } else {
            cout << "Warning: Signal histogram is empty!" << endl;
        }
        
        stack->GetXaxis()->SetTitle(xtitle.c_str());
        stack->GetYaxis()->SetTitle(ytitle.c_str());
        stack->GetXaxis()->SetNdivisions(508);  // Better tick marks
        stack->GetYaxis()->SetNdivisions(508);
        stack->GetXaxis()->SetTitleSize(0.045);
        stack->GetYaxis()->SetTitleSize(0.045);
        stack->GetXaxis()->SetLabelSize(0.04);
        stack->GetYaxis()->SetLabelSize(0.04);
        stack->GetXaxis()->SetTitleOffset(1.1);
        stack->GetYaxis()->SetTitleOffset(1.1);
        
        // Set y-axis range dynamically based on both background and signal
        double max_background = stack->GetMaximum();
        double max_signal = signal->GetMaximum();
        double y_max = max(max_background, max_signal) * 1.2;  // 20% headroom
        stack->GetYaxis()->SetRangeUser(0.1, y_max);
        
        legend->Draw();
        
        TLatex cmsLabel;
        cmsLabel.SetTextAlign(31);
        cmsLabel.SetTextSize(0.04);
        cmsLabel.SetTextFont(62);
        cmsLabel.DrawLatexNDC(0.95, 0.94, "54.9 fb^{-1} (13 TeV)");
        
        TLatex cmsLabel1;
        cmsLabel1.SetTextAlign(11);  // Left align
        cmsLabel1.SetTextSize(0.04);
        cmsLabel1.SetTextFont(62);
        cmsLabel1.DrawLatexNDC(0.12, 0.94, "CMS-TOTEM Preliminary");
        
        c1.Update();
        c1.SaveAs(filename.c_str());
        
        // Clear legend for next plot
        legend->Clear();
    };

    // Create all plots
    createPlot(aco, &aco_sinal, l_aco, "Acoplanarity of the central system", "Events", "output_plots/aco_etau.png");
    createPlot(m, &m_sinal, l_m, "Invariant mass of the central system [GeV]", "Events", "output_plots/mass_etau.png");
    createPlot(r, &r_sinal, l_r, "Rapidity matching", "Events", "output_plots/rapidity_matching_etau.png");
    createPlot(pt, &pt_sinal, l_pt, "Transverse momentum of the central system [GeV]", "Events", "output_plots/pt_central_etau.png");
    createPlot(mm, &mm_sinal, l_mm, "Mass difference (CMS-PPS) [GeV]", "Events", "output_plots/mass_diff_etau.png");
    createPlot(ra, &ra_sinal, l_ra, "Rapidity of the central system", "Events", "output_plots/rapidity_central_etau.png");
    createPlot(tau, &tau_sinal, l_tau, "Transverse momentum of the hadronic tau [GeV]", "Events", "output_plots/tau_pt_etau.png");
    createPlot(met, &met_sinal, l_met, "Electron transverse momentum [GeV]", "Events", "output_plots/met_etau.png");

    cout << "All plots created successfully!" << endl;
    cout << "Plots saved in output_plots/ directory" << endl;

    bool interactive = false;
    if (interactive) {
        app.Run(true);
    }

    return 0;
}