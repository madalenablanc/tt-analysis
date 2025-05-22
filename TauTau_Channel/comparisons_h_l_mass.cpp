#include <iostream>
#include <cmath>
#include "TApplication.h"
#include "TGraph.h"
#include "TCanvas.h"
#include <fstream>

using namespace std;

int main(){

	TApplication app("app", NULL, NULL);

	TGraph gr1;
	TGraph gr2;

	ifstream ifile_low,ifile_high;

	ifile_low.open("cross_sections_low_masses.txt");
	ifile_high.open("cross_section_bsm_last.txt");

	double ind, sigma;
	int i=0;
	int j=0;

	while (ifile_low>> ind >> sigma){

		double C = -40 + 0.8*double(ind-1);
		gr1.SetPoint(i,2.*1.777*(C)/(0.3*13042.8),sigma);
		i++;
	}

	while (ifile_high>> ind >> sigma){

		double C = -40 + 0.8*double(ind-1);
                gr2.SetPoint(j,2.*1.777*(C)/(0.3*13042.8),sigma);
                j++;
        }


	TCanvas c1;

	gr1.Draw("AP");
	gr2.Draw("SAME");


	TCanvas c2;

	TGraph gr3;
	TGraph gr4;

	for(int k=0; k<=i; k++){

		gr3.SetPoint(k,gr1.GetPointX(k), gr1.GetPointY(k)/gr1.GetPointY(50));

	}

	for(int k=0; k<j; k++){

                gr4.SetPoint(k,gr2.GetPointX(k), gr2.GetPointY(k)/gr2.GetPointY(50));

        }

	gr3.SetMarkerColor(2);
	gr4.SetLineColor(4);

	gr3.Draw("AP");
	gr4.Draw("SAME");

	app.Run("true");
	return 0;

}
