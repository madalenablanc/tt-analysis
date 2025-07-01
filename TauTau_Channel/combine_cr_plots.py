#!/usr/bin/env python3
"""
Script to combine all 12 control region plots into a single figure
Creates a 3x4 grid: DY (top), TT (middle), QCD (bottom) × 4 variables
"""

import matplotlib.pyplot as plt
import matplotlib.image as mpimg
import numpy as np
from matplotlib.patches import Rectangle
import os

def combine_control_region_plots():
    """
    Combine all control region plots into a single figure matching the reference layout
    """
    
    # Define the plot files in the correct order
    plot_files = {
        # DY Control Region (top row)
        'DY': [
            'output_plots/DY_CR_acoplanarity.png',
            'output_plots/DY_CR_mass.png', 
            'output_plots/DY_CR_rapidity.png',
            'output_plots/DY_CR_pt.png'
        ],
        # TT Control Region (middle row)
        'TT': [
            'output_plots/TT_CR_acoplanarity.png',
            'output_plots/TT_CR_mass.png',
            'output_plots/TT_CR_rapidity.png',
            'output_plots/TT_CR_pt.png'
        ],
        # QCD Control Region (bottom row)  
        'QCD': [
            'output_plots/QCD_CR_acoplanarity.png',
            'output_plots/QCD_CR_mass.png',
            'output_plots/QCD_CR_rapidity.png', 
            'output_plots/QCD_CR_pt.png'
        ]
    }
    
    # Check if all files exist
    missing_files = []
    for region, files in plot_files.items():
        for file in files:
            if not os.path.exists(file):
                missing_files.append(file)
    
    if missing_files:
        print("Missing plot files:")
        for file in missing_files:
            print(f"  - {file}")
        print("Please run the ROOT analysis first to generate all plots.")
        return
    
    # Create the combined figure
    fig, axes = plt.subplots(3, 4, figsize=(20, 15))
    fig.suptitle('τₕτₕ final state: DY (top row), TT (middle row), QCD (bottom row) CRs.\n' + 
                 'For each CR, distributions of acoplanarity, τₕτₕ invariant mass, rapidity and transverse momentum of\n' +
                 'the central system are shown. In the lower panels the data-to-simulation ratio is shown. Only\n' +
                 'statistical uncertainties are shown.', 
                 fontsize=14, y=0.98)
    
    # Row labels
    row_labels = ['DY CR', 'TT CR', 'QCD CR']
    regions = ['DY', 'TT', 'QCD']
    
    # Load and display each plot
    for row, (region, files) in enumerate(zip(regions, [plot_files['DY'], plot_files['TT'], plot_files['QCD']])):
        for col, plot_file in enumerate(files):
            try:
                # Load the image
                img = mpimg.imread(plot_file)
                
                # Display in the subplot
                axes[row, col].imshow(img)
                axes[row, col].axis('off')
                
                # Add column headers only on the top row
                if row == 0:
                    column_titles = [
                        'Acoplanarity of the central system',
                        'Invariant mass of the central system [GeV]', 
                        'Rapidity of the central system',
                        'Transverse momentum of the central system [GeV]'
                    ]
                    axes[row, col].set_title(column_titles[col], fontsize=12, pad=10)
                
                print(f"✓ Loaded {plot_file}")
                
            except Exception as e:
                print(f"✗ Error loading {plot_file}: {e}")
                axes[row, col].text(0.5, 0.5, f'Error loading\n{plot_file}', 
                                   ha='center', va='center', transform=axes[row, col].transAxes)
                axes[row, col].axis('off')
    
    # Add row labels on the left side
    for row, label in enumerate(row_labels):
        fig.text(0.02, 0.85 - row*0.28, label, fontsize=14, fontweight='bold', 
                rotation=90, va='center', ha='center')
    
    # Adjust layout to reduce white space
    plt.tight_layout(rect=[0.03, 0.03, 0.97, 0.94])
    
    # Remove spacing between subplots
    plt.subplots_adjust(hspace=0.05, wspace=0.05)
    
    # Save the combined figure
    output_file = 'combined_control_regions.png'
    plt.savefig(output_file, dpi=300, bbox_inches='tight', facecolor='white')
    print(f"\n✓ Combined plot saved as: {output_file}")
    
    # Also save as PDF for better quality
    output_pdf = 'combined_control_regions.pdf'
    plt.savefig(output_pdf, bbox_inches='tight', facecolor='white')
    print(f"✓ Combined plot saved as: {output_pdf}")
    
    # Show the plot
    plt.show()

def create_summary_info():
    """
    Create a text summary of the control region definitions
    """
    summary = """
Control Region Definitions Used (in order: DY, TT, QCD):

• DY CR: τₕτₕ invariant mass M_μτ ∈ [40,100] GeV + acoplanarity a < 0.3 + |η| < 2.4 + 
         2 isolated τₕ, opposite sign leptons + N_b-jets = 0. By choosing events with dilepton 
         invariant mass below 100 GeV allows to select events from the Z boson decays;

• TT CR: at least one b-jet + acoplanarity a > 0.5 + |η| < 2.4 + 2 isolated τₕ of opposite 
         charge + M_τₕτₕ ∈ [200,650] GeV + p_T(τₕ⁺τₕ⁻) < 125 GeV.

• QCD CR: M_τₕτₕ ∈ [100,300] GeV + N_b-jets = 0 + |η| < 2.4 + 2 isolated τₜ's of same 
          sign + system acoplanarity a > 0.8 + p_T(τₕ⁺τₕ⁻) < 75 GeV;

Note: All cuts use tau_mass for selection, but:
- DY plots tau_mass (hence ~70-100 GeV range in histograms)
- QCD/TT plot sist_mass (hence ~400-1000 GeV range in histograms)
"""
    
    with open('control_regions_summary.txt', 'w') as f:
        f.write(summary)
    
    print("✓ Control region summary saved as: control_regions_summary.txt")

if __name__ == "__main__":
    print("Combining control region plots...")
    print("=" * 50)
    
    # Combine the plots
    combine_control_region_plots()
    
    # Create summary info
    create_summary_info()
    
    print("=" * 50)
    print("Done! Check the generated files:")
    print("  - combined_control_regions.png (high resolution)")
    print("  - combined_control_regions.pdf (publication quality)")
    print("  - control_regions_summary.txt (definitions)")
    print("Layout: DY (top), TT (middle), QCD (bottom)")