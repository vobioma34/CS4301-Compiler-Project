//Esai Barron and Victor Obioma
//CS 4301
//Stage 0
#include <stage0.h>
#include <iostream>
#include <ctime>

using namespace std;

Compiler::Compiler(char **argv) 
{
   /*
   open sourceFile using argv[1]
   open listingFile using argv[2]
   open objectFile using argv[3]
   */
    //open source file using argv[1]
    sourceFile.open(argv[1]);
    //open listing file using argv[2]
    listingFile.open(argv[2]);
    //open objectFile using argv[3]
    objectFile.open(argv[3]);
}
Compiler::~Compiler()
{
	// close all open files
	sourceFile.close();
    listingFile.close();
    objectFile.close();
}
void Compiler::createListingHeader()
{
	time_t result = time(nullptr);
	cout << "STAGE0:" << "Esai Baron and Victor Obioma" << ctime(&result) << endl;
	cout << "LINE NO:" << "SOURCE STATEMENT" << endl;
}
// "the new int main()"
void Compiler::parser()
{
	
}
void Compiler::createListingTrailer()
{
	cout << "COMPILATION TERMINATED" << "# ERRORS ENCOUNTERED" << endl;
}



