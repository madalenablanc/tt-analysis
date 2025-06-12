//*******************************************************************
//  makeCRplots.C  —  stand‑alone ROOT macro (ACLiC‑ready)
//  -------------------------------------------------------
//  Streams the three *fase‑1* TauhTauh background ntuples from EOS
//  and produces **one PNG per physics variable** (acoplanarity, mass,
//  rapidity, pT). Each PNG contains three vertically‑stacked pads
//  showing DY‑CR, QCD‑CR and tt̄‑CR.
//
//  Usage (in a CMSSW/ROOT env):
//     root -l -q 'makeCRplots.C+("2018")'
//
//  The era string ("2018") is only used in the output filenames.
//  ---------------------------------------------------------------
//  Author: ChatGPT assistance • 2025‑06‑12
//*******************************************************************

#include <TChain.h>
#include <TH1D.h>
#include <THStack.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <TLatex.h>
#include <TStyle.h>
#include <TSystem.h>
#include <vector>
#include <string>
#include <iostream>

// ------------------------- helper: book var‑bin histogram ----------
static void book(TH1D*& h,const char* name,const std::vector<double>& edges)
{
  h = new TH1D(name,name,static_cast<int>(edges.size())-1,&edges[0]);
  h->Sumw2();
}

// ------------------------- CR predicates ---------------------------
inline bool passDY (double M,double a,int Q,int nb){return (M> 40&&M<100 && a<0.30 && Q<0 && nb==0);} // OS, 0‑b
inline bool passQCD(double M,double a,int Q,int nb){return (M>100&&M<300 && a>0.80 && Q>0 && nb==0);} // SS, 0‑b
inline bool passTT (double M,double a,int Q,int nb){return (M>300         && a>0.50 && Q<0 && nb>=1);} // OS, ≥1‑b

// ------------------------- holder for four histos ------------------
struct CRHist {TH1D* acop; TH1D* mass; TH1D* rap; TH1D* pt;};

