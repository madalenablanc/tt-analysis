# Tau Analysis Project - Point of Situation

**Project**: Tau-Tau Analysis with Central Exclusive Production
**Framework**: CMSSW 12.2.4
**Dataset**: 2018 Ultra Legacy (UL)
**Date**: December 18, 2025

---

## Executive Summary

This analysis studies tau lepton pair production in association with forward protons using 2018 CMS data. The analysis employs a multi-phase approach to filter and reconstruct events, focusing on three decay channels: MuTau, ETau, and TauTau. The project uses ROOT/RDataFrame for event processing and implements proton tagging requirements for central exclusive production studies.

---

## Analysis Channels

### 1. MuTau Channel (Primary Focus)
- **Phase 0**: Initial skimming with 1 tau + 1 muon selection
- **Phase 1**: Particle identification and kinematic cuts
- **Phase 2**: Proton selection and tagging requirements

### 2. ETau Channel
- Electron-tau final state
- Similar phase structure to MuTau

### 3. TauTau Channel
- Di-tau final state
- Control region and signal region separation

---

## Data Processing Pipeline

### Phase 0 - Initial Skimming
**Objective**: Reduce dataset size by selecting events with basic topology

**Selection Criteria**:
- Exactly 1 tau candidate
- Exactly 1 muon candidate (MuTau) or electron (ETau)
- HLT trigger: `HLT_IsoMu24` (MuTau) or `HLT_Ele32_WPTight` (ETau)
- Luminosity filtering for data

**Status**: [COMPLETED]
- Data events after Phase 0: 698,931,141
- Proton variables saved in output

### Phase 1 - Event Selection
**Objective**: Apply stringent identification and kinematic requirements

**Selection Criteria**:

| Variable | Cut Value | Description |
|----------|-----------|-------------|
| `tau_id1` | > 63 | DeepTau2017v2p1 vs Jet |
| `tau_id2` | > 7 | DeepTau2017v2p1 vs Electron |
| `tau_id3` | > 1 | DeepTau2017v2p1 vs Muon |
| `tau_pt` | > 100 GeV | Tau transverse momentum |
| `muon_id` | >= 3 | Muon ID (Medium or better) |
| `muon_pt` | > 35 GeV | Muon transverse momentum |
| `delta_R` | > 0.4 | Angular separation |
| `|eta|` | < 2.4 | Detector acceptance |
| Charge signs | Opposite (signal) / Same (QCD) | Lepton charges |

**Event Yields After Phase 1**:

| Sample | Starting Events | Phase 0 | Phase 1 | Efficiency |
|--------|----------------|---------|---------|------------|
| Data | 984,127,247 | 19,698 | 6,859 | 0.0007% |
| QCD | 984,127,247 | 19,698 | 1,011 | 0.0001% |
| ttjets | 304,895,029 | 42,500,241 | 26,649 | 0.009% |
| DY | 195,510,810 | 41,388,654 | 1,704 | 0.0009% |
| Signal | 299,979 | N/A | 13,893 | 4.6% |

**Status**: [COMPLETED] for most samples
- [WARNING] Note: New processing with proton variables yields significantly different statistics

### Phase 2 - Proton Selection
**Objective**: Select events with forward protons in PPS (Precision Proton Spectrometer)

**Proton Selection Strategy**:

1. **MultiRP Requirement**: Only multiRP protons used (better reconstruction)
2. **Two-Arm Topology**: Require at least one proton in each arm (0 and 1)
3. **Highest xi Selection**: For multiple protons per arm, select largest xi value
   - xi = 1 - (p_final/p_initial): fractional momentum loss
   - Larger xi corresponds to primary scattered proton
   - Reduces pile-up contamination

**Output Variables**:
- `selected_xi_arm0`: Proton momentum loss in arm 0
- `selected_xi_arm1`: Proton momentum loss in arm 1

**Status**: [IN PROGRESS]

---

## Technical Implementation

