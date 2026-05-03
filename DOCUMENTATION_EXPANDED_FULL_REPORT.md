# Expanded Full Documentation Report

Date: 2026-04-27

## Scope

This report expands the earlier documentation review to include not only the top-level `documentation/` folder, but also the documentation-related material stored elsewhere in the repository.

Covered sources:

- `documentation/`
- `MuTau_channel/slides/`
- `updates/`
- top-level summary and status files:
  - `README.md`
  - `README.pdf`
  - `project_status.md`
  - `project_status.pdf`
- top-level RTF instruction and result files:
  - `combine_instructions.rtf`
  - `combine_results_mu_tau.rtf`
  - `run bdt instructions.rtf`

This report is therefore a review of the repository’s full documentation corpus, not just one folder.

## Main conclusion

The documentation in this repository falls into five distinct layers:

1. broad scientific motivation and project framing;
2. early signal-variable, correlation, and plot studies;
3. workflow reconstruction and code-organization notes;
4. mature MuTau technical documentation for normalization, proton mixing, BDT/TMVA, signal weighting, and Combine;
5. operational instructions and recorded statistical outputs.

The documentation is strongest for the MuTau channel. That is where the repository contains:

- clear process flow;
- explicit weight definitions;
- an identified and fixed proton-mixing bug;
- a documented transition from older normalization conventions to a more principled one;
- a documented signal weighting path;
- a documented Combine procedure and example results.

The documentation is weaker for ETau and TauTau in the sense that:

- those channels appear regularly in historical and workflow-recovery decks;
- but they do not have the same polished end-to-end note structure that MuTau has.

The subdirectory material matters because it contains:

- newer MuTau slide exports and dated variants;
- the signal-weighting note in slide form;
- later Combine-update decks;
- the operational “how to run” instructions;
- a concrete RTF record of Combine outputs not summarized elsewhere.

## Inventory by location

### A. `documentation/`

This is the main archive of formal and semi-formal notes. It contains:

- broad physics presentations;
- historical weekly-meeting decks;
- plot-review decks;
- current Markdown notes on MuTau normalization, proton mixing, BDT, and Combine.

### B. `MuTau_channel/slides/`

This directory contains a second, partly duplicated but still valuable documentation layer:

- PDF exports of the MuTau Markdown notes;
- later dated slide revisions such as:
  - `slides_combine_13_04.pdf`
  - `slides_combine_16_04.pdf`
  - `slides_mutau_13_04.pdf`
- the signal-weighting note:
  - `slides_weights.md`
  - `slides_weights.pdf`
- one additional MuTau note not mirrored in the top-level `documentation/` folder:
  - `slides_tmva_combine.md`
- HTML and TeX render/export artifacts.

### C. `updates/`

Contains:

- `25_02_02.pdf`

This is a short but useful progress update deck with early event-count and plotting information.

### D. Top-level notes

Contains:

- `README.md`
- `README.pdf`
- `project_status.md`
- `project_status.pdf`

These provide the top-level repository summaries and checkpoint descriptions.

### E. Top-level RTF instruction/results files

Contains:

- `combine_instructions.rtf`
- `combine_results_mu_tau.rtf`
- `run bdt instructions.rtf`

These are critical because they contain:

- explicit run instructions;
- the exact Combine command sequence;
- a concrete saved record of statistical outputs.

## What the expanded corpus adds beyond `documentation/`

Compared with reviewing only the `documentation/` folder, the additional directories add three important classes of information.

### 1. The latest MuTau slide-state

The `MuTau_channel/slides/` directory is important because some slide PDFs there are later than the versions mirrored in `documentation/`.

Especially relevant:

- `slides_combine_13_04.pdf`
- `slides_combine_16_04.pdf`
- `slides_mutau_13_04.pdf`
- `slides_weights.md`
- `slides_tmva_combine.md`

These files show the later April 2026 state of the MuTau analysis, beyond the earlier March 2026 notes in `documentation/`.

### 2. Signal weighting is documented more fully there

The single most detailed explanation of signal processing is not in `documentation/`; it is in:

- `MuTau_channel/slides/slides_weights.md`

That note describes:

- SM normalization from reweighting branches;
- detailed cutflow;
- muon scale factors;
- proton-arm requirements;
- `theta_x` and geometric fiducial logic;
- era-dependent pixel acceptance;
- radiation-damage weights;
- xi systematic up/down variations;
- storage of BSM weight arrays.

