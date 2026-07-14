#!/bin/bash
# Merge phase-1 MuTau chunk files (per sample) into single merged ROOT files.
# Run this on a machine with ROOT/hadd available (e.g. via ssh to lxplus/CMS).
set -euo pipefail

BASE="/eos/user/m/mblancco/samples_2018_mutau"
OUTDIR="${BASE}/final_samples"

declare -A SRC_DIRS=(
    [Data]="${BASE}/fase1_data_proton_vars"
    [DY]="${BASE}/fase1_dy"
    [QCD]="${BASE}/fase1_qcd_proton_vars"
    [ttjets]="${BASE}/fase1_ttjets"
)

declare -A OUT_NAMES=(
    [Data]="Data_2018_UL_MuTau_merged.root"
    [DY]="DY_2018_UL_MuTau_merged.root"
    [QCD]="QCD_2018_UL_MuTau_merged.root"
    [ttjets]="ttjets_2018_UL_MuTau_merged.root"
)

if ! command -v hadd >/dev/null 2>&1; then
    echo "ERROR: 'hadd' not found in PATH. Make sure ROOT is set up (e.g. 'cmsenv' or 'source thisroot.sh') before running this script." >&2
    exit 1
fi

mkdir -p "$OUTDIR"

for sample in "${!SRC_DIRS[@]}"; do
    src="${SRC_DIRS[$sample]}"
    out="${OUTDIR}/${OUT_NAMES[$sample]}"

    echo "=== ${sample} ==="
    echo "Source: ${src}"

    shopt -s nullglob
    files=("${src}"/*.root)
    shopt -u nullglob

    if [ ${#files[@]} -eq 0 ]; then
        echo "  WARNING: no .root files found in ${src}, skipping." >&2
        continue
    fi

    echo "  Merging ${#files[@]} files -> ${out}"
    hadd -f "${out}" "${files[@]}"
    echo "  Done."
    echo
done

echo "All merges complete. Output directory: ${OUTDIR}"
