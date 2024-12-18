
#!/bin/bash

voms-proxy-init --rfc --voms cms --valid 172:00

g++ -o mc_fase1_try mc_fase1_try.cpp $(root-config --cflags --glibs)

#for i in {1..354}
#do
#	echo "$i" | ./ttJets_fase0
#	clear
#done

./mc_fase1_try
