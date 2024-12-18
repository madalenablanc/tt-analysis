voms-proxy-init --rfc --voms cms --valid 172:00

g++ -o cep_fase0_2 cep_fase0_2.cpp `root-config --cflags --glibs`

./cep_fase0_2
