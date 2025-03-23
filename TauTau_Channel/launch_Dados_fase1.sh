voms-proxy-init --rfc --voms cms --valid 172:00

g++ -o nanotry_fase1_Dados nanotry_fase1_Dados.cpp `root-config --cflags --glibs`

for i in {1..268}
do
  	echo "$i" | ./nanotry_fase1_Dados >> cortes_cinematicos_fase1_Dados.txt
        clear
done
