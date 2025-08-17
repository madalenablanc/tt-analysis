import numpy as np
import uproot
import os.path
from subprocess import call
import matplotlib.pyplot as plt
import awkward.operations as ak
import sys
import os
import pandas as pd
import seaborn as sns


# dir="/lstore/cms/boletti/ntuples/"
# filename="2018Data_passPreselection_passSPlotCuts_mergeSweights.root"

dir="/eos/user/m/mblancco/samples_2018_mutau/ficheiros_fase1/"
filename="merged.root"
#filename="ttJetscode_GammaGammaTauTau_SignalMC_SM_18UL_23k_NANOAODSIM_fase0_with_xi_and_deltaphi.root"
#ilename2="MC_JPSI_2018_preBDT_Nov21.root"

data = uproot.open(dir+filename)
#data_mc=uproot.open(dir+filename2)

variables_path = 'variables2.txt'
df = pd.read_csv(variables_path,header=0)
#df.columns = df.columns.str.strip()
print(df)

#Tree=data["ntuple"]
#Tree_mc=data_mc["ntuple"]

def plot_heatmap(data_type):

    if data_type=="signal":
        Tree=data["tree_out"]
    elif data_type=="background":
        Tree=data_mc["tree_out"]
    else:
        print("not valid value")
        
   #read all variables
    combined_signal_df = pd.DataFrame()
    df["var_name"]=df["var_name"].str.strip()
    for v in df["var_name"]:  
        print(v)
        #composite_value = df.loc[df["var_name"] == v, "composite"].iloc[0]
        value = df.loc[df["var_name"] == v].iloc[0]
        print(value)
        signal=Tree.arrays(v,library="pd")

        # if value==0:
        #     signal=Tree.arrays(v,library="pd")
        # elif (value)==1:
        #     signal=Tree.arrays(v,aliases={v:v},library="pd")
        combined_signal_df = pd.concat([combined_signal_df, signal], axis=1)
    
    correlation_matrix = combined_signal_df.corr()

    # Create a heatmap using seaborn
    plt.figure(figsize=(15, 15))  # Set the figure size (optional)
    color = plt.get_cmap('RdYlGn')   # default color
    color.set_bad('lightblue')  
    sns.heatmap(correlation_matrix, annot=True, fmt=".2f",cmap=color, center=0)

    # Add labels and title to the heatmap
    plt.title('Correlation Heatmap: '+data_type)
    plt.xlabel('Variables')
    plt.ylabel('Variables')
    

    #remove error handling if it is not used
    while True:
        try:
            plt.savefig(data_type+'_correlation_all_background.png')
            break
        except FileNotFoundError:
            print("bad name")
            v=v.replace("/", "_div_")

plot_heatmap("signal")
#plot_heatmap("background")