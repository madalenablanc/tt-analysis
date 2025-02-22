#include <stdio.h>
#include <iostream>
#include <cmath>
#include "TApplication.h"
#include "TFile.h"
#include "TTree.h"
#include "TLeaf.h"
#include "TH1D.h"
#include "TLorentzVector.h"
#include <string>
#include <fstream>
#include <cstdlib>
#include <sstream>

void myFunction();

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Usage: %s <input ROOT file> <output ROOT file> [options]\n", argv[0]);
        return 1;
    }

    //printf(argv[1]);

    //TODO: use gnu getopt or another thing to parse command
    // line options when running this file to select which "filter" to apply 
    //https://medium.com/@mostsignificant/3-ways-to-parse-command-line-arguments-in-c-quick-do-it-yourself-or-comprehensive-36913284460f
    myFunction();  // call the function
    return 0;
}    

void myFunction() {
    std::cout << "I just got executed!\n";
  }