voms-proxy-init --rfc --voms cms --valid 172:00

g++ -o QCD_fase0 QCD_fase0.cpp `root-config --cflags --glibs`

for i in {266..268}
do
  	echo "$i" | ./QCD_fase0
        clear
done
