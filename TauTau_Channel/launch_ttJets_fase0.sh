#!/bin/bash

# Initialize VOMS proxy
voms-proxy-init --rfc --voms cms --valid 172:00

# Compile with optimizations
g++ -O3 -pthread -o ttJets_fase0 ttJets_fase0.cpp `root-config --cflags --libs`

# Check compilation
if [ $? -ne 0 ]; then
    echo "Compilation failed!"
    exit 1
fi

# GNU parallel version - most efficient for CERN computing
echo "Processing files 159-354 using GNU parallel..."

# Create a function to process each file
process_single_file() {
    local file_num=$1
    echo "Starting file $file_num at $(date)"
    echo "$file_num" | ./ttJets_fase0
    local status=$?
    if [ $status -eq 0 ]; then
        echo "✓ File $file_num completed at $(date)"
    else
        echo "✗ File $file_num failed at $(date)"
    fi
    return $status
}

export -f process_single_file

# Run with parallel (adjust -j based on your resources)
# -j 8: 8 parallel jobs
# --progress: show progress bar
# --joblog: create log file
seq 159 354 | parallel -j 8 --progress --joblog parallel_log.txt process_single_file {}

echo "Processing complete!"
echo "Check parallel_log.txt for detailed job information"

# Optional: Show summary
echo ""
echo "=== SUMMARY ==="
total_jobs=$(seq 1 354 | wc -l)
successful_jobs=$(awk '$7==0 {count++} END {print count+0}' parallel_log.txt)
failed_jobs=$((total_jobs - successful_jobs))

echo "Total files: $total_jobs"
echo "Successful: $successful_jobs"
echo "Failed: $failed_jobs"

if [ $failed_jobs -gt 0 ]; then
    echo ""
    echo "Failed file numbers:"
    awk '$7!=0 {print $9}' parallel_log.txt
fi