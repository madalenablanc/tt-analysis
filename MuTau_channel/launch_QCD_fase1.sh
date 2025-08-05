voms-proxy-init --rfc --voms cms --valid 172:00

for i in {357..389}
do
  	echo "$i" | ./open_QCD >> cortes_QCD_2018_UL_MuTau.txt
        clear
done
