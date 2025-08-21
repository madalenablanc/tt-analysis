voms-proxy-init --rfc --voms cms --valid 172:00

for i in {68..70}
do
  	echo "$i" | ./nanotry_QCD > cortes_QCD_2018_1.txt
        rm cortes_QCD_2018_*
done
