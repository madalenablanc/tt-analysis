#include "TFile.h"
#include "TTree.h"
#include "TH1F.h"
#include "TLegend.h"
#include "TColor.h"
#include "TAttMarker.h"
#include "TString.h"
#include <iostream>
#include <initializer_list>

namespace {

  // this is all needed bc i gave diff names to the same variables in the same sample
bool has_branch_or_leaf(TTree *tree, const char *expr)
{
  if (!tree || !expr) return false;
  TString base(expr);
  Ssiz_t bracket = base.First('[');
  if (bracket != kNPOS)
    base = base(0, bracket);
  return tree->GetListOfBranches()->FindObject(base) != nullptr ||
         tree->GetListOfLeaves()->FindObject(base) != nullptr;
}

TString resolve_expr(TTree *tree, std::initializer_list<const char *> options)
{
  for (auto opt : options)
  {
    if (has_branch_or_leaf(tree, opt))
      return opt;
  }
  return "";
}

bool configure_muon_aliases(TTree *tree)
{
  if (!tree)
    return false;

  struct AliasEntry
  {
    const char *alias;
    std::initializer_list<const char *> candidates;
  };

  const AliasEntry entries[] = {
      {"mu_pt_sel", {"mu_pt", "muon_pt", "Muon_pt[0]"}},
      {"mu_eta_sel", {"mu_eta", "muon_eta", "Muon_eta[0]"}},
      {"mu_charge_sel", {"mu_charge", "muon_charge", "Muon_charge[0]"}},
      {"mu_id_sel", {"mu_id", "muon_id"}}};

  for (const auto &entry : entries)
  {
    TString expr = resolve_expr(tree, entry.candidates);
    if (expr.IsNull())
    {
      std::cerr << "[TauTauPlots] ERROR: cannot find branch for alias '"
                << entry.alias << "' in tree '" << tree->GetName() << "'"
                << std::endl;
      return false;
    }
    tree->SetAlias(entry.alias, expr.Data());
  }

  return true;
}

bool configure_all_aliases(std::initializer_list<TTree *> trees)
{
  for (TTree *tree : trees)
  {
    if (!configure_muon_aliases(tree))
      return false;
  }
  return true;
}

void style_histogram(TH1F *hist, Color_t fill_color, Color_t line_color, bool is_data = false)
{
  if (!hist)
    return;

  hist->SetTitle("m_{#mu#tau} distribution");
  hist->GetXaxis()->SetTitle("m_{#mu#tau} [GeV]");
  hist->GetYaxis()->SetTitle("Events / 30 GeV");
  hist->SetFillColor(fill_color);
  hist->SetLineColor(line_color);
  hist->SetLineWidth(is_data ? 2 : 1);

  if (is_data)
  {
    hist->SetFillColor(0);
    hist->SetMarkerStyle(kFullCircle);
    hist->SetMarkerSize(1.0);
    hist->SetMarkerColor(line_color);
  }
}

} // namespace

