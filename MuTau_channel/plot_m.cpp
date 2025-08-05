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

using namespace std;


int main(){

        TApplication app("app",NULL,NULL);

	string luminosity;
// samples for DY  region
          //TFile data("/eos/user/m/mpisano/samples_2018_emu/fase1/Dados_fase1_PICskimmed_EMu_2018_total.root");
          TFile dy("DY_total-protons.root");
          TFile ttjets("ttjets_total-protons.root");
          TFile qcd("QCD_total-protons.root");

	TFile sinal("../MuTau_sinal_SM_2018_july.root");


	TFile output("DY_CR_e_mu_UL_2018_shapes.root","RECREATE","");

        //double w_data=1.;
        double w_qcd=1.;
        double w_ttjets=0.15;
        double w_dy=1.81;
	double w_sinal;

        double min_aco=0.0;
        double max_aco=1.0;
        int bin_aco=20;

        double min_m=0;
        double max_m=1200;
        int  bin_m=30;

        double min_r=-2.2;
        double max_r=2.2;
        int bin_r=10;

	double min_pt=100;
        double max_pt=600;
        int bin_pt=20;

	double min_mm=-1500;
        double max_mm=200;
        int bin_mm=20;

	double min_ra=-2.2;
        double max_ra=2.2;
        int bin_ra=10;

	double min_tau=100;
        double max_tau=500;
        int bin_tau=20;

	double min_met=0;
        double max_met=300;
        int bin_met=20;

        THStack *aco =new THStack("aco","");
        THStack *m =new THStack("m","");
        THStack *r =new THStack("r","");
	THStack *pt =new THStack("pt","");
	THStack *mm =new THStack("mm","");
	THStack *ra =new THStack("ra","");
	THStack *tau =new THStack("tau","");
	THStack *met =new THStack("met","");

        auto l_aco = new TLegend(0.18,0.7,0.48,0.9);
        auto l_m = new TLegend(0.18,0.7,0.48,0.9);
        auto l_r = new TLegend(0.18,0.7,0.48,0.9);
	auto l_pt =new TLegend(0.18,0.7,0.48,0.9);
	auto l_mm =new TLegend(0.18,0.7,0.48,0.9);
	auto l_ra =new TLegend(0.18,0.7,0.48,0.9);
	auto l_tau =new TLegend(0.18,0.7,0.48,0.9);
	auto l_met =new TLegend(0.18,0.7,0.48,0.9);

	l_aco->SetBorderSize(0);
	l_m->SetBorderSize(0);
	l_r->SetBorderSize(0);
        l_pt->SetBorderSize(0);
	l_mm->SetBorderSize(0);
	l_ra->SetBorderSize(0);
	l_tau->SetBorderSize(0);
	l_met->SetBorderSize(0);

        //TH1D aco_data("aco_data","aco_data",bin_aco, min_aco, max_aco);
        TH1D aco_qcd("aco_qcd","aco_qcd",bin_aco, min_aco, max_aco);
        TH1D aco_dy("aco_dy","aco_dy",bin_aco, min_aco, max_aco);
        TH1D aco_ttjets("aco_ttjets","aco_ttjets",bin_aco, min_aco, max_aco);
        TH1D sum_aco("sum_aco_ttjets","sum_aco_ttjets",bin_aco, min_aco, max_aco);
	TH1D aco_sinal("aco_sinal","aco_sinal",bin_aco, min_aco, max_aco);

        //TH1D m_data("m_data","m_data",bin_m, min_m, max_m);
        TH1D m_qcd("m_qcd","m_qcd",bin_m, min_m, max_m);
        TH1D m_dy("m_dy","m_dy",bin_m, min_m, max_m);
        TH1D m_ttjets("m_ttjets","m_ttjets",bin_m, min_m, max_m);
        TH1D sum_m("sum_m_ttjets","sum_m_ttjets",bin_m, min_m, max_m);
	TH1D m_sinal("m_sinal","m_sinal",bin_m, min_m, max_m);

        //TH1D r_data("r_data","r_data",bin_r, min_r, max_r);
        TH1D r_qcd("r_qcd","r_qcd",bin_r, min_r, max_r);
        TH1D r_dy("r_dy","r_dy",bin_r, min_r, max_r);
        TH1D r_ttjets("r_ttjets","r_ttjets",bin_r, min_r, max_r);
        TH1D sum_r("sum_r_ttjets","sum_r_ttjets",bin_r, min_r, max_r);
	TH1D r_sinal("r_sinal","r_sinal",bin_r, min_r, max_r);

	//TH1D pt_data("pt_data","pt_data",bin_pt, min_pt, max_pt);
        TH1D pt_qcd("pt_qcd","pt_qcd",bin_pt, min_pt, max_pt);
        TH1D pt_dy("pt_dy","pt_dy",bin_pt, min_pt, max_pt);
        TH1D pt_ttjets("pt_ttjets","pt_ttjets",bin_pt, min_pt, max_pt);
        TH1D sum_pt("sum_pt_ttjets","sum_pt_ttjets",bin_pt, min_pt, max_pt);
	TH1D pt_sinal("pt_sinal","pt_sinal",bin_pt, min_pt, max_pt);


	//TH1D mm_data("mm_data","mm_data",bin_mm, min_mm, max_mm);
        TH1D mm_qcd("mm_qcd","mm_qcd",bin_mm, min_mm, max_mm);
        TH1D mm_dy("mm_dy","mm_dy",bin_mm, min_mm, max_mm);
        TH1D mm_ttjets("mm_ttjets","mm_ttjets",bin_mm, min_mm, max_mm);
        TH1D sum_mm("sum_mm_ttjets","sum_mm_ttjets",bin_mm, min_mm, max_mm);
        TH1D mm_sinal("mm_sinal","mm_sinal",bin_mm, min_mm, max_mm);

	//TH1D ra_data("ra_data","ra_data",bin_ra, min_ra, max_ra);
        TH1D ra_qcd("ra_qcd","ra_qcd",bin_ra, min_ra, max_ra);
        TH1D ra_dy("ra_dy","ra_dy",bin_ra, min_ra, max_ra);
        TH1D ra_ttjets("ra_ttjets","ra_ttjets",bin_ra, min_ra, max_ra);
        TH1D sum_ra("sum_ra_ttjets","sum_ra_ttjets",bin_ra, min_ra, max_ra);
        TH1D ra_sinal("ra_sinal","ra_sinal",bin_ra, min_ra, max_ra);

	//TH1D tau_data("tau_data","tau_data",bin_tau, min_tau, max_tau);
        TH1D tau_qcd("tau_qcd","tau_qcd",bin_tau, min_tau, max_tau);
        TH1D tau_dy("tau_dy","tau_dy",bin_tau, min_tau, max_tau);
        TH1D tau_ttjets("tau_ttjets","tau_ttjets",bin_tau, min_tau, max_tau);
        TH1D sum_tau("sum_tau_ttjets","sum_tau_ttjets",bin_tau, min_tau, max_tau);
        TH1D tau_sinal("tau_sinal","tau_sinal",bin_tau, min_tau, max_tau);

	//TH1D met_data("met_data","met_data",bin_met, min_met, max_met);
        TH1D met_qcd("met_qcd","met_qcd",bin_met, min_met, max_met);
        TH1D met_dy("met_dy","met_dy",bin_met, min_met, max_met);
        TH1D met_ttjets("met_ttjets","met_ttjets",bin_met, min_met, max_met);
        TH1D sum_met("sum_met_ttjets","sum_met_ttjets",bin_met, min_met, max_met);
        TH1D met_sinal("met_sinal","met_sinal",bin_met, min_met, max_met);


        //TH1D aco_fraction("aco_fraction","aco_fraction", bin_aco, min_aco, max_aco);
        //TH1D m_fraction("m_fraction","m_fraction", bin_m, min_m, max_m);
        //TH1D r_fraction("r_fraction","r_fraction", bin_r, min_r, max_r);
	//TH1D pt_fraction("pt_fraction","pt_fraction", bin_pt, min_pt, max_pt);

        //TTree* tree_data=(TTree*) data.Get("tree");
        TTree* tree_dy = (TTree*) dy.Get("tree");
        TTree* tree_qcd= (TTree*) qcd.Get("tree");
        TTree* tree_ttjets= (TTree*) ttjets.Get("tree");
	TTree* tree_sinal= (TTree*) sinal.Get("tree");

        //int n_evt_data=tree_data->GetEntries();
        int n_evt_dy=tree_dy->GetEntries();
        int n_evt_qcd=tree_qcd->GetEntries();
        int n_evt_ttjets=tree_ttjets->GetEntries();
	int n_evt_sinal = tree_sinal->GetEntries();

        //cout<<tree_dy->GetEntries() <<endl;

        //for(int i=0; i<n_evt_data; i++){
        //        int o=tree_data->GetEvent(i);
		//if(tree_data->GetLeaf("sist_mass") ->GetValue(0) <=100 && tree_data->GetLeaf("sist_acop")->GetValue(0)<=0.3 && tree_data->GetLeaf("n_b_jet")->GetValue(0)==0){
		//w_data = tree_data->GetLeaf("weight")->GetValue(0);
	        //aco_data.Fill(tree_data->GetLeaf("sist_acop")->GetValue(0),w_data);
                //m_data.Fill(tree_data->GetLeaf("sist_mass")->GetValue(0),w_data);
                //r_data.Fill(tree_data->GetLeaf("sist_rap")->GetValue(0),w_data);
		//pt_data.Fill(tree_data->GetLeaf("sist_pt")->GetValue(0),w_data);
        //}
        //}
       
        double n_qcd=0;
        for(int i=0; i<n_evt_qcd; i++){
                int o=tree_qcd->GetEvent(i);
		if(tree_qcd->GetLeaf("sist_mass") ->GetValue(0) >=0){
		//NAO ATIVAR//w_qcd = tree_qcd->GetLeaf("weight")->GetValue(0);
                n_qcd = n_qcd + w_qcd;
                aco_qcd.Fill(tree_qcd->GetLeaf("sist_acop")->GetValue(0),w_qcd);
                m_qcd.Fill(tree_qcd->GetLeaf("sist_mass")->GetValue(0),w_qcd);
                r_qcd.Fill(tree_qcd->GetLeaf("sist_rap")->GetValue(0)-0.5*log(tree_qcd->GetLeaf("xi_arm1_1")->GetValue(0)/tree_qcd->GetLeaf("xi_arm2_1")->GetValue(0)),w_qcd);
		pt_qcd.Fill(tree_qcd->GetLeaf("sist_pt")->GetValue(0),w_qcd);
		mm_qcd.Fill(tree_qcd->GetLeaf("sist_mass")->GetValue(0)-13000.*sqrt(tree_qcd->GetLeaf("xi_arm1_1")->GetValue(0)*tree_qcd->GetLeaf("xi_arm2_1")->GetValue(0)),w_qcd);
		ra_qcd.Fill(tree_qcd->GetLeaf("sist_rap")->GetValue(0),w_qcd);
		tau_qcd.Fill(tree_qcd->GetLeaf("tau_pt")->GetValue(0),w_qcd);
		met_qcd.Fill(tree_qcd->GetLeaf("mu_pt")->GetValue(0),w_qcd);
 	}
        }

	 m_qcd.SetFillColor(kRed);
        cout<<n_qcd<<endl;

        aco_qcd.SetFillColor(kRed);
        r_qcd.SetFillColor(kRed);
	pt_qcd.SetFillColor(kRed);
        mm_qcd.SetFillColor(kRed);
	ra_qcd.SetFillColor(kRed);
	tau_qcd.SetFillColor(kRed);
	met_qcd.SetFillColor(kRed);

	aco_qcd.SetLineWidth(0);
	m_qcd.SetLineWidth(0);
        r_qcd.SetLineWidth(0);
        pt_qcd.SetLineWidth(0);
        mm_qcd.SetLineWidth(0);
        ra_qcd.SetLineWidth(0);
        tau_qcd.SetLineWidth(0);
        met_qcd.SetLineWidth(0);

        double n_dy=0;
        for(int i=0; i<n_evt_dy; i++){
                int o=tree_dy->GetEvent(i);
		if(tree_dy->GetLeaf("sist_mass") ->GetValue(0) >=0){
		w_dy = tree_dy->GetLeaf("weight")->GetValue(0);
                n_dy = n_dy + w_dy;
                aco_dy.Fill(tree_dy->GetLeaf("sist_acop")->GetValue(0),w_dy);
                m_dy.Fill(tree_dy->GetLeaf("sist_mass")->GetValue(0),w_dy);
		r_dy.Fill(tree_dy->GetLeaf("sist_rap")->GetValue(0)-0.5*log(tree_dy->GetLeaf("xi_arm1_1")->GetValue(0)/tree_dy->GetLeaf("xi_arm2_1")->GetValue(0)),w_dy);
                pt_dy.Fill(tree_dy->GetLeaf("sist_pt")->GetValue(0),w_dy);
		mm_dy.Fill(tree_dy->GetLeaf("sist_mass")->GetValue(0)-13000.*sqrt(tree_dy->GetLeaf("xi_arm1_1")->GetValue(0)*tree_dy->GetLeaf("xi_arm2_1")->GetValue(0)),w_dy);
		ra_dy.Fill(tree_dy->GetLeaf("sist_rap")->GetValue(0),w_dy);
		tau_dy.Fill(tree_dy->GetLeaf("tau_pt")->GetValue(0),w_dy);
                met_dy.Fill(tree_dy->GetLeaf("mu_pt")->GetValue(0),w_dy);
	}
        }
        cout<<n_dy*1.81<<endl;

        aco_dy.SetFillColor(kYellow);
        m_dy.SetFillColor(kYellow);
        r_dy.SetFillColor(kYellow);
	pt_dy.SetFillColor(kYellow);
	mm_dy.SetFillColor(kYellow);
	ra_dy.SetFillColor(kYellow);
	tau_dy.SetFillColor(kYellow);
	met_dy.SetFillColor(kYellow);

	aco_dy.SetLineWidth(0);
        m_dy.SetLineWidth(0);   
        r_dy.SetLineWidth(0);   
        pt_dy.SetLineWidth(0);   
        mm_dy.SetLineWidth(0);   
        ra_dy.SetLineWidth(0);   
        tau_dy.SetLineWidth(0);   
        met_dy.SetLineWidth(0);

        double n_tt=0;
        for(int i=0; i<n_evt_ttjets; i++){
                int o=tree_ttjets->GetEvent(i);
                if(tree_ttjets->GetLeaf("sist_mass") ->GetValue(0)>=0){
		w_ttjets = tree_ttjets->GetLeaf("weight")->GetValue(0);
                n_tt=n_tt+w_ttjets;
                aco_ttjets.Fill(tree_ttjets->GetLeaf("sist_acop")->GetValue(0),w_ttjets);
                m_ttjets.Fill(tree_ttjets->GetLeaf("sist_mass")->GetValue(0),w_ttjets);
		r_ttjets.Fill(tree_ttjets->GetLeaf("sist_rap")->GetValue(0)-0.5*log(tree_ttjets->GetLeaf("xi_arm1_1")->GetValue(0)/tree_ttjets->GetLeaf("xi_arm2_1")->GetValue(0)),w_ttjets);
		pt_ttjets.Fill(tree_ttjets->GetLeaf("sist_pt")->GetValue(0),w_ttjets);
		mm_ttjets.Fill(tree_ttjets->GetLeaf("sist_mass")->GetValue(0)-13000.*sqrt(tree_ttjets->GetLeaf("xi_arm1_1")->GetValue(0)*tree_ttjets->GetLeaf("xi_arm2_1")->GetValue(0)),w_ttjets);
		ra_ttjets.Fill(tree_ttjets->GetLeaf("sist_rap")->GetValue(0),w_ttjets);
		tau_ttjets.Fill(tree_ttjets->GetLeaf("tau_pt")->GetValue(0),w_ttjets);
                met_ttjets.Fill(tree_ttjets->GetLeaf("mu_pt")->GetValue(0),w_ttjets);
        }
        }
        cout<<n_tt*0.15<<endl;

        aco_ttjets.SetFillColor(kGreen);
        m_ttjets.SetFillColor(kGreen);
        r_ttjets.SetFillColor(kGreen);
	pt_ttjets.SetFillColor(kGreen);
	mm_ttjets.SetFillColor(kGreen);
	ra_ttjets.SetFillColor(kGreen);
	tau_ttjets.SetFillColor(kGreen);
	met_ttjets.SetFillColor(kGreen);

	aco_ttjets.SetLineWidth(0);
        m_ttjets.SetLineWidth(0);   
        r_ttjets.SetLineWidth(0);   
        pt_ttjets.SetLineWidth(0);   
        mm_ttjets.SetLineWidth(0);   
        ra_ttjets.SetLineWidth(0);   
        tau_ttjets.SetLineWidth(0);   
        met_ttjets.SetLineWidth(0);

	for(int i=0; i<n_evt_sinal; i++){
                int o=tree_sinal->GetEvent(i);
                if(tree_sinal->GetLeaf("sist_mass") ->GetValue(0)>=0){
                w_sinal = tree_sinal->GetLeaf("weight")->GetValue(0)*5000;
                m_sinal.Fill(tree_sinal->GetLeaf("sist_mass")->GetValue(0),w_sinal);
		aco_sinal.Fill(tree_sinal->GetLeaf("sist_acop")->GetValue(0),w_sinal);
		r_sinal.Fill(tree_sinal->GetLeaf("sist_rap")->GetValue(0)-0.5*log(tree_sinal->GetLeaf("xi_arm1_1")->GetValue(0)/tree_sinal->GetLeaf("xi_arm2_1")->GetValue(0)),w_sinal);
		pt_sinal.Fill(tree_sinal->GetLeaf("sist_pt")->GetValue(0),w_sinal);
		mm_sinal.Fill(tree_sinal->GetLeaf("sist_mass")->GetValue(0)-13000.*sqrt(tree_sinal->GetLeaf("xi_arm1_1")->GetValue(0)*tree_sinal->GetLeaf("xi_arm2_1")->GetValue(0)),w_sinal);
		ra_sinal.Fill(tree_sinal->GetLeaf("sist_rap")->GetValue(0),w_sinal);
		tau_sinal.Fill(tree_sinal->GetLeaf("tau_pt")->GetValue(0),w_sinal);
                met_sinal.Fill(tree_sinal->GetLeaf("mu_pt")->GetValue(0),w_sinal);
        }
	}

        m_sinal.SetLineColor(kBlack);
	aco_sinal.SetLineColor(kBlack);
	pt_sinal.SetLineColor(kBlack);
	r_sinal.SetLineColor(kBlack);
	mm_sinal.SetLineColor(kBlack);
	ra_sinal.SetLineColor(kBlack);
	tau_sinal.SetLineColor(kBlack);
	met_sinal.SetLineColor(kBlack);

        //aco_fraction=aco_data;
        //m_fraction=m_data;
        //r_fraction=r_data;
	//pt_fraction=pt_data;

        sum_aco=aco_dy;
        sum_aco.Add(&aco_qcd);
        sum_aco.Add(&aco_ttjets);

	sum_ra=ra_dy;
        sum_ra.Add(&ra_qcd);
        sum_ra.Add(&ra_ttjets);

        sum_m=m_ttjets;
        sum_m.Add(&m_qcd);
        sum_m.Add(&m_dy);

        sum_r=r_dy;
        sum_r.Add(&r_qcd);
        sum_r.Add(&r_ttjets);

	sum_pt=pt_dy;
        sum_pt.Add(&pt_qcd);
        sum_pt.Add(&pt_ttjets);

	sum_mm=mm_dy;
        sum_mm.Add(&mm_qcd);
        sum_mm.Add(&mm_ttjets);

	sum_tau=tau_dy;
        sum_tau.Add(&tau_qcd);
        sum_tau.Add(&tau_ttjets);

	sum_met=met_dy;
        sum_met.Add(&met_qcd);
        sum_met.Add(&met_ttjets);

        aco->Add(&aco_ttjets);
        aco->Add(&aco_qcd);
        aco->Add(&aco_dy);

        //aco_fraction.Divide(&sum_aco);

        m->Add(&m_ttjets);
        m->Add(&m_qcd);
        m->Add(&m_dy);

        //m_fraction.Divide(&sum_m);

	r->Add(&r_ttjets);
        r->Add(&r_qcd);
        r->Add(&r_dy);

        //r_fraction.Divide(&sum_r);

	pt->Add(&pt_ttjets);
        pt->Add(&pt_qcd);
        pt->Add(&pt_dy);

        //pt_fraction.Divide(&sum_pt);

	mm->Add(&mm_ttjets);
        mm->Add(&mm_qcd);
        mm->Add(&mm_dy);

        //pt_fraction.Divide(&sum_pt);

	ra->Add(&ra_ttjets);
        ra->Add(&ra_qcd);
        ra->Add(&ra_dy);

        //ra_fraction.Divide(&sum_ra);

	met->Add(&met_ttjets);
        met->Add(&met_qcd);
        met->Add(&met_dy);

        //met_fraction.Divide(&sum_met);

	tau->Add(&tau_ttjets);
        tau->Add(&tau_qcd);
        tau->Add(&tau_dy);

        //tau_fraction.Divide(&sum_tau);

	aco_dy.Write();
	//aco_qcd.Write();
	aco_ttjets.Write();
	//aco_data.Write();
	output.Close();

        TF1 f1 ("f1","1",0,1);

        TCanvas c1;
	c1.SetCanvasSize(700,800);
	gPad->SetPad(0.1, 0.1, 1.0, 1.0);
	gPad->Draw();
	//l_aco->AddEntry(&aco_data, "Data (2018)", "lep");
        l_aco->AddEntry(&aco_ttjets, "t \\bar{t}", "f");
        l_aco->AddEntry(&aco_dy, "Drell Yan", "f");
        l_aco->AddEntry(&aco_qcd, "QCD (Data driven)", "f");
	l_aco->AddEntry(&aco_sinal, "Signal (x5000)", "l");
        aco->Draw("histo");
	aco_sinal.Draw("same && histo");
	aco_sinal.SetLineWidth(3);
        aco->GetXaxis()->SetTitle("Acoplanarity of the central system");
	aco->GetYaxis()->SetTitle("Events");
        //aco_data.Draw("same && E");
	//aco_data.SetLineColor(kBlack);
	//aco_data.SetLineWidth(2);
        //gPad->SetLogy();
        l_aco->Draw();
	aco->GetXaxis()->SetNdivisions(5);
  	aco->GetYaxis()->SetNdivisions(5);
	aco->GetXaxis()->SetTitleSize(0.03);
	aco->GetYaxis()->SetTitleSize(0.03);
	aco->GetXaxis()->SetLabelSize(0.03);
	aco->GetYaxis()->SetLabelSize(0.03);
	aco->GetXaxis()->SetLabelOffset(0.01);
  	aco->GetYaxis()->SetLabelOffset(0.01);
  	aco->GetXaxis()->SetTitleOffset(1.);
	aco->GetYaxis()->SetRangeUser(0.1,200);
	//aco_data.SetMarkerStyle(20);

	TLatex cmsLabel;
	cmsLabel.SetTextAlign(31);
	cmsLabel.SetTextSize(0.04);
	cmsLabel.SetTextFont(62);
	cmsLabel.DrawLatexNDC(0.90, 0.92, "54.9 fb^{-1} (13 TeV)");
	
	TLatex cmsLabel1;
        cmsLabel1.SetTextAlign(31);
        cmsLabel1.SetTextSize(0.04);
        cmsLabel1.SetTextFont(62);
        cmsLabel1.DrawLatexNDC(0.5, 0.92, "CMS-TOTEM Preliminary");

	c1.Update();

        f1.Draw("same");

	c1.Update();

        TF1 f2 ("f2","1",-1100,1100);

        TCanvas c2;
	c2.SetCanvasSize(700,800);
        gPad->SetPad(0.1, 0.1, 1.0, 1.0);
        gPad->Draw();             // Draw the upper pad: pad1
	//l_m->AddEntry(&m_data, "Data (2018)", "lep");
        l_m->AddEntry(&m_ttjets, "t \\bar{t}", "f");
        l_m->AddEntry(&m_dy, "Drell Yan", "f");
        l_m->AddEntry(&m_qcd, "QCD (Data driven)", "f");
	l_m->AddEntry(&m_sinal, "Signal x 5000", "l");
        m->Draw("histo");
        m->GetXaxis()->SetTitle("Invariant mass of the central system [GeV]");
	m->GetYaxis()->SetTitle("Events");
        m_sinal.Draw("same && histo");
	m_sinal.SetLineWidth(3);
        //m_data.Draw("same && E");
	//m_data.SetLineColor(kBlack);
        //m_data.SetLineWidth(2);
        //gPad->SetLogy();
        l_m->Draw();
	m->GetXaxis()->SetNdivisions(5);
        m->GetYaxis()->SetNdivisions(5);
        m->GetXaxis()->SetTitleSize(0.03);
        m->GetYaxis()->SetTitleSize(0.03);
        m->GetXaxis()->SetLabelSize(0.03);
        m->GetYaxis()->SetLabelSize(0.03);
        m->GetXaxis()->SetLabelOffset(0.01);
        m->GetYaxis()->SetLabelOffset(0.01);
        m->GetXaxis()->SetTitleOffset(1.);
        m->GetYaxis()->SetRangeUser(0.1,200);
        //m_data.SetMarkerStyle(20);

        TLatex cmsLabel2;
        cmsLabel2.SetTextAlign(31);
        cmsLabel2.SetTextSize(0.04);
        cmsLabel2.SetTextFont(62);
        cmsLabel2.DrawLatexNDC(0.90, 0.92, "54.9 fb^{-1} (13 TeV)");
        
       	TLatex cmsLabel3;
        cmsLabel3.SetTextAlign(31);
        cmsLabel3.SetTextSize(0.04);
        cmsLabel3.SetTextFont(62);
        cmsLabel3.DrawLatexNDC(0.5, 0.92, "CMS-TOTEM Preliminary");

        c2.Update();


	TCanvas c3;
	c3.SetCanvasSize(700,800);
	gPad->SetPad(0.1, 0.1, 1.0, 1.0);
        //gPad->Draw();             // Draw the upper pad: pad1
        gPad->Draw();
	//l_r->AddEntry(&r_data, "Data (2018)", "lep");
        l_r->AddEntry(&r_ttjets, "t \\bar{t}", "f");
        l_r->AddEntry(&r_dy, "Drell Yan", "f");
        l_r->AddEntry(&r_qcd, "QCD (Data driven)", "f");
	l_r->AddEntry(&r_sinal, "Signal (x 5000)", "l");
        r->Draw("histo");
        r->GetXaxis()->SetTitle("Rapidity matching");
	r->GetYaxis()->SetTitle("Events");
	r_sinal.Draw("histo && same");
	r_sinal.SetLineWidth(3);
	l_r->Draw();
	r->GetXaxis()->SetNdivisions(5);
        r->GetYaxis()->SetNdivisions(5);
        r->GetXaxis()->SetTitleSize(0.03);
        r->GetYaxis()->SetTitleSize(0.03);
        r->GetXaxis()->SetLabelSize(0.03);
        r->GetYaxis()->SetLabelSize(0.03);
        r->GetXaxis()->SetLabelOffset(0.01);
        r->GetYaxis()->SetLabelOffset(0.01);
        r->GetXaxis()->SetTitleOffset(1.);
        r->GetYaxis()->SetRangeUser(0.1,200);
        //r_data.SetMarkerStyle(20);
        //gPad->SetLogy();
	//r_data.SetLineColor(kBlack);
        //r_data.SetLineWidth(2);
	//r_data.Draw("same && E");

	TLatex cmsLabel4;
        cmsLabel4.SetTextAlign(31);
        cmsLabel4.SetTextSize(0.04);
        cmsLabel4.SetTextFont(62);
        cmsLabel4.DrawLatexNDC(0.90, 0.92, "54.9 fb^{-1} (13 TeV)");
        
       	TLatex cmsLabel5;
        cmsLabel5.SetTextAlign(31);
        cmsLabel5.SetTextSize(0.04);
        cmsLabel5.SetTextFont(62);
        cmsLabel5.DrawLatexNDC(0.5, 0.92, "CMS-TOTEM Preliminary");

        c3.Update();


	TCanvas c4;
	c4.SetCanvasSize(700,800);
	gPad->SetPad(0.1, 0.1, 1.0, 1.0);
        gPad->Draw();
	//l_pt->AddEntry(&r_data, "Data (2018)", "lep");
        l_pt->AddEntry(&pt_ttjets, "t \\bar{t}", "f");
        l_pt->AddEntry(&pt_dy, "Drell Yan", "f");
        l_pt->AddEntry(&pt_qcd, "QCD (Data driven)", "f");
	l_pt->AddEntry(&pt_sinal, "Signal (x 5000)", "l");
        pt->Draw("histo");
        pt->GetXaxis()->SetTitle("Transverse momentum of the central system [GeV]");
	pt->GetYaxis()->SetTitle("Events");
	pt_sinal.Draw("same && histo");
	pt_sinal.SetLineWidth(3);
	l_pt->Draw();
	pt->GetXaxis()->SetNdivisions(5);
        pt->GetYaxis()->SetNdivisions(5);
        pt->GetXaxis()->SetTitleSize(0.03);
        pt->GetYaxis()->SetTitleSize(0.03);
        pt->GetXaxis()->SetLabelSize(0.03);
        pt->GetYaxis()->SetLabelSize(0.03);
        pt->GetXaxis()->SetLabelOffset(0.01);
        pt->GetYaxis()->SetLabelOffset(0.01);
        pt->GetXaxis()->SetTitleOffset(1.);
        //pt_data.Draw("same && E");
	//pt_data.SetLineColor(kBlack);
        //pt_data.SetLineWidth(2);
	//pt_data.SetMarkerStyle(20);
        //gPad->SetLogy();
	TLatex cmsLabel6;
        cmsLabel6.SetTextAlign(31);
        cmsLabel6.SetTextSize(0.04);
        cmsLabel6.SetTextFont(62);
        cmsLabel6.DrawLatexNDC(0.90, 0.92, "54.9 fb^{-1} (13 TeV)");
        
       	TLatex cmsLabel7;
        cmsLabel7.SetTextAlign(31);
        cmsLabel7.SetTextSize(0.04);
        cmsLabel7.SetTextFont(62);
        cmsLabel7.DrawLatexNDC(0.5, 0.92, "CMS-TOTEM Preliminary");

        c4.Update();

	TCanvas c5;
        c5.SetCanvasSize(700,800);
        gPad->SetPad(0.1, 0.1, 1.0, 1.0);
        gPad->Draw();
        //l_pt->AddEntry(&r_data, "Data (2018)", "lep");
        l_mm->AddEntry(&mm_ttjets, "t \\bar{t}", "f");
        l_mm->AddEntry(&mm_dy, "Drell Yan", "f");
        l_mm->AddEntry(&mm_qcd, "QCD (Data driven)", "f");
        l_mm->AddEntry(&mm_sinal, "Signal (x 5000)", "l");
        mm->Draw("histo");
        mm->GetXaxis()->SetTitle("Mass difference (CMS-PPS) [GeV]");
        mm->GetYaxis()->SetTitle("Events");
        mm_sinal.Draw("same && histo");
	mm_sinal.SetLineWidth(3);
	l_mm->Draw();
        mm->GetXaxis()->SetNdivisions(5);
        mm->GetYaxis()->SetNdivisions(5);
        mm->GetXaxis()->SetTitleSize(0.03);
        mm->GetYaxis()->SetTitleSize(0.03);
        mm->GetXaxis()->SetLabelSize(0.03);
        mm->GetYaxis()->SetLabelSize(0.03);
        mm->GetXaxis()->SetLabelOffset(0.01);
        mm->GetYaxis()->SetLabelOffset(0.01);
        mm->GetXaxis()->SetTitleOffset(1.);
        //pt_data.Draw("same && E");
        //pt_data.SetLineColor(kBlack);
        //pt_data.SetLineWidth(2);
        //pt_data.SetMarkerStyle(20);
        //gPad->SetLogy();
        TLatex cmsLabel8;
        cmsLabel8.SetTextAlign(31);
        cmsLabel8.SetTextSize(0.04);
        cmsLabel8.SetTextFont(62);
        cmsLabel8.DrawLatexNDC(0.90, 0.92, "54.9 fb^{-1} (13 TeV)");
        
       	TLatex cmsLabel9;
        cmsLabel9.SetTextAlign(31);
        cmsLabel9.SetTextSize(0.04);
        cmsLabel9.SetTextFont(62);
        cmsLabel9.DrawLatexNDC(0.5, 0.92, "CMS-TOTEM Preliminary");

        c5.Update();

	TCanvas c6;
        c6.SetCanvasSize(700,800);
        gPad->SetPad(0.1, 0.1, 1.0, 1.0);
        gPad->Draw();
        //l_pt->AddEntry(&r_data, "Data (2018)", "lep");
        l_ra->AddEntry(&ra_ttjets, "t \\bar{t}", "f");
        l_ra->AddEntry(&ra_dy, "Drell Yan", "f");
        l_ra->AddEntry(&ra_qcd, "QCD (Data driven)", "f");
        l_ra->AddEntry(&ra_sinal, "Signal (x 5000)", "l");
        ra->Draw("histo");
        ra->GetXaxis()->SetTitle("Rapidity of the central system");
        ra->GetYaxis()->SetTitle("Events");
        ra_sinal.Draw("same && histo");
	ra_sinal.SetLineWidth(3);
        ra->GetXaxis()->SetNdivisions(5);
        ra->GetYaxis()->SetNdivisions(5);
        ra->GetXaxis()->SetTitleSize(0.03);
        ra->GetYaxis()->SetTitleSize(0.03);
        ra->GetXaxis()->SetLabelSize(0.03);
        ra->GetYaxis()->SetLabelSize(0.03);
        ra->GetXaxis()->SetLabelOffset(0.01);
        ra->GetYaxis()->SetLabelOffset(0.01);
        ra->GetXaxis()->SetTitleOffset(1.);
        //pt_data.Draw("same && E");
        //pt_data.SetLineColor(kBlack);
        //pt_data.SetLineWidth(2);
        //pt_data.SetMarkerStyle(20);
        //gPad->SetLogy();
        l_ra->Draw();
        TLatex cmsLabel10;
        cmsLabel10.SetTextAlign(31);
        cmsLabel10.SetTextSize(0.04);
        cmsLabel10.SetTextFont(62);
        cmsLabel10.DrawLatexNDC(0.90, 0.92, "54.9 fb^{-1} (13 TeV)");
        
       	TLatex cmsLabel11;
        cmsLabel11.SetTextAlign(31);
        cmsLabel11.SetTextSize(0.04);
        cmsLabel11.SetTextFont(62);
        cmsLabel11.DrawLatexNDC(0.5, 0.92, "CMS-TOTEM Preliminary");

        c6.Update();

	TCanvas c7;
        c7.SetCanvasSize(700,800);
        gPad->SetPad(0.1, 0.1, 1.0, 1.0);
        gPad->Draw();
        //l_pt->AddEntry(&r_data, "Data (2018)", "lep");
        l_tau->AddEntry(&tau_ttjets, "t \\bar{t}", "f");
        l_tau->AddEntry(&tau_dy, "Drell Yan", "f");
        l_tau->AddEntry(&tau_qcd, "QCD (Data driven)", "f");
        l_tau->AddEntry(&tau_sinal, "Signal (x 5000)", "l");
        tau->Draw("histo");
        tau->GetXaxis()->SetTitle("Transverse momentum of the hadronic tau [GeV]");
        tau->GetYaxis()->SetTitle("Events");
        tau_sinal.Draw("same && histo");
	tau_sinal.SetLineWidth(3);
        tau->GetXaxis()->SetNdivisions(5);
        tau->GetYaxis()->SetNdivisions(5);
        tau->GetXaxis()->SetTitleSize(0.03);
        tau->GetYaxis()->SetTitleSize(0.03);
        tau->GetXaxis()->SetLabelSize(0.03);
        tau->GetYaxis()->SetLabelSize(0.03);
        tau->GetXaxis()->SetLabelOffset(0.01);
        tau->GetYaxis()->SetLabelOffset(0.01);
        tau->GetXaxis()->SetTitleOffset(1.);
        //pt_data.Draw("same && E");
        //pt_data.SetLineColor(kBlack);
        //pt_data.SetLineWidth(2);
        //pt_data.SetMarkerStyle(20);
        //gPad->SetLogy();
        l_tau->Draw();
       	TLatex cmsLabel12;
        cmsLabel12.SetTextAlign(31);
        cmsLabel12.SetTextSize(0.04);
        cmsLabel12.SetTextFont(62);
        cmsLabel12.DrawLatexNDC(0.90, 0.92, "54.9 fb^{-1} (13 TeV)");
        
        TLatex cmsLabel13;
        cmsLabel13.SetTextAlign(31);
        cmsLabel13.SetTextSize(0.04);
        cmsLabel13.SetTextFont(62);
        cmsLabel13.DrawLatexNDC(0.5, 0.92, "CMS-TOTEM Preliminary");

        c7.Update();

	TCanvas c8;
        c8.SetCanvasSize(700,800);
        gPad->SetPad(0.1, 0.1, 1.0, 1.0);
        gPad->Draw();
        //l_pt->AddEntry(&r_data, "Data (2018)", "lep");
        l_met->AddEntry(&met_ttjets, "t \\bar{t}", "f");
        l_met->AddEntry(&met_dy, "Drell Yan", "f");
        l_met->AddEntry(&met_qcd, "QCD (Data driven)", "f");
        l_met->AddEntry(&met_sinal, "Signal (x 5000)", "l");
        met->Draw("histo");
        met->GetXaxis()->SetTitle("Muon transverse momentum [GeV]");
        met->GetYaxis()->SetTitle("Events");
        met_sinal.Draw("same && histo");
	met_sinal.SetLineWidth(3);
        met->GetXaxis()->SetNdivisions(5);
        met->GetYaxis()->SetNdivisions(5);
        met->GetXaxis()->SetTitleSize(0.03);
        met->GetYaxis()->SetTitleSize(0.03);
        met->GetXaxis()->SetLabelSize(0.03);
        met->GetYaxis()->SetLabelSize(0.03);
        met->GetXaxis()->SetLabelOffset(0.01);
        met->GetYaxis()->SetLabelOffset(0.01);
        met->GetXaxis()->SetTitleOffset(1.);
        //pt_data.Draw("same && E");
        //pt_data.SetLineColor(kBlack);
        //pt_data.SetLineWidth(2);
        //pt_data.SetMarkerStyle(20);
        //gPad->SetLogy();
        l_met->Draw();
       	TLatex cmsLabel14;
        cmsLabel14.SetTextAlign(31);
        cmsLabel14.SetTextSize(0.04);
        cmsLabel14.SetTextFont(62);
        cmsLabel14.DrawLatexNDC(0.90, 0.92, "54.9 fb^{-1} (13 TeV)");
        
        TLatex cmsLabel15;
        cmsLabel15.SetTextAlign(31);
        cmsLabel15.SetTextSize(0.04);
        cmsLabel15.SetTextFont(62);
        cmsLabel15.DrawLatexNDC(0.5, 0.92, "CMS-TOTEM Preliminary");

        c8.Update();


        app.Run("true");

        return 0;

}


