#include <cstdlib>

#include <iostream>

#include <map>

#include <string>


#include "TChain.h"

#include "TFile.h"

#include "TTree.h"

#include "TLeaf.h"

#include "TString.h"

#include "TObjString.h"

#include "TSystem.h"

#include "TROOT.h"


#include "TMVA/Factory.h"

#include "TMVA/DataLoader.h"

#include "TMVA/Tools.h"

#include "TMVA/TMVAGui.h"


int TMVAClassification_fast( TString myMethodList = "" )

{

   TMVA::Tools::Instance();


   std::map<std::string,int> Use;

   Use["BDT"] = 1; // only BDT with AdaBoost

   std::cout << std::endl;

   std::cout << "==> Start TMVAClassification_fast (simplified BDT)" << std::endl;


   if (myMethodList != "") {

      for (std::map<std::string,int>::iterator it = Use.begin(); it != Use.end(); it++) it->second = 0;

      std::vector<TString> mlist = TMVA::gTools().SplitString( myMethodList, ',' );

      for (UInt_t i=0; i<mlist.size(); i++) {

         std::string regMethod(mlist[i]);

         if (Use.find(regMethod) == Use.end()) {

            std::cout << "Method \"" << regMethod << "\" not known in TMVA under this name. Choose among the following:" << std::endl;

            for (std::map<std::string,int>::iterator it = Use.begin(); it != Use.end(); it++) std::cout << it->first << " ";

            std::cout << std::endl;

            return 1;

         }

         Use[regMethod] = 1;

      }

   }


   // --------------------------------------------------------------------------------------------------

   // Input files (same as original)

   TFile *inputBKG(0);

   TFile *inputSIGN(0);


   TString fnameBKG  = "background_total-protons_syst.root";

   TString fnameSIGN = "MuTau_sinal_SM_2018_july.root";


   if (!gSystem->AccessPathName(fnameBKG) || !gSystem->AccessPathName(fnameSIGN) ) {

      inputBKG  = TFile::Open( fnameBKG );

      inputSIGN = TFile::Open( fnameSIGN );

   }

   else {

      TFile::SetCacheFileDir(".");

      inputSIGN = TFile::Open("http://root.cern.ch/files/tmva_class_example.root", "CACHEREAD");

   }

   if (!inputBKG) {

      std::cout << "ERROR: could not open data file" << std::endl;

      exit(1);

   }

   std::cout << "--- TMVAClassification_fast  : Using input BKG file: "    << inputBKG->GetName()  << std::endl;

   std::cout << "--- TMVAClassification_fast  : Using input SIGNAL file: " << inputSIGN->GetName() << std::endl;


   TTree *background = (TTree*)inputBKG->Get("tree");

   TTree *signalTree = (TTree*)inputSIGN->Get("tree");


   // Output goes to a DIFFERENT file so the original results are preserved
   TString outfileName( "TMVA_allBkg_Mutau_2018_fast.root" );

   TFile* outputFile = TFile::Open( outfileName, "RECREATE" );


   TMVA::Factory *factory = new TMVA::Factory( "TMVAClassification", outputFile,

                                               "!V:!Silent:Color:DrawProgressBar:Transformations=I:AnalysisType=Classification" );


   // Weights saved to dataset_fast/ so the original dataset/ directory is untouched
   TMVA::DataLoader *dataloader = new TMVA::DataLoader("dataset_fast");


   // Same input variables as the original
   dataloader->AddVariable("sist_rap",    "Central system rapidity",       "",    'F');

   dataloader->AddVariable("acop",        "Central system acoplanarity",   "",    'F');

   dataloader->AddVariable("sist_pt",     "Total Momentum",                "GeV", 'F');

   dataloader->AddVariable("mu_pt",       "muon pt",                       "GeV", 'F');

   dataloader->AddVariable("tau_pt",      "tau pt",                        "GeV", 'F');

   dataloader->AddVariable("sist_mass",   "Invariant Mass",                "GeV", 'F');

   dataloader->AddVariable("sist_mass - sqrt(13000.0*13000.0*xi_arm1_1*xi_arm2_1)", "Invariant Mass Matching", "GeV", 'F');

   dataloader->AddVariable("met_pt",      "Missing energy",                "GeV", 'D');

   dataloader->AddVariable("sist_rap-0.5*log(xi_arm1_1/xi_arm2_1)", "Rapidity matching", "", 'D');


   dataloader->SetSignalWeightExpression("weight*weights_bsm_sf[51]");

   dataloader->SetBackgroundWeightExpression("weight");


   double signalWeight     = 0.00021;

   double backgroundWeight = 104.;

   dataloader->AddSignalTree    ( signalTree, signalWeight     );

   dataloader->AddBackgroundTree( background, backgroundWeight );


   TCut mycuts = "";

   TCut mycutb = "";


   // Fewer training events for faster training
   dataloader->PrepareTrainingAndTestTree( mycuts, mycutb,

                                           "nTrain_Signal=2000:nTrain_Background=5000:SplitMode=Random:NormMode=NumEvents:!V" );


   // Simpler BDT: fewer trees (200 vs 850), shallower depth (2 vs 3)
   if (Use["BDT"])

      factory->BookMethod( dataloader, TMVA::Types::kBDT, "BDT",

                           "!H:!V:NTrees=200:MinNodeSize=2.5%:MaxDepth=2:BoostType=AdaBoost:AdaBoostBeta=0.5:UseBaggedBoost:BaggedSampleFraction=0.5:SeparationType=GiniIndex:nCuts=20" );


   factory->TrainAllMethods();

   factory->TestAllMethods();

   factory->EvaluateAllMethods();


   outputFile->Close();


   std::cout << "==> Wrote root file: " << outputFile->GetName() << std::endl;

   std::cout << "==> TMVAClassification_fast is done!" << std::endl;


   delete factory;

   delete dataloader;

   if (!gROOT->IsBatch()) TMVA::TMVAGui( outfileName );


   return 0;

}
