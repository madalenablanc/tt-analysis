voms-proxy-init --rfc --voms cms --valid 172:00

g++ -o nanotry_fase1_DY nanotry_fase1_DY.cpp `root-config --cflags --glibs`

for i in {2..203}
do
  	echo "$i" | ./nanotry_fase1_DY >> cortes_DY_fase1_francisco.txt
        clear
done
