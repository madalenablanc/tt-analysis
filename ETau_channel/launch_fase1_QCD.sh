voms-proxy-init --rfc --voms cms --valid 172:00

for i in {1..728}
do
  	echo "$i" | ./nanotry_fase1_QCD >> cortes_dados_fase1_francisco.txt
        clear
done