### Software Stack
- **Framework**: CMSSW 12.2.4
- **Language**: Python 3 with ROOT/RDataFrame
- **Storage**: EOS (`/eos/user/m/mblancco/`)
- **Authentication**: VOMS proxy (CMS VO)

### Key Technologies
- ROOT RDataFrame for columnar analysis
- XRootD for remote file access
- Multithreading enabled for performance
- Resume capability with JSON checkpointing

### File Structure
```
tau_analysis/
|-- MuTau_channel/          # MuTau analysis scripts
|-- ETau_channel/           # ETau analysis scripts
|-- TauTau_Channel/         # TauTau analysis scripts
|-- fase0_code/             # Phase 0 processing
|-- fase1_code/             # Phase 1 + CMSSW dependencies
|-- plots/                  # Plotting scripts
|-- background/             # Background estimation
+-- output_plots*/          # Generated plots
```

---

## Current Analysis Focus

### Proton Variable Handling
**Challenge**: Initial approach of adding pileup protons post-processing was incorrect.

**Solution**: Save proton-related variables directly from NanoAOD during Phase 0:
- `nProton_multiRP` / `nProton_singleRP`
- `Proton_multiRP_xi` (momentum loss)
- `Proton_multiRP_arm` (detector arm: 0 or 1)
- `Proton_multiRP_t` (momentum transfer squared)
- `Proton_multiRP_thetaX/Y` (scattering angles)
- `Proton_multiRP_time` / `timeUnc`
- `Proton_singleRP_xi`

### Current Processing
**File**: [fase1_qcd.py](MuTau_channel/fase1_qcd.py)
- Processing QCD sample with Phase 1 cuts
- Same-sign requirement (muon_charge × tau_charge > 0)
- Output: `/eos/user/m/mblancco/samples_2018_mutau/fase1_qcd_proton_vars/`

---

## Analysis Variables

### Kinematic Variables
| Variable | Description |
|----------|-------------|
| `sist_mass` | Invariant mass of mu+tau system |
| `sist_pt` | Transverse momentum of system |
| `sist_rap` | Rapidity of system |
| `acop` | Acoplanarity: abs(Delta-phi) / pi |
| `met_pt` | Missing transverse energy |
| `delta_r` | Delta-R separation between leptons |

### Jet Variables
| Variable | Description |
|----------|-------------|
| `jet_pt` | Leading jet pT |
| `jet_btag` | DeepCSV b-tagging score |
| `n_b_jet` | Number of b-tagged jets (> 0.4506) |

### Weights
- MC samples: Generator weights + scale factors
- Data samples: weight = 1.0
- Scale factors applied: Pileup, Tau ID, Muon ID

---

## Generated Plots

### Plot Categories

1. **No Proton Requirements**
   - Baseline kinematic distributions
   - All Phase 1 selected events

2. **Proton Requirements (Data Only)**
   - Forward proton tagging applied
   - Two-arm requirement enforced

3. **Proton Requirements (Data + QCD)**
   - QCD background with proton selection
   - For background estimation validation

### Key Distributions
- Invariant mass (`mass`)
- System rapidity (`rapidity_central`, `rapidity_matching`)
- Transverse momentum (`pt_central`, `tau_pt`)
- Mass difference (`mass_diff`)
- Acoplanarity (`aco`)
- Missing ET (`met`)

---

## Known Issues and Concerns

### 1. Event Yield Discrepancy
**Observation**: Processing with saved proton variables shows different event counts

| Sample Type | Original Phase 1 | With Proton Vars | Ratio |
|-------------|------------------|------------------|-------|
| Data | 6,859 | 1,708 | 0.25 |
| QCD | 1,011 | 234 | 0.23 |

**Hypothesis**: Code changes for proton variable saving in Phase 1 may have introduced additional implicit cuts.

**Action Required**:
- [DONE] Re-ran Phase 0 with correct script
- [TODO] Investigating Phase 1 processing differences
- [TODO] Validate proton variable preservation

