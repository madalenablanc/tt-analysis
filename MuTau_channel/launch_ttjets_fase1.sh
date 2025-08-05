voms-proxy-init --rfc --voms cms --valid 172:00

g++ -o open_ttjets open_ttjets.cpp `root-config --cflags --glibs`

for i in {1..352}
do
  	echo "$i" | ./open_ttjets >> cortes_ttjets_2018_UL_MuTau.txt
        clear
done
