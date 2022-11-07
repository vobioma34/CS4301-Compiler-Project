//Esai Barron and Victor Obioma
//CS 4301
//Stage 0

#include <stage0.h>
#include <iostream>
#include <ctime>
#include <string>
//#include <array>
#include <cctype>
#include <iomanip>

using namespace std;

static bool hasErrorBeenFound = false;

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
	listingFile << "STAGE0:          " << "ESAI BARON, VICTOR OBIOMA         " << ctime(&result) << endl;
	listingFile << "LINE NO.         " << "SOURCE STATEMENT" << "\r\n";
	listingFile << endl;
	//lineNo++;
	//listingFile << setw(6) << lineNo << "|";

}


/*---------------------------------------PARSER----------------------------------------------------*/
//"new int main() - Womack 2022"
void Compiler::parser() {
    //cout << "sussy balls" << endl;
	//listingFile << "testing of next char: " << nextChar() << nextChar() << nextChar() << endl;
	
	
	/*
	cout << isNonKeyId("boolean") << endl;
	cout << isNonKeyId("stage0no001") << endl;
	cout << isNonKeyId("true") << endl;
	cout << isNonKeyId("const") << endl;
	*/

	/*
	nextChar();
	nextToken();
	*/
	

	
	nextChar();
	nextToken();
	if(token != "program"){
		processError("keyword \"program\" expected");
	}
	prog();
	
	
}

void Compiler::createListingTrailer(){
	if(hasErrorBeenFound == true){
    	listingFile << "COMPILATION TERMINATED   " << "1"  << " ERROR ENCOUNTERED" << endl;
	}
	else {
		listingFile << "COMPILATION TERMINATED   " << "0"  << " ERRORS ENCOUNTERED" << endl;
	}
}



void Compiler:: processError(string err) {
    //output error to listing file and call exit()
    listingFile << '\n' << "Error: Line " << lineNo << ": " << err << endl;
	listingFile << endl;
	hasErrorBeenFound = true;
	createListingTrailer();
    exit(0);
}

char Compiler :: nextChar(){
    sourceFile.get(ch);
    static char prevCh ='\n';
 
    if(sourceFile.eof()) 
        ch = END_OF_FILE;

    if(ch != '$'){//we don't output the $ or line numbers after that
        if( prevCh == '\n')
        {
            lineNo++;
            listingFile <<  setw(5) << right<< lineNo << "|";
        }

        listingFile << ch;
    }
    else
        listingFile << endl;//need extra line at the bottom

    prevCh = ch;


    return ch;
}
//nees to be fixed
string Compiler :: nextToken() {
	token = "";
	//ch = nextChar();
	while (token == "")
	{
		if (ch == '{') { // process comment
			while (ch != END_OF_FILE && ch != '}') {
				nextChar();
			}
			if (ch == END_OF_FILE) {
				processError("unexpected end of file");
			} 
			if(ch == '}') 
				nextChar();
		}
		else if (ch == '}') {
			processError("'}' cannot begin token");
		}
		else if (isspace(ch)) {
			nextChar();
		}
		else if (isSpecialSymbol(ch)) {
			token = ch;
			nextChar();
		}
		else if (islower(ch)) {
			token = ch;
			nextChar();
			while (((ch == '_') || (isalpha(ch)) || (isdigit(ch))) && (ch != END_OF_FILE)) 
			{
				token += ch;
				nextChar();
			}
			if (ch == END_OF_FILE) {
				processError("unexpected end of file");
			}
			//nextChar();
		}
		else if (isdigit(ch)) {
			token = ch;
			nextChar();
			while ((isdigit(ch)) && (ch != END_OF_FILE))
			{
				token += ch;
				nextChar();
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
//determines if s is a nonKeyid
bool Compiler :: isNonKeyId(string s) const {

	//make sure s starts with an alpha
	if(islower(s[0]) == false){
		return false;
	}
	//make sure s is not a keyword
	if(isKeyword(s)){
		return false;
	}

	//make sure all chars are lowercase
	for(uint i = 0; i < s.length(); i++){
		if(isdigit(s[i]) == false && islower(s[i]) == false){
				return false;
		}
	}

	//make sure the 15th character is not an underscore
	if(s.length() >= 15){
		if(s[14] == '_'){
			return false;
		}
	}

	//make sure no special symbols are in s, with exceiption of underscore '_', also make sure no spaces in name
	int uscoreCount = 0;
    for(uint i = 0; i < s.length(); i++){
		//non key id can have oneunderscore
		if(isSpecialSymbol(s[i])){
			if(s[i] == '_' && uscoreCount < 1){
				uscoreCount++;
			}
			else{
				return false;
			}
		}
		/*
		else if(isSpecialSymbol(s[i]) || uscoreCount > 1 || isspace(s[i])){
			return false;
		}
		*/
	}
	
	//dummy return to satify warning
	return true;
}
//determines if s is an integer
bool Compiler :: isInteger(string s) const {
	if( s == "integer")
		return true;
	for(uint i = 0; i < s.length() - 1; i++){
		if(!isdigit(s[i]) && s[i] != '-' && s[i] != '+'){
			return false;
		}
	}
	return true;
}

bool Compiler :: isBoolean(string s) const{
	if(s == "boolean")
		return true;
    if (s == "true" || s == "false") {
        return true;
    }
    else {
        return false;
    }
}

bool Compiler :: isLiteral(string s) const {
    if(isBoolean(s) == false && isInteger(s) == false){
		return false;
	}
	return true;
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
		tempString = temp + "," + ids();
	}
	return tempString;
}

storeTypes Compiler :: whichType(string name){
	//will need store types enum called dataType
	storeTypes dataType;
	//cout << token;
	//cout << endl << name << endl;
	if(isLiteral(name)){
		if(isBoolean(name)){
			dataType = BOOLEAN;
		}
		else{
			dataType = INTEGER;
		}
	}
	else {
		//auto it = symbolTable.find(name);

		if(symbolTable.count(name) == 1){
			dataType = symbolTable.at(name).getDataType();
		}
		else{
			processError("reference to undefined constant");
		}
	}

	return dataType;
}

string Compiler :: whichValue(string name){
	string value = "hahah";
	if(isLiteral(name)){
		value = name;
	}
	else {
		auto it = symbolTable.find(name);

		if(symbolTable.count(name) == 1){
			value = (*it).second.getValue();
		}
		else{
			processError("reference to undefined constant");
		}
	}
	return value;
}

//need to finish insert
void Compiler::insert(string externalName, storeTypes inType, modes inMode, string inValue, allocation inAlloc, int inUnits)
{
	string name;
	unsigned int nameOfCurrentList = 0;
	while (nameOfCurrentList < externalName.length()) {
		name = "";
		while (name == "") {
			while (nameOfCurrentList < externalName.length() && externalName[nameOfCurrentList] != ',') {
			   name = name + externalName[nameOfCurrentList];
			   nameOfCurrentList = nameOfCurrentList + 1;
			}
			
			nameOfCurrentList += 1;
			
			name = name.substr(0, 15);
			
			if (symbolTable.count(name) > 0) {
				processError("symbol " + name + " is multiply defined");
			} else if (isKeyword(name) && name != "true" && name != "false") {
				processError("illegal use of keyword");
			} else {
				if (isupper(name[0])) {
					symbolTable.insert({ name, SymbolTableEntry(name, inType, inMode, inValue, inAlloc, inUnits) });
				} else {
					symbolTable.insert({ name, SymbolTableEntry(genInternalName(inType), inType, inMode, inValue, inAlloc, inUnits) });
				}
			}
		}
		if (symbolTable.size() > 256) {
			processError("symbol table cannot exceed 256");
		}
	}
}

void Compiler::constStmts() // token should be NON_KEY_ID
{
	string x,y;
	if (!isNonKeyId(token)) {
		processError("non-keyword identifier expected");
	}

	x = token;

	if (nextToken() != "=") {
		if (token == ",") {
			processError("no lists of constants allowed");
		}
		if (isNonKeyId(token)) {
			processError("no spaces in a variable name");
		}
		processError("\"=\" expected");
	}

	y = nextToken();
	//cout << endl << x << endl;
	//cout << y << endl;

	if (y != "+" && y != "-" && y != "not" && !isNonKeyId(y) && y != "true" && y != "false" && whichType(y) != INTEGER) {
		processError("token to right of \"=\" illegal");
	}

	if (y == "+" || y == "-") {

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

	if (whichType(y) != INTEGER && whichType(y) != BOOLEAN) {
		processError("data type of token on the right-hand side must be INTEGER or BOOLEAN");
	}

	insert(x, whichType(y), CONSTANT, whichValue(y), YES, 1);

	x = nextToken();

	if (x != "begin" && x != "var" && !isNonKeyId(x)) {
		processError("non-keyword identifier, \"begin\", or \"var\" expected");
	}

	if (isNonKeyId(x)) {
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
	varStmts();
}

void Compiler::prog() // token should be "program"
{
	
	if (token != "program") {
	
		processError("keyword \"program\" expected");
	}
	
	progStmt();
	
	if (token == "const") {
		
		consts();
	}

	if (token == "var") {
		vars();
	}
	
	if (token != "begin") {
		processError("keyword \"begin\" expected");
	}
	
	beginEndStmt();
	if (token != "$") { // ?
		processError("no text may follow \"end\"");
	}
}
void Compiler::progStmt() // token should be "program"
{
	
	string x;
	
	if (token != "program") {
		
		processError("keyword \"program\" expected");
	}
	
	//cout << token << endl;
	x = nextToken();
	
	//cout << token << endl;
	
	if (isNonKeyId(token) == false) {
		
		//cout << token << endl;
		processError("program name expected");
	}
	
	nextToken();
	//cout << token << endl;
	if (token != ";") {
		
		processError("semicolon expected");
	}
	
	nextToken();
	
	code("program", x);
	
	insert(x, PROG_NAME, CONSTANT, x, NO, 0);
}
void Compiler::beginEndStmt() // token should be "begin" 
{ 
	if (token != "begin") {
		processError("keyword \"begin\" expected");
	}
	if (nextToken() != "end") {
		processError("keyword \"end\" expected");
	}
	if (nextToken() != ".") {
		processError("period expected");
	}
	nextToken();
	code("end", ".");
}

void Compiler :: varStmts(){
	string x, y;

	if(isNonKeyId(token) == false){
		processError("non-keyword identifier expected");
	}

	x = ids();

	if(token != ":"){
		processError("\":\" expected");
	}
	nextToken();
	if(isInteger(token) == false && isBoolean(token) == false){
		processError("illegal type follows \":\"");
	}

	y = token;

	if(nextToken() != ";"){
		processError("semicolon expected");
	}

	insert(x,whichType(y),VARIABLE,"",YES,1);
	nextToken();
	if(token != "begin" &&  isNonKeyId(token) == false){
		processError("(non-keyword identifier or \"begin\" expected");
	}
	if(isNonKeyId(token)){
		varStmts();
	}
}

void Compiler :: code(string op, string operand1, string operand2) {
	if(op == "program"){
		emitPrologue(operand1);
	}
	else if(op == "end"){
		emitEpilogue();
	}
	else{
		processError("compiler error since function code should not be called with illegal arguments");
	}
}

/*-------------------EMIT FUNCTIONS----------------------------*/
void Compiler::emit(string label, string instruction, string operands, string comment)
{
	/*
	Turn on left justification in objectFile
	Output label in a field of width 8
	Output instruction in a field of width 8
	Output the operands in a field of width 24
	Output the comment
	*/
	objectFile << left << setw(8) << label;
	objectFile << left<<setw(8) << instruction; 
	objectFile << left << setw(24) << operands;
	objectFile << left << comment << endl;
}

void Compiler :: emitPrologue(string progName, string operand2){
	 time_t result = time(nullptr);
	objectFile << "; ESAI BARRON, VICTOR OBIOMA " << ctime(&result);
	objectFile << "%INCLUDE \"Along32.inc\"" << endl << "%INCLUDE \"Macros_Along.inc\"" << endl << endl;

	emit("SECTION", ".text");
	emit("global", "_start", "", "; program " + progName);
	objectFile << endl;
	emit("_start:");
}


void Compiler::emitEpilogue(string operand1, string operand2)
{
	emit("", "Exit", "{0}", "");
	objectFile << endl;
	emitStorage();
}


void Compiler::emitStorage()
{
	emit("SECTION", ".data", "", "");
	 for (auto it = symbolTable.cbegin(); it != symbolTable.cend(); ++it) {
		 if (((*it).second.getAlloc() == YES) && ((*it).second.getMode() == CONSTANT)) {
			 //cout << "TESTING VALUE:" << (*it).second.getValue() << endl;
			 if((*it).second.getValue() == "false" ){
			 	emit((*it).second.getInternalName(), "dd", "0", ("; "+(*it).first));
			 }
			 else if ((*it).second.getValue() == "true"){
				emit((*it).second.getInternalName(), "dd", "-1", ("; "+(*it).first));
			 }
			 else {
				emit((*it).second.getInternalName(), "dd", (*it).second.getValue(), ("; "+(*it).first));

			 }
		 }
	 }

	 objectFile << endl;

	 emit("SECTION", ".bss", "", "");
	 for (auto it = symbolTable.cbegin(); it != symbolTable.cend(); ++it) {
		 if (((*it).second.getAlloc() == YES) && ((*it).second.getMode() == VARIABLE)) {
			 emit((*it).second.getInternalName(), "resd", "1", ("; "+(*it).first));
		 }
	 }
}

//these variables are used for genInternalName()
static int boolCount = 0;
static int integerCount = 0;
string Compiler :: genInternalName(storeTypes stype) const{

	string internalName;

	if(stype == INTEGER){
		internalName =  "I" + to_string(integerCount);
		integerCount++;
	}
	if(stype == BOOLEAN){
		internalName = "B" + to_string(boolCount);
		boolCount++;
	}

	return internalName;
}
