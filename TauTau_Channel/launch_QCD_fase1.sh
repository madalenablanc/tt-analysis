voms-proxy-init --rfc --voms cms --valid 172:00

g++ -o nanotry_fase1_QCD nanotry_fase1_QCD.cpp `root-config --cflags --glibs`

for i in {1..268}
do
  	echo "$i" | ./nanotry_fase1_QCD >> cortes_cinematicos_fase1_QCD.txt
        clear
done
