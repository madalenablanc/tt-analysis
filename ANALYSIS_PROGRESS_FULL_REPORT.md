# Tau Analysis: Detailed Progress Report from Slides and Documentation

**Project:** Central exclusive tau production with PPS tagging  
**Dataset:** 2018 Ultra Legacy, 13 TeV, 54.9 fb^-1  
**Channels covered in the documentation:** TauTau, ETau, MuTau  
**Report purpose:** Reconstruct the full analysis progression from the available documentation, slides, update notes, and instruction files, organized both by physics topic and by date.

---

## 1. Scope and method

This document is not a file inventory. It is a reconstruction of the analysis itself based on the content of the available documentation:

- early physics-motivation slides;
- early signal-variable studies;
- recovered code-flow and cut descriptions;
- MuTau normalization and weighting notes;
- BDT/TMVA and Combine slides;
- operational notes for running the final statistical workflow;
- saved Combine outputs.

Where multiple documents conflict, the later documents are treated as the more mature interpretation, especially when they explicitly describe a bug fix or a normalization correction.

---

## 2. Physics goal of the analysis

The documented target process is central exclusive or proton-tagged tau production, motivated by photon-induced production,

$$
pp \to p + X(\to \tau\tau) + p,
$$

with PPS-tagged forward protons and tau decay products reconstructed in CMS. The long-term physics motivation is sensitivity to anomalous tau couplings and, in particular, the tau anomalous magnetic moment through

$$
\gamma\gamma \to \tau^+\tau^-.
$$

The documents consistently show two connected goals:

1. build a realistic event-selection and proton-tagged analysis in several tau decay channels;
2. use kinematic and proton-matching observables to separate signal from backgrounds and set a limit on the signal strength or cross section.

The final documented mature workflow is centered on the **MuTau** channel, with ETau and TauTau appearing mainly as earlier development or control/recovery channels.

---

## 3. Core analysis strategy as it emerges from the documentation

Across the full documentation set, the analysis converges to the following workflow:

1. start from CMS 2018 UL NanoAOD or MINIAOD-derived inputs;
2. apply a first skim (`phase 0`) requiring the channel topology and trigger;
3. apply a tighter event-selection stage (`phase 1`) with object ID, kinematic, and charge requirements;
4. treat MC and real-data proton information differently:
   - for **DY** and **ttbar**, add pileup protons later using a proton pool;
   - for **Data** and **QCD**, preserve real proton variables from the beginning;
5. construct central-system and proton-system observables;
6. compare signal-like and background-like samples in key distributions;
7. train multivariate discriminants, primarily a BDT;
8. convert BDT or mass shapes into Combine datacards and workspaces;
9. extract limits, significance, fit diagnostics, goodness-of-fit, and nuisance impacts.

This final structure was not present from the beginning. The documentation shows that the analysis matured in stages: first physics motivation, then signal-variable studies, then code recovery and debugging, then proton-handling corrections, then normalization corrections, and finally the statistical interpretation.

---

## 4. Main observables and their physics role

The same small set of observables appears repeatedly from the earliest signal studies to the final BDT input list.

### 4.1 Central-system variables

These are reconstructed from the visible tau-decay products in the central detector:

- invariant mass of the central system, usually `sist_mass` or `m_X`;
- transverse momentum of the central system, `sist_pt`;
- rapidity of the central system, `sist_rap`;
- acoplanarity, `acop`, measuring how far the visible system is from a back-to-back topology;
- missing transverse energy, `met_pt`.

Physics role:
- exclusive or quasi-exclusive production tends to produce a centrally balanced topology;
- acoplanarity and system `p_T` help reject ordinary inclusive backgrounds;
- the invariant mass is the simplest one-dimensional discriminant for limit setting.

### 4.2 Proton variables

The proton-tagging side is built from PPS quantities:

- proton fractional momentum loss, `\xi`;
- proton arm assignment, 0 and 1;
- scattering angles, especially `\theta_x`;
- proton track positions in the roman pot sensors;
- proton-derived mass and rapidity estimates.

