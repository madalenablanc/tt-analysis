#!/bin/bash

# Default values
fase=""
background=""

# Function to show usage
usage() {
    echo "Usage: $0 -f <fase0|fase1> -b <drell-yan|ttjets|qcd>"
    exit 1
}

# Parse options using getopts
while getopts ":f:b:" opt; do
    case "${opt}" in
        f)
            fase="${OPTARG}"
            ;;
        b)
            background="${OPTARG}"
            ;;
        *)
            usage
            ;;
    esac
done

# Validate inputs
if [[ -z "$fase" || -z "$background" ]]; then
    echo "Error: Missing required options."
    usage
fi

# Ensure fase is either "fase0" or "fase1"
if [[ "$fase" != "fase0" && "$fase" != "fase1" ]]; then
    echo "Error: Invalid -f option. Must be 'fase0' or 'fase1'."
    usage
fi

# Ensure background is valid
if [[ "$background" != "drell-yan" && "$background" != "ttjets" && "$background" != "qcd" ]]; then
    echo "Error: Invalid -b option. Must be 'drell-yan', 'ttjets', or 'qcd'."
    usage
fi

# ✅ Successfully parsed options
echo "Running with:"
echo "  - Fase: $fase"
echo "  - Background: $background"

# Example: Run different commands based on options
if [[ "$fase" == "fase0" ]]; then
    echo "Processing Fase 0 with $background"
    # Add your command for fase0
elif [[ "$fase" == "fase1" ]]; then
    echo "Processing Fase 1 with $background"
    # Add your command for fase1
fi