void TauTauPlotsNoProtons()
{

  TFile *f1 = TFile::Open("/eos/user/m/mblancco/samples_2018_mutau/Data_2018_UL_MuTau_nano_merged_proton_vars.root");
  TFile *f2 = TFile::Open("/eos/user/m/mblancco/samples_2018_mutau/QCD_2018_UL_MuTau_nano_merged_proton_vars_new.root");
  TFile *f3 = TFile::Open("/eos/user/m/mblancco/samples_2018_mutau/DY_2018_UL_MuTau_nano_merged.root");
  TFile *f4 = TFile::Open("/eos/user/m/mblancco/samples_2018_mutau/ttjets_2018_UL_MuTau_nano_merged.root");
  
  TTree *t1 = (TTree *)f1->Get("tree");
  TTree *t2 = (TTree *)f2->Get("tree");
  TTree *t3 = (TTree *)f3->Get("tree");
  TTree *t4 = (TTree *)f4->Get("tree");

  TH1F *h1 = new TH1F("h1","h1",50,0,1500);
  TH1F *h2 = new TH1F("h2","h2",50,0,1500);
  TH1F *h3 = new TH1F("h3","h3",50,0,1500);
  TH1F *h4 = new TH1F("h4","h4",50,0,1500);

  style_histogram(h1, kBlack, kBlack, true);
  style_histogram(h2, kRed - 4, kRed + 1);
  style_histogram(h3, kAzure + 5, kAzure + 2);
  style_histogram(h4, kGreen + 1, kGreen + 3);

  if (!configure_all_aliases({t1, t2, t3, t4}))
  {
    std::cerr << "[TauTauPlots] Aborting: input tree missing required branches." << std::endl;
    return;
  }

  if (!configure_all_aliases({t1, t2, t3, t4}))
  {
    std::cerr << "[TauTauPlots] Aborting: input tree missing required branches." << std::endl;
    return;
  }

  // Cuts applied to MC, data, and data-driven QCD
  TString mccuts = "(mu_pt_sel>35 && tau_pt>100 && (mu_charge_sel*tau_charge==-1) && abs(mu_eta_sel)<2.3 && abs(tau_eta)<2.3 && mu_id_sel>=3 && tau_id1>63 && tau_id2>7 && tau_id3>1)";
  TString datacuts = "mu_pt_sel>35 && tau_pt>100 && (mu_charge_sel*tau_charge==-1)  && abs(mu_eta_sel)<2.3 && abs(tau_eta)<2.3 && mu_id_sel>=3 && tau_id1>63 && tau_id2>7 && tau_id3>1";
  TString qcdcuts = "mu_pt_sel>35 && tau_pt>100 && (mu_charge_sel*tau_charge==1)  && abs(mu_eta_sel)<2.3 && abs(tau_eta)<2.3 && mu_id_sel>=3 && tau_id1>63 && tau_id2>7 && tau_id3>1";
  
  t3->Draw("sist_mass >> h3",mccuts,"hist");
  t4->Draw("sist_mass >> h4",mccuts,"hist");
  t2->Draw("sist_mass >> h2",qcdcuts,"hist");
  t1->Draw("sist_mass >> h1",datacuts,"esame");

  h2->Sumw2();
  h3->Sumw2();
  h4->Sumw2();
  h1->Sumw2();

  // Normalization of MC based on cross section and luminosity 
  h3->Scale(1.81); // DY
  h4->Scale(0.15); // ttbar

  std::cout << "Data = " << h1->GetSumOfWeights() << std::endl
	    << "QCD = " << h2->GetSumOfWeights() << std::endl
	    << "DY = " << h3->GetSumOfWeights() << std::endl
	    << "ttbar = " << h4->GetSumOfWeights() << std::endl;
  
  h4->Add(h3);
  h4->Add(h2);
  h3->Add(h2);

  h4->SetMaximum(h3->GetMaximum() * 1.5);
  h4->Draw("hist");
  h3->Draw("histsame");
  h2->Draw("histsame");
  h1->Draw("esame");

  TLegend *leg = new TLegend(0.62, 0.65, 0.86, 0.86);
  leg->SetBorderSize(0);
  leg->SetFillStyle(0);
  leg->AddEntry(h1, "Data", "ep");
  leg->AddEntry(h2, "QCD", "f");
  leg->AddEntry(h3, "DY", "f");
  leg->AddEntry(h4, "ttbar", "f");
  leg->Draw();

  gPad->Update();
  gPad->SaveAs("mass_no_protons.png");
}

