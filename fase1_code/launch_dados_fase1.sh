voms-proxy-init --rfc --voms cms --valid 172:00

g++ -o dados_fase1 dados_fase1.cpp `root-config --cflags --glibs`

./dados_fase1
