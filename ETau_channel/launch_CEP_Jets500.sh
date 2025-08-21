voms-proxy-init --rfc --voms cms --valid 172:00

g++ -o nanotry_CEP_Jets500_UL nanotry_CEP_Jets500_UL.cpp `root-config --cflags --glibs`

for i in {1..1}
do
  	echo "$i" | ./nanotry_CEP_Jets500_UL
        clear
done

