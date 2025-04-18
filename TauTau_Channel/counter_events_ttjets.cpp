#include <iostream>
#include <cmath>
#include <string>
#include <fstream>

using namespace std;

int main(){
    
    string a;
    double id_iso;
    double pt;
    double carga;
   
    double suma_id_iso=0;
    double suma_pt=0;
    double suma_carga=0;
    
    ifstream ifile;
    ifile.open("cortes_cinematicos_fase1_ttjets.txt");
    double weight =0.15; 
    
    while(ifile>>a>>a>>a>>a>>a>>a>>a>>a>>a>>a>>id_iso>>a>>a>>a>>a>>a>>carga>>a>>a>>a>>a>>a>>a>>a>>a>>a>>pt>>a>>a>>a>>a){
        
        suma_id_iso = suma_id_iso + id_iso;
        suma_pt = suma_pt + pt;
        suma_carga = suma_carga + carga;
        
        
    }
    
    cout << "Suma dos acontecimentos com e+tau ID e ISO: " << suma_id_iso << endl;
    cout << "Suma dos acontecimentos com pt suficiente: " << suma_pt << endl;
    cout << "Suma dos acontecimentos com carga oposta: " << suma_carga << endl << endl;

    cout << "VALORES PESADOS" << endl;

    cout << "Suma dos acontecimentos com e+tau ID e ISO: " << suma_id_iso*weight << endl;
    cout << "Suma dos acontecimentos com pt suficiente: " << suma_pt*weight << endl;
    cout << "Suma dos acontecimentos com carga oposta: " << suma_carga*weight << endl << endl;

}

