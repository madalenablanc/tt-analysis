#!/usr/bin/env python3
"""
Plot BDT output distributions for MuTau Channel
Creates publication-quality stacked histograms similar to plot_m.py style
"""

import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
import matplotlib.patches as mpatches
from pathlib import Path
import argparse

# Set style
plt.style.use('seaborn-v0_8-paper')
plt.rcParams['font.size'] = 11
plt.rcParams['axes.labelsize'] = 12
plt.rcParams['axes.titlesize'] = 14
plt.rcParams['xtick.labelsize'] = 11
plt.rcParams['ytick.labelsize'] = 11
plt.rcParams['legend.fontsize'] = 10
plt.rcParams['figure.titlesize'] = 14


def load_bdt_results(file_path):
    """Load BDT results from pickle file"""
    print(f"Loading {file_path}")
    df = pd.read_pickle(file_path)
    return df


def create_stacked_histogram(samples, output_dir, log_scale=True):
    """
    Create stacked histogram of BDT output
    Similar style to plot_m.py
    """

    fig, ax = plt.subplots(figsize=(10, 8))

    # Define bins
    bins = np.linspace(0, 1, 30)
    bin_centers = (bins[:-1] + bins[1:]) / 2
    bin_width = bins[1] - bins[0]

    # Prepare backgrounds (stacked)
    bkg_samples = ['ttjets', 'qcd', 'dy']
    bkg_labels = {'ttjets': r't $\bar{t}$', 'qcd': 'QCD (Data driven)', 'dy': 'Drell-Yan'}
    bkg_colors = {'ttjets': '#2ecc71', 'qcd': '#e74c3c', 'dy': '#f39c12'}

    bkg_hists = []
    bkg_names = []
    bkg_color_list = []

    for sample_name in bkg_samples:
        if sample_name in samples:
            df = samples[sample_name]
            weights = df['weight'].abs().values if 'weight' in df.columns else np.ones(len(df))

            hist, _ = np.histogram(df['bdt_score'].values, bins=bins, weights=weights)
            bkg_hists.append(hist)
            bkg_names.append(bkg_labels[sample_name])
            bkg_color_list.append(bkg_colors[sample_name])

    # Stack backgrounds
    ax.hist([samples[name]['bdt_score'].values for name in bkg_samples if name in samples],
            bins=bins,
            weights=[samples[name]['weight'].abs().values if 'weight' in samples[name].columns
                     else np.ones(len(samples[name])) for name in bkg_samples if name in samples],
            stacked=True,
            label=[bkg_labels[name] for name in bkg_samples if name in samples],
            color=[bkg_colors[name] for name in bkg_samples if name in samples],
            edgecolor='black',
            linewidth=0.5,
            alpha=0.8)

    # Add total background uncertainty
    if bkg_hists:
        total_bkg = np.sum(bkg_hists, axis=0)
        bkg_error = np.sqrt(total_bkg)  # Poisson errors

        # Error band
        ax.bar(bin_centers, 2*bkg_error, bottom=total_bkg-bkg_error,
               width=bin_width, color='gray', alpha=0.4, hatch='///',
               label='Stat. uncertainty', edgecolor='none')

    # Add data
    if 'data' in samples:
        df_data = samples['data']
        weights_data = df_data['weight'].abs().values if 'weight' in df_data.columns else np.ones(len(df_data))

        hist_data, _ = np.histogram(df_data['bdt_score'].values, bins=bins, weights=weights_data)
        data_error = np.sqrt(hist_data)

        ax.errorbar(bin_centers, hist_data, yerr=data_error,
                   fmt='o', color='black', markersize=6,
                   label='Data', linewidth=1.5, capsize=3)

    # Add signal (overlaid, scaled)
    if 'signal' in samples:
        df_signal = samples['signal']
        weights_signal = df_signal['weight'].abs().values if 'weight' in df_signal.columns else np.ones(len(df_signal))

        # Scale signal for visibility
        signal_scale = 5000
        weights_signal_scaled = weights_signal * signal_scale

        ax.hist(df_signal['bdt_score'].values, bins=bins, weights=weights_signal_scaled,
               histtype='step', color='black', linewidth=2.5,
               label=f'Signal (×{signal_scale})', linestyle='-')

    # Formatting
    ax.set_xlabel('BDT Output', fontsize=13, fontweight='bold')
    ax.set_ylabel('Events', fontsize=13, fontweight='bold')
    ax.set_title('BDT Output Distribution - MuTau Channel', fontsize=15, fontweight='bold', pad=20)

    if log_scale:
        ax.set_yscale('log')
        ax.set_ylim(bottom=0.1)
    else:
        ax.set_ylim(bottom=0)

    ax.grid(True, alpha=0.3, linestyle='--', linewidth=0.5)
    ax.legend(loc='upper right', frameon=True, framealpha=0.95, edgecolor='black')

    # Add CMS-style labels
    ax.text(0.02, 0.96, 'CMS-TOTEM', transform=ax.transAxes,
            fontsize=16, fontweight='bold', va='top')
    ax.text(0.02, 0.91, 'Preliminary', transform=ax.transAxes,
            fontsize=12, style='italic', va='top')
    ax.text(0.98, 0.96, r'54.9 fb$^{-1}$ (13 TeV)', transform=ax.transAxes,
            fontsize=12, va='top', ha='right')

    plt.tight_layout()

    # Save
    output_file = output_dir / 'bdt_output_stacked.png'
    plt.savefig(output_file, dpi=300, bbox_inches='tight')
    print(f"Saved: {output_file}")

    if not log_scale:
        output_file_linear = output_dir / 'bdt_output_stacked_linear.png'
        plt.savefig(output_file_linear, dpi=300, bbox_inches='tight')
        print(f"Saved: {output_file_linear}")

    plt.close()


