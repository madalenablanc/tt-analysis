#include <iostream>
#include <vector>

void check_proton_multiplicity() {
  TFile *f = TFile::Open("yourfile.root");
  TTree *ntp1 = (TTree*) f->Get("ntp1");

  std::vector<float> *proton_xi = 0;
  ntp1->SetBranchAddress("proton_xi", &proton_xi);

  Long64_t nEntries = ntp1->GetEntries();
  int count_eq2 = 0, count_gt2 = 0;

  for (Long64_t i=0; i<nEntries; i++) {
    ntp1->GetEntry(i);
    if (proton_xi->size() == 2) count_eq2++;
    if (proton_xi->size() > 2) count_gt2++;
  }

  std::cout << "Events with exactly 2 protons = " << count_eq2 << std::endl;
  std::cout << "Events with >2 protons        = " << count_gt2 << std::endl;
}
