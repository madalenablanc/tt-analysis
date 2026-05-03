# Full Report on `documentation/`

Date: 2026-04-27

## Scope

This report is a full documentation-only review of the repository folder:

`/Users/utilizador/cernbox/tau_analysis/documentation`

It is intentionally different from the earlier repository/code report. The goal here is not to explain the source code directly, but to explain what the documentation corpus says, how it evolved over time, what is duplicated, what is current, and what scientific and technical story it tells.

This report covers every file currently present in `documentation/`:

- `01_08_2025.pdf`
- `05_02_25.pdf`
- `06_11 (1).pdf`
- `07_08.pdf`
- `12_09.pdf`
- `16_10.pdf`
- `21_08.pdf`
- `27_10.pdf`
- `28_08.pdf`
- `29_10-questions regarding plots (1).pdf`
- `Cópia de apresentacao.pdf`
- `NORMALIZATION_DOCUMENTATION.md`
- `NORMALIZATION_DOCUMENTATION.pdf`
- `apresentacao.pdf`
- `first-stage-analysis.pdf`
- `general.pdf`
- `plots.pdf`
- `presentation_meeting.md`
- `presentation_meeting.pdf`
- `presentation_mutau_weights.md`
- `presentation_mutau_weights.pdf`
- `presentation_mutau_weights_slides.pdf`
- `presentation_weights.pdf`
- `slides_combine.md`
- `slides_combine_26_03.pdf`
- `slides_mutau.md`
- `slides_mutau.pdf`
- `slides_mutau_05_02.pdf`
- `slides_mutau_07_03.pdf`
- `slides_mutau_26_02.pdf`

Important note:

- several PDFs are exports of Markdown files or near-duplicate slide variants;
- several earlier dated PDFs are working slide decks, not polished technical notes;
- some older PDFs are clearly historical snapshots and sometimes contain inconsistent or superseded assumptions.

## High-level conclusion

The `documentation/` folder is not a single coherent manual. It is a layered record of the analysis development. It contains four major kinds of material:

1. physics motivation and broad project framing;
2. codeflow and early workflow design;
3. plot-validation and debugging presentations;
4. mature MuTau analysis notes on normalization, proton mixing, BDT/TMVA, and Combine.

The overall documentation story is:

- the project begins from a broad exclusive tau-physics motivation, especially anomalous tau couplings and the tau magnetic moment;
- early work is centered on understanding signal variables and correlations, initially with strong TauTau influence;
- the middle phase is dominated by reconstruction, selection, plotting, and normalization debugging, especially proton-enrichment and event-count consistency;
- the most mature documented branch is the MuTau channel, for which the folder now contains explicit notes on normalization, proton mixing, event weights, BDT inputs, and statistical inference with Combine.

The folder therefore works best if read as a historical and technical archive rather than as a polished single-source manual.

## The main documentation themes

Across all files, the dominant themes are:

- exclusive or proton-tagged tau-pair production at the LHC;
- sensitivity to anomalous tau couplings and the tau magnetic moment;
- central exclusive production with PPS-tagged protons;
- channel-by-channel event processing;
- tau and muon kinematic and ID cuts;
- proton-matching observables:
  - central-system mass;
  - proton-pair mass;
  - central rapidity;
  - proton rapidity;
  - mass difference between central and proton-based systems;
  - rapidity matching;
- pileup-proton enrichment for MC;
- proton acceptance weights;
- data-driven QCD handling;
- multivariate discrimination with BDT/TMVA;
- Combine-based upper limits.

## Chronological narrative of the documentation corpus

### Earliest broad scientific framing

The older presentation-style files such as:

- `apresentacao.pdf`
- `Cópia de apresentacao.pdf`
- `05_02_25.pdf`
- parts of `first-stage-analysis.pdf`

present the project as a new-physics search through proton interactions at the LHC. Their main emphasis is not workflow mechanics but physics motivation:

- why proton-proton collisions at the LHC are a useful environment;
- why exclusive processes are interesting;
- why taus are attractive probes of Beyond Standard Model effects;
- why anomalous tau magnetic moment studies are scientifically motivated.