That is a major documentation source and must be part of any truly full report.

### 3. Instructions and recorded statistical outputs

The RTF files add content absent from the Markdown/PDF notes:

- `combine_instructions.rtf` gives an environment and command cookbook for Combine;
- `run bdt instructions.rtf` gives the BDT-to-Combine operational sequence;
- `combine_results_mu_tau.rtf` records actual Combine numerical outputs, including significance and fit diagnostics.

Without these files, the documentation picture is incomplete.

## Top-level summaries

### `README.md` and `README.pdf`

These act as checkpoint documentation rather than as a polished overview.

They emphasize:

- plots made from MuTau samples;
- comparison of proton-requirement plots versus no-proton-requirement plots;
- a summary table of MuTau cuts for `ttjets`, `dy`, `qcd`, and `data`;
- an explanation of proton selection:
  - multi-RP preference;
  - one proton per arm requirement;
  - largest-`xi` selection when multiple protons are present;
- event counts before and after proton-variable saving;
- concern that phase-1 behavior changed after the proton-variable-saving modification;
- a simplified description of pileup proton enrichment;
- the old statement that a fixed `0.13` weight was added for PPS acceptance/efficiency.

Documentation value:

- useful as a concrete checkpoint log;
- reveals the transition point before the later normalization cleanup.

Limitations:

- incomplete and partly rough;
- contains unresolved notes and unfinished sections;
- not the best place to learn the final state of the analysis.

### `project_status.md` and `project_status.pdf`

These are higher-level status documents.

They present the analysis as a three-channel program:

- MuTau;
- ETau;
- TauTau;

They also summarize:

- phase-0 purpose;
- phase-1 cuts;
- proton-selection philosophy;
- current concerns about changed event counts after proton-variable handling;
- the role of saved proton variables;
- plot categories;
- next steps.

Documentation value:

- best top-level summary of the whole repository;
- useful as the first file to read before diving into channel-specific notes.

Limitations:

- still project-summary style, not a full technical note;
- necessarily more polished and simplified than the actual historical working decks.

## Historical presentation layer in `documentation/`

### Broad scientific motivation files

These include:

- `05_02_25.pdf`
- `apresentacao.pdf`
- `Cópia de apresentacao.pdf`
- broad segments of `first-stage-analysis.pdf`

Together they tell the “why” of the project:

- new physics with proton interactions at the LHC;
- central exclusive production as a clean topology;
- anomalous tau couplings and tau magnetic moment motivation;
- forward proton tagging with PPS;
- the potential of tau channels to probe BSM effects.

They are especially useful because later technical notes assume the reader already understands this motivation.

### Signal-variable and correlation files

These include:

- `05_02_25.pdf`
- `first-stage-analysis.pdf`
- `plots.pdf`
- `updates/25_02_02.pdf`

They emphasize:

- signal kinematics and expected behavior;
- proton energy-loss distributions;
- rapidity and invariant-mass definitions;
- pileup versus no-pileup comparisons;
- correlations expected for signal.

These files explain the origin of the later chosen BDT variables and matching observables.

### Workflow-recovery and code-organization files

These include:

- `07_08.pdf`
- `21_08.pdf`
- `28_08.pdf`
- `16_10.pdf`

They are essential for understanding:

- what was recovered from earlier analysis products;
- how the old channel analyzers were supposed to work;
- which files correspond to phase 0 and phase 1;
- how ETau was used as a control for understanding sample structure;
- which branches existed in merged or phase-0 outputs;
- where event-count inconsistencies first became obvious.

### Debugging and plot-review files

These include:

- `01_08_2025.pdf`
- `06_11 (1).pdf`
- `29_10-questions regarding plots (1).pdf`
- `plots.pdf`
- `27_10.pdf`

These files document:

- control-region definitions;
- plot inconsistencies;
- ad hoc attempts to rescale samples to mimic expected distributions;
- concern that data and QCD were too small;
- sample and plot questions to collaborators or supervisors.

They are not final notes, but they show the actual problems being worked through.

## Current MuTau technical layer

This is the most mature part of the corpus.

Main current files:

