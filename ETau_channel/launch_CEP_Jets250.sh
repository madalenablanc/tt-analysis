voms-proxy-init --rfc --voms cms --valid 172:00

g++ -o nanotry_CEP_Jets250_UL nanotry_CEP_Jets250_UL.cpp `root-config --cflags --glibs`

for i in {1..1}
do
  	echo "$i" | ./nanotry_CEP_Jets250_UL
        clear
done

