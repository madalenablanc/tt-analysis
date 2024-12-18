voms-proxy-init --rfc --voms cms --valid 172:00

g++ -o dy_fase1 dy_fase1.cpp `root-config --cflags --glibs`

./dy_fase1

