
#!/bin/bash

voms-proxy-init --rfc --voms cms --valid 172:00

g++ -o ttJets_fase0 ttJets_fase0.cpp $(root-config --cflags --glibs)

#for i in {1..354}
#do
#	echo "$i" | ./ttJets_fase0
#	clear
#done

./ttJets_fase0