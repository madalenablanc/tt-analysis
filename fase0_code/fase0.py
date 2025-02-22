import argparse
import ROOT
import numpy as np

def parse_arguments():
    parser = argparse.ArgumentParser(description="ROOT data processing script with filtering options.")
    parser.add_argument("--input", type=str, required=True, help="Path to the input ROOT file")
    parser.add_argument("--output", type=str, required=True, help="Path to the output ROOT file")
    parser.add_argument("--apply_id_filter", action="store_true", help="Apply tau ID filter")
    parser.add_argument("--apply_charge_filter", action="store_true", help="Apply charge filter")
    parser.add_argument("--apply_pt_filter", action="store_true", help="Apply pT filter")
    parser.add_argument("--apply_eta_filter", action="store_true", help="Apply eta filter")
    return parser.parse_args()