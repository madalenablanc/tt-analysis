# Documentation Map

Date: 2026-04-26

## Purpose

This file is a quick guide to the documentation in this repository. It is meant to answer three questions:

- what each document is about;
- whether it is current, duplicated, or mostly historical;
- where to start depending on the kind of context needed.

## How to read this map

Status labels used here:

- `current`: useful as an active reference for the present analysis workflow
- `current-duplicate`: same content exists elsewhere in Markdown or another PDF
- `historical`: useful for analysis history, early design choices, or debugging context
- `plot-review`: mostly plot dumps, validation comments, or discussion material

## Best starting points

If you only want the most useful current documentation, start here:

1. [project_status.md](/Users/utilizador/cernbox/tau_analysis/project_status.md:1)
2. [documentation/NORMALIZATION_DOCUMENTATION.md](/Users/utilizador/cernbox/tau_analysis/documentation/NORMALIZATION_DOCUMENTATION.md:1)
3. [documentation/presentation_mutau_weights.md](/Users/utilizador/cernbox/tau_analysis/documentation/presentation_mutau_weights.md:1)
4. [documentation/slides_combine.md](/Users/utilizador/cernbox/tau_analysis/documentation/slides_combine.md:1)
5. [MuTau_channel/slides/slides_tmva_combine.md](/Users/utilizador/cernbox/tau_analysis/MuTau_channel/slides/slides_tmva_combine.md:1)
6. [MuTau_channel/slides/slides_weights.md](/Users/utilizador/cernbox/tau_analysis/MuTau_channel/slides/slides_weights.md:1)

If you want the broader analysis history, start here:

1. [documentation/05_02_25.pdf](/Users/utilizador/cernbox/tau_analysis/documentation/05_02_25.pdf:1)
2. [documentation/07_08.pdf](/Users/utilizador/cernbox/tau_analysis/documentation/07_08.pdf:1)
3. [documentation/16_10.pdf](/Users/utilizador/cernbox/tau_analysis/documentation/16_10.pdf:1)
4. [updates/25_02_02.pdf](/Users/utilizador/cernbox/tau_analysis/updates/25_02_02.pdf:1)
5. [documentation/plots.pdf](/Users/utilizador/cernbox/tau_analysis/documentation/plots.pdf:1)

## Repository-wide summaries

### [README.md](/Users/utilizador/cernbox/tau_analysis/README.md:1)

Status: `current`

Use for:

- quick MuTau-oriented checkpoint overview
- proton-requirement plots
- event-count comparisons
- short notes on proton-variable handling

Notes:

- focused on a specific analysis checkpoint rather than the whole repository
- useful operationally, but not a full technical note

### [README.pdf](/Users/utilizador/cernbox/tau_analysis/README.pdf:1)

Status: `current-duplicate`

Use for:

- PDF version of the README content

Notes:

- no clear added value over the Markdown version

### [project_status.md](/Users/utilizador/cernbox/tau_analysis/project_status.md:1)

Status: `current`

Use for:

- high-level overview of the three channels
- phase-0 / phase-1 / proton-tagging structure
- current open issues and next steps

### [project_status.pdf](/Users/utilizador/cernbox/tau_analysis/project_status.pdf:1)

Status: `current-duplicate`

Use for:

- PDF export of the project status note

## Current MuTau documentation

### [documentation/NORMALIZATION_DOCUMENTATION.md](/Users/utilizador/cernbox/tau_analysis/documentation/NORMALIZATION_DOCUMENTATION.md:1)

Status: `current`

Use for:

- MC normalization formula
- generator-weight sums
- DY and ttbar normalization constants
- relation between phase-1 event weights and final plotting weights

### [documentation/NORMALIZATION_DOCUMENTATION.pdf](/Users/utilizador/cernbox/tau_analysis/documentation/NORMALIZATION_DOCUMENTATION.pdf:1)

Status: `current-duplicate`

Use for:

- PDF export of the normalization note

### [documentation/presentation_meeting.md](/Users/utilizador/cernbox/tau_analysis/documentation/presentation_meeting.md:1)

Status: `current`

Use for:

- short status note on the proton-mixing bug
- acceptance measurement discussion
- weight-structure summary

### [documentation/presentation_meeting.pdf](/Users/utilizador/cernbox/tau_analysis/documentation/presentation_meeting.pdf:1)

Status: `current-duplicate`

### [documentation/presentation_mutau_weights.md](/Users/utilizador/cernbox/tau_analysis/documentation/presentation_mutau_weights.md:1)