These documents are useful because they show that the project was not initially framed only as a technical reconstruction/selection exercise. It was framed as a targeted physics program: using exclusive tau production and forward proton tagging to probe deviations from the Standard Model.

### Signal-variable and correlation phase

Files such as:

- `05_02_25.pdf`
- `first-stage-analysis.pdf`
- `apresentacao.pdf`
- `plots.pdf`
- `updates/25_02_02.pdf` in the separate `updates/` folder

show a strong early focus on understanding the expected signal:

- central-system transverse momentum;
- tau-pair rapidity;
- proton energy loss `xi`;
- proton-pair rapidity;
- tau-pair invariant mass;
- proton-pair invariant mass;
- central-proton matching variables;
- variable correlations.

This phase is important because it defines the analysis language later reused in the MuTau BDT and Combine documentation. The variables that eventually become BDT inputs and shape-analysis observables are already identified here as physically meaningful discriminants.

### Workflow and code-organization phase

Files such as:

- `07_08.pdf`
- `21_08.pdf`
- `28_08.pdf`

document a period where the main problem was understanding the recovered samples and the processing chain rather than just the physics. These files describe:

- what phase 0 computes and stores;
- how phase 1 is organized;
- which text lists feed which analyzers;
- what is different between DY, ttbar, QCD, and Data;
- what branch layouts are available in the recovered ETau/TauTau/MuTau files;
- how event counts compare between channels and stages.

These notes are especially valuable because they show how much of the analysis effort was about reconstructing or standardizing an inherited workflow, not only developing a new one from scratch.

### Debugging and plot-validation phase

Files such as:

- `01_08_2025.pdf`
- `06_11 (1).pdf`
- `29_10-questions regarding plots (1).pdf`
- `plots.pdf`
- `27_10.pdf`

are working review decks. Their purpose is to discuss problems such as:

- control-region definitions;
- unexpected data/QCD normalization behavior;
- mismatches between cut definitions and plot ranges;
- why data and QCD look smaller than expected;
- whether a fixed normalization factor was accidentally masking a deeper issue;
- questions about which sample versions or datasets should be trusted.

These are not polished analysis notes, but they are important because they expose the actual practical concerns that shaped the final workflow.

### Mature MuTau note phase

The most mature material in the folder is:

- `NORMALIZATION_DOCUMENTATION.md`
- `presentation_meeting.md`
- `presentation_mutau_weights.md`
- `slides_mutau.md`
- `slides_combine.md`

and their PDF counterparts.

This cluster turns the analysis into a readable, channel-specific program with:

- clearly stated samples;
- clearly stated cuts;
- explicit event-weight formulas;
- explicit proton-mixing logic;
- explicit proton-acceptance measurement;
- explicit cross-section normalization constants;
- explicit BDT input variables;
- explicit Combine datacard and limit-setting procedure.

This is the point where the documentation becomes close to a technical analysis note rather than a weekly meeting archive.

## Detailed file-by-file review

### 1. `documentation/01_08_2025.pdf`

Type:

- historical slide deck
- control-region review

Main content:

- defines control regions in the hadronic tau-tau analysis;
- shows CR-specific cut boxes for:
  - DY;
  - QCD;
  - ttjets;
- records the actual numerical CR choices:
  - DY: invariant mass in a lower window, low acoplanarity, zero b-jets, opposite sign;
  - QCD: same-sign, high acoplanarity, low tau-pair pT;
  - ttjets: opposite-sign, at least one b-jet, restricted system pT.

Documentation value:

- one of the clearest early records of control-region logic;
- useful for understanding how background validation was intended to work in TauTau-like studies;
- more analysis-design oriented than code-oriented.

Limitations:

- not a complete note;
- more of a meeting deck;
- not tied to the final MuTau statistical workflow.

### 2. `documentation/05_02_25.pdf`

Type:

- historical physics motivation presentation

Main content:

- frames the project as an anomalous tau magnetic moment / anomalous tau coupling study;
- explains why `γγ → ττ` in central exclusive production is relevant;
- lists the four decay-mode program:
  - fully hadronic;
  - `e + τh`;
  - `μ + τh`;
  - `e + μ`;
- references an existing benchmark analysis by Matteo;
- identifies variables of interest:
  - tau-system pT;
  - rapidity;
  - acoplanarity;
  - proton energy loss;
  - central-PPS matching observables;
