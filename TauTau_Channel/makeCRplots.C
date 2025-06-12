//*******************************************************************
//  makeCRplots.C  --  stand‑alone ROOT macro (ACLiC‑ready)
//  -------------------------------------------------------
//  Produces the 3×4 grid of stacked data/MC histograms + ratios
//  for DY, QCD and tt̄ control regions in the τhτh final state.
//
//  2024‑06‑12 — based on macro skeleton shared in chat and updated
//  with explicit EOS/XRootD paths supplied by the user.
//
//  How to run on lxplus (or any machine with CMSSW / ROOT ≥6.26):
//
//     cmsrel CMSSW_13_0_13  # or any recent CMSSW
//     cd CMSSW_13_0_13/src && cmsenv
//     root -l -q 'makeCRplots.C+()'
//
//  The output PDF will be written to CRgrid_2018.pdf
//*******************************************************************

#include <TChain.h>
#include <TH1D.h>
#include <THStack.h>
#include <TCanvas.h>
#include <TPad.h>
#include <TLegend.h>
#include <TGraphAsymmErrors.h>
#include <TLatex.h>
#include <TStyle.h>
#include <TSystem.h>
#include <vector>
#include <iostream>

//------------------------------------------------------------------
// Helper: book histogram with variable binning
//------------------------------------------------------------------
static TH1D *book(const char *name, const std::vector<double> &edges)
{
    auto h = new TH1D(name, "", static_cast<int>(edges.size()) - 1, edges.data());
    h->Sumw2();
    return h;
}

//------------------------------------------------------------------
// Control‑region predicates (τhτh final state)
//------------------------------------------------------------------
inline bool passDY(double M, double a, int qprod, int nb)
{ return (M >  40 && M < 100 && a < 0.3 && qprod < 0 && nb == 0); }

inline bool passQCD(double M, double a, int qprod, int nb)
{ return (M > 100 && M < 300 && a > 0.8 && qprod > 0 && nb == 0); }

inline bool passTT(double M, double a, int qprod, int nb)
{ return (M > 300            && a > 0.5 && qprod < 0 && nb >= 1); }