Status: `current`

Use for:

- phase-0 / phase-1 / proton-mixing workflow
- event weight definitions
- proton pool description
- data-driven proton acceptance explanation

### [documentation/presentation_mutau_weights.pdf](/Users/utilizador/cernbox/tau_analysis/documentation/presentation_mutau_weights.pdf:1)

Status: `current-duplicate`

### [documentation/presentation_mutau_weights_slides.pdf](/Users/utilizador/cernbox/tau_analysis/documentation/presentation_mutau_weights_slides.pdf:1)

Status: `current-duplicate`

Use for:

- slide version of the same proton-mixing and weight-normalization story

### [documentation/presentation_weights.pdf](/Users/utilizador/cernbox/tau_analysis/documentation/presentation_weights.pdf:1)

Status: `current-duplicate`

Use for:

- shorter weight-oriented presentation version

### [documentation/slides_combine.md](/Users/utilizador/cernbox/tau_analysis/documentation/slides_combine.md:1)

Status: `current`

Use for:

- Combine workflow
- shape production
- datacard structure
- nuisances, rate parameters, and asymptotic limits

### [documentation/slides_combine_26_03.pdf](/Users/utilizador/cernbox/tau_analysis/documentation/slides_combine_26_03.pdf:1)

Status: `current-duplicate`

### [documentation/slides_mutau.md](/Users/utilizador/cernbox/tau_analysis/documentation/slides_mutau.md:1)

Status: `current`

Use for:

- MuTau BDT training/application overview
- weight evolution
- plotting after proton requirements

### [documentation/slides_mutau.pdf](/Users/utilizador/cernbox/tau_analysis/documentation/slides_mutau.pdf:1)

Status: `current-duplicate`

### [documentation/slides_mutau_05_02.pdf](/Users/utilizador/cernbox/tau_analysis/documentation/slides_mutau_05_02.pdf:1)

Status: `current-duplicate`

### [documentation/slides_mutau_07_03.pdf](/Users/utilizador/cernbox/tau_analysis/documentation/slides_mutau_07_03.pdf:1)

Status: `current-duplicate`

### [documentation/slides_mutau_26_02.pdf](/Users/utilizador/cernbox/tau_analysis/documentation/slides_mutau_26_02.pdf:1)

Status: `current-duplicate`

### [MuTau_channel/slides/slides_tmva_combine.md](/Users/utilizador/cernbox/tau_analysis/MuTau_channel/slides/slides_tmva_combine.md:1)

Status: `current`

Use for:

- multi-method TMVA summary
- propagation of MVA outputs to Combine
- method-comparison framing

### [MuTau_channel/slides/slides_weights.md](/Users/utilizador/cernbox/tau_analysis/MuTau_channel/slides/slides_weights.md:1)

Status: `current`

Use for:

- detailed documentation of `sinal.py`
- signal normalization and cutflow
- PPS fiducial logic
- xi systematics and radiation-damage weights

## Current MuTau PDF mirrors in `MuTau_channel/slides/`

These are useful if a PDF is more convenient, but they mostly duplicate Markdown-backed content already present in the repository:

- [MuTau_channel/slides/NORMALIZATION_DOCUMENTATION.pdf](/Users/utilizador/cernbox/tau_analysis/MuTau_channel/slides/NORMALIZATION_DOCUMENTATION.pdf:1) — `current-duplicate`
- [MuTau_channel/slides/presentation_meeting.pdf](/Users/utilizador/cernbox/tau_analysis/MuTau_channel/slides/presentation_meeting.pdf:1) — `current-duplicate`
- [MuTau_channel/slides/presentation_mutau_weights.pdf](/Users/utilizador/cernbox/tau_analysis/MuTau_channel/slides/presentation_mutau_weights.pdf:1) — `current-duplicate`
- [MuTau_channel/slides/presentation_mutau_weights_slides.pdf](/Users/utilizador/cernbox/tau_analysis/MuTau_channel/slides/presentation_mutau_weights_slides.pdf:1) — `current-duplicate`
- [MuTau_channel/slides/presentation_weights.pdf](/Users/utilizador/cernbox/tau_analysis/MuTau_channel/slides/presentation_weights.pdf:1) — `current-duplicate`
- [MuTau_channel/slides/slides_combine_13_04.pdf](/Users/utilizador/cernbox/tau_analysis/MuTau_channel/slides/slides_combine_13_04.pdf:1) — `current`
- [MuTau_channel/slides/slides_combine_16_04.pdf](/Users/utilizador/cernbox/tau_analysis/MuTau_channel/slides/slides_combine_16_04.pdf:1) — `current`
- [MuTau_channel/slides/slides_combine_26_03.pdf](/Users/utilizador/cernbox/tau_analysis/MuTau_channel/slides/slides_combine_26_03.pdf:1) — `current-duplicate`
- [MuTau_channel/slides/slides_mutau.pdf](/Users/utilizador/cernbox/tau_analysis/MuTau_channel/slides/slides_mutau.pdf:1) — `current-duplicate`
- [MuTau_channel/slides/slides_mutau_05_02.pdf](/Users/utilizador/cernbox/tau_analysis/MuTau_channel/slides/slides_mutau_05_02.pdf:1) — `current-duplicate`
- [MuTau_channel/slides/slides_mutau_07_03.pdf](/Users/utilizador/cernbox/tau_analysis/MuTau_channel/slides/slides_mutau_07_03.pdf:1) — `current-duplicate`
- [MuTau_channel/slides/slides_mutau_13_04.pdf](/Users/utilizador/cernbox/tau_analysis/MuTau_channel/slides/slides_mutau_13_04.pdf:1) — `current`
- [MuTau_channel/slides/slides_mutau_26_02.pdf](/Users/utilizador/cernbox/tau_analysis/MuTau_channel/slides/slides_mutau_26_02.pdf:1) — `current-duplicate`
- [MuTau_channel/slides/slides_weights.pdf](/Users/utilizador/cernbox/tau_analysis/MuTau_channel/slides/slides_weights.pdf:1) — `current`

Notes:

- the dated April PDFs are useful because they can contain the latest slide-state even when the older Markdown note is more generic
- the February and March slide PDFs mostly duplicate content already represented in the Markdown versions

## Historical and early-development PDFs

### [documentation/05_02_25.pdf](/Users/utilizador/cernbox/tau_analysis/documentation/05_02_25.pdf:1)

Status: `historical`

Use for:

- original physics motivation
- anomalous tau magnetic moment framing
- explanation of why exclusive `gamma gamma -> tau tau` is interesting
- early list of decay channels

Best value:

- shows the broad scientific motivation before the repository settled into its current MuTau-centered workflow

### [documentation/first-stage-analysis.pdf](/Users/utilizador/cernbox/tau_analysis/documentation/first-stage-analysis.pdf:1)

Status: `historical`

Use for:

- long early-development slide notebook
- first signal variable studies
- early correlation studies
- evolution of the project presentation

Best value:

- useful for reconstructing the earliest signal-oriented thinking and variable choices

### [documentation/07_08.pdf](/Users/utilizador/cernbox/tau_analysis/documentation/07_08.pdf:1)

Status: `historical`

Use for:

- early codeflow and dataflow
- older phase-0 / phase-1 file organization
- old background normalization assumptions
- bug notes from proton enrichment

Best value:

- one of the best docs for understanding the older TauTau-oriented workflow and why some legacy code exists

### [documentation/16_10.pdf](/Users/utilizador/cernbox/tau_analysis/documentation/16_10.pdf:1)

Status: `historical`

Use for:

- transition from the old proton-handling workflow to the newer one
- what changed in processing for data/QCD vs DY/ttbar
- summary of signal-processing logic and proton cuts

Best value:

- best historical explanation of the move toward preserving proton variables in real data products

### [documentation/27_10.pdf](/Users/utilizador/cernbox/tau_analysis/documentation/27_10.pdf:1)

Status: `historical`

Use for:

- later-stage plot review
- signal and data plot presentation
- BDT input visualization

Best value:

- useful mainly as a snapshot of what was being visually checked at that stage

### [documentation/01_08_2025.pdf](/Users/utilizador/cernbox/tau_analysis/documentation/01_08_2025.pdf:1)

Status: `historical`

Use for:

- early weekly-meeting style slides

Notes:

- overlaps with the project-motivation material found in `first-stage-analysis.pdf`

### [documentation/06_11 (1).pdf](/Users/utilizador/cernbox/tau_analysis/documentation/06_11%20(1).pdf:1)

Status: `historical`

Use for:

- later development checkpoint

Notes:

- should be consulted if you want to reconstruct week-to-week project evolution

### [documentation/12_09.pdf](/Users/utilizador/cernbox/tau_analysis/documentation/12_09.pdf:1)

Status: `historical`

### [documentation/21_08.pdf](/Users/utilizador/cernbox/tau_analysis/documentation/21_08.pdf:1)

