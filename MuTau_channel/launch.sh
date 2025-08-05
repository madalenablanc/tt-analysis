voms-proxy-init --rfc --voms cms --valid 172:00

g++ -o nanotry_ttjets nanotry_ttjets.cpp `root-config --cflags --glibs`

for i in {1..354}
do
  	echo "$i" | ./nanotry_ttjets >> cortes_ttjets_2018_UL_ETau_1.txt
      
done