- includes early signal plots and correlation plots;
- states obvious next steps at the time:
  - use more events;
  - add QCD.

Documentation value:

- strongest physics-motivation document in the folder;
- best single file for answering “what is the scientific point of this project?”

Limitations:

- not yet a mature technical analysis note;
- mostly signal-centric and conceptual.

### 3. `documentation/06_11 (1).pdf`

Type:

- historical debugging / normalization slide deck

Main content:

- explicitly states a problem:
  data and QCD samples look smaller than they should;
- tests several ad hoc fixes:
  - `w_dy = 1.81`
  - `w_ttjets = 0.15`
  - `w_data = 2`
  - `w_qcd = 2`
- compares resulting BDT-input plots under those modified weight choices.

Documentation value:

- direct evidence that normalization and relative sample scaling were a major late-2025 problem;
- important historically because it shows the period before the later 2026 documentation standardized the MuTau normalization narrative.

Limitations:

- problem-driven, not explanatory;
- contains temporary or ad hoc fixes rather than final recommendations.

### 4. `documentation/07_08.pdf`

Type:

- historical workflow and code-organization note

Main content:

- explains phase-0 responsibilities:
  - tau-ID variables;
  - tau momenta;
  - central-system variables;
  - jets;
  - MET;
  - generator-level information;
- states the trigger used in that analysis branch;
- gives a table of file organization:
  - `DY_fase0`
  - `QCD_fase0`
  - `ttJets_fase0`
  - `nanotry_fase1_DY`
  - `nanotry_fase1_QCD`
  - `nanotry_fase1_ttJets`
- gives a feature table for backgrounds showing:
  - sample type;
  - lumi filtering;
  - gen matching;
  - generator-weight constants like `1.81` and `0.15`;
  - HLT versions;
- includes a MuTau section with cuts and no-pileup ttjets examples;
- includes the note that pileup protons had not yet been introduced in a given phase-1 context.

Documentation value:

- one of the best historical records of how the older workflow was organized;
- useful for relating many legacy files in the repository to the intended processing stages;
- exposes the older normalization convention very explicitly.

Limitations:

- partially TauTau-oriented, partially MuTau-oriented;
- represents an older processing design, not the final MuTau documentation state.

### 5. `documentation/12_09.pdf`

Type:

- historical checkpoint deck

Main content:

- compares event counts in MuTau;
- discusses signal processing starting from MINIAOD;
- explains that the signal workflow:
  - reads muon scale factors;
  - loads proton acceptance corrections and systematics;
  - applies muon/tau ID and eta cuts;
  - uses BSM weights;
- lists proton-related cuts after signal processing:
  - two reconstructed protons;
  - xi and theta_x fiducial acceptance cuts;
  - period-dependent geometric cuts;
  - radiation-damage corrections;
  - xi up/down systematic shifts;
- includes data plots after adding pileup protons and signal plots from `sinal.cpp`.

Documentation value:

- strong bridge between the early workflow and the later `sinal.py`/`slides_weights` documentation;
- useful for seeing how advanced the signal-processing logic already was by September 2025.

Limitations:

- still a slide deck rather than a linear note;
- emphasizes visual results more than detailed derivations.

### 6. `documentation/16_10.pdf`

Type:

- historical transition note

Main content:

- restates original per-channel cuts;
- explains signal processing from MINIAOD;
- gives a compact list of proton-related cuts and corrections:
  - two reconstructed protons;
  - fiducial acceptance;
  - period-dependent geometry;
  - radiation-damage weights;
  - xi systematics;
- most importantly, explains what changed in the processing:
  - DY and ttbar stay on the simulated route and receive pileup protons later;
  - Data and QCD keep proton-related variables from the beginning instead of losing them.

Documentation value:

- probably the best historical explanation for the shift from the older common treatment of all samples to the later “preserve proton variables for real data” strategy;
- directly relevant to why the repository now contains both old and new conventions.

Limitations:

- concise and partially visual;
- does not fully document the downstream statistical workflow.

### 7. `documentation/21_08.pdf`

Type:

- historical ETau/TauTau recovery and control note

Main content:

