voms-proxy-init --rfc --voms cms --valid 172:00

g++ -o nanotry_CEP_Jets500 nanotry_CEP_Jets500.cpp `root-config --cflags --glibs`

for i in {1..1}
do
  	echo "$i" | ./nanotry_CEP_Jets500
        clear
done