Physics role:
- a genuine proton-tagged signal should have one proton on each PPS arm;
- the proton pair defines an independent estimate of the central-system mass and rapidity,
  
  $$M_{pp}^2 \sim s\,\xi_1\xi_2, \qquad Y_{pp} = \tfrac{1}{2}\ln(\xi_1/\xi_2),$$
  
  which can be compared with the mu+tau system;
- pileup contamination is one of the dominant technical problems, so proton handling is central to the whole analysis.

### 4.3 Matching observables

Several documents emphasize:

- `M_CMS - M_PPS`, or equivalent mass-matching terms;
- `Y_CMS - Y_PPS`, or equivalent rapidity-matching terms.

These are among the strongest signal/background discriminants because true exclusive candidates should be consistent simultaneously in the central detector and in PPS.

---

## 5. Chronological development of the analysis

## 5.1 Early conceptual stage: broad exclusive-physics motivation

### Early presentation material (`apresentacao.pdf`, `first-stage-analysis.pdf` preface)

The earliest material is not yet a channel-level analysis note. It frames the project around central exclusive production in proton-proton collisions as a general precision and BSM probe. The message is:

- proton tagging gives extra event-level information beyond the central detector;
- exclusive final states suppress ordinary QCD activity;
- tau final states are attractive because the tau is heavy and may be more sensitive to BSM effects than electrons or muons.

This stage is still mostly conceptual. It does not yet define the final MuTau workflow. It establishes why proton-tagged tau physics is worth pursuing.

### 05/02/2025: anomalous tau-moment motivation (`documentation/05_02_25.pdf`)

This is the first clearly dated physics-motivation deck. It sharpens the objective:

- the process of interest is photon-induced tau production, `\gamma\gamma \to \tau^+\tau^-`;
- the target observable is sensitivity to the tau anomalous magnetic moment;
- the project is explicitly tied to an existing reference analysis (“Matteo’s analysis”);
- all four decay categories are listed: TauTau, ETau, MuTau, and e\mu.

At this point, the analysis is not yet documented as a fully rebuilt local workflow. The focus is understanding the signal physics and the set of observables likely to separate signal from backgrounds.

---

## 5.2 February-April 2025: first signal-variable studies and basic kinematics

### 02/02/2025 update (`updates/25_02_02.pdf`)

This update is one of the first concrete technical steps. It studies a simulated signal sample of about 23k events and documents:

- the trigger requirement in the early TauTau-like signal workflow;
- a proton selection requiring one proton per arm;
- event reduction from the starting sample to the selected sample;
- early definitions of tau-pair rapidity, proton rapidity, tau-pair invariant mass, and proton invariant mass;
- correlation plots between tau variables and proton-derived quantities.

Two concrete technical points appear here:

1. **proton selection was fixed**;
2. the invariant-mass calculation was changed to a `TLorentzVector`-based implementation.

This matters because it shows the project began with direct signal-kinematics validation before the mature background treatment and statistical framework existed.

### 10/04/2025 plot note (`documentation/plots.pdf`)

By April 2025, the early signal studies have become more explicit. The documents show:

- event reduction such as `23k -> 294` in one signal-processing path, and a separate count for no-pileup selections;
- proton selection based on one proton in each arm and choosing the largest `\xi` when multiple candidates exist;
- correlations expected from signal topology:
  - strong `\phi` correlation between the two taus;
  - negative correlation in tau pseudorapidities;
  - strong dependence of the system mass on tau `p_T`;
  - consistency between tau-system and proton-system mass and rapidity.

This period is still dominated by understanding **signal structure**, not by building the final background model.

---

## 5.3 Summer 2025: code recovery, cut reconstruction, and channel-specific workflows

### 07/08/2025: recovered code flow and old normalization conventions (`documentation/07_08.pdf`)

This document is the first strong code-and-analysis reconstruction note. It shows that the project was recovering and understanding a pre-existing workflow rather than writing the whole analysis from scratch.

Key content:

- the split into `fase0` and `fase1`;
- old background processing for DY, ttbar, and QCD;
- channel cuts for MuTau;
- early normalization conventions:
  - DY weight `1.81`;
  - ttbar weight `0.15`;
  - QCD from same-sign data.

At this stage, the workflow already distinguishes:

- `phase 0`: channel topology, trigger, basic variables;
- `phase 1`: tighter IDs, kinematics, sign selection, scale factors.

It also shows the analysis was still partly built around empirical or inherited scale factors rather than a fully documented luminosity-times-cross-section normalization.

### 21/08/2025: ETau as a recovery/control channel (`documentation/21_08.pdf`)

The ETau channel appears not as a final-statistics target but as a **control and code-recovery exercise**.

The documentation emphasizes:

- available ETau data files at different processing stages;
- branch content and the fact that many branches are stored as vectors;
- the difficulty of rebuilding a phase-1 analyzer without a clean reference;
- the use of ETau to understand how the data-processing chain should behave.

This is important for the project history because it shows that a significant part of 2025 was spent reconstructing missing processing steps, not only doing physics optimization.

### 28/08/2025: cross-channel cut recovery and BDT-input emphasis (`documentation/28_08.pdf`)

This document broadens the view again across channels. It summarizes recovered cut logic for TauTau and ETau and already points toward the variables later used in BDT training.

The channel story by this point is:

- TauTau: original entry point for signal-variable studies;
- ETau: control/recovery channel to understand the processing;
- MuTau: increasingly the most operational and mature channel.

---

## 5.4 September-October 2025: signal processing and the MC-vs-data proton-handling split

### 12/09/2025: signal processing from MINIAOD (`documentation/12_09.pdf`)

This is a major milestone because it describes the signal chain much more concretely.

The signal processing code (`sinal.cpp` / later `sinal.py`) is documented as doing the following:

- reading muon scale factors;
- loading proton acceptance corrections and systematic uncertainties;
- applying muon trigger, ID, ISO, and reconstruction corrections;
- applying muon/tau ID and kinematic cuts;
- normalizing signal events using BSM-weight branches;
- applying proton-related acceptance cuts and systematic variations.

Proton-related signal cuts are now detailed:

- require reconstructed protons;
- apply fiducial acceptance cuts based on `\xi` and `\theta_x`;
- apply period-dependent geometric cuts;
- apply radiation-damage efficiency corrections;
- compute `\xi` up/down systematics per arm.

This is a decisive change from the earlier signal-only variable studies. The signal workflow is now recognizably close to the final one.

### 16/10/2025: explicit processing change for real data vs simulated backgrounds (`documentation/16_10.pdf`)

This document explains one of the most important structural changes in the whole project.

Earlier approach:
- all samples were treated in the same way, and proton information was effectively reintroduced later.

Newer approach:
- **DY and ttbar** remain simulated samples, so they pass through central selection first and then get pileup protons mixed in later;
- **Data and QCD** already contain real proton information, so proton variables are saved from the first step instead of being lost.

This is a mature methodological improvement. It acknowledges that adding fake pileup protons to real data is conceptually wrong. It also explains why later notes emphasize preserving proton branches in data/QCD outputs.

The same October note also shows that by this stage the analysis was already thinking in terms of **BDT input variables**, not only raw kinematic plots.

---

## 5.5 Late 2025: debugging plots, normalization mismatches, and inherited scaling conventions

### 27/10/2025 and 29/10/2025: plot review stage (`documentation/27_10.pdf`, `documentation/29_10-questions regarding plots (1).pdf`)

These notes are less about new algorithms and more about visual validation:

- data vs background shapes;
- BDT input distributions;
- whether control-region and signal-like plots look physically reasonable;
- whether the selected variables show the expected signal/background behavior.

This stage suggests the project had enough working outputs to enter a consistency-check phase, but it was still not fully stabilized.

### 06/11/2025: explicit normalization concern (`documentation/06_11 (1).pdf`)

This is the clearest debugging note for background normalization before the final 2026 normalization cleanup.

The central issue documented is that **Data and QCD appeared too small** compared with expectations or previous plots. Several ad hoc tests were tried:

- fixed `w_dy = 1.81` and `w_ttjets = 0.15`;
- `w_data = 2`;
- `w_data = 2` and `w_qcd = 2`.

The fact that these tests were tried shows that, at this stage, the normalization model was still not under full control and plot agreement depended on empirical rescaling.

### General note on historical weighting (`documentation/general.pdf`)

The older “general” slide export is particularly useful because it records the inherited weight interpretation:

- `0.15` for background normalization in at least part of the workflow;
- `0.13` for the probability of having at least one proton per PPS arm;
- an additional unexplained `0.8` factor on background-only plots.

This is strong evidence that the late-2025 analysis still contained a mix of physically motivated factors and empirical inherited conventions.

---

## 5.6 End of 2025: project consolidation around MuTau

### Project status note (`project_status.md`, dated 18/12/2025)

By December 2025, the project is summarized as a three-channel analysis but with MuTau clearly the most advanced channel.

The note codifies:

- the phase structure of the MuTau workflow;
- the event counts at each stage;
- the strategy of saving proton variables directly in data samples;
- the core MuTau observables (`sist_mass`, `sist_pt`, `sist_rap`, `acop`, `met_pt`, `n_b_jet`, etc.);
- a clear awareness of the event-yield discrepancy between the original and proton-variable-preserving workflows.

This is the point where the project moves from recovery/debugging into a stable local description of the MuTau analysis chain.

---

## 5.7 January-February 2026: normalization bug fixing and proton-acceptance reinterpretation

### January 2026 status update (`presentation_meeting.md`)

This is a major turning point.

The note identifies and fixes a concrete bug in proton mixing:

- the original merging code overwrote the event weight with a flat value,
  
  ```python
  weight = 0.13
  ```
  
  which discarded `event_weight` from phase 1;
- the corrected logic becomes
  
  ```python
  weight = event_weight * 0.13
  ```

At the same time, proton acceptance is measured directly from the MuTau data sample:

- total selected data events: `6970`;
- events with protons on both arms: `1708`;
- resulting acceptance: `1708 / 6970 = 0.245`.

This creates a documented tension:

- old inherited value: `0.13`;
- directly measured value: `0.245`.

This is one of the most important physics/technical transitions in the analysis. It moves the proton-acceptance factor from an inherited constant to a quantity tested on the current data sample.

### February 2026 normalization note (`NORMALIZATION_DOCUMENTATION.md`)

The normalization note is the first document that rewrites the MC normalization in a physically explicit way:

$$
weight = \left(\frac{L\sigma}{\Sigma w_{gen}}\right) \times generator\_weight \times SF_\mu \times eff_{proton}.
$$

Key numerical content:

- luminosity `L = 54900 pb^-1`;
- DY cross section `6077.22 pb`;
- ttbar cross section `831.76 pb`;
- `\Sigma w_{gen}` extracted from the NanoAOD `Runs` tree;
- proton acceptance taken as `0.245`.

This produces the explicit scales:

- DY scale `1.004e-4`;
- ttbar scale `7.476e-5`.

The note also explains why the old `0.15` ttbar normalization happened to work approximately:

$$
7.476\times 10^{-5} \times \langle w_{gen}\rangle \approx 0.15.
$$

This is not just a documentation improvement. It is a methodological cleanup that replaces empirical normalization with a cross-section-based one.

---

## 5.8 March-April 2026: mature MuTau analysis, BDT training, and Combine

### 07/03/2026: MuTau BDT training and application (`slides_mutau.md`)

By March 2026, the MuTau channel is the clearly dominant analysis branch.

The documented pipeline is:

```text
NanoAOD -> Phase0 -> Phase1 -> Merge -> Proton Mixing -> Plots
```

with the statistical extension

```text
correlation studies -> TMVA training -> TMVA application -> BDT output plots.
```

Selection cuts are stabilized:

- muon ID `> 3`;
- tau DeepTau IDs `VSjet > 63`, `VSe > 7`, `VSmu > 1`;
- `p_T^\mu > 35 GeV`;
- `p_T^\tau > 100 GeV`;
- opposite sign for the signal region;
- same sign for the QCD control estimate;
- proton requirements on both arms.

The note also documents a transition from old to new weight handling:

Old approach:
- fill MC with a flat proton-acceptance weight;
- scale DY by `1.81`, ttbar by `0.15`;
- no proper generator-weight treatment.

New approach:
- use the stored `weight` branch carrying generator weight, muon SFs, and proton acceptance;
- scale DY with `1.004e-4`;
- keep ttbar scale effectively at `1.0` because the `0.15` factor is already embedded upstream.

This is the stage where the MuTau selection, normalization, and MVA feature set are all aligned.

### 02/04/2026: multi-method TMVA comparison (`slides_tmva_combine.md`)

This note broadens the MVA study from BDT alone to multiple methods:

- BDT;
- Likelihood;
- Fisher;
- in some configurations also MLP and PDERS.

The 9 main training variables are documented explicitly:

- `sist_rap`;
- `acop`;
- `sist_pt`;
- `mu_pt`;
- `tau_pt`;
- `sist_mass`;
- proton/central mass matching;
- `met_pt`;
- proton/central rapidity matching.

This is an important maturity indicator: the feature set is no longer exploratory; it is fixed enough to support classifier comparisons and downstream limits.

The same note still shows a low-sensitivity stage, with very large limits on `r`. This later becomes obsolete once the signal normalization and proton-selection bugs are fixed.

### 16/04/2026: signal-normalization and proton-selection fixes before final statistical results (`slides_combine.md`)

This is the single most important mature-analysis note.

It documents two major bug fixes.

#### Bug fix 1: signal proton selection

Old behavior:
- the signal code required exactly two proton entries in total;
- this discarded events with more than two proton entries, even if they still had valid protons in both arms.

Corrected behavior:
- require at least one valid proton in arm 0 and at least one in arm 1.

Documented effect:
- unweighted selected signal events increase from `14085` to `21398`;
- weighted signal yield increases from `0.0633` to `0.0794`.

This is a real selection-efficiency correction, not a cosmetic change.

#### Bug fix 2: signal cross-section normalization

Old behavior:

$$
 w_{sample} = \frac{54900\times w_{SM}[0]}{4000\times 1000},
$$

with a denominator of four million that the later note states has no physical basis.

Corrected behavior:

$$
 w_{sample} = \frac{54900\times 0.0047 \times w_{SM}[0]}{402.661}.
$$

Inputs used:
- total upstream signal events: `299979`;
- sum of SM weights: `402.661`;
- SM cross section from reweight index 51: `0.0047 pb = 4.7 fb`.

This fix completely changes the interpretation of the expected sensitivity. The earlier huge values of `r` are explicitly identified as artifacts of the wrong normalization.

#### BDT and Combine results after the fixes

The corrected BDT scan identifies a best configuration around AdaBoost with `NTrees=600`, `MaxDepth=2`.

Documented post-fix sensitivity:
- expected median limit near `r < 2.04`;
- observed limit near `r < 6.03`;
- observed significance about `2.41 sigma`;
- expected significance around `1.62 sigma`;
- best-fit signal strength around `r = 2.13^{+1.92}_{-1.17}`;
- goodness-of-fit p-value around `0.51`.

This is the final physically meaningful result presented in the documentation set.

### 25/04/2026: full signal-processing note (`slides_weights.md`)

This note gives the most detailed step-by-step description of the signal code itself.

It documents the full `sinal.py` logic:

1. normalize every event using the SM reweight and total cross section;
2. require non-empty muon and tau collections;
3. apply baseline muon/tau ID, eta, and `\Delta R` cuts;
4. compute and apply muon trigger, ID+ISO, and reconstruction scale factors;
5. apply `p_T` cuts;
6. apply opposite-sign selection;
7. require at least one proton per arm;
8. apply `\theta_x` fiducial cuts;
9. apply era-dependent track-position fiducial cuts;
10. apply radiation-damage and multi-RP efficiency corrections;
11. store final event weights, systematic branches, and BSM-weight arrays.

This note is effectively the mature reference for how the signal sample is produced.

---

## 6. Topic-by-topic synthesis of the analysis evolution

## 6.1 Channel evolution

### TauTau

TauTau is the earliest visible working context in the documentation.

What it contributes:
- earliest signal-variable studies;
- first proton-pair and tau-pair correlations;
- proof-of-principle kinematic and proton matching;
- earliest trigger/cut validation.

What it does not provide in the final documentation:
- a mature final statistical result comparable to the later MuTau result.

### ETau

ETau functions mainly as a control/recovery channel.

What it contributes:
- understanding data structures and branch content;
- reconstructing how an analyzer should be built when reference outputs are missing;
- a cross-check that the phase structure is channel-general.

### MuTau

MuTau is the mature final channel.

What it contributes:
- fully documented phase-0 and phase-1 cuts;
- documented MC normalization logic;
- explicit proton-mixing strategy;
- signal processing with detailed PPS/systematics handling;
- TMVA training and application;
- Combine statistical interpretation.

The documentation set strongly indicates that MuTau became the main physics-delivery channel because it reached the most complete and internally consistent state.

---

## 6.2 Evolution of proton handling

This is the most important technical theme across the documentation.

### Initial stage

Early studies already used a one-proton-per-arm logic, usually selecting the largest `\xi` value in each arm. This was appropriate for signal studies but did not yet separate the treatment of MC and real data.

### Intermediate stage

A common processing path for all samples led to conceptual and technical problems:
- real data proton information could be lost and then effectively reintroduced incorrectly;
- proton multiplicity and branch-format issues created inconsistencies;
- some samples were enriched with pileup protons in a way that obscured the original event content.

### Mature stage

The documentation converges to a cleaner split:

- **Data and QCD:** keep real proton variables from the start;
- **DY and ttbar:** add pileup protons later using a proton pool extracted from data.

This is combined with two further refinements:

1. acceptance weighting for the fact that not every event has protons on both arms;
2. signal-specific PPS fiducial, geometric, and radiation-damage corrections.

### Final signal-side proton logic

The mature signal code requires:
- at least one proton in arm 0;
- at least one proton in arm 1;
- kinematic and fiducial acceptance in `\xi` and `\theta_x`;
- track-position acceptance in the RP sensors;
- run-era efficiency corrections.

This is the most detector-specific part of the analysis.

---

## 6.3 Evolution of event weights and normalization

This is the second major long-term theme.

### Historical weighting

Older notes show a mixed, inherited convention:
- DY normalized with `1.81`;
- ttbar normalized with `0.15`;
- proton probability represented by `0.13`;
- in some cases a further unexplained `0.8` factor on background.

This was enough to make plots but not enough to provide a clean physics interpretation.

### First major correction: preserve `event_weight` in proton mixing

The January 2026 note identifies that the proton-merging stage had overwritten the upstream event weight. That meant generator weights and muon SFs were being discarded. Fixing this was essential.

### Second major correction: measure proton acceptance on the current data sample

The shift from `0.13` to a measured `0.245` changes the effective normalization of MC samples with pileup-proton mixing.

### Third major correction: explicit cross-section normalization

The February 2026 note finally expresses normalization as:

$$
\frac{L\sigma}{\Sigma w_{gen}} \times generator\_weight \times SF_\mu \times eff_{proton}.
$$