Status: `historical`

### [documentation/28_08.pdf](/Users/utilizador/cernbox/tau_analysis/documentation/28_08.pdf:1)

Status: `historical`

### [documentation/Cópia de apresentacao.pdf](/Users/utilizador/cernbox/tau_analysis/documentation/C%C3%B3pia%20de%20apresentacao.pdf:1)

Status: `historical`

### [documentation/apresentacao.pdf](/Users/utilizador/cernbox/tau_analysis/documentation/apresentacao.pdf:1)

Status: `historical`

Notes for the last five:

- these appear to be intermediate presentation decks rather than enduring technical references
- they are useful if you need timeline context, but not the best place to learn the current workflow

## Plot-review and debugging material

### [documentation/plots.pdf](/Users/utilizador/cernbox/tau_analysis/documentation/plots.pdf:1)

Status: `plot-review`

Use for:

- variable and correlation plots
- old weighting assumptions
- control-region plot observations
- comments about inconsistencies in histogram ranges and shapes

Best value:

- good for understanding what plot-level issues were being debugged

### [updates/25_02_02.pdf](/Users/utilizador/cernbox/tau_analysis/updates/25_02_02.pdf:1)

Status: `plot-review`

Use for:

- short progress update
- event-count changes after fixes
- early signal/background plot comparisons
- summary of variable studies

### [documentation/29_10-questions regarding plots (1).pdf](/Users/utilizador/cernbox/tau_analysis/documentation/29_10-questions%20regarding%20plots%20(1).pdf:1)

Status: `plot-review`

Use for:

- questions and concerns raised about plotted results

Notes:

- useful for internal review context rather than for the nominal analysis workflow

## Recommended usage by purpose

### If you want to understand the current analysis

Read:

1. [project_status.md](/Users/utilizador/cernbox/tau_analysis/project_status.md:1)
2. [documentation/presentation_mutau_weights.md](/Users/utilizador/cernbox/tau_analysis/documentation/presentation_mutau_weights.md:1)
3. [documentation/NORMALIZATION_DOCUMENTATION.md](/Users/utilizador/cernbox/tau_analysis/documentation/NORMALIZATION_DOCUMENTATION.md:1)
4. [documentation/slides_combine.md](/Users/utilizador/cernbox/tau_analysis/documentation/slides_combine.md:1)
5. [MuTau_channel/slides/slides_tmva_combine.md](/Users/utilizador/cernbox/tau_analysis/MuTau_channel/slides/slides_tmva_combine.md:1)
6. [MuTau_channel/slides/slides_weights.md](/Users/utilizador/cernbox/tau_analysis/MuTau_channel/slides/slides_weights.md:1)

### If you want the original physics motivation

Read:

1. [documentation/05_02_25.pdf](/Users/utilizador/cernbox/tau_analysis/documentation/05_02_25.pdf:1)
2. [documentation/first-stage-analysis.pdf](/Users/utilizador/cernbox/tau_analysis/documentation/first-stage-analysis.pdf:1)

### If you want the evolution of proton handling and weighting

Read:

1. [documentation/07_08.pdf](/Users/utilizador/cernbox/tau_analysis/documentation/07_08.pdf:1)
2. [documentation/16_10.pdf](/Users/utilizador/cernbox/tau_analysis/documentation/16_10.pdf:1)
3. [documentation/presentation_meeting.md](/Users/utilizador/cernbox/tau_analysis/documentation/presentation_meeting.md:1)
4. [documentation/NORMALIZATION_DOCUMENTATION.md](/Users/utilizador/cernbox/tau_analysis/documentation/NORMALIZATION_DOCUMENTATION.md:1)

### If you want plot-validation and control-region context

Read:

1. [documentation/plots.pdf](/Users/utilizador/cernbox/tau_analysis/documentation/plots.pdf:1)
2. [updates/25_02_02.pdf](/Users/utilizador/cernbox/tau_analysis/updates/25_02_02.pdf:1)
3. [documentation/29_10-questions regarding plots (1).pdf](/Users/utilizador/cernbox/tau_analysis/documentation/29_10-questions%20regarding%20plots%20(1).pdf:1)

## Bottom line

The repository documentation is concentrated around the MuTau workflow, and the Markdown notes are the best active references. The extra PDFs are still valuable, but mostly for:

- project history;
- older weighting/proton-handling conventions;
- early signal-variable studies;
- plot-validation and debugging context.

For everyday analysis work, prefer the Markdown-backed notes first and use the older PDFs as historical supplements.
