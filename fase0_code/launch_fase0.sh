#!/bin/bash

voms-proxy-init --rfc --voms cms --valid 172:00

input="/eos/cms/store/user/jjhollar/TauTau_NanoAOD_Madalena/GammaGammaTauTau_SignalMC_SM_18UL_23k_NANOAODSIM.root"
echo $input
output="/eos/user/m/mblancco/samples_2018_tautau/fase0_2/test_file.root"
echo $output

g++ -o test_fase0 test_fase0.cpp $(root-config --cflags --glibs)


./test_fase0 $input $output --apply_id_filter --apply_charge_filter