This is the point at which the background normalization becomes physics-driven instead of purely inherited.

### Fourth major correction: signal normalization

The April 2026 note fixes the signal sample normalization using the actual total SM reweight sum and the correct SM cross section of `4.7 fb`.

This correction is what makes the final MuTau statistical result interpretable.

---

## 6.4 Signal processing details

The detailed signal note shows that the signal chain is not just a simplified version of the background chain. It is structurally richer.

### Inputs

The signal ntuple includes:
- muon and tau reconstructed variables;
- proton branches;
- 102 SM/BSM weights.

### Corrections applied

The signal receives:
- muon trigger SFs;
- muon ID+ISO SFs;
- muon reconstruction SFs;
- proton `\xi` systematics;
- PPS geometric acceptance;
- PPS radiation-damage efficiency;
- per-era RP efficiency handling.

### Physics meaning

This matters because the signal is the only sample where the analysis attempts to model both:
- the central-object selection efficiency;
- the detector response and acceptance of the proton spectrometer at a detailed level.

That is why the later signal normalization and proton-selection fixes have such large downstream impact.

---

## 6.5 BDT/TMVA development

The BDT feature set evolves naturally from the early signal-variable studies.

### Early stage

The project first studies whether variables such as system mass, rapidity, acoplanarity, and proton-derived quantities show the expected exclusive-like structure.

### Mature training set

By 2026, the final training inputs are stable and physically motivated:

- central-system mass, rapidity, and `p_T`;
- acoplanarity;
- muon and tau `p_T`;
- missing energy;
- mass matching between central and PPS systems;
- rapidity matching between central and PPS systems.

### Why these variables are sensible

- signal-like exclusive events should be cleaner, more balanced, and more self-consistent between the central detector and PPS;
- ordinary backgrounds can imitate some pieces of the topology but usually not all correlations at once;
- the mass- and rapidity-matching variables directly exploit the proton tag, which is the distinctive asset of this analysis.

### Method choice

Although several TMVA methods are tested, the later documents clearly favor the BDT as the main discriminator.

---

## 6.6 Statistical interpretation with Combine

The statistical workflow is documented in both slides and operational instructions.

### Shapes

`save_shapes.cpp` creates histograms for:
- data;
- signal;
- DY;
- ttbar;
- QCD.

These can be either mass histograms or MVA-score histograms.

### Datacards and workspace

The standard chain is:

1. build the shape ROOT file;
2. create a text datacard;
3. convert with `text2workspace.py`;
4. run Combine.

### Quantities extracted

The instructions and result files document the following outputs:
- asymptotic 95% CL upper limits;
- observed and expected significance;
- fit diagnostics;
- best-fit signal strength with uncertainties;
- goodness-of-fit with toys;
- nuisance impacts.

### Interpretation of the final MuTau result

The corrected April 2026 result is the only one in the documentation that is consistent with the cleaned-up normalization and corrected signal acceptance. It should therefore be treated as the current best documented result.

---

## 7. Best current reconstruction of the mature MuTau workflow

Combining the later documentation, the best current step-by-step picture of the MuTau analysis is:

### Step 1: phase 0 skimming

- select one muon and one tau candidate;
- require the appropriate single-muon trigger;
- save central-object, jet, MET, and, for real data, proton variables.

### Step 2: phase 1 event selection

Apply:
- muon ID;
- DeepTau discriminants;
- `p_T^\mu > 35 GeV`;
- `p_T^\tau > 100 GeV`;
- opposite sign for signal, same sign for QCD estimate;
- `\Delta R > 0.4`;
- `|\eta| < 2.4`.

### Step 3: apply or preserve proton information

- for Data/QCD: keep real proton branches;
- for DY/ttbar: merge in protons from the proton pool.

### Step 4: assign event weights