- shows a dataflow table for background samples;
- includes an ETau channel section used as a control to understand data-sample processing;
- lists available ETau files;
- documents the branch content of the ETau merged data file in detail;
- notes that the branches are stored as vectors and that this complicates building a phase-1 analyzer;
- states a clear practical objective:
  recreate the phase-1 analyzer to reproduce the existing results, especially because equivalent MuTau phase-1 outputs were not available.

Documentation value:

- very useful for understanding why ETau appears in the repository partly as a control/reconstruction channel rather than only as a primary physics channel;
- explains part of the repository heterogeneity.

Limitations:

- less about final physics;
- more about sample recovery and technical reconstruction.

### 8. `documentation/27_10.pdf`

Type:

- historical plot-review deck

Main content:

- focuses on MuTau-oriented BDT-input plots, data plots, and signal plots;
- reiterates the cuts used in the signal plots:
  - `mu pT > 35`;
  - `tau pT > 100`;
  - `eta < 2.4`;
  - opposite sign;
  - at least one proton per PPS arm.

Documentation value:

- useful as a visual progress checkpoint;
- shows what was being checked after more of the MuTau chain existed.

Limitations:

- contains little explanatory material beyond the visuals.

### 9. `documentation/28_08.pdf`

Type:

- historical cross-channel comparison deck

Main content:

- compares original Matteo-code cuts per channel:
  - TauTau;
  - ETau;
  - MuTau;
  - EMu;
- includes surviving-event counts per phase in recovered samples;
- notes that MuTau had too many events and needed code checks;
- mentions ETau and TauTau available sample sets.

Documentation value:

- best file for quick cross-channel comparison in the older analysis phase;
- useful if the question is “how did the recovered sample counts compare across channels?”

Limitations:

- not a final reference;
- mostly counts-and-cuts comparison.

### 10. `documentation/29_10-questions regarding plots (1).pdf`

Type:

- historical review/questions deck

Main content:

- essentially a plot-review package;
- contains MuTau BDT-input slides, data plot slides, and signal plot slides;
- framed as “questions regarding plots,” so it belongs to the internal review/debug stage rather than formal documentation.

Documentation value:

- shows which plots were under scrutiny;
- useful for internal analysis-history context.

Limitations:

- little new algorithmic or physics content compared with other MuTau slide decks.

### 11. `documentation/Cópia de apresentacao.pdf`

Type:

- historical broad project presentation

Main content:

- a shortened broad physics presentation on proton interactions at the LHC;
- discusses anomalous tau couplings;
- highlights precision tests, sensitivity to BSM, and forward proton tagging;
- closes with generic “analysis improvements” and “few plots” placeholders.

Documentation value:

- useful mainly as an early or alternate presentation version.

Limitations:

- overlaps heavily with `apresentacao.pdf` and the broad-intro portion of `first-stage-analysis.pdf`.

### 12. `documentation/NORMALIZATION_DOCUMENTATION.md`

Type:

- current mature technical note

Main content:

- the MuTau MC normalization prescription;
- integrated luminosity and sample cross sections;
- generator-weight sums;
- scale-factor formulas;
- proton-acceptance factor `0.245`;
- phase-0 / phase-1 / merge chain;
- explicit difference between DY and ttbar treatment;
- plotting-stage scaling choices.

Documentation value:

- one of the strongest files in the folder;
- the most explicit answer to “how should MuTau MC be normalized?”

Limitations:

- channel-specific;
- still inherits the awkward asymmetry that ttbar has `0.15` included upstream.

### 13. `documentation/NORMALIZATION_DOCUMENTATION.pdf`

Type:

- PDF export of the normalization note

Main content:

- essentially the same as the Markdown file.

Documentation value:

- useful as a shareable frozen version.

Limitations:

- duplicate in content.

### 14. `documentation/apresentacao.pdf`

Type:

- historical broad introductory presentation

Main content:

- “Exploring new physics with proton interactions at the LHC”;
- work done so far;
- broad case for proton-proton collisions as a discovery environment;
- future work emphasizing anomalous tau magnetic moment motivation.

Documentation value:

- a compact early summary of project motivation;
- less specific than `05_02_25.pdf`.

Limitations:

- very high-level;
- not a technical note.