// *******************************************************************
void makeCRplots(const char* era="2018")
{
  gStyle->SetOptStat(0);

  // ---------- 1. set up TChains ------------------------------------
  const char* xrd = "root://xrootd-cms.infn.it";
  const char* pQCD = "/eos/cms/store/user/jjhollar/TauTau_NanoAOD_Madalena/BackgroundSamples/"
                    "TauhTauh/QCD_2018_UL_skimmed_TauTau_nano_fase1total-protons_2018.root";
  const char* pDY  = "/eos/cms/store/user/jjhollar/TauTau_NanoAOD_Madalena/BackgroundSamples/"
                    "TauhTauh/DY_2018_UL_skimmed_TauTau_nano_fase1total-protons_2018.root";
  const char* pTT  = "/eos/cms/store/user/jjhollar/TauTau_NanoAOD_Madalena/BackgroundSamples/"
                    "TauhTauh/ttJets_2018_UL_skimmed_TauTau_nano_fase1total-protons_2018.root";

  TChain chQCD("tree"), chDY("tree"), chTT("tree");
  chQCD.Add(Form("%s/%s",xrd,pQCD));
  chDY .Add(Form("%s/%s",xrd,pDY ));
  chTT .Add(Form("%s/%s",xrd,pTT ));

  // ---------- 2. binning vectors -----------------------------------
  const std::vector<double> bAcop_DY  ={0.00,0.05,0.10,0.15,0.20,0.25,0.30};
  const std::vector<double> bAcop_QCD ={0.50,0.60,0.70,0.80,0.90,1.00};
  const std::vector<double> bAcop_TT  ={0.80,0.86,0.92,0.96,1.00};
  const std::vector<double> bMass_DY  ={ 40, 60, 80,100};
  const std::vector<double> bMass_QCD ={100,200,400,600,800,1000};
  const std::vector<double> bMass_TT  ={300,400,500,600,800,1000,1200};
  const std::vector<double> bRap      ={-2.4,-1.6,-0.8,0.0,0.8,1.6,2.4};
  const std::vector<double> bPt       ={  0, 50,100,150,200,250,300,350};

  // ---------- 3. book histograms -----------------------------------
  CRHist hDY [3];  // index 0=DY‑CR, 1=QCD‑CR, 2=tt‑CR
  CRHist hQCD[3];
  CRHist hTT [3];
  auto bookSet=[&](CRHist& h,const char* tag,const std::vector<double>& a,const std::vector<double>& m){
    book(h.acop,Form("acop_%s",tag),a);
    book(h.mass,Form("mass_%s",tag),m);
    book(h.rap ,Form("rap_%s" ,tag),bRap);
    book(h.pt  ,Form("pt_%s"  ,tag),bPt );
  };
  bookSet(hDY [0],"dy_DY" ,bAcop_DY ,bMass_DY );
  bookSet(hDY [1],"dy_QCD",bAcop_QCD,bMass_QCD);
  bookSet(hDY [2],"dy_TT" ,bAcop_TT ,bMass_TT );
  bookSet(hQCD[0],"qcd_DY",bAcop_DY ,bMass_DY );
  bookSet(hQCD[1],"qcd_QCD",bAcop_QCD,bMass_QCD);
  bookSet(hQCD[2],"qcd_TT",bAcop_TT ,bMass_TT );
  bookSet(hTT [0],"tt_DY" ,bAcop_DY ,bMass_DY );
  bookSet(hTT [1],"tt_QCD",bAcop_QCD,bMass_QCD);
  bookSet(hTT [2],"tt_TT" ,bAcop_TT ,bMass_TT );

  // ---------- 4. fill lambda ---------------------------------------
  auto fill=[&](TChain& ch,CRHist (&h)[3],double w=1.0){
    Double_t M=0,a=0,rap=0,pt=0,chg0=0,chg1=0; Int_t nb=0;
    ch.SetBranchAddress("sist_mass",  &M   );
    ch.SetBranchAddress("sist_acop",  &a   );
    ch.SetBranchAddress("sist_rap" ,  &rap );
    ch.SetBranchAddress("sist_pt"  ,  &pt  );
    ch.SetBranchAddress("tau0_charge",&chg0);
    ch.SetBranchAddress("tau1_charge",&chg1);
    ch.SetBranchAddress("n_b_jet",    &nb  );
    const Long64_t N=ch.GetEntries();
    for(Long64_t i=0;i<N;++i){ ch.GetEntry(i);
      int Q = (chg0>=0?1:-1)*(chg1>=0?1:-1);  // OS (<0) or SS (>0)
      if(passDY (M,a,Q,nb)){h[0].acop->Fill(a,w); h[0].mass->Fill(M,w); h[0].rap->Fill(rap,w); h[0].pt->Fill(pt,w);} 
      if(passQCD(M,a,Q,nb)){h[1].acop->Fill(a,w); h[1].mass->Fill(M,w); h[1].rap->Fill(rap,w); h[1].pt->Fill(pt,w);} 
      if(passTT (M,a,Q,nb)){h[2].acop->Fill(a,w); h[2].mass->Fill(M,w); h[2].rap->Fill(rap,w); h[2].pt->Fill(pt,w);} 
    }
  };

  // optional: scale factors here (currently 1.0)
  fill(chDY ,hDY );
  fill(chQCD,hQCD);
  fill(chTT ,hTT );

  // ---------- 5. drawing helper ------------------------------------
  auto makeCanvas=[&](TH1D* mcQ[3],TH1D* mcDY[3],TH1D* mcTT[3],const char* xlab,const char* base){
    std::unique_ptr<TCanvas> can(new TCanvas(Form("c_%s",base),"",700,1200));
    can->Divide(1,3,0,0);
    const char* reg[3] = {"DY‑CR","QCD‑CR","tt̄‑CR"};
    for(int r=0;r<3;++r){ can->cd(r+1);
      THStack st("st","");
      mcQ [r]->SetFillColor(kYellow);    st.Add(mcQ [r]);
      mcDY[r]->SetFillColor(kGreen+2);   st.Add(mcDY[r]);
      mcTT[r]->SetFillColor(kRed);       st.Add(mcTT[r]);
      st.Draw("hist");
      st.GetXaxis()->SetTitle(xlab);
      st.GetYaxis()->SetTitle("Events (arb. norm)");
      if(r==0){
        TLegend* leg=new TLegend(0.60,0.70,0.88,0.88);
        leg->AddEntry(mcDY[r],"DY"   ,"f");
        leg->AddEntry(mcQ [r],"QCD"  ,"f");
        leg->AddEntry(mcTT[r],"tt̄"   ,"f");
        leg->Draw();
      }
      TLatex text; text.SetNDC(); text.SetTextSize(0.05);
      text.DrawLatex(0.15,0.85,reg[r]);
    }
    can->SaveAs(Form("%s_%s.png",base,era));
  };

  // ---------- 6. build and save each variable ----------------------
  TH1D* q[3]; TH1D* d[3]; TH1D* t[3];

  // (a) acoplanarity
  for(int i=0;i<3;++i){ q[i]=hQCD[i].acop; d[i]=hDY[i].acop; t[i]=hTT[i].acop; }
  makeCanvas(q,d,t,"Acoplanarity","acoplanarity");

  // (b) invariant mass
  for(int i=0;i<3;++i){ q[i]=hQCD[i].mass; d[i]=hDY[i].mass; t[i]=hTT[i].mass; }
  makeCanvas(q,d,t,"m_{#tau#tau} [GeV]","mass");

  // (c) rapidity
  for(int i=0;i<3;++i){ q[i]=hQCD[i].rap; d[i]=hDY[i].rap; t[i]=hTT[i].rap; }
  makeCanvas(q,d,t,"y_{#tau#tau}","rapidity");

  // (d) pT
  for(int i=0;i<3;++i){ q[i]=hQCD[i].pt; d[i]=hDY[i].pt; t[i]=hTT[i].pt; }
  makeCanvas(q,d,t,"p_{T}^{#tau#tau} [GeV]","pt");
}