- Data: weight 1;
- QCD: data-driven treatment;
- DY/ttbar: generator weight or equivalent normalization factor, muon SFs, proton acceptance;
- Signal: SM normalization weight, muon SFs, PPS efficiency and radiation-damage factors.

### Step 5: build proton-tagged observables

Use:
- proton `\xi` per arm;
- proton-derived mass and rapidity;
- central-proton matching terms.

### Step 6: validate distributions

Compare data, DY, ttbar, QCD, and signal in:
- mass;
- rapidity;
- system `p_T`;
- tau `p_T`;
- MET;
- acoplanarity;
- matching variables.

### Step 7: train the BDT

Train signal vs background using the 9-variable set dominated by central kinematics plus central/PPS consistency variables.

### Step 8: build shapes and run Combine

Use either mass shapes or BDT shapes, then extract:
- expected/observed limits on `r = \sigma/\sigma_SM`;
- significance;
- best-fit `r`;
- goodness-of-fit;
- nuisance impacts.

---

## 8. Final documented analysis status

From the full documentation set, the final state of the analysis can be summarized as follows.

### What is mature

- the MuTau central-object selection;
- the split between real-proton data and mixed-proton MC;
- the detailed signal processing chain;
- the physics-based MC normalization model;
- the BDT feature set;
- the Combine workflow;
- the corrected MuTau statistical result.

### What was historically unstable but later improved

- proton acceptance treatment;
- preservation of `event_weight` in proton mixing;
- signal proton-selection logic;
- signal cross-section normalization;
- consistency between inherited scale factors and physics normalization.

### What remains visibly less mature in the documentation

- a comparably final statistical result for ETau or TauTau;
- a completely uniform normalization convention across every historical stage;
- full reconciliation of all old plots with the final corrected normalization;
- complete explanation of every historical empirical factor used in older versions.

---

## 9. Current best-physics conclusion from the documentation

The documentation shows a real progression from exploratory signal studies to a physically interpretable MuTau analysis.

The mature conclusion is not that the analysis discovered a signal. The mature conclusion is:

- the MuTau workflow was progressively corrected in proton handling and normalization;
- after those corrections, the analysis achieved sensitivity at the level of a few times the SM prediction rather than thousands of times the SM prediction;
- the best documented result is a mild excess but still statistically limited;
- the major analysis value lies in combining central tau kinematics with PPS proton matching, which is the central discriminating idea throughout the entire project.

---

## 10. Practical reading order for the documentation, if the goal is to understand the analysis itself

If the goal is to understand the full analysis progression rather than the repository structure, the most useful reading order is:

1. `documentation/05_02_25.pdf` for the physics motivation;
2. `updates/25_02_02.pdf` and `documentation/plots.pdf` for the earliest signal-variable logic;
3. `documentation/07_08.pdf`, `documentation/12_09.pdf`, and `documentation/16_10.pdf` for code recovery and the proton-handling transition;
4. `documentation/06_11 (1).pdf` and `project_status.md` for the late-2025 debugging state;
5. `documentation/presentation_meeting.md` and `documentation/NORMALIZATION_DOCUMENTATION.md` for the normalization cleanup;
6. `MuTau_channel/slides/slides_mutau.md` for the stable MuTau workflow;
7. `MuTau_channel/slides/slides_weights.md` for the full signal-processing chain;
8. `MuTau_channel/slides/slides_combine.md` plus `combine_results_mu_tau.rtf` for the corrected final statistical interpretation.

---

## 11. Bottom line

The documentation does not describe a static analysis. It records an analysis that changed significantly over time.

The progression is:

- broad exclusive-physics motivation;
- early signal-kinematics studies;
- recovery of missing code structure and cuts;
- separation of MC and real-data proton treatments;
- debugging of yields and plot normalization;
- explicit cross-section-based MC normalization;
- correction of signal proton selection and signal normalization;
- final BDT+Combine MuTau result with physically meaningful sensitivity.

That is the clearest full reconstruction of the analysis progress supported by the available slides and documentation.