- `documentation/NORMALIZATION_DOCUMENTATION.md`
- `documentation/presentation_meeting.md`
- `documentation/presentation_mutau_weights.md`
- `documentation/slides_mutau.md`
- `documentation/slides_combine.md`
- `MuTau_channel/slides/slides_tmva_combine.md`
- `MuTau_channel/slides/slides_weights.md`

### `NORMALIZATION_DOCUMENTATION.md`

What it contributes:

- a formal normalization formula:
  `weight = (L * sigma / Sum_w_gen) * generator_weight * SF_muon * eff_proton`
- exact DY and ttbar cross sections;
- the generator-weight sums;
- a preferred proton acceptance `0.245`;
- explicit DY scale:
  `1.004e-4`
- explanation that ttbar still carries a built-in `0.15` factor upstream;
- explicit plotting-stage scaling rules.

Why it matters:

- this is the cleanest formalization of MuTau normalization in the documentation corpus.

### `presentation_meeting.md`

What it contributes:

- the short, clear story of the proton-mixing bug;
- the statement that the original code wrongly replaced the tree weight with `0.13`;
- the fixed logic: multiply `event_weight` by the proton acceptance instead;
- explicit comparison between the older `0.13` and the measured `0.245`;
- direct statement of the open discussion at that time.

Why it matters:

- this is the best short explanation of how and why the normalization treatment changed.

### `presentation_mutau_weights.md`

What it contributes:

- the clearest workflow note for MuTau processing;
- a nice pipeline diagram from NanoAOD to proton mixing to plots;
- phase-0 and phase-1 explanations;
- event-weight definitions for data, DY, ttbar, and QCD;
- proton-pool origin and content;
- proton-mixing algorithm details;
- acceptance definition from data.

Why it matters:

- this is one of the best “how the MuTau workflow works” documents.

### `slides_mutau.md`

What it contributes:

- a presentation version of the MuTau chain;
- old versus corrected weighting logic;
- BDT-oriented context;
- post-proton-merging plots and their interpretation.

Why it matters:

- it compresses several technical points into a quick-read slide format.

### `slides_combine.md`

What it contributes:

- the full mass-shape Combine pipeline;
- explicit samples;
- shape definitions;
- nuisances and `rateParam`s;
- commands for `text2workspace.py` and `combine`;
- reported observed and expected limits.

Why it matters:

- it is the core statistical-interpretation note for the MuTau mass-shape analysis.

### `slides_tmva_combine.md`

This file lives in `MuTau_channel/slides/`, not `documentation/`, but it is central to the full documentation corpus.

What it contributes:

- extension from single-method BDT to multiple discriminants:
  - BDT;
  - Likelihood;
  - Fisher;
- summary of the nine TMVA input variables;
- the training sample summary;
- method comparison framing;
- connection from TMVA outputs to Combine limits;
- updated results and “next steps” toward comparing discriminants.

Why it matters:

- it is the only note that clearly documents the multi-method MVA-to-Combine program.

### `slides_weights.md`

Also in `MuTau_channel/slides/`, and extremely important.

What it contributes:

- a step-by-step explanation of signal processing in `sinal.py`;
- normalization before cuts;
- muon SF decomposition;
- proton-arm requirement;
- `theta_x` acceptance cuts;
- era-dependent geometric cuts;
- radiation-damage and pixel-efficiency weights;
- xi systematic branch production;
- final output branch structure;
- BSM weight handling.

Why it matters:

- this is the deepest single technical note on the signal side of the analysis.

## Dated slide exports in `MuTau_channel/slides/`

The directory `MuTau_channel/slides/` contains several dated PDFs that are partly duplicated in top-level `documentation/`, but still useful as time-stamped analysis states.

### `slides_combine_13_04.pdf`

Main contribution:

- later April 2026 Combine slide state;
- likely incorporates updated plots or status beyond the original March deck.

### `slides_combine_16_04.pdf`

Main contribution:

- title indicates:
  “signal normalization fix and combine tool results”
- this is especially important because it likely represents the next step after the earlier normalization notes.

Relative value:

- one of the most important extra PDFs outside `documentation/`, because it likely documents the combination of the normalization fix with later statistical outputs.

### `slides_mutau_13_04.pdf`

Main contribution:

- later dated version of the MuTau BDT/application deck;
- useful to track what changed between the March and April presentations.

### Other PDFs in the slide directory