def create_normalized_comparison(samples, output_dir):
    """Create normalized comparison plot (signal vs background)"""

    fig, ax = plt.subplots(figsize=(10, 8))

    bins = np.linspace(0, 1, 50)

    # Signal
    if 'signal' in samples:
        df_signal = samples['signal']
        weights_signal = df_signal['weight'].abs().values if 'weight' in df_signal.columns else np.ones(len(df_signal))

        ax.hist(df_signal['bdt_score'].values, bins=bins, weights=weights_signal,
               histtype='step', color='blue', linewidth=2.5, density=True,
               label='Signal', linestyle='-', alpha=0.8)

    # Combined background
    bkg_samples = ['ttjets', 'qcd', 'dy']
    all_bkg_scores = []
    all_bkg_weights = []

    for sample_name in bkg_samples:
        if sample_name in samples:
            df = samples[sample_name]
            weights = df['weight'].abs().values if 'weight' in df.columns else np.ones(len(df))

            all_bkg_scores.extend(df['bdt_score'].values)
            all_bkg_weights.extend(weights)

    if all_bkg_scores:
        ax.hist(all_bkg_scores, bins=bins, weights=all_bkg_weights,
               histtype='stepfilled', color='red', linewidth=2, density=True,
               label='Background (combined)', alpha=0.5, edgecolor='darkred')

    ax.set_xlabel('BDT Output', fontsize=13, fontweight='bold')
    ax.set_ylabel('Normalized Events', fontsize=13, fontweight='bold')
    ax.set_title('BDT Signal vs Background Separation', fontsize=15, fontweight='bold', pad=20)
    ax.set_yscale('log')
    ax.grid(True, alpha=0.3, linestyle='--', linewidth=0.5)
    ax.legend(loc='upper center', frameon=True, framealpha=0.95, edgecolor='black', fontsize=12)

    # Add labels
    ax.text(0.02, 0.96, 'CMS-TOTEM', transform=ax.transAxes,
            fontsize=16, fontweight='bold', va='top')
    ax.text(0.02, 0.91, 'Preliminary', transform=ax.transAxes,
            fontsize=12, style='italic', va='top')

    plt.tight_layout()

    output_file = output_dir / 'bdt_signal_vs_background.png'
    plt.savefig(output_file, dpi=300, bbox_inches='tight')
    print(f"Saved: {output_file}")
    plt.close()


def create_summary_table(samples, output_dir):
    """Create summary table of BDT scores"""

    summary = []

    for name, df in samples.items():
        bdt_scores = df['bdt_score'].values
        weights = df['weight'].abs().values if 'weight' in df.columns else np.ones(len(df))

        summary.append({
            'Sample': name.capitalize(),
            'Events': len(df),
            'Weighted Events': f"{np.sum(weights):.2f}",
            'Mean BDT': f"{np.average(bdt_scores, weights=weights):.4f}",
            'Median BDT': f"{np.median(bdt_scores):.4f}",
            'BDT > 0.5': f"{np.sum(weights[bdt_scores > 0.5]):.2f}",
            'BDT > 0.7': f"{np.sum(weights[bdt_scores > 0.7]):.2f}",
            'BDT > 0.9': f"{np.sum(weights[bdt_scores > 0.9]):.2f}",
        })

    summary_df = pd.DataFrame(summary)

    print("\n" + "="*80)
    print("BDT Score Summary")
    print("="*80)
    print(summary_df.to_string(index=False))
    print("="*80)

    # Save to file
    output_file = output_dir / 'bdt_summary.txt'
    with open(output_file, 'w') as f:
        f.write("BDT Score Summary - MuTau Channel\n")
        f.write("="*80 + "\n")
        f.write(summary_df.to_string(index=False))
        f.write("\n" + "="*80 + "\n")

    print(f"\nSaved summary to {output_file}")


def main():
    parser = argparse.ArgumentParser(description='Plot BDT output for MuTau channel')
    parser.add_argument('--dy', help='Path to DY BDT output pickle')
    parser.add_argument('--qcd', help='Path to QCD BDT output pickle')
    parser.add_argument('--ttjets', help='Path to TTJets BDT output pickle')
    parser.add_argument('--signal', help='Path to Signal BDT output pickle')
    parser.add_argument('--data', help='Path to Data BDT output pickle')
    parser.add_argument('--output-dir', default='bdt_plots', help='Output directory for plots')
    parser.add_argument('--log', action='store_true', help='Use log scale for y-axis')

    args = parser.parse_args()

    # Create output directory
    output_dir = Path(args.output_dir)
    output_dir.mkdir(exist_ok=True)

    print("="*60)
    print("MuTau Channel - Plot BDT Output")
    print("="*60)

    # Load samples
    samples = {}

    if args.dy:
        samples['dy'] = load_bdt_results(args.dy)
    if args.qcd:
        samples['qcd'] = load_bdt_results(args.qcd)
    if args.ttjets:
        samples['ttjets'] = load_bdt_results(args.ttjets)
    if args.signal:
        samples['signal'] = load_bdt_results(args.signal)
    if args.data:
        samples['data'] = load_bdt_results(args.data)

    if not samples:
        print("Error: No input samples provided!")
        return

    print(f"\nLoaded {len(samples)} samples")

    # Create plots
    print("\nCreating plots...")
    create_stacked_histogram(samples, output_dir, log_scale=args.log)
    create_normalized_comparison(samples, output_dir)
    create_summary_table(samples, output_dir)

    print("\n" + "="*60)
    print("Plotting Complete!")
    print(f"Output directory: {output_dir.absolute()}")
    print("="*60)


if __name__ == "__main__":
    main()
