#include <iostream>
#include <cmath>
#include <string>
#include <fstream>

using namespace std;

int main(){
    
    string a,b;
    double id_iso;
    double pt;
    double carga;
    double help;

    double suma_id_iso=0;
    double suma_pt=0;
    double suma_carga=0;
    
    ifstream ifile;
    ifile.open("cortes_ttjets_2018_UL_MuTau.txt");
    double weight =1.;
    
    while(ifile>>a>>b){

	if(a!="progress:"){

        ifile>>a>>a>>a>>help>>a>>a>>a>>a>>a>>a>>a>>a>>a>>a>>a>>id_iso>>a>>a>>a>>a>>a>>a>>pt>>a>>a>>a>>a>>carga;

            suma_id_iso = suma_id_iso + id_iso;
            suma_pt = suma_pt + pt;
            suma_carga = suma_carga + carga;
	}
        
    }
    
    cout << "Suma dos acontecimentos com e+tau ID e ISO: " << suma_id_iso << endl;
    cout << "Suma dos acontecimentos com pt suficiente: " << suma_pt << endl;
    cout << "Suma dos acontecimentos com carga oposta: " << suma_carga << endl << endl;

    cout << "VALORES PESADOS" << endl;

    cout << "Suma dos acontecimentos com e+tau ID e ISO: " << suma_id_iso*weight << endl;
    cout << "Suma dos acontecimentos com pt suficiente: " << suma_pt*weight << endl;
    cout << "Suma dos acontecimentos com carga oposta: " << suma_carga*weight << endl << endl;

}