//------------------------------------------------------------------
// Main function (can be run with ACliC)
//------------------------------------------------------------------
void makeCRplots()
{
    gStyle->SetOptStat(0);
    gStyle->SetHatchesSpacing(1.2);
    gStyle->SetHatchesLineWidth(2);

    //----------------------------------------------------------------
    // 1. Build TChains pointing to the EOS files (streamed via XRootD)
    //----------------------------------------------------------------
    const char *xrd = "root://xrootd-cms.infn.it/";

    TChain chQCD("tree");
    TChain chDY ("tree");
    TChain chTT ("tree");
    TChain chData("tree"); // Uncomment if you have the data file.

    chQCD.Add(Form("%s//%s", xrd,
                   "/eos/cms/store/user/jjhollar/TauTau_NanoAOD_Madalena/BackgroundSamples/"
                   "TauhTauh/QCD_2018_UL_skimmed_TauTau_nano_fase1total-protons_2018.root"));

    chDY .Add(Form("%s//%s", xrd,
                   "/eos/cms/store/user/jjhollar/TauTau_NanoAOD_Madalena/BackgroundSamples/"
                   "TauhTauh/DY_2018_UL_skimmed_TauTau_nano_fase1total-protons_2018.root"));

    chTT .Add(Form("%s//%s", xrd,
                   "/eos/cms/store/user/jjhollar/TauTau_NanoAOD_Madalena/BackgroundSamples/"
                   "TauhTauh/ttJets_2018_UL_skimmed_TauTau_nano_fase1total-protons_2018.root"));

    // If you also want to overlay data, supply the filename:
    // chData.Add(Form("%s//%s", xrd,
    //                "/eos/cms/store/user/jjhollar/TauTau_NanoAOD_Madalena/BackgroundSamples/"
    //                "TauhTauh/Dados_fase1total-protons_2018.root"));

    //----------------------------------------------------------------
    // 2. Define binning for each row (low, mid, high)
    //----------------------------------------------------------------
    const std::vector<double> acopLow  {0.00,0.05,0.10,0.15,0.20,0.25,0.30};
    const std::vector<double> massLow  { 40, 60, 80,100};

    const std::vector<double> acopMid  {0.50,0.60,0.70,0.80,0.90,1.00};
    const std::vector<double> massMid  {100,200,400,600,800,1000};

    const std::vector<double> acopHigh {0.80,0.86,0.92,0.96,1.00};
    const std::vector<double> massHigh {300,400,500,600,800,1000,1200};

    const std::vector<double> rapBins {-2.4,-1.6,-0.8,0.0,0.8,1.6,2.4};
    const std::vector<double> ptBins  {  0, 50,100,150,200,250,300,350};

    //----------------------------------------------------------------
    // 3. Book histograms  [index 0 = DY row, 1 = QCD row, 2 = tt row]
    //----------------------------------------------------------------
    struct Hset { TH1D *acop,*mass,*rap,*pt; };
    Hset hData[3]{}, hQCD[3]{}, hDYMC[3]{}, hTTMC[3]{};

    auto makeSet = [&](Hset &h,const char *tag,
                       const std::vector<double> &aE,
                       const std::vector<double> &mE){
        h.acop = book(Form("h_acop_%s",tag),aE);
        h.mass = book(Form("h_mass_%s",tag),mE);
        h.rap  = book(Form("h_rap_%s" ,tag),rapBins);
        h.pt   = book(Form("h_pt_%s"  ,tag),ptBins);
    };

    makeSet(hData[0], "data_DY" , acopLow , massLow );
    makeSet(hData[1], "data_QCD", acopMid , massMid );
    makeSet(hData[2], "data_TT" , acopHigh, massHigh);

    makeSet(hQCD[0],  "qcd_DY"  , acopLow , massLow );
    makeSet(hQCD[1],  "qcd_QCD" , acopMid , massMid );
    makeSet(hQCD[2],  "qcd_TT"  , acopHigh, massHigh);

    makeSet(hDYMC[0], "dy_DY"   , acopLow , massLow );
    makeSet(hDYMC[1], "dy_QCD"  , acopMid , massMid );
    makeSet(hDYMC[2], "dy_TT"   , acopHigh, massHigh);

    makeSet(hTTMC[0], "tt_DY"   , acopLow , massLow );
    makeSet(hTTMC[1], "tt_QCD"  , acopMid , massMid );
    makeSet(hTTMC[2], "tt_TT"   , acopHigh, massHigh);

    //----------------------------------------------------------------
    // 4. Utility lambda to fill chains according to CR predicate
    //----------------------------------------------------------------
    auto loop = [&](TChain &ch, Hset h[3], double weightScale){
        double M,a,rap,pt; int q0,q1,nb;
        ch.SetBranchAddress("sist_mass", &M);
        ch.SetBranchAddress("sist_acop", &a);
        ch.SetBranchAddress("sist_rap" , &rap);
        ch.SetBranchAddress("sist_pt"  , &pt);
        ch.SetBranchAddress("tau0_charge", &q0);
        ch.SetBranchAddress("tau1_charge", &q1);
        ch.SetBranchAddress("n_b_jet", &nb);

        const Long64_t N = ch.GetEntries();
        for(Long64_t i=0;i<N;++i){
            ch.GetEntry(i);
            const int qprod = q0 * q1;
            const double w = weightScale;

            if(passDY(M,a,qprod,nb)){
                h[0].acop->Fill(a,w); h[0].mass->Fill(M,w);
                h[0].rap ->Fill(rap,w); h[0].pt  ->Fill(pt,w);
            }
            if(passQCD(M,a,qprod,nb)){
                h[1].acop->Fill(a,w); h[1].mass->Fill(M,w);
                h[1].rap ->Fill(rap,w); h[1].pt  ->Fill(pt,w);
            }
            if(passTT(M,a,qprod,nb)){
                h[2].acop->Fill(a,w); h[2].mass->Fill(M,w);
                h[2].rap ->Fill(rap,w); h[2].pt  ->Fill(pt,w);
            }
        }
    };

    // lumi * cross section / Ngen  (put your own numbers here)
    const double lumi_pb = 59800.0; // 59.8 fb‑1
    const double xs_dy_pb  = 6225.4; // DY -> tau tau inclusive (example)
    const double xs_tt_pb  = 831.6;  // ttbar inclusive (example)
    const double xs_qcd_pb = 1.0;    // will normalise to data later

    loop(chDY , hDYMC , lumi_pb * xs_dy_pb  / chDY .GetEntries());
    loop(chTT , hTTMC , lumi_pb * xs_tt_pb  / chTT .GetEntries());
    loop(chQCD, hQCD  , lumi_pb * xs_qcd_pb / chQCD.GetEntries());

    // If data is available, weight = 1.0
    if(chData.GetEntries()>0) loop(chData, hData, 1.0);

    //----------------------------------------------------------------
    // 5. Build canvas  (3 rows × 4 columns, ratio panel under each)
    //----------------------------------------------------------------
    const char *varLabel[4] = {"Acoplanarity of the central system",
                               "Invariant mass of the central system [GeV]",
                               "Rapidity of the central system",
                               "Transverse momentum of the central system [GeV]"};

    TCanvas can("can","",1800,1350);
    can.Divide(4,3,0.001,0.001);

    auto drawRow = [&](int row, const Hset &d, const Hset &dy, const Hset &tt, const Hset &qcd){
        TH1D* mc[4] = { (TH1D*)qcd.acop->Clone(),
                        (TH1D*)qcd.mass->Clone(),
                        (TH1D*)qcd.rap ->Clone(),
                        (TH1D*)qcd.pt  ->Clone() };
        for(int v=0; v<4; ++v){
            mc[v]->Add( dy.acop + v ); // pointer arithmetic: relies on struct layout
            mc[v]->Add( tt.acop + v );
        }

        const Hset *sets[3] = { &qcd, &dy, &tt };
        const Color_t colors[3] = {kYellow+1, kGreen+2, kRed+1};

        for(int col=0; col<4; ++col){
            const int padIdx = row*4 + col + 1;
            can.cd(padIdx);
            gPad->SetBottomMargin(0.28);

            // Stack
            THStack st(Form("row%d_col%d",row,col),"");
            for(int k=0;k<3;++k){
                TH1D *h = *((TH1D**)(&sets[k]->acop) + col); // same trick as above
                h->SetFillColor(colors[k]);
                h->SetLineColor(kBlack);
                h->SetLineWidth(1);
                st.Add(h,"HIST");
            }
            st.Draw("HIST");
            st.GetXaxis()->SetLabelSize(0);
            st.GetYaxis()->SetTitle("Events");
            st.SetTitle("");

            // Data points (if available)
            if(chData.GetEntries()>0){
                TH1D *hd = *((TH1D**)(&d.acop) + col);
                hd->SetMarkerStyle(20);
                hd->SetMarkerSize(0.9);
                hd->SetLineColor(kBlack);
                hd->Draw("E SAME");
            }

            // Legend only in first column of each row
            if(col==0){
                auto leg = new TLegend(0.55,0.65,0.88,0.88);
                leg->AddEntry(sets[0]->acop,"QCD (Data‑driven)","f");
                leg->AddEntry(sets[1]->acop,"DY","f");
                leg->AddEntry(sets[2]->acop,"t#bar{t}","f");
                if(chData.GetEntries()>0) leg->AddEntry(d.acop,"Data","lep");
                leg->SetBorderSize(0);
                leg->SetFillStyle(0);
                leg->Draw();
            }

            // Ratio pad
            TPad *rp = new TPad(Form("rp_%d_%d",row,col),"",0,0,1,0.28);
            rp->SetTopMargin(0);
            rp->SetBottomMargin(0.35);
            rp->Draw(); rp->cd();
            TH1D *ratio = (TH1D*)((*((TH1D**)(&d.acop) + col))->Clone());
            ratio->Divide(mc[col]);
            ratio->SetMarkerStyle(20);
            ratio->SetMarkerSize(0.8);
            ratio->GetYaxis()->SetNdivisions(503);
            ratio->GetYaxis()->SetTitle("Data/MC");
            ratio->GetYaxis()->SetTitleSize(0.09);
            ratio->GetYaxis()->SetTitleOffset(0.45);
            ratio->GetYaxis()->SetLabelSize(0.08);
            ratio->GetXaxis()->SetTitle(varLabel[col]);
            ratio->GetXaxis()->SetLabelSize(0.08);
            ratio->GetXaxis()->SetTitleSize(0.09);
            ratio->GetXaxis()->SetTitleOffset(1.0);
            ratio->SetMinimum(0);
            ratio->SetMaximum(2);
            ratio->Draw("E");
            rp->Update();
            can.cd();
        }
    };

    drawRow(0,hData[0],hDYMC[0],hTTMC[0],hQCD[0]);
    drawRow(1,hData[1],hDYMC[1],hTTMC[1],hQCD[1]);
    drawRow(2,hData[2],hDYMC[2],hTTMC[2],hQCD[2]);

    can.SaveAs("CRgrid_2018.pdf");
    std::cout << "Saved CRgrid_2018.pdf" << std::endl;
}
