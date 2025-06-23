void quick_plot() {
    // Open files
    TFile* file_data = TFile::Open("/eos/cms/store/user/jjhollar/TauTau_NanoAOD_Madalena/BackgroundSamples/TauhTauh/Dados_2018_UL_skimmed_TauTau_nano_fase1total-protons_2018.root");
    TFile* file_dy = TFile::Open("/eos/cms/store/user/jjhollar/TauTau_NanoAOD_Madalena/BackgroundSamples/TauhTauh/DY_2018_UL_skimmed_TauTau_nano_fase1total-protons_2018.root");
    TFile* file_qcd = TFile::Open("/eos/cms/store/user/jjhollar/TauTau_NanoAOD_Madalena/BackgroundSamples/TauhTauh/QCD_2018_UL_skimmed_TauTau_nano_fase1total-protons_2018.root");
    TFile* file_ttjets = TFile::Open("/eos/cms/store/user/jjhollar/TauTau_NanoAOD_Madalena/BackgroundSamples/TauhTauh/ttJets_2018_UL_skimmed_TauTau_nano_fase1total-protons_2018.root");
    
    // Get trees
    TTree* tree_data = (TTree*)file_data->Get("tree");
    TTree* tree_dy = (TTree*)file_dy->Get("tree");
    TTree* tree_qcd = (TTree*)file_qcd->Get("tree");
    TTree* tree_ttjets = (TTree*)file_ttjets->Get("tree");
    
    // Create histograms - ONLY acoplanarity in DY CR
    TH1D h_aco_data("h_aco_data", "Data", 5, 0, 0.25);
    TH1D h_aco_dy("h_aco_dy", "DY", 5, 0, 0.25);
    TH1D h_aco_qcd("h_aco_qcd", "QCD", 5, 0, 0.25);
    TH1D h_aco_ttjets("h_aco_ttjets", "ttjets", 5, 0, 0.25);
    
    // Fill data
    int n_evt_data = tree_data->GetEntries();
    for(int i=0; i<n_evt_data; i++){
        tree_data->GetEvent(i);
        if(tree_data->GetLeaf("sist_mass")->GetValue(0) >= 0){
            double sist_mass = tree_data->GetLeaf("sist_mass")->GetValue(0);
            double sist_acop = tree_data->GetLeaf("sist_acop")->GetValue(0);
            double n_bjets = tree_data->GetLeaf("N_b_jet")->GetValue(0);
            
            // DY control region cuts
            bool isDY_CR = (sist_mass >= 40 && sist_mass <= 100) && (sist_acop < 0.3) && (n_bjets == 0);
            
            if(isDY_CR) {
                h_aco_data.Fill(sist_acop);
            }
        }
    }
    
    // Fill DY MC
    int n_evt_dy = tree_dy->GetEntries();
    for(int i=0; i<n_evt_dy; i++){
        tree_dy->GetEvent(i);
        if(tree_dy->GetLeaf("sist_mass")->GetValue(0) >= 0){
            double sist_mass = tree_dy->GetLeaf("sist_mass")->GetValue(0);
            double sist_acop = tree_dy->GetLeaf("sist_acop")->GetValue(0);
            double n_bjets = tree_dy->GetLeaf("N_b_jet")->GetValue(0);
            
            bool isDY_CR = (sist_mass >= 40 && sist_mass <= 100) && (sist_acop < 0.3) && (n_bjets == 0);
            
            if(isDY_CR) {
                h_aco_dy.Fill(sist_acop);
            }
        }
    }
    
    // Fill QCD MC
    int n_evt_qcd = tree_qcd->GetEntries();
    for(int i=0; i<n_evt_qcd; i++){
        tree_qcd->GetEvent(i);
        if(tree_qcd->GetLeaf("sist_mass")->GetValue(0) >= 0){
            double sist_mass = tree_qcd->GetLeaf("sist_mass")->GetValue(0);
            double sist_acop = tree_qcd->GetLeaf("sist_acop")->GetValue(0);
            double n_bjets = tree_qcd->GetLeaf("N_b_jet")->GetValue(0);
            
            bool isDY_CR = (sist_mass >= 40 && sist_mass <= 100) && (sist_acop < 0.3) && (n_bjets == 0);
            
            if(isDY_CR) {
                h_aco_qcd.Fill(sist_acop);
            }
        }
    }
    
    // Fill ttjets MC
    int n_evt_ttjets = tree_ttjets->GetEntries();
    for(int i=0; i<n_evt_ttjets; i++){
        tree_ttjets->GetEvent(i);
        if(tree_ttjets->GetLeaf("sist_mass")->GetValue(0) >= 0){
            double sist_mass = tree_ttjets->GetLeaf("sist_mass")->GetValue(0);
            double sist_acop = tree_ttjets->GetLeaf("sist_acop")->GetValue(0);
            double n_bjets = tree_ttjets->GetLeaf("N_b_jet")->GetValue(0);
            
            bool isDY_CR = (sist_mass >= 40 && sist_mass <= 100) && (sist_acop < 0.3) && (n_bjets == 0);
            
            if(isDY_CR) {
                h_aco_ttjets.Fill(sist_acop);
            }
        }
    }
    
    // Print entries
    cout << "Data events: " << h_aco_data.GetEntries() << endl;
    cout << "DY events: " << h_aco_dy.GetEntries() << endl;
    cout << "QCD events: " << h_aco_qcd.GetEntries() << endl;
    cout << "ttjets events: " << h_aco_ttjets.GetEntries() << endl;
    
    // Set colors
    h_aco_data.SetMarkerStyle(20);
    h_aco_data.SetMarkerColor(kBlack);
    h_aco_dy.SetFillColor(kYellow);
    h_aco_qcd.SetFillColor(kRed);
    h_aco_ttjets.SetFillColor(kGreen);
    
    // Create stack
    THStack stack("stack", "Acoplanarity in DY Control Region;Acoplanarity;Events");
    stack.Add(&h_aco_ttjets);  // Green (bottom)
    stack.Add(&h_aco_qcd);     // Red (middle)
    stack.Add(&h_aco_dy);      // Yellow (top)
    
    // Create canvas
    TCanvas c("c", "Acoplanarity DY CR", 800, 600);
    
    // Draw
    stack.Draw("HIST");
    h_aco_data.Draw("SAME E");
    
    // Legend
    TLegend leg(0.7, 0.7, 0.9, 0.9);
    leg.AddEntry(&h_aco_data, "Data", "p");
    leg.AddEntry(&h_aco_dy, "DY", "f");
    leg.AddEntry(&h_aco_qcd, "QCD", "f");
    leg.AddEntry(&h_aco_ttjets, "ttjets", "f");
    leg.Draw();
    
    // Save
    c.SaveAs("acoplanarity_DY_CR.png");
    
    cout << "Plot saved as acoplanarity_DY_CR.png" << endl;
}