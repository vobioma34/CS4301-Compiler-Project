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

/*----------------------helper functions frompage 4 of pseudo code-----------------*/
//determines if s is a keyword, true if s is a keyword and false otherwise
bool Compiler :: isKeyword(string s) const{
    string keywords[] = {"program", "const", "var", "integer", "boolean", 
                            "begin", "end", "true", "false", "not"};

    //go through keywords array and see if s matches nay key words
    for(int i = 0; i < 10; i++){ //change hardcoded 10 and ask Dr.Motl
        if (s == keywords[i]) {
            return true; //if s matches a keyword, return true
        }
    }
    return false; //s did not match any keywords
}
//determines if c is a special symbol, true is it is a special symbol otherwise false
bool Compiler :: isSpecialSymbol(char c) const {
    char specialSymbols[] = {':', ',', ';', '=', '+', '-', '.'};

    for(int i = 0; i < 7; i++){
         if (c == specialSymbols[i]) {
            return true; //if c matches a special symbol, return true
        }
    }

    return false; //c is not a special symbol
}

bool Compiler :: isNonKeyId(string s) const {
    return !isKeyword(s);
}

//determines if s is an integer
bool Compiler :: isInteger(string s) const {
    if(isdigit(stoi(s))){
        return true;
    }
    else{
        return false;
    }
}

bool Compiler :: isBoolean(string s) const{
    if (s == "boolean") {
        return true;
    }
    else {
        return false;
    }
}

bool Compiler :: isLiteral(string s) const {
    if(isBoolean(s) || isInteger(s)){
		return true;
	}
	return false;
}

//determines if s is a nonKeyid
bool Compiler :: isNonKeyId(string s) const {

	//make sure s starts with an alpha
	if(isalpha(s[0]) == false){
		return false;
	}
	//make sure s is not a keyword
	if(isKeyword(s)){
		return false;
	}

	//make sure all chars are lowercase
	for(uint i = 0; i < s.length(); i++){
		if(isdigit(s[i]) == false){
			if(islower(s[i]) == false){
				return false;
			}
		}
	}

	//make sure the 15th character is not an underscore
	if(s.length() >= 15){
		if(s[15] == '_'){
			return false;
		}
	}

	//make sure no special symbols are in s, with exceiption of underscore '_', also make sure no spaces in name
	int uscoreCount = 0;
    for(uint i = 0; i < s.length(); i++){
		//non key id can have oneunderscore
		if(s[i] == '_'){
			uscoreCount++;
			continue;
		}
		else if(isSpecialSymbol(s[i]) || uscoreCount > 1 || isspace(s[i])){
			return false;
		}
	}
	
	//dummy return to satify warning
	return false;
}

string Compiler::ids()
{
	string temp, tempString;
	if (!isNonKeyId(token)) {
		processError("non-keyword identifier expected");
	}
	tempString = token;
	temp = token;
	if (nextToken() == ",") {
		if (!isNonKeyId(nextToken())) {
			processError("non-keyword identifier expected");
		}
		tempString = temp;
		tempString += ",";
		tempString += ids();
	}
	return tempString;
}
storeTypes Compiler :: whichType(string name){
	//will need store types enum called dataType
	storeTypes dataType;

	if(isLiteral(name)){
		if(isBoolean(name)){
			dataType = BOOLEAN;
		}
		else{
			dataType = INTEGER;
		}
	}
	else {
		auto it = symbolTable.find(name);

		if(symbolTable.count(name) == 1){
			dataType = (*it).second.getDataType();
		}
		else{
			processError("reference to undefined constant");
		}
	}

	return dataType;
}

string Compiler :: whichValue(string name){
	string value;
	if(isLiteral(name) == true){
		value = name;
	}
	else {
		auto it = symbolTable.find(name);

		if(symbolTable.count(name) == 1){
			value = (*it).second.getDataType();
		}
		else{
			processError("reference to undefined constant");
		}
	}
	return value;
}

void Compiler::insert(string externalName, storeTypes inType, modes InMode, string inValue, allocation inAlloc, int inUnits)
{
	// create symbol table entry for each identifier in list of external names
	// Multiply inserted names are illegal
	//    KEY        VALUE
	//map<string, SymbolTableEntry> symbolTable;
	//string name;
	//name = "";
	//...
	
}
void Compiler::constStmts() // token should be NON_KEY_ID
{
	string x,y;
	if (!isNonKeyId(token)) {
		processError("non-keyword identifier expected");
	}
	
	x = token;
	
	if (nextToken() != "=") {
		processError("\"=\" expected");
	}
	
	y = nextToken();
	
	if (y != "+" && y != "-" && y != "not" && !isNonKeyId(x) && y != "true" && y != "false" && whichType(y) != INTEGER) {
		processError("token to right of \"=\" illegal");
	}
	
	if (y == "+" || y != "-") {
		
		if (whichType(nextToken()) != INTEGER) {
			processError("integer expected after sign");
		}
		
		y = y + token;
	}
	
	if (y == "not") {
		
		if (whichType(nextToken()) != BOOLEAN) {
			processError("boolean expected after \"not\"");
		}
		
		if (token == "true") {
			y = "false";
		}
		
		else {
			y = "true";
		}
	}
	
	if (nextToken() != ";") {
		processError("semicolon expected");
	}
	
	if (whichType(y) != INTEGER || whichType(y) != BOOLEAN) {
		processError("data type of token on the right-hand side must be INTEGER or BOOLEAN");
	}
	
	insert(x, whichType(y), CONSTANT, whichValue(y), YES, 1);
	
	x = nextToken();
	
	if (x != "begin" && x != "var" && !isNonKeyId(x)) {
		processError("non-keyword identifier, \"begin\", or \"var\" expected");
	}
	
	if (isNonKeyId(x) == true) {
		constStmts();
	}
}

void Compiler :: consts() {

	if(token != "const"){   
		processError("keyword \"const\" expected");
	}

	if(isNonKeyId(nextToken()) == false){
		processError("non-keyword identifier must follow \"const\"");
	}
	constStmts();
}

void Compiler :: vars(){
	if(token != "var"){
		processError("keyword \"var\" expected");
	}
	if(isNonKeyId(nextToken()) == false){
		processError("non-keyword identifier must follow \"var\"");
	}
}

