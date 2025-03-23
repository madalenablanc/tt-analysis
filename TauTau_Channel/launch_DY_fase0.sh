voms-proxy-init --rfc --voms cms --valid 172:00

g++ -o DY_fase0 DY_fase0.cpp `root-config --cflags --glibs`

for i in {159..204}
do
  	echo "$i" | ./DY_fase0
        clear
done
