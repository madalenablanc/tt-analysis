#include <TFile.h>
#include <TTree.h>
#include <TH1.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <TStyle.h>

int main() {
    // Open your TMVApp file (contains both variables AND BDT scores)
    TFile *file = TFile::Open("TMVApp_dados.root"); // or any sample you want
    TTree *tree = (TTree*)file->Get("TestTree");
    
    // Variables to study
    float sist_pt, sist_mass, sist_acop, met_pt, BDT_score;
    
    tree->SetBranchAddress("sist_pt", &sist_pt);
    tree->SetBranchAddress("sist_mass", &sist_mass);
    tree->SetBranchAddress("sist_acop", &sist_acop);
    tree->SetBranchAddress("met_pt", &met_pt);
    tree->SetBranchAddress("BDT", &BDT_score);
    
    // Create histograms for before/after BDT cuts
    TH1F *h_pt_all = new TH1F("h_pt_all", "sist_pt: All Events", 50, 0, 200);
    TH1F *h_pt_bdt = new TH1F("h_pt_bdt", "sist_pt: BDT > 0", 50, 0, 200);
    
    TH1F *h_mass_all = new TH1F("h_mass_all", "sist_mass: All Events", 50, 0, 500);
    TH1F *h_mass_bdt = new TH1F("h_mass_bdt", "sist_mass: BDT > 0", 50, 0, 500);
    
    TH1F *h_acop_all = new TH1F("h_acop_all", "sist_acop: All Events", 50, 0, 3.14);
    TH1F *h_acop_bdt = new TH1F("h_acop_bdt", "sist_acop: BDT > 0", 50, 0, 3.14);
    
    // Fill histograms
    Long64_t nentries = tree->GetEntries();
    for (Long64_t i = 0; i < nentries; i++) {
        tree->GetEntry(i);
        
        // All events (before cuts)
        h_pt_all->Fill(sist_pt);
        h_mass_all->Fill(sist_mass);
        h_acop_all->Fill(sist_acop);
        
        // Only events passing BDT cut
        if (BDT_score > 0) {  // Adjust cut value as needed
            h_pt_bdt->Fill(sist_pt);
            h_mass_bdt->Fill(sist_mass);
            h_acop_bdt->Fill(sist_acop);
        }
    }
    
    // Normalize histograms for shape comparison
    h_pt_all->Scale(1.0/h_pt_all->Integral());
    h_pt_bdt->Scale(1.0/h_pt_bdt->Integral());
    h_mass_all->Scale(1.0/h_mass_all->Integral());
    h_mass_bdt->Scale(1.0/h_mass_bdt->Integral());
    h_acop_all->Scale(1.0/h_acop_all->Integral());
    h_acop_bdt->Scale(1.0/h_acop_bdt->Integral());
    
    // Style histograms
    h_pt_all->SetLineColor(kBlue); h_pt_all->SetLineWidth(2);
    h_pt_bdt->SetLineColor(kRed); h_pt_bdt->SetLineWidth(2);
    h_mass_all->SetLineColor(kBlue); h_mass_all->SetLineWidth(2);
    h_mass_bdt->SetLineColor(kRed); h_mass_bdt->SetLineWidth(2);
    h_acop_all->SetLineColor(kBlue); h_acop_all->SetLineWidth(2);
    h_acop_bdt->SetLineColor(kRed); h_acop_bdt->SetLineWidth(2);
    
    // Create canvas with subplots
    TCanvas *c = new TCanvas("c", "Variable Distributions: Before vs After BDT", 1200, 800);
    c->Divide(2, 2);
    
    // Plot sist_pt
    c->cd(1);
    h_pt_all->SetTitle("System p_{T}");
    h_pt_all->GetXaxis()->SetTitle("sist_pt [GeV]");
    h_pt_all->GetYaxis()->SetTitle("Normalized Events");
    h_pt_all->Draw("HIST");
    h_pt_bdt->Draw("HIST SAME");
    
    TLegend *leg1 = new TLegend(0.6, 0.7, 0.9, 0.9);
    leg1->AddEntry(h_pt_all, "All Events", "l");
    leg1->AddEntry(h_pt_bdt, "BDT > 0", "l");
    leg1->Draw();
    
    // Plot sist_mass
    c->cd(2);
    h_mass_all->SetTitle("System Mass");
    h_mass_all->GetXaxis()->SetTitle("sist_mass [GeV]");
    h_mass_all->GetYaxis()->SetTitle("Normalized Events");
    h_mass_all->Draw("HIST");
    h_mass_bdt->Draw("HIST SAME");
    
    TLegend *leg2 = new TLegend(0.6, 0.7, 0.9, 0.9);
    leg2->AddEntry(h_mass_all, "All Events", "l");
    leg2->AddEntry(h_mass_bdt, "BDT > 0", "l");
    leg2->Draw();
    
    // Plot sist_acop
    c->cd(3);
    h_acop_all->SetTitle("System Acoplanarity");
    h_acop_all->GetXaxis()->SetTitle("sist_acop [rad]");
    h_acop_all->GetYaxis()->SetTitle("Normalized Events");
    h_acop_all->Draw("HIST");
    h_acop_bdt->Draw("HIST SAME");
    
    TLegend *leg3 = new TLegend(0.6, 0.7, 0.9, 0.9);
    leg3->AddEntry(h_acop_all, "All Events", "l");
    leg3->AddEntry(h_acop_bdt, "BDT > 0", "l");
    leg3->Draw();
    
    // Plot BDT distribution itself
    c->cd(4);
    TH1F *h_bdt = new TH1F("h_bdt", "BDT Score Distribution", 50, -1, 1);
    for (Long64_t i = 0; i < nentries; i++) {
        tree->GetEntry(i);
        h_bdt->Fill(BDT_score);
    }
    h_bdt->SetTitle("BDT Score");
    h_bdt->GetXaxis()->SetTitle("BDT Output");
    h_bdt->GetYaxis()->SetTitle("Events");
    h_bdt->SetLineColor(kBlack);
    h_bdt->SetLineWidth(2);
    h_bdt->Draw("HIST");
    
    // Add vertical line at cut
    TLine *line = new TLine(0, 0, 0, h_bdt->GetMaximum());
    line->SetLineColor(kRed);
    line->SetLineWidth(2);
    line->SetLineStyle(2);
    line->Draw();
    
    c->SaveAs("variable_comparison.png");
    
    cout << "=== Event Yields ===" << endl;
    cout << "Total events: " << nentries << endl;
    cout << "Events passing BDT > 0: " << h_pt_bdt->GetEntries() << endl;
    cout << "Selection efficiency: " << (float)h_pt_bdt->GetEntries()/nentries * 100 << "%" << endl;
    
    return 0;
}