#include <iostream>
#include <cmath>
#include "TApplication.h"
#include "TH1D.h"
#include "TFile.h"
#include "TTree.h"
#include "TLeaf.h"
#include "TCanvas.h"
#include "TROOT.h"
#include "ROOT/TThreadExecutor.hxx"
#include <memory>
#include <string>
#include <vector>
#include <mutex>

using namespace std;

struct TreeTask {
	string label;
	TTree* tree;
	double weight;
	bool fillFromNPuLeaf;
};

struct HistResults {
	unique_ptr<TH1D> xi_arm0;
	unique_ptr<TH1D> xi_arm1;
	unique_ptr<TH1D> n_PU_0;
	unique_ptr<TH1D> n_PU_1;
};

static std::mutex gLogMutex;

static void PrintProgress(const string &label, Long64_t current, Long64_t total) {
	lock_guard<std::mutex> lock(gLogMutex);
	const double fraction = (total > 0) ? double(current) / double(total) : 1.0;
	const int pct = static_cast<int>(fraction * 100.);
	cout << "[progress] Tree " << label << " " << pct << "% (" << current << "/" << total << ")" << endl;
}

static HistResults ProcessTree(const TreeTask &task) {
	HistResults results;
	results.xi_arm0 = make_unique<TH1D>(("xi_arm0_" + task.label).c_str(), "xi_arm0", 100, 0.01, 0.3);
	results.xi_arm1 = make_unique<TH1D>(("xi_arm1_" + task.label).c_str(), "xi_arm1", 100, 0.01, 0.3);
	results.n_PU_0 = make_unique<TH1D>(("n_PU_0_" + task.label).c_str(), "n_PU arm 0", 10, 0, 10);
	results.n_PU_1 = make_unique<TH1D>(("n_PU_1_" + task.label).c_str(), "n_PU arm 1", 10, 0, 10);

	if (!task.tree) {
		return results;
	}

	TLeaf *leafArm = task.tree->GetLeaf("proton_arm");
	TLeaf *leafXi = task.tree->GetLeaf("proton_xi");
	TLeaf *leafNPu = task.tree->GetLeaf("n_pu");

	const Long64_t entries = task.tree->GetEntries();
	PrintProgress(task.label, 0, entries);
	if (entries <= 0) {
		return results;
	}

	Long64_t reportEvery = entries / 10;
	if (reportEvery <= 0) reportEvery = 1;
	Long64_t nextReport = reportEvery;
	for (Long64_t i = 0; i < entries; ++i) {
		task.tree->GetEntry(i);

		if (!leafArm || !leafXi) {
			continue;
		}

		const Int_t len = leafArm->GetLen();
		if (len <= 0) {
			results.n_PU_0->Fill(0., task.weight);
			results.n_PU_1->Fill(0., task.weight);
			continue;
		}

		const double firstArm = leafArm->GetValue(0);
		const double firstXi = leafXi->GetValue(0);
		if (firstArm == 0) {
			results.xi_arm0->Fill(firstXi, task.weight);
		} else if (firstArm == 1) {
			results.xi_arm1->Fill(firstXi, task.weight);
		}

		if (task.fillFromNPuLeaf && leafNPu) {
			const double npuVal = leafNPu->GetValue(0);
			if (firstArm == 0) {
				results.n_PU_0->Fill(npuVal, task.weight);
			} else if (firstArm == 1) {
				results.n_PU_1->Fill(npuVal, task.weight);
			}
		}

		int pu_0 = 0;
		int pu_1 = 0;
		for (Int_t j = 0; j < len; ++j) {
			const double armVal = leafArm->GetValue(j);
			if (armVal == 0) pu_0++;
			if (armVal == 1) pu_1++;
		}

		results.n_PU_0->Fill(pu_0, task.weight);
		results.n_PU_1->Fill(pu_1, task.weight);

		const Long64_t processed = i + 1;
		if (processed >= nextReport || processed == entries) {
			PrintProgress(task.label, processed, entries);
			nextReport += reportEvery;
		}
	}
	return results;
}