Files such as:

- `NORMALIZATION_DOCUMENTATION.pdf`
- `presentation_meeting.pdf`
- `presentation_mutau_weights.pdf`
- `presentation_mutau_weights_slides.pdf`
- `presentation_weights.pdf`
- `slides_combine_26_03.pdf`
- `slides_mutau.pdf`
- `slides_mutau_05_02.pdf`
- `slides_mutau_07_03.pdf`
- `slides_mutau_26_02.pdf`
- `slides_weights.pdf`

are mainly:

- PDF mirrors of the Markdown notes;
- time-stamped presentation variants;
- still useful as frozen presentation states, but not generally richer than the Markdown originals.

### Additional non-PDF artifacts

The same directory also contains:

- `NORMALIZATION_DOCUMENTATION.html`
- `slides_mutau.html`
- `slides_combine.tex`

These are render/export artifacts rather than separate content sources. They matter operationally but do not add new scientific information.

## `updates/25_02_02.pdf`

This file is a short but important progress update.

Main content:

- signal and variable plots;
- event-count comparison such as:
  `23k -> 700 events`, with `508` for no pileup protons in one early version;
- explicit note that proton selection was fixed;
- explicit note that the invariant-mass calculation was changed to use `TLorentzVector`;
- signal/background comparison plots;
- early BDT-input variable list.

Documentation value:

- strong evidence of how event yields changed during early fixes;
- useful for understanding why some later notes emphasize corrected invariant-mass formulas and fixed proton logic.

## Operational instruction files

### `run bdt instructions.rtf`

This file is a concise execution cookbook for the MVA pipeline.

It defines the operational sequence:

1. train the BDT with `TMVAClassification.C`;
2. apply the BDT to all samples with `run_bdt_all.sh`;
3. build plots from the BDT output with `bdt_output.cpp`;
4. build shape histograms with `save_shapes.cpp`;
5. convert the datacard to a workspace;
6. run asymptotic limits;
7. plot the limits.

Documentation value:

- this is the clearest operational summary of the MuTau TMVA-to-Combine chain;
- it complements the higher-level slide notes with actual commands.

### `combine_instructions.rtf`

This file is the most concrete environment-and-execution guide for Combine.

It documents:

- the need for an SL7 container on lxplus;
- the chosen `SCRAM_ARCH`;
- the CMSSW/Combine environment setup;
- the location of the `CombinedLimit` scripts in `PATH`;
- how to recompile `save_shapes.cpp`;
- commands for:
  - workspace creation for mass-shape and likelihood-shape analyses;
  - asymptotic limits;
  - significance;
  - goodness-of-fit;
  - `combineTool.py -M CollectLimits`;
  - `plotLimits.py`;
  - impact plots.

Documentation value:

- essential for reproducibility;
- shows the intended runtime environment and the exact command sequence used in practice.

Limitations:

- rough formatting;
- contains a typo-like fragment (`szakg++`) that should be interpreted carefully;
- still clearly a working instruction sheet.

### `combine_results_mu_tau.rtf`

This is a saved log of actual Combine outputs. It is extremely important because it adds numerical results not all captured elsewhere.

Recorded content includes:

- asymptotic limits for `mutau_bdt_workspace.root`;
- significance results;
- expected significance;
- `FitDiagnostics` best-fit `r`;
- `MultiDimFit` output;
- `GoodnessOfFit` output;
- toy-GOF generation with many toy test-statistic values;
- associated runtime warnings and ROOT/XRootD messages.

Key recorded numbers:

- observed limit around `r < 281.5513` for one BDT-based workspace;
- expected median around `r < 95.0000`;
- observed significance around `2.41418`;
- expected significance around `0.0458656`;
- best-fit `r` around `99.5` with asymmetric uncertainty.

Why this matters:

- these numbers differ substantially from some of the earlier mass-shape limit values described elsewhere;
- this means the RTF results file is documenting a later or different discriminant/workspace configuration than the earlier mass-shape note;
- it provides concrete evidence that the BDT-based statistical analysis reached a more signal-like result than the earlier simple shape note suggested.

This is arguably one of the most important individual documentation artifacts outside the Markdown notes, because it captures an actual analysis result rather than only a plan.

## Cross-document evolution of key analysis topics

### 1. Physics motivation

Earliest broad docs:

- emphasize anomalous tau couplings and tau `g-2`;
- present all channels as part of a broad physics program;
- use central exclusive production as the discovery context.

Later technical docs:

- shift attention toward the concrete MuTau analysis workflow and statistical inference.

### 2. Variables and observables

Early docs:

- define and motivate signal variables and correlations;
- explain the physical meaning of mass matching and rapidity matching.

Later docs:

- formalize many of those same observables as BDT inputs and Combine shapes.

### 3. Event weighting

Early/intermediate docs:

- use `1.81` for DY and `0.15` for ttbar;
- apply `0.13` as a proton-acceptance-like correction;
- include ad hoc debugging attempts with data or QCD scale changes.

Later MuTau docs:

- move toward explicit `L*sigma/Sum_w` scaling for DY;
- preserve the odd ttbar special case but explain it more transparently;
- measure proton acceptance from data as `0.245`.

### 4. Proton handling

Early docs:

- treat proton observables as signal variables;
- enrich MC with pileup protons in a simplified way.

Transition docs:

- identify the need to preserve proton variables in data/QCD rather than adding them later.

Late MuTau docs:

- provide explicit proton-pool logic;
- formalize the acceptance discussion;
- explain detailed fiducial and systematic treatment for the signal.

### 5. Statistical interpretation

Earlier docs:

- focus on plotting and selection behavior.

Later docs and RTF logs:

- provide full Combine steps;
- provide actual limit, significance, GoF, and fit results.

## Relationship between `documentation/` and `MuTau_channel/slides/`

These two locations are strongly connected but not identical.

### `documentation/` is:

- broader in scope;
- more historical;
- includes early cross-channel and TauTau/ETau material;
- includes the core MuTau Markdown notes.

### `MuTau_channel/slides/` is:

- more focused on MuTau;
- more presentation-oriented;
- contains later dated slide states;
- contains the deepest signal-weighting note;
- contains the TMVA/Combine comparison note not mirrored at the top level.

Therefore:

- `documentation/` explains the evolution and the broad project;
- `MuTau_channel/slides/` explains the mature MuTau implementation in finer detail.

## Best documents by purpose in the expanded corpus

### To understand the whole repository at a high level

- `project_status.md`
- `README.md`
- `documentation/05_02_25.pdf`

### To understand the historical evolution

- `documentation/07_08.pdf`
- `documentation/16_10.pdf`
- `documentation/28_08.pdf`
- `updates/25_02_02.pdf`

### To understand MuTau normalization and proton mixing

- `documentation/presentation_mutau_weights.md`
- `documentation/presentation_meeting.md`
- `documentation/NORMALIZATION_DOCUMENTATION.md`

### To understand signal processing

- `MuTau_channel/slides/slides_weights.md`

### To understand BDT/TMVA

- `documentation/slides_mutau.md`
- `MuTau_channel/slides/slides_tmva_combine.md`
- `run bdt instructions.rtf`

### To understand Combine and statistical outputs

- `documentation/slides_combine.md`
- `combine_instructions.rtf`
- `combine_results_mu_tau.rtf`

### To understand plot and CR issues

- `documentation/plots.pdf`
- `documentation/01_08_2025.pdf`
- `documentation/06_11 (1).pdf`
- `documentation/29_10-questions regarding plots (1).pdf`

## Final assessment

Once the subdirectories and top-level instruction files are included, the repository documentation becomes much more complete than the `documentation/` folder alone suggests.

The expanded corpus shows:

- the scientific motivation of the project;
- the development of the core observables;
- the recovery and reconstruction of legacy workflows;
- the long normalization/proton-handling debugging phase;
- the mature MuTau implementation;
- the actual command-line operational steps;
- recorded statistical outputs from Combine.

The most important additions beyond `documentation/` itself are:

1. `MuTau_channel/slides/slides_weights.md`
   - deepest signal-processing note;
2. `MuTau_channel/slides/slides_tmva_combine.md`
   - TMVA multi-method and Combine comparison note;
3. `combine_instructions.rtf`
   - concrete environment and execution manual;
4. `combine_results_mu_tau.rtf`
   - actual saved statistical results.

So the full documentation story is not only:

- “what analysis was intended,”

but also:

- “how it changed,”
- “how it was actually run,”
- and “what numerical outputs it produced.”
