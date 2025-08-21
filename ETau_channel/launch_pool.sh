voms-proxy-init --rfc --voms cms --valid 172:00

for i in {1..738}
do
  	echo "$i" | ./proton_pool_EGamma
        clear
done
