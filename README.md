# tt-analysis

Instructions to run analysis

### Key points:
- ttjets, dy, signal are **simulated** samples;
- qcd and data samles are **real** data - qcd is data driven;

### Changes from original analysis to new framework:
- In original analysis, 
- switch from C++ to python 

___

## MuTau Channel
### Old analysis


``` 
#Process phase0 samples - starts from NANOAOD
./launch_dy.sh # Drell-Yan background runs nanotry_DY.cpp
./launch_qcd.sh # QCD background runs nanotry_QCD.cpp
./launch.sh # Ttjets background runs nanotry_ttjets.cpp
./launch_data.sh  #Data sample runs nanotry_data.cpp

#Processes phase1 samples
./launch_DY_fase1.sh # runs open_DY.cpp
./launch_QCD_fase1.sh # runs open_QCD.cpp
./launch_ttjets_fase1.sh # runs open_ttjets.cpp
./launch_data_fase1.sh # runs open.cpp

#Creates proton pool to enrich simulated samples
g++ -O2 -std=c++17 proton_pool_MuGamma.cpp $(root-config --cflags --libs) -o proton_pool
./proton_pool
```
**Signal Samples**
```
# runs processing of signal sample starting from MINIAOD
g++ -O2 -std=c++17 sinal.cpp $(root-config --cflags --libs) -o sinal 
./sinal
```


### New analysis
```
# Phase0 samples
python3 fase0_dy.py
python3 fase0_qcd_data.py
python3 fase0_ttjets.py

#Phase1 samples
python3 fase1_dy.py
python3 fase1_qcd.py
python3 fase1_ttjets.py
python3 fase1_data.py
```

### BDT Training


---
## TauTauh Channel
### Old analysis


### New analysis


--- 