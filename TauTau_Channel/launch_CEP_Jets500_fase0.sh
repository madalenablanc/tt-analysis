voms-proxy-init --rfc --voms cms --valid 172:00

g++ -o CEP_Jets500_fase0 CEP_Jets500_fase0.cpp `root-config --cflags --glibs`

for i in {2}
do
  	echo "$i" | ./CEP_Jets500_fase0
        clear
done
