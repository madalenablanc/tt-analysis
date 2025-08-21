voms-proxy-init --rfc --voms cms --valid 172:00

g++ -O2 -std=c++17 nanotry_data.cpp $(root-config --cflags --libs) -o nanotry_data

./nanotry_data