### 15. `documentation/first-stage-analysis.pdf`

Type:

- large historical slide notebook

Main content:

- broad motivation slides;
- anomalous tau magnetic moment framing;
- central exclusive tau-pair production setup;
- variables of interest;
- early signal plots;
- several versions of correlation studies;
- later weekly-meeting material embedded in the same file;
- signs of iterative deck merging or aggregation.

Documentation value:

- large historical archive of the first analysis stage;
- useful for reconstructing the conceptual birth of the variable set and the early exclusive-physics story.

Limitations:

- very long;
- repetitive;
- not cleanly structured as a single document.

### 16. `documentation/general.pdf`

Type:

- mixed summary deck

Main content:

- variable and distribution slides;
- expected correlations;
- BDT-input variable explanations;
- weight comments such as:
  - `.15` for MC normalization;
  - `.13` as PPS two-arm probability;
  - unresolved concern about an extra `0.8` factor;
- codeflow and phase descriptions;
- control-region observations and plotting concerns.

Documentation value:

- useful because it compresses several strands:
  variables, correlations, weights, codeflow, and plot issues;
- one of the best files for understanding the messy intermediate stage of the project.

Limitations:

- not fully consistent internally;
- combines historical assumptions and diagnostic comments.

### 17. `documentation/plots.pdf`

Type:

- plot and debugging review deck

Main content:

- variable and distribution summaries;
- expected correlations;
- BDT-input variable explanations;
- a direct statement of old weighting assumptions;
- comments on control-region shape mismatches and range inconsistencies;
- historical note about a fixed background-only factor with unclear meaning.

Documentation value:

- probably the strongest plot-debugging and “what looked suspicious?” document in the folder;
- useful for understanding why later MuTau normalization notes became more formal.

Limitations:

- diagnostic rather than prescriptive;
- some assumptions documented here are later superseded.

### 18. `documentation/presentation_meeting.md`

Type:

- current short status note

Main content:

- normalization issue in proton-mixed MC;
- the bug in proton mixing;
- measurement of proton acceptance from data;
- comparison between `0.13` and `0.245`;
- immediate next steps and open questions.

Documentation value:

- important because it gives the clearest short explanation of why the old MuTau weighting was wrong and how it was fixed.

Limitations:

- short meeting note, not a full analysis manual.

### 19. `documentation/presentation_meeting.pdf`

Type:

- PDF export of the meeting note

Content:

- duplicate of the Markdown note.

### 20. `documentation/presentation_mutau_weights.md`

Type:

- current mature workflow note

Main content:

- MuTau analysis goal;
- processing pipeline from NanoAOD to plots;
- phase-0 and phase-1 definitions;
- exact phase-1 cuts;
- event weights for data, DY, ttbar, QCD;
- proton-pool explanation;
- proton-mixing algorithm;
- proton acceptance definition and measurement from data.

Documentation value:

- together with the normalization note, this is one of the two core MuTau workflow documents.

Limitations:

- still assumes some repository context;
- not a complete code reference.

### 21. `documentation/presentation_mutau_weights.pdf`

Type:

- PDF export of the workflow note

Content:

- duplicate of the Markdown note.

### 22. `documentation/presentation_mutau_weights_slides.pdf`

Type:

- slide-form export of the workflow note

Main content:

- essentially the same MuTau weight/proton-mixing content in beamer/slides format.

Documentation value:

- better for presentations than for reading.

### 23. `documentation/presentation_weights.pdf`

Type:

- shorter weight-focused presentation export

Main content:

- a presentation-format version of the MuTau weight story.

Documentation value:

- concise briefing deck.

### 24. `documentation/slides_combine.md`

Type:

- current mature statistical-analysis note

Main content:

- purpose of the Combine step;
- shape production from `save_shapes.cpp`;
- datacard structure;
- nuisance parameters and `rateParam`s;
- asymptotic-limit commands;
- limit plotting;
- actual quoted 95% CL upper-limit values.

Documentation value:

- the clearest explanation in the folder of how the analysis turns event selections into a statistical result.

Limitations:

- only covers the MuTau mass-shape path.

### 25. `documentation/slides_combine_26_03.pdf`

Type:

- PDF export of the Combine slide note

