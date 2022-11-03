//Esai Barron and Victor Obioma
//CS 4301
//Stage 0

#include <stage0.h>
#include <iostream>

using namespace std;

//constructor
Compiler(char **argv) {
    //open source file using argv[1]
    sourceFile.open(argv[1]);
    //open listing file using argv[2]
    listingFile.open(argv[2]);
    //open objectFile using argv[3]
    objectFile.open(argv[3]);

}

//destructor
~Compiler(){
    sourceFile.close();
    listingFile.close();
    objectFile.close();
}

void createListingHeader(){

    //GET CODE FROM VICTOR/GITHUB
}

