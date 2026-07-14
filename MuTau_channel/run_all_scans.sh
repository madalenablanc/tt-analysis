#!/bin/bash
# Run BDT scans A, B, C sequentially
# Usage: bash run_all_scans.sh
# Must be run from MuTau_channel/ where dataset/weights/ and datacards/ exist

set -e

# ---------------------------------------------------------------
# BDT options for each scan (single-line strings)
# ---------------------------------------------------------------
OPTS_A="!H:!V:NTrees=600:MinNodeSize=5%:MaxDepth=2:BoostType=AdaBoost:AdaBoostBeta=0.2:UseBaggedBoost:BaggedSampleFraction=0.5:SeparationType=GiniIndex:nCuts=40:PruneMethod=CostComplexity:PruneStrength=5:NegWeightTreatment=IgnoreNegWeightsInTraining"
OPTS_B="!H:!V:NTrees=850:MinNodeSize=2.5%:MaxDepth=3:BoostType=AdaBoost:AdaBoostBeta=0.3:UseBaggedBoost:BaggedSampleFraction=0.5:SeparationType=GiniIndex:nCuts=40:PruneMethod=CostComplexity:PruneStrength=3:NegWeightTreatment=PairNegWeightsGlobal"
OPTS_C="!H:!V:NTrees=600:MinNodeSize=2.5%:MaxDepth=2:BoostType=Grad:Shrinkage=0.10:UseBaggedBoost:BaggedSampleFraction=0.5:SeparationType=GiniIndex:nCuts=40:PruneMethod=CostComplexity:PruneStrength=2:NegWeightTreatment=IgnoreNegWeightsInTraining"

RESULTS="scan_results.txt"
printf "%-6s | %-8s | %-12s | %-12s | %-12s | %-12s | %-12s | %-12s\n" \
    "Scan" "AUC" "-2sigma" "-1sigma" "Expected" "+1sigma" "+2sigma" "Observed" > "$RESULTS"
echo "-------+----------+--------------+--------------+--------------+--------------+--------------+--------------" >> "$RESULTS"