Content:

- duplicate in substance of the Markdown file.

### 26. `documentation/slides_mutau.md`

Type:

- current MuTau BDT and weight-summary slide note

Main content:

- MuTau processing pipeline;
- phase-1 cuts;
- old and new weight logic;
- proton requirements for data/QCD and MC;
- post-proton-merging kinematic plots;
- visual explanation of corrected weighting.

Documentation value:

- good compact overview of MuTau analysis flow with emphasis on what changed in the weighting treatment.

### 27. `documentation/slides_mutau.pdf`

Type:

- PDF export of the MuTau slide note

Content:

- duplicate of the slide Markdown content.

### 28. `documentation/slides_mutau_05_02.pdf`

Type:

- longer slide variant of the MuTau proton-mixing and weight-normalization note

Main content:

- same overall content as `presentation_mutau_weights.md` and `slides_mutau.md`, but in a longer presentation form;
- useful as a dated snapshot from 05 February 2026.

Documentation value:

- helpful if one wants to know what was being presented at that exact stage of the MuTau analysis.

### 29. `documentation/slides_mutau_07_03.pdf`

Type:

- slide export for the BDT training/application note

Main content:

- same overall topic as `slides_mutau.md`, with the emphasis shifted toward BDT training and application.

Documentation value:

- useful date-stamped presentation version of the MuTau BDT workflow.

### 30. `documentation/slides_mutau_26_02.pdf`

Type:

- slide export for the weight-normalization note

Main content:

- processing pipeline;
- phase-1 cuts;
- old weighting path:
  - proton factor `0.13`;
  - DY scale `1.81`;
  - ttbar factor `0.15`;
- notes that this should eventually be made consistent.

Documentation value:

- important historical record of the intermediate normalization convention before the later `1.004e-4`-style DY scaling note was formalized.

## Duplicate and near-duplicate structure

The folder contains several clusters of duplicate or near-duplicate content.

### Cluster A: broad physics intro

- `apresentacao.pdf`
- `Cópia de apresentacao.pdf`
- parts of `first-stage-analysis.pdf`

Shared content:

- proton interactions at the LHC;
- new-physics motivation;
- anomalous tau couplings;
- high-level “future work”.

### Cluster B: MuTau proton mixing and weights

- `presentation_mutau_weights.md`
- `presentation_mutau_weights.pdf`
- `presentation_mutau_weights_slides.pdf`
- `slides_mutau.md`
- `slides_mutau.pdf`
- `slides_mutau_05_02.pdf`
- `slides_mutau_26_02.pdf`
- `presentation_weights.pdf`

Shared content:

- MuTau processing pipeline;
- phase-1 cuts;
- old and new weight logic;
- proton-pool explanation;
- acceptance discussion.

Difference inside the cluster:

- some are short notes;
- some are beamer slide exports;
- some are date-specific snapshots;
- some retain the older `0.13 + 1.81` convention more explicitly than the later corrected notes.

### Cluster C: MuTau normalization

- `NORMALIZATION_DOCUMENTATION.md`
- `NORMALIZATION_DOCUMENTATION.pdf`

Shared content:

- the finalized MuTau normalization logic;
- generator-weight sums;
- explicit DY scaling formula;
- `eff_proton = 0.245`.

### Cluster D: MuTau statistical interpretation

- `slides_combine.md`
- `slides_combine_26_03.pdf`

Shared content:

- shape building;
- datacard;
- nuisance model;
- Combine commands;
- limit extraction.

## Scientific content extracted from the folder

If the documentation corpus is read as a single scientific narrative, it says the following.

### Physics target

The analysis is motivated as a study of exclusive or proton-tagged tau-pair production at the LHC, with emphasis on sensitivity to anomalous tau couplings and the tau magnetic moment.

The forward proton spectrometer is essential because it provides:

- a way to tag exclusive or semi-exclusive topologies;
- extra kinematic information from proton energy losses;
- matching observables that compare the proton system to the central tau system.

### Channels considered

The documentation at different stages references several channels:

- fully hadronic `τhτh`;
- `μτh`;
- `eτh`;
- `eμ`.

However, the mature documentation in 2026 is overwhelmingly centered on MuTau.