### 2. Luminosity File
**File**: `dadosluminosidade.txt` referenced but usage unclear in current code
- Lumi filtering lines are commented out in fase1_qcd.py

### 3. Background Estimation
**QCD Background**: Same-sign to opposite-sign extrapolation method
- Phase 1 QCD processing uses same-sign requirement
- Need transfer factor calculation for opposite-sign region

---

## Next Steps

### Immediate Actions
1. [DONE] Complete Phase 1 processing for QCD with proton variables
2. [TODO] Run Phase 1 for Data (opposite-sign) with proton variables
3. [TODO] Debug event count discrepancy between processing versions
4. [TODO] Implement Phase 2 proton selection on Phase 1 output

### Short-Term Goals
- Validate proton variable integrity across all phases
- Generate comparison plots: with/without proton requirements
- Calculate QCD normalization factors
- Implement systematic uncertainties

### Long-Term Objectives
- Statistical analysis and limit setting
- Signal vs. background discrimination
- Cross-section measurements
- Publication preparation

---

## Data Samples

### Monte Carlo Samples
- **DY** (Drell-Yan): Z/$\gamma$* $\to$ $\tau\tau$ background
- **ttjets**: Top quark pair production background
- **QCD**: Multijet background (data-driven)
- **Signal**: Central exclusive production of tau pairs (SM)

### Data Sample
- **2018 UL**: CMS 2018 Ultra Legacy dataset
- **Trigger**: Single muon (IsoMu24) for MuTau channel
- **Integrated Luminosity**: ~59.83 fb$^{-1}$ (2018)

### Storage Locations
- Input: `/eos/home-m/mblancco/samples_2018_mutau/`
- Phase 0 output: `/eos/user/m/mblancco/samples_2018_mutau/fase0_mutau_proton_vars/`
- Phase 1 output: `/eos/user/m/mblancco/samples_2018_mutau/fase1_qcd_proton_vars/`

---

## Collaboration and Resources

### Dependencies
- TauPOG corrections and scale factors
- Pileup reweighting profiles
- B-tagging scale factors (DeepCSV medium WP: 0.4506)
- Tau ID scale factors (DeepTau2017v2p1)

### Computing Resources
- EOS storage allocation
- CERN lxplus/batch system access
- VOMS proxy authentication

---

## Revision History

| Date | Milestone | Notes |
|------|-----------|-------|
| 2025-12 | Phase 0 complete | Re-ran with correct proton variable saving |
| 2025-12 | Phase 1 QCD | Processing with same-sign requirement |
| Current | Phase 1 Data | TODO: Process opposite-sign data |
| Future | Phase 2 | Proton selection implementation |

---

## Contact and Documentation

**Project Directory**: `/Users/utilizador/cernbox/tau_analysis/`
**README**: [README.md](README.md)
**Main README PDF**: [README.pdf](README.pdf)

---

## Appendix: Cut Flow Summary

### MuTau Channel Cut Flow

| Cut Stage | Data | QCD (SS) | ttjets | DY | Signal |
|-----------|------|----------|--------|-----|--------|
| NanoAOD | 984M | 984M | 305M | 196M | 300k |
| Phase 0 | 698M* | 698M* | 42.5M | 41.4M | - |
| Phase 0 (skim) | 19,698 | 19,698 | 42.5M | 41.4M | - |
| HLT + 1mu+1tau | 19,698 | 19,698 | 42.5M | 41.4M | - |
| Lepton ID | - | - | - | - | - |
| Kinematic cuts | - | - | - | - | - |
| Charge requirement | 6,859 | 1,011 | 26,649 | 1,704 | 13,893 |
| **Final Phase 1** | **6,859** | **1,011** | **26,649** | **1,704** | **13,893** |

*Initial skim with proton variable saving

---

**Document Status**: DRAFT
**Last Updated**: 2025-12-18
**Version**: 1.0