int main(){


	TApplication app("app",NULL,NULL);
	gROOT->SetBatch(kTRUE);
	ROOT::EnableImplicitMT();
	TH1::AddDirectory(kFALSE);
	cout << "[checkpoint] Application initialized" << endl;

	//Ficheiros de input

	TFile *fA = TFile::Open("/eos/cms/store/group/phys_smp/Exclusive_DiTau/proton_pool_2018/proton_pool_2018.root");
        //TFile *fA = TFile::Open("proton_pool_2018_UL_fromQCD_nano_1.root");
	TFile *fB = TFile::Open("/eos/cms/store/group/phys_smp/Exclusive_DiTau/proton_pool_2018/proton_pool_2018.root");
	TFile *fC = TFile::Open("/eos/cms/store/group/phys_smp/Exclusive_DiTau/proton_pool_2018/proton_pool_2018.root");
	TFile *fD = TFile::Open("/eos/cms/store/group/phys_smp/Exclusive_DiTau/proton_pool_2018/proton_pool_2018.root");

	TTree *treeA = (TTree*) fA->Get("tree");
	TTree *treeB = (TTree*) fB->Get("tree");
	TTree *treeC = (TTree*) fC->Get("tree");
	TTree *treeD = (TTree*) fD->Get("tree");
	cout << "[checkpoint] Trees retrieved" << endl;

	int n_a = treeA->GetEntries();
	int n_b = treeB->GetEntries();
	int n_c = treeC->GetEntries();
	int n_d = treeD->GetEntries();
	cout << "[checkpoint] Entries loaded: A=" << n_a << " B=" << n_b << " C=" << n_c << " D=" << n_d << endl;

	double n_tot = n_a + n_b + n_c + n_d;

	double frac_a= 0.21*0+1.;
	double frac_b= 0.16*0;
	double frac_c= 0.13*0;
	double frac_d= 0.50*0;

	double w_a=frac_a*n_tot/double(n_a);
	double w_b=frac_b*n_tot/double(n_b);
	double w_c=frac_c*n_tot/double(n_c);
	double w_d=frac_d*n_tot/double(n_d);

	//Declaracao histogramas

	TH1D xi_arm1("xi_arm1","xi_arm1",100,0.01,0.3);
	TH1D xi_arm0("xi_arm0","xi_arm0",100,0.01,0.3);

	TH1D n_PU_1("n_PU_1","n_PU arm 1",10,0,10);
	TH1D n_PU_0("n_PU_0","n_PU arm 0",10,0,10);

	vector<TreeTask> tasks = {
		{"A", treeA, w_a, false},
		{"B", treeB, w_b, false},
		{"C", treeC, w_c, false},
		{"D", treeD, w_d, true}
	};

	cout << "[checkpoint] Launching parallel processing" << endl;
	ROOT::TThreadExecutor executor;
	vector<HistResults> partialHists = executor.Map(ProcessTree, tasks);
	cout << "[checkpoint] Finished parallel processing" << endl;

	for (auto &res : partialHists) {
		if (res.xi_arm0) xi_arm0.Add(res.xi_arm0.get());
		if (res.xi_arm1) xi_arm1.Add(res.xi_arm1.get());
		if (res.n_PU_0) n_PU_0.Add(res.n_PU_0.get());
		if (res.n_PU_1) n_PU_1.Add(res.n_PU_1.get());
	}

	cout << "Total number of events with 0 PU protons in arm 0 " << n_PU_0.GetBinContent(0) << endl;
	cout << "Total number of events with 1 PU proton in arm 0 " << n_PU_0.GetBinContent(1) << endl;
	cout << "Total number of events with 2 PU protons in arm 0 " << n_PU_0.GetBinContent(2) << endl;
        cout << "Total number of events with 3 PU protons in arm 0 " << n_PU_0.GetBinContent(3) << endl;
	cout << "Total number of events with 4 PU protons in arm 0 " << n_PU_0.GetBinContent(4) << endl;
        cout << "Total number of events with 5 PU protons in arm 0 " << n_PU_0.GetBinContent(5) << endl;
	cout << "Number of entries of n_PU_0 histogram " << n_PU_0.GetEntries() << endl << endl;


	cout << "Total number of events with 0 PU protons in arm 1 " << n_PU_1.GetBinContent(0) << endl;
        cout << "Total number of events with 1 PU proton in arm 1 " << n_PU_1.GetBinContent(1) << endl;
        cout << "Total number of events with 2 PU protons in arm 1 " << n_PU_1.GetBinContent(2) << endl;
        cout << "Total number of events with 3 PU protons in arm 1 " << n_PU_1.GetBinContent(3) << endl;
        cout << "Total number of events with 4 PU protons in arm 1 " << n_PU_1.GetBinContent(4) << endl;
        cout << "Total number of events with 5 PU protons in arm 1 " << n_PU_1.GetBinContent(5) << endl;
	cout << "Number of entries of n_PU_1 histogram " << n_PU_1.GetEntries() << endl << endl;

	cout << "[checkpoint] Starting canvas export" << endl;

	TCanvas c1("c1","n_PU_arm0",800,600);

	n_PU_0.SetLineColor(kBlue);
	n_PU_1.SetLineColor(kRed);

	n_PU_0.DrawNormalized("hist");
	c1.SaveAs("n_PU_arm0.png");
	cout << "[checkpoint] Saved n_PU_arm0.png" << endl;

	TCanvas c2("c2","n_PU_arm1",800,600);

	n_PU_1.DrawNormalized("hist");
	c2.SaveAs("n_PU_arm1.png");
	cout << "[checkpoint] Saved n_PU_arm1.png" << endl;

	TCanvas c3("c3","xi_arms",800,600);

	xi_arm0.SetLineColor(kBlue);
	xi_arm0.DrawNormalized("hist");

	xi_arm1.SetLineColor(kRed);
	xi_arm1.DrawNormalized("hist same");
	c3.SaveAs("xi_arms.png");
	cout << "[checkpoint] Saved xi_arms.png" << endl;

	return 0;

}
