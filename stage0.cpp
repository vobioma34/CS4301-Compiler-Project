//Esai Barron and Victor Obioma
//CS 4301
//Stage 0

#include <stage0.h>
#include <iostream>
#include <ctime>
#include <string>

using namespace std;

//constructor
Compiler::Compiler(char **argv) {
    //open source file using argv[1]
    sourceFile.open(argv[1]);
    //open listing file using argv[2]
    listingFile.open(argv[2]);
    //open objectFile using argv[3]
    objectFile.open(argv[3]);

}

//destructor
Compiler::~Compiler(){
    sourceFile.close();
    listingFile.close();
    objectFile.close();
}

void Compiler::createListingHeader(){
    time_t result = time(nullptr);
	listingFile << "STAGE0:" << "Esai Baron and Victor Obioma " << ctime(&result) << endl;
	listingFile << "LINE NO:" << "SOURCE STATEMENT" << endl;
}

//"new int main() - Womack 2022"
void Compiler::parser() {
    //cout << "sussy balls" << endl;
}

void Compiler::createListingTrailer(){
    listingFile << "COMPILATION TERMINATED" << "# ERRORS ENCOUNTERED" << endl;
}

void Compiler:: processError(string err) {
    //output error to listing file and call exit()
    listingFile << err;
    exit(0);
}

char Compiler :: nextChar(){
    //char nextChar; 
    //read in next char
    //sourceFile.get(ch);
    if(sourceFile.eof()){
        ch = END_OF_FILE;
    }
    else {
        sourceFile.get(ch);
    }
    listingFile << ch;
    return ch;
}

string Compiler :: nextToken() {
	token = "";
	while (token == "")
	{
		if (ch == '{') { // process comment
			while (nextChar() != END_OF_FILE || nextChar() != '}') {
				// Empty body
			}
			if (ch == END_OF_FILE) {
				processError("unexpected end of file");
			} else {
				nextChar();
			}
		}
		else if (ch == '}') {
			processError("'}' cannot begin token");
		}
		else if (isspace(ch) == true) {
			nextChar();
		}
		else if (isSpecialSymbol(ch) == true) {
			token = ch;
			nextChar();
		}
		else if (islower(ch) == true) {
			token = ch;
			while (((nextChar() == '_') || (isalpha(nextChar())) || (isdigit(nextChar()))) && (nextChar() != END_OF_FILE)) 
			{
				token += ch;
			}
			if (ch == END_OF_FILE) {
				processError("unexpected end of file");
			}
		}
		else if (isdigit(ch) == true) {
			token = ch;
			while ((isdigit(nextChar()) == true) && (nextChar() != END_OF_FILE))
			{
				token += ch;
			}
			if (ch == END_OF_FILE) {
				processError("unexpected end of file");
			}
		}
		else if (ch == END_OF_FILE) {
			token =  ch;
		}
		else {
			processError("illegal symbol");
		}
	}
	return token;
}
char Compiler::nextChar()
{
	//char nextChar; 
    //read in next char
    //sourceFile.get(ch);
    if(sourceFile.eof()){
        ch = END_OF_FILE;
    }
    else {
        sourceFile.get(ch);
    }
    listingFile << ch;
    return ch;
}



