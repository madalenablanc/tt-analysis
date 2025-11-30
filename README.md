# tt-analysis

## Check point commit -> BEFORE saving the proton related variables in the real data samples

# Plots
- Made with plot_m_new.cpp
- samples used are:
    - DY_2018_UL_MuTau_nano_merged_pileup_protons.root
    - ttjets_2018_UL_MuTau_nano_merged_pileup_protons.root
    - QCD_2018_UL_MuTau_nano_merged_pileup_protons.root
    - Data_2018_UL_MuTau_nano_merged_pileup_protons.root
    - MuTau_sinal_SM_2018_july.root -> signal file
- The processing for these files is the same as done in original analysis;
- For the samples where the proton related variables are saved from the beginning the number of events **should be the same**
    - this was done because adding pileup protons to real data was not the most correct procedure

### The Data and QCD samples in these plots have added pileup protons to them
|           |   | |
:-------------------------:|:-------------------------:|:-------------------------:
![](output_plots/mass.png)  |  ![](output_plots/rapidity_central.png) | ![](output_plots/rapidity_matching.png)|
|![](output_plots/pt_central.png)|![](output_plots/tau_pt.png)| ![](output_plots/rapidity_matching.png)|
|![](output_plots/mass_diff.png)|![](output_plots/mass.png)|![](output_plots/aco.png)|
|![](output_plots/met.png)|||



## Cuts applied
## MuTau Channel

| Category / Cut        | ttjets           | dy              | qcd             | data            |
|-----------------------|------------------|------------------|------------------|------------------|
| **FASE0**             | 1 tau, 1 muon    | 1 tau, 1 muon    | 1 tau, 1 muon    | 1 tau, 1 muon    |
| Double_medium         |                  |                  |                  |                  |
| Iso_Mu24              | ✓                | ✓                | ✓                | ✓                |
| Ele32_WpTight         |                  |                  |                  |                  |
| Electron_             |                  |                  |               |               |
| lumi filtering        |                  |                  | ✓                 |  ✓                |
| **FASE1**             |                  |                  |                  |                  |
| tau id1               | >63              | >63              | >63              | >63              |
| tau id2               | >7               | >7               | >7               | >7               |
| tau id3               | >1               | >1               | >1               | >1               |
| tau pt                | >100             | >100             | >100             | >100             |
| mu id                 | ≥3               | ≥3               | ≥3               | ≥3               |
| mu pt                 | >35              | >35              | >35              | >35              |
| ele id                |                  |                  |                  |                  |
| ele pt                |                  |                  |                  |                  |
| delta R               | >0.4             | >0.4             | >0.4             | >0.4             |
| eta                   | <2.4             | <2.4             | <2.4             | <2.4             |
| charge signs          | opposite         | opposite         | same             | opposite         |
| scaling factors       | ✓                | ✓                |                  |                  |
| valid jet and MET info | ✓              | ✓                |                  |                  |

## Nr of events


|       |Starting sample| Phase0 | Phase1 |
| ------|---------------|--------|--------|
| Data **|984,127,247|698,931,141|1,708|
|QCD **|984,127,247|698,931,141|234|
| Data|984,127,247|19,698|6,859|
|QCD|984,127,247|19,698|1,011|
|ttjets|304,895,029|42,500,241|26,649|
|DY|195,510,810|41,388,654|1,704|
|signal|299,979|NA|13,893|

** Is the samples that save the proton related variables instead of adding pileup protons later. 

### --> Looks like the issue is the change in code to save proton related variables in phase1 

### TODO: re-run analysis ; back to original code and then go from there