### Important observables

Repeated across many files:

- central-system invariant mass;
- system transverse momentum;
- system rapidity;
- acoplanarity;
- proton energy loss `xi`;
- proton-pair mass and rapidity;
- mass difference between central and proton-reconstructed systems;
- rapidity matching between central and proton systems.

### Event-selection philosophy

Across the documentation, the signal-like selections consistently prefer:

- tight tau identification;
- hard tau pT threshold;
- hard lepton pT threshold;
- opposite-sign final states;
- geometric acceptance requirements;
- proton requirements on both arms for signal-like regions.

### Background strategy

The documentation consistently identifies the main backgrounds as:

- Drell-Yan;
- ttbar/tt jets;
- QCD multijet.

QCD is treated through same-sign control logic, and the control-region decks indicate a lot of attention was paid to validating those regions and their plotting choices.

### Proton treatment

This is the most technically distinctive theme in the folder.

The documentation evolves through three stages:

1. early stage:
   proton observables studied largely at the signal level;
2. intermediate stage:
   MC enriched with pileup protons using a proton pool and a fixed acceptance factor;
3. mature stage:
   proton variables preserved in data/QCD outputs from the first step, and acceptance treated more explicitly and data-driven.

This evolution is one of the main reasons the repository contains mixed conventions.

### Statistical interpretation

Only the later MuTau notes describe a full statistical chain:

- build shapes;
- define datacard;
- include nuisance parameters;
- run Combine;
- extract upper limits.

The quoted sensitivity is still weak relative to the Standard Model expectation, with documented limits at the level of thousands of times the SM signal strength.

## Main historical inconsistencies documented in the folder

The folder itself reveals several important tensions.

### 1. Proton acceptance value

Older decks repeatedly use:

- `0.13`

Later MuTau notes argue for:

- `0.245`

This is one of the clearest examples of the documentation evolving rather than staying static.

### 2. DY vs ttbar normalization conventions

Older material often documents:

- DY scaled in plotting with `1.81`
- ttbar carrying a built-in `0.15`

Later MuTau normalization notes replace the DY picture with:

- `DY_SCALE = 1.004e-4 = L*sigma/Sum_w`

while ttbar remains special-cased upstream.

### 3. Data/QCD proton handling

Older processing appears to have been more uniform across samples, while the later notes make a point of changing the treatment so that:

- Data and QCD keep proton variables directly;
- DY and ttbar receive proton mixing later.

### 4. Plot-definition mismatches

The plot-review decks explicitly complain about mismatches such as:

- histogram ranges not matching CR definitions;
- high-end discrepancies in data/MC ratios;
- background shapes needing attention.

This means some plotting products documented in the older decks should be interpreted as iterative debugging outputs rather than validated final plots.

## Which files are the best references today?

If the question is “which documents in `documentation/` should be trusted as the best present-day references?”, the answer is:

### Best current technical references

- `NORMALIZATION_DOCUMENTATION.md`
- `presentation_mutau_weights.md`
- `presentation_meeting.md`
- `slides_mutau.md`
- `slides_combine.md`

### Best current historical/transition references

- `16_10.pdf`
- `07_08.pdf`
- `05_02_25.pdf`

### Best plot/debugging references

- `plots.pdf`
- `06_11 (1).pdf`
- `29_10-questions regarding plots (1).pdf`

## Final assessment

The `documentation/` folder is valuable precisely because it is not overly curated. It preserves:

- the physics motivation;
- the signal-variable development;
- the codeflow reconstruction effort;
- the normalization and proton-handling debugging;
- the final MuTau channel formalization.

Its weaknesses are:

- duplication;
- mixed levels of maturity;
- coexistence of old and new normalization conventions;
- several slide decks that are more “working meetings” than stable notes.

Its strengths are:

- unusually rich visibility into how the analysis actually evolved;
- clear documentation of the proton-mixing problem and its correction;
- enough mature MuTau material to reconstruct the analysis logic without reading only the code;
- enough historical decks to understand why legacy files and mixed conventions remain in the repository.

In short:

- the early PDFs explain why the project exists;
- the middle PDFs explain why the workflow became complicated;
- the 2026 MuTau notes explain how the current mature analysis branch works.
