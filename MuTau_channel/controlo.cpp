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

using namespace std;

int main(){

        TApplication app("app",NULL,NULL);

// samples for TTJets  region
          TFile sinal("./ETau_2018_UL_sgn_SM_highstat_skimmed.root");
          TFile dy("DY_merged_2018_UL_ETau.root");
          TFile ttjets("ttJets_2018_UL_skimmed_ETau_nano_merged.root");
          TFile qcd("QCD/QCD_merged_2018_UL_ETau_prov.root");

        double w_sinal=1;
        double w_qcd=738/124;
        double w_ttjets=0.171;
        double w_dy=2.08;

        double min_aco=0.;
        double max_aco=1.;
        int bin_aco=50;

        double min_m=0;
        double max_m=1100.0;
        int  bin_m=20;

        double min_r=-2.2;
        double max_r=2.2;
        //int bin_r=100;

        THStack *aco =new THStack("aco","");
        THStack *m =new THStack("m","");
        //THStack *r =new THStack("r","");

        auto l_aco = new TLegend(0.1,0.7,0.48,0.9);
        auto l_m = new TLegend(0.1,0.7,0.48,0.9);
        //auto l_r = new TLegend(0.1,0.7,0.48,0.9);

        TH1D aco_sinal("aco_sinal","aco_sinal",bin_aco, min_aco, max_aco);
        TH1D aco_qcd("aco_qcd","aco_qcd",bin_aco, min_aco, max_aco);
        TH1D aco_dy("aco_dy","aco_dy",bin_aco, min_aco, max_aco);
        TH1D aco_ttjets("aco_ttjets","aco_ttjets",bin_aco, min_aco, max_aco);
        TH1D sum_aco("sum_aco_ttjets","sum_aco_ttjets",bin_aco, min_aco, max_aco);


        TH1D m_sinal("m_sinal","m_sinal",bin_m, min_m, max_m);
        TH1D m_qcd("m_qcd","m_qcd",bin_m, min_m, max_m);
        TH1D m_dy("m_dy","m_dy",bin_m, min_m, max_m);
        TH1D m_ttjets("m_ttjets","m_ttjets",bin_m, min_m, max_m);
        TH1D sum_m("sum_m_ttjets","sum_m_ttjets",bin_m, min_m, max_m);

        //TH1D r_sinal("r_sinal","r_sinal",bin_r, min_r, max_r);
        //TH1D r_qcd("r_qcd","r_qcd",bin_r, min_r, max_r);
        //TH1D r_dy("r_dy","r_dy",bin_r, min_r, max_r);
        //TH1D r_ttjets("r_ttjets","r_ttjets",bin_r, min_r, max_r);
        //TH1D sum_r("sum_r_ttjets","sum_r_ttjets",bin_r, min_r, max_r);

        TH1D aco_fraction("aco_fraction","aco_fraction", bin_aco, min_aco, max_aco);
        TH1D m_fraction("m_fraction","m_fraction", bin_m, min_m, max_m);
        //TH1D r_fraction("r_fraction","r_fraction", bin_r, min_r, max_r);

        TTree* tree_sinal=(TTree*) sinal.Get("tree");
        TTree* tree_dy = (TTree*) dy.Get("tree");
        TTree* tree_qcd= (TTree*) qcd.Get("tree");
        TTree* tree_ttjets= (TTree*) ttjets.Get("tree");

        int n_evt_sinal=tree_sinal->GetEntries();
        int n_evt_dy=tree_dy->GetEntries();
        int n_evt_qcd=tree_qcd->GetEntries();
        int n_evt_ttjets=tree_ttjets->GetEntries();

        cout<<tree_dy->GetEntries() <<endl;

        for(int i=0; i<n_evt_sinal; i++){
                int o=tree_sinal->GetEvent(i);
		if(tree_sinal->GetLeaf("n_b_jet") ->GetValue(0) >=1){
                aco_sinal.Fill(tree_sinal->GetLeaf("sist_acop")->GetValue(0),w_sinal);
                m_sinal.Fill(tree_sinal->GetLeaf("sist_mass")->GetValue(0),w_sinal);
        //      r_sinal.Fill(tree_sinal->GetLeaf("tau_pair_rapidity")->GetValue(0),w_sinal);
        }
       }

        for(int i=0; i<n_evt_qcd; i++){
                int o=tree_qcd->GetEvent(i);
		if(tree_qcd->GetLeaf("n_b_jet") ->GetValue(0) >=1){
                aco_qcd.Fill(tree_qcd->GetLeaf("sist_acop")->GetValue(0),w_qcd);
                m_qcd.Fill(tree_qcd->GetLeaf("sist_mass")->GetValue(0),w_qcd);
               // r_qcd.Fill(tree_qcd->GetLeaf("tau_pair_rapidity")->GetValue(0),w_qcd);
 	}       
        }

        aco_qcd.SetFillColor(kRed);
        m_qcd.SetFillColor(kRed);
        //r_qcd.SetFillColor(kRed);

        for(int i=0; i<n_evt_dy; i++){
                int o=tree_dy->GetEvent(i);
		if(tree_dy->GetLeaf("n_b_jet") ->GetValue(0) >=1){
                //double w_dy=tree_dy->GetLeaf("weight")->GetValue(0);
                aco_dy.Fill(tree_dy->GetLeaf("sist_acop")->GetValue(0),w_dy);
                m_dy.Fill(tree_dy->GetLeaf("sist_mass")->GetValue(0),w_dy);
              //  r_dy.Fill(tree_dy->GetLeaf("tau_pair_rapidity")->GetValue(0),w_dy);
                

	}
        }

        aco_dy.SetFillColor(kYellow);
        m_dy.SetFillColor(kYellow);
        //r_dy.SetFillColor(kYellow);

        for(int i=0; i<n_evt_ttjets; i++){
                int o=tree_ttjets->GetEvent(i);
                if(tree_ttjets ->GetLeaf("n_b_jet")->GetValue(0)>=1){
                aco_ttjets.Fill(tree_ttjets->GetLeaf("sist_acop")->GetValue(0),w_ttjets);
                m_ttjets.Fill(tree_ttjets->GetLeaf("sist_mass")->GetValue(0),w_ttjets);
                //r_ttjets.Fill(tree_ttjets->GetLeaf("tau_pair_rapidity")->GetValue(0),w_ttjets);
        }
        }

        aco_ttjets.SetFillColor(kGreen);
        m_ttjets.SetFillColor(kGreen);
        //r_ttjets.SetFillColor(kGreen);

        aco_fraction=aco_sinal;
        m_fraction=m_sinal;
        //r_fraction=r_sinal;

        sum_aco=aco_dy;
        sum_aco.Add(&aco_qcd);
        sum_aco.Add(&aco_ttjets);

        sum_m=m_dy;
        sum_m.Add(&m_qcd);
        sum_m.Add(&m_ttjets);

        //sum_r=r_dy;
        //sum_r.Add(&r_qcd);
        //sum_r.Add(&r_ttjets);

        aco->Add(&aco_qcd);
        aco->Add(&aco_dy);
        aco->Add(&aco_ttjets);
        

        aco_fraction.Divide(&sum_aco);

        m->Add(&m_qcd);
        m->Add(&m_dy);
        m->Add(&m_ttjets);
      

        m_fraction.Divide(&sum_m);

        //r->Add(&r_qcd);
        //r->Add(&r_ttjets);
        //r->Add(&r_dy);

        //r_fraction.Divide(&sum_r);
        TF1* reta = new TF1("fa1","1",0,1000);
      

        TCanvas c1;
        c1.Divide(1,2);
        c1.cd(1);
        
        l_aco->AddEntry(&aco_dy, "DY contribution in 100/fb", "f");
        l_aco->AddEntry(&aco_ttjets, "TTJets contribution in 100/fb", "f");
        l_aco->AddEntry(&aco_qcd, "QCD contribution in 100/fb", "f");
        l_aco->AddEntry(&aco_sinal, "sinal", "lep");
        
        aco->Draw("histo");
        aco->GetXaxis()->SetTitle("Acoplanarity of the central system");
        aco_sinal.GetYaxis()->SetRangeUser(0., 5);
        aco_sinal.Draw("same && E");
        gPad->SetLogy();
        l_aco->Draw();
        
        c1.cd(2);
        aco_fraction.Draw("E");
        aco_fraction.GetXaxis()->SetTitle("Acoplanarity of the central system");
        aco_fraction.GetYaxis()->SetTitle("sinal/MC");
        aco_fraction.GetYaxis()->SetRangeUser(0., 2.);
        reta->Draw("same");
        
        
        TCanvas c2;
        c2.Divide(1,2);
        c2.cd(1);
        l_m->AddEntry(&m_dy, "DY contribution in 100/fb", "f");
        l_m->AddEntry(&m_ttjets, "TTJets contribution in 100/fb", "f");
        l_m->AddEntry(&m_qcd, "QCD contribution in 100/fb", "f");
        l_m->AddEntry(&m_sinal, "sinal", "lep");
        m->Draw("histo");
        m->GetXaxis()->SetTitle("Invariant mass of the central system [GeV]");
        m_sinal.Draw("same && E");
        gPad->SetLogy();
        l_m->Draw();
        c2.cd(2);
        m_fraction.Draw("E");
        m_fraction.GetXaxis()->SetTitle("Invariant mass of the central system [GeV]");
        m_fraction.GetYaxis()->SetTitle("sinal/MC");
        m_fraction.GetYaxis()->SetRangeUser(0., 2.);
        reta->Draw("same");

        TCanvas c3;
        c3.Divide(1,2);
        c3.cd(1);
        //l_r->AddEntry(&r_ttjets, "TTJets contribution in 100/fb", "f");
        //l_r->AddEntry(&r_dy, "DY contribution in 100/fb", "f");
        //l_r->AddEntry(&r_qcd, "QCD contribution in 100/fb", "f");
        //l_r->AddEntry(&r_sinal, "sinal", "lep");
        //r->Draw("histo");
        //r->GetXaxis()->SetTitle("Rapidity of the central system");
        //r_sinal.Draw("same && E");
        gPad->SetLogy();
        //l_r->Draw();
        c3.cd(2);
        //r_fraction.Draw("E");
        //r_fraction.GetXaxis()->SetTitle("Rapidity of the central system");
        //r_fraction.GetYaxis()->SetTitle("sinal/MC");

        app.Run("true");

        return 0;

}
