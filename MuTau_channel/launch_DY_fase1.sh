voms-proxy-init --rfc --voms cms --valid 172:00

g++ -o open_DY open_DY.cpp `root-config --cflags --glibs`

for i in {1..204}
do
  	echo "$i" | ./open_DY >> cortes_DY_2018_UL_MuTau.txt
        clear
done