void TauTauPlotsWithProtons()
{
  TFile *f1 = TFile::Open("/eos/user/m/mblancco/samples_2018_mutau/Data_2018_UL_MuTau_nano_merged_proton_vars.root");
  TFile *f2 = TFile::Open("/eos/user/m/mblancco/samples_2018_mutau/QCD_2018_UL_MuTau_nano_merged_proton_vars_new.root");
  TFile *f3 = TFile::Open("/eos/user/m/mblancco/samples_2018_mutau/DY_2018_UL_MuTau_nano_merged_pileup_protons.root");
  TFile *f4 = TFile::Open("/eos/user/m/mblancco/samples_2018_mutau/ttjets_2018_UL_MuTau_nano_merged_pileup_protons.root");

  TTree *t1 = (TTree *)f1->Get("tree");
  TTree *t2 = (TTree *)f2->Get("tree");
  TTree *t3 = (TTree *)f3->Get("tree");
  TTree *t4 = (TTree *)f4->Get("tree");

  TH1F *h1 = new TH1F("h1","h1",50,0,1500);
  TH1F *h2 = new TH1F("h2","h2",50,0,1500);
  TH1F *h3 = new TH1F("h3","h3",50,0,1500);
  TH1F *h4 = new TH1F("h4","h4",50,0,1500);

  style_histogram(h1, kBlack, kBlack, true);
  style_histogram(h2, kRed - 4, kRed + 1);
  style_histogram(h3, kAzure + 5, kAzure + 2);
  style_histogram(h4, kGreen + 1, kGreen + 3);

  if (!configure_all_aliases({t1, t2, t3, t4}))
  {
    std::cerr << "[TauTauPlots] Aborting: input tree missing required branches." << std::endl;
    return;
  }

  // Cuts applied to MC, data, and data-driven QCD
  // The weighting factor 0.20473458 for MC accounts for the fraction of events with 1 or 2 protons on each arm, as derived
  // from the data/QCD samples
  TString mccuts = "(mu_pt_sel>35 && tau_pt>100 && (mu_charge_sel*tau_charge==-1) && abs(mu_eta_sel)<2.3 && abs(tau_eta)<2.3 && mu_id_sel>=3 && tau_id1>63 && tau_id2>7 && tau_id3>1) * 0.245";
  TString datacuts = "Sum$(proton_multi_arm==0) > 0 && Sum$(proton_multi_arm==1) > 0 && mu_pt_sel>35 && tau_pt>100 && (mu_charge_sel*tau_charge==-1)  && abs(mu_eta_sel)<2.3 && abs(tau_eta)<2.3 && mu_id_sel>=3 && tau_id1>63 && tau_id2>7 && tau_id3>1";
  TString qcdcuts = "Sum$(proton_multi_arm==0) > 0 && Sum$(proton_multi_arm==1) > 0 && mu_pt_sel>35 && tau_pt>100 && (mu_charge_sel*tau_charge==1)  && abs(mu_eta_sel)<2.3 && abs(tau_eta)<2.3 && mu_id_sel>=3 && tau_id1>63 && tau_id2>7 && tau_id3>1";

  t3->Draw("sist_mass >> h3",mccuts,"hist");
  t4->Draw("sist_mass >> h4",mccuts,"hist");
  t2->Draw("sist_mass >> h2",qcdcuts,"hist");
  t1->Draw("sist_mass >> h1",datacuts,"esame");

  h2->Sumw2();
  h3->Sumw2();
  h4->Sumw2();
  h1->Sumw2();

  
  // Normalization of MC based on cross section and luminosity
  h3->Scale(1.81); // DY
  h4->Scale(0.15); // ttbar

  std::cout << "Data = " << h1->GetSumOfWeights() << std::endl
            << "QCD = " << h2->GetSumOfWeights() << std::endl
            << "DY = " << h3->GetSumOfWeights() << std::endl
            << "ttbar = " << h4->GetSumOfWeights() << std::endl;

  h4->Add(h3);
  h4->Add(h2);
  h3->Add(h2);

  h4->SetMaximum(h1->GetMaximum() * 1.5);
  h4->Draw("hist");
  h3->Draw("histsame");
  h2->Draw("histsame");
  h1->Draw("esame");

  TLegend *leg = new TLegend(0.62, 0.65, 0.86, 0.86);
  leg->SetBorderSize(0);
  leg->SetFillStyle(0);
  leg->AddEntry(h1, "Data", "ep");
  leg->AddEntry(h2, "QCD", "f");
  leg->AddEntry(h3, "DY", "f");
  leg->AddEntry(h4, "ttbar", "f");
  leg->Draw();

  gPad->Update();
  gPad->SaveAs("mass_with_protons.png");
}

void PMix()
{
  TFile *fm = TFile::Open("/eos/user/m/mblancco/samples_2018_mutau/Data_2018_UL_MuTau_nano_merged_pileup_protons.root");
  TFile *fp = TFile::Open("/eos/user/m/mblancco/samples_2018_mutau/Data_2018_UL_MuTau_nano_merged_proton_vars.root");

  TTree *tmm = (TTree *)fm->Get("tree");
  TTree *tpp = (TTree *)fp->Get("tree");

  tpp->Draw("Sum$(proton_multi_arm==0)","","e");
  tmm->Draw("(xi_arm1_1 > 0) + (xi_arm1_2 > 0)","","histsame");

}