run_scan() {
    local SCAN=$1
    local OPTS=$2

    echo ""
    echo "============================================"
    echo "  BDT Scan ${SCAN}"
    echo "============================================"

    # --- 1. Generate modified training macro for this scan ---
    python3 - <<PYEOF
import re

with open('TMVAClassification.C', 'r') as f:
    content = f.read()

# Replace the entire if(Use["BDT"]) block with a clean single-option version
new_block = '''   if (Use["BDT"])  // Scan ${SCAN}
      factory->BookMethod( dataloader, TMVA::Types::kBDT, "BDT", "${OPTS}" );'''

content = re.sub(
    r'   if \(Use\["BDT"\]\).*?\n   \}',
    new_block,
    content,
    flags=re.DOTALL
)

content = content.replace('int TMVAClassification(', 'int TMVAClassification_scan(', 1)
content = content.replace('return TMVAClassification(', 'return TMVAClassification_scan(', 1)

with open('TMVAClassification_scan.C', 'w') as f:
    f.write(content)

print("Generated TMVAClassification_scan.C for Scan ${SCAN}")
PYEOF

    # --- 2. Train ---
    echo "--- Training ---"
    root -l -b -q TMVAClassification_scan.C 2>&1 | tee train_scan${SCAN}.log
    cp train_scan${SCAN}.log tmva_outputs/train_scan${SCAN}.log
    AUC=$(grep -o "ROC-integ.*: [0-9.]*" train_scan${SCAN}.log | tail -1 | awk '{print $NF}')
    [ -z "$AUC" ] && AUC="N/A"
    echo "  AUC = $AUC"

    # --- Save per-scan TMVA output for ROC comparison ---
    cp TMVA_allBkg_Mutau_2018.root tmva_outputs/TMVA_scan${SCAN}.root
    echo "  Saved tmva_outputs/TMVA_scan${SCAN}.root"

    # --- 3. Apply BDT to all samples ---
    echo "--- Applying BDT to all samples ---"
    bash run_bdt_all.sh

    # --- 4. Build shape histograms ---
    echo "--- Building shapes ---"
    root -l -b -q save_shapes.cpp

    # --- 5. Run Combine (blinded) ---
    echo "--- Running limits (Scan ${SCAN}) ---"
    text2workspace.py datacards/mutau_bdt_shape.txt \
        -o datacards/ws_scan${SCAN}.root -m 120

    combine -M AsymptoticLimits datacards/ws_scan${SCAN}.root \
        -m 120 -n MuTau_scan${SCAN} --cl 0.95 \
        > combine_scan${SCAN}.log 2>&1
    cat combine_scan${SCAN}.log

    # --- 6. Fit diagnostics (best-fit signal strength) ---
    echo "--- FitDiagnostics (Scan ${SCAN}) ---"
    combine -M FitDiagnostics datacards/ws_scan${SCAN}.root \
        -m 120 -n MuTau_scan${SCAN} --saveShapes --saveWithUncertainties \
        --rMin -1 --rMax 1000 \
        > fitdiag_scan${SCAN}.log 2>&1
    cat fitdiag_scan${SCAN}.log

    # --- 7. Observed significance ---
    echo "--- Significance observed (Scan ${SCAN}) ---"
    combine -M Significance datacards/ws_scan${SCAN}.root \
        -m 120 -n MuTau_scan${SCAN} \
        > sig_obs_scan${SCAN}.log 2>&1
    cat sig_obs_scan${SCAN}.log

    # --- 8. Expected significance (Asimov) ---
    echo "--- Significance expected (Scan ${SCAN}) ---"
    combine -M Significance datacards/ws_scan${SCAN}.root \
        -m 120 -n MuTau_scan${SCAN}_exp -t -1 --expectSignal=1 \
        > sig_exp_scan${SCAN}.log 2>&1
    cat sig_exp_scan${SCAN}.log

    # --- 9. Extract limits and significance ---
    M2=$(grep  "Expected  2.5%" combine_scan${SCAN}.log | awk '{print $NF}')
    M1=$(grep  "Expected 16.0%" combine_scan${SCAN}.log | awk '{print $NF}')
    MED=$(grep "Expected 50.0%" combine_scan${SCAN}.log | awk '{print $NF}')
    P1=$(grep  "Expected 84.0%" combine_scan${SCAN}.log | awk '{print $NF}')
    P2=$(grep  "Expected 97.5%" combine_scan${SCAN}.log | awk '{print $NF}')
    OBS=$(grep "Observed Limit" combine_scan${SCAN}.log | awk '{print $NF}')
    SIG_OBS=$(grep "Significance:" sig_obs_scan${SCAN}.log | awk '{print $NF}')
    SIG_EXP=$(grep "Significance:" sig_exp_scan${SCAN}.log | awk '{print $NF}')
    MU_HAT=$(grep "Best fit r:" fitdiag_scan${SCAN}.log | awk '{print $4}')

    # --- 10. Record ---
    printf "%-6s | %-8s | %-12s | %-12s | %-12s | %-12s | %-12s | %-12s\n" \
        "${SCAN}" "$AUC" "$M2" "$M1" "$MED" "$P1" "$P2" "$OBS" >> "$RESULTS"

    echo ""
    echo "  Scan ${SCAN} done:"
    echo "    Limits:      Expected = $MED  Observed = $OBS"
    echo "    Significance: obs = ${SIG_OBS}sigma  exp = ${SIG_EXP}sigma"
    echo "    Best-fit mu: ${MU_HAT}"
}

SCANS_TO_RUN="${1:-ABC}"  # default: all three

[[ "$SCANS_TO_RUN" == *A* ]] && run_scan "A" "$OPTS_A"
[[ "$SCANS_TO_RUN" == *B* ]] && run_scan "B" "$OPTS_B"
[[ "$SCANS_TO_RUN" == *C* ]] && run_scan "C" "$OPTS_C"

echo ""
echo "============================================"
echo "  All scans complete. Results:"
echo "============================================"
cat "$RESULTS"
echo ""
echo "Logs: train_scanA.log, train_scanB.log, train_scanC.log"
echo "      combine_scanA.log, combine_scanB.log, combine_scanC.log"
