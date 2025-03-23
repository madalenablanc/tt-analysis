voms-proxy-init --rfc --voms cms --valid 172:00

g++ -o nanotry_fase1_ttjets nanotry_fase1_ttjets.cpp `root-config --cflags --glibs`

for i in {1..354}
do
  	echo "$i" | ./nanotry_fase1_ttjets >> cortes_cinematicos_fase1_ttjets.txt
        clear
done
