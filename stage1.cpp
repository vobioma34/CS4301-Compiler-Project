//Esai Barron and Victor Obioma
//CS 4301
//Stage 0

#include <stage1.h>
#include <iostream>
#include <ctime>
#include <string>
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
	listingFile << "STAGE0:          " << "ESAI BARRON, VICTOR OBIOMA         " << ctime(&result) << endl;
	listingFile << "LINE NO.              " << "SOURCE STATEMENT" << "\r\n";
	listingFile << endl;
	//lineNo++;
	//listingFile << setw(6) << lineNo << "|";

}


/*---------------------------------------PARSER----------------------------------------------------*/
//"new int main() - Womack 2022"
void Compiler::parser() {
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
    	listingFile << "COMPILATION TERMINATED      " << "1"  << " ERROR ENCOUNTERED" << endl;
	}
	else {
		listingFile << "COMPILATION TERMINATED      " << "0"  << " ERRORS ENCOUNTERED" << endl;
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

			//handle compound operators here
			if(token == ":"){
				if(ch == '='){
					token = token + ch;
					nextChar();
				}
			}

			if(token == "<"){
				if(ch == '='){
					token = token + ch;
					nextChar();
				}
				else if(ch == '>'){
					token = token + ch;
					nextChar();
				}
			}

			if(token == ">"){
				if(ch == '='){
					token = token + ch;
					nextChar();
				}
			}	
		}
		else if (islower(ch)) {
			token = ch;
			nextChar();
			char prevCh = ' ';
			while (((ch == '_') || (isalpha(ch)) || (isdigit(ch))) && (ch != END_OF_FILE)) 
			{
				prevCh = ch;
				token += ch;
				nextChar();
				if (prevCh == '_' && ch == '_') {
					processError("error '_' must be followed by a letter or number");
				}
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
		if(isdigit(s[i]) == false && islower(s[i]) == false && s[i] != '_'){
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
    //add code for EXEC_STMT

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
	else if (op == "read") {
		emitReadCode(operand1, operand2);
	}
	else if (op == "write") {
		emitWriteCode(operand1, operand2);
	}
	else if (op == "+") { // this must be binary '+'
		// call to the emitAdditionCode() function
	}
	else if (op == "-") { // this must be binary '-'
		// call to the emitSubtractionCode() function
	}
	else if (op == "neg") { // this must be unary '-'
		// call to the emitNegationCode() function
	}
	else if (op == "not") {
		// call to the emitNotCode() function
	}
	else if (op == "*") {
		// call to the emitMultiplicationCode() function
	}
	else if (op == "div") {
		// call to the emitDivisionCode() function
	}
	else if (op == "mod") {
		// call to the emitModuloCode() function
	} 
	else if (op == "and") {
		// call to the emitAndCode() function
	}
	else if (op == "or") {
		// call to the emitOrCode() function
	}
	else if (op == "=") {
		// call to the emitEqualityCode() function
	}
	else if (op == ":=") {
		emitAssignCode(operand1, operand2);
	}
	else if (op == "<>") {
		// call to the emitInequalityCode() function
	}
	else if (op == "<") {
		// call to the emitLessThanCode() function
	}
	else if (op == "<=") {
		// call to the emitLessThanOrEqualToCode() function
	}
	else if (op == ">") {
		// call to the emitGreaterThanCode() function
	}
	else if (op == ">=") {
		// call to the emitGreaterThanOrEqualCode() function
	}
	else {
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
    if(stype == PROG_NAME){
        internalName = "P" + to_string(0);
    }

	return internalName;
}

/*----------------------- NEW STUFF FOR STAGE1 ----------------------*/

void Compiler :: pushOperator(string op){
    operatorStk.push(op);
}
void Compiler::pushOperand(string name) { // push name into operandStk

	if (isLiteral(name) == true) {
		symbolTable.insert({ name, SymbolTableEntry(name, whichType(name), CONSTANT, whichValue(name), YES, 1) }); // maybe this works...
	}
	if (isLiteral(name) && symbolTable.count(name) == 0) {
		insert(name, whichType(name), CONSTANT, whichValue(name), YES, 1);
	}
	operandStk.push(name);
}
string Compiler:: popOperator(){

   if(operatorStk.empty() == false){
        string topOfStack = operatorStk.top();
        operatorStk.pop();
        return topOfStack;
   }
   else{
        processError("compiler error; operator stack underflow");
   }
   //dummy return
   return "";
}
string Compiler::popOperand() {
	if (!operandStk.empty()) {
		string topElement = operandStk.top();
		operandStk.pop();
		return topElement;
	} else {
		processError("compiler error; operand stack underflow");
	}
	return "";
}

void Compiler :: part(){
	if(token == "not"){
		nextToken();
		if(token == "("){
			nextToken();
			//ADD express() WHEN FINISHED WITH IT
			nextToken();
			if(token != ")"){
				processError("no closing parentheses to match opening.");
			}
			code("not", popOperand());
		}
		else if(isBoolean(token)){
			pushOperand(token);
		} 
		else if(isNonKeyId(token)){
			code("not", token);
		}
		else{
			processError("a non boolean operator cannot follow \"not\" goofy");
		}
	}
	else if(token == "+"){
		nextToken();
		if(token == "("){
			nextToken();
			//ADD express() WHEN FINISHED WITH IT
			nextToken();
			if(token != ")"){
				processError("no closing parentheses to match opening.");
			}
			code("not", popOperand());
		}
		else if(isInteger(token) || isNonKeyId(token)){
			pushOperand(token);
		}
		else{
			processError("must have an integer valte after \'+\' GOPFY");
		}
	}
	else if(token == "-"){
		nextToken();
		if(token == "("){
			nextToken();
			//ADD express() WHEN FINISHED WITH IT
			nextToken();
			if(token != ")"){
				processError("no closing parentheses to match opening.");
			}
			code("neg", popOperand());
		}
		else if(isInteger(token)){
			pushOperand("-" + token);
		}
		else if(isNonKeyId(token)){
			code("neg", token);
		} 
		else{
			processError("Integer value must follow \"-\"");
		}
	}
	else if(isInteger(token) || isBoolean(token) || isNonKeyId(token)){
		pushOperand(token);
	}
	else{
		if(token == "("){
			nextToken();
			//ADD express() WHEN FINISHED WITH IT
			nextToken();
			if(token != ")"){
				processError("no closing parentheses to match opening.");
			}
		}
	}
}
void Compiler::factors() {
	if (token == "*" || token == "div" || token == "mod" || token == "and") {
		pushOperator(token);
		part();
	}
	code(popOperator(), popOperand(), popOperand());
	factors();
}
void Compiler::express() {
	term();
	expresses();
}
void Compiler::factor() {
	part();
	factors();
}
void Compiler::term() {
	factor();
	terms();
}
void Compiler::terms() {
	if (token == "+" || token == "-" || token == "or") {
		pushOperator(token);
		factor();
	}
	code(popOperator(), popOperand(), popOperand());
	terms();
}
void Compiler::expresses() {
	if (token == "=" || token == "<>" || token == "<=" || token == ">=" || token == "<" || token == ">") {
		pushOperator(token);
		term();
	}
	code(popOperator(), popOperand(), popOperand());
	expresses();
}


//Ask about production 8, WRITE_LIST, 
void Compiler :: writeStmt(){
	if (token == "write"){
		//do we need write list??
		nextToken();
		if(token == "("){
			ids();
			nextToken();
			if(token != ")"){
				processError("Error expected \")\"");
			}
			code("write", token);
		}
		if(token != ";"){
			processError("Error. Expected \";\"");
		}
	} 
	else{
		processError("Expected keyword \"write\"");
	}
}

void Compiler :: readStmt(){
	if(token == "read"){
		nextToken();
		if(token == "("){
			ids();
			nextToken(); 
			if(token != ")"){
				processError("Error. Expected \")\" ");
			}
			code("read", token);
		}
		if(token != ";"){
			processError("Error. Expected \";\"");
		}
	}
	else{
		processError("Expected keyword \"read\" ");
	}
}

void Compiler :: assignStmt(){
	if(isNonKeyId(token)){
		pushOperand(token);
		nextToken();
		if(token == ":="){
			pushOperator(":=");
			express();
		}
		nextToken();
		if(token != ";"){
			processError("Error \";\" expected");
		}
		code("read", token);
	} 
	else {
		processError("NonKeyID epxected before \":=\" ");
	}
}

void Compiler :: execStmt(){
	if(token == "read"){
		readStmt();
	}
	else if(token == "write"){
		writeStmt();
	}
	else if (isNonKeyId(token) && nextToken() == ":="){
		assignStmt();
	}
}

void Compiler :: execStmts(){
	if(isNonKeyId(token) || token == "read" || token == "write"){
		execStmt();
		nextToken();
		execStmts();
	}
	else if(token == "end"){
		processError("are you serious rn bro? 🤨");
	}
}
void Compiler :: emitReadCode(string operand, string operand2) {
	cout << operand << "    " << operand2 << endl;
	string name;
	unsigned int nameOfCurrentList = 0;
	while (nameOfCurrentList < operand.length()) {
		name = "";
		while (name == "") {
			while (nameOfCurrentList < operand.length() && operand[nameOfCurrentList] != ',') {
			   name = name + operand[nameOfCurrentList];
			   nameOfCurrentList = nameOfCurrentList + 1;
			}
			
			nameOfCurrentList += 1;
			
			name = name.substr(0, 15);
			
			if (symbolTable.count(name) == 0) {
				processError("reference to undefined symbol");
			} 
			else if (isInteger(name) == false) {
				processError("cannot read varaibles of this type");
			}
			else if (symbolTable.at(name).getMode() != VARIABLE){
				processError("attempting to read to a read-only location");
			}
			else{
				/*
				objectFile << "call ReadInt" << endl;
				objectFile << "mov " +  symbolTable.at(name).getInternalName() + ", eax" << endl;
				*/
				emit("", "call", "ReadInt", ";call to ReadInt function");
				emit("", "mov", operand + ", eax", "move contenets of a Register to variable" );
				contentsOfAReg = name;
			}
		}
	}
}

//get write code from victor
void Compiler :: emitWriteCode(string operand, string operand2) {
	string name;
	unsigned int nameOfCurrentOperand = 0;
	while(nameOfCurrentOperand < operand.length()) {
		name = "";
		while(name == "") {
			while(nameOfCurrentOperand < operand.length() && operand[nameOfCurrentOperand] != ',') {
				name = name + operand[nameOfCurrentOperand];
				nameOfCurrentOperand = nameOfCurrentOperand + 1;
			}
			nameOfCurrentOperand += 1;
			name = name.substr(0, 15);
			
			if (symbolTable.count(name) == 0) {
				processError("reference to undefined symbol");
			}
			else if (name != contentsOfAReg) {
				emit("", "mov", "[" + symbolTable.at(name).getInternalName() + "], " + "eax", "; load name in the A register");
				contentsOfAReg = name;
			} else if (isInteger(name) == true || isBoolean(name) == true) {
				emit("", "call", "WriteInt", "; call the Irvine WriteInt function");
			}
			emit("", "call", "CrLf", "; call the Irvine CrLf function");
		}		
	}
}
//op2 = op1
//op2 = op1
void Compiler :: emitAssignCode(string operand1, string operand2){
	if(symbolTable.find(operand1) == symbolTable.end()){
		processError("refernece to undefined variable");
	}
	if(symbolTable.find(operand2) == symbolTable.end()){
		processError("refernce to undefined variable");
	}
	//make sure types are the same
	if(whichType(operand1) != whichType(operand2)){
		processError("ERROR type mismatch. The operands on either side of \":=\" must be of the same type");
	}
	//make sure LHS is a variable
	if(symbolTable.at(operand2).getMode() != VARIABLE){
		processError("YOU CANNOT ASSIGN A VALUE TO A NON VARIABLE NON KEY ID");
	}
	//both operands are the same thing
	if(operand1 == operand2){
		return;
	}
	//give Areg operand1
	if(contentsOfAReg != operand1){
		emit("", "mov", "eax,[" + symbolTable.at(operand1).getInternalName() + "]", ";move contents of variables to eax");
		contentsOfAReg = operand1;
	}
	//store contenets of op1(eax) into memeory location(op2)
	emit("", "mov", "[" + symbolTable.at(operand2).getInternalName() + "], eax", ";move contents of variable into eax");
	contentsOfAReg = operand2;
	
	if(isTemporary(operand1)){
		freeTemp();
	}

}

//op2 + op1
void Compiler :: emitAdditionCode(string operand1, string operand2){
	if(whichType(operand1) != INTEGER || whichType(operand2) != INTEGER){
		processError("error only integers may be used with \"+\" ");
	}
	//Call is temp
	if(isTemporary(contentsOfAReg) && contentsOfAReg != operand1 && contentsOfAReg != operand2){
		emit("", "mov", "[" + contentsOfAReg + "], eax", ";emit code to load operand2 into the A register" );
		symbolTable.at(contentsOfAReg).setAlloc(YES);
		contentsOfAReg = "";
	}
	if(contentsOfAReg != operand1 && contentsOfAReg != operand2){
		emit("", "mov", "eax,[" + symbolTable.at(operand2).getInternalName() + "]", ";move contents of variables to eax");
		contentsOfAReg = operand2;
	}
	//emit code for addition
	if(contentsOfAReg == operand1){
		emit("", "add", "eax, [" + symbolTable.at(operand2).getInternalName() + "]", "; fill this in" );
	}
	else {
		emit("", "add", "eax, [" + symbolTable.at(operand1).getInternalName() + "]", "; fill this in" );
	}

	//if operands are temporaroy deassign them
	if(isTemporary(operand1)){
		freeTemp();
	}
	if(isTemporary(operand2)){
		freeTemp();
	}
	//give a reg the next availbe temp
	contentsOfAReg = getTemp();
	//make a reg an integer
	symbolTable.at(contentsOfAReg).setDataType(INTEGER);
	
	pushOperand(contentsOfAReg);
}
// op2 -  op1
void Compiler :: emitSubtractionCode(string operand1, string operand2){
	if(whichType(operand1) != INTEGER || whichType(operand2) != INTEGER){
		processError("error only integers may be used with \"+\" ");
	}
	//Call is temp
	if(isTemporary(contentsOfAReg) && contentsOfAReg != operand1 && contentsOfAReg != operand2){
		emit("", "mov", "[" + contentsOfAReg + "], eax", ";emit code to load operand2 into the A register" );
		symbolTable.at(contentsOfAReg).setAlloc(YES);
		contentsOfAReg = "";
	}
	if(contentsOfAReg != operand2){
		emit("", "mov", "eax,[" + symbolTable.at(operand2).getInternalName() + "]", ";move contents of variables to eax");
		contentsOfAReg = operand2;
	}
	//emit code for subtraction
	emit("", "sub", "eax, [" + symbolTable.at(operand1).getInternalName() + "]", "; fill this in" );

	//if operands are temporaroy deassign them
	if(isTemporary(operand1)){
		freeTemp();
	}
	if(isTemporary(operand2)){
		freeTemp();
	}
	//give a reg the next availbe temp
	contentsOfAReg = getTemp();
	//make a reg an integer
	symbolTable.at(contentsOfAReg).setDataType(INTEGER);
	
	pushOperand(contentsOfAReg);
}

//get help on this!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
void Compiler:: emitMultiplicationCode(string operand1, string operand2) {
	if(symbolTable.find(operand1) == symbolTable.end()){
		processError("refernece to undefined variable");
	}
	if(symbolTable.find(operand2) == symbolTable.end()){
		processError("refernce to undefined variable");
	}
	if(whichType(operand1) != INTEGER || whichType(operand2) != INTEGER){
		processError("error only integers may be used with \"-\" ");
	}
	emit("", "mov ", "eax, " + symbolTable.at(operand1).getInternalName(), ";move contenets of operand into eax");
	emit("", "mov ", "ecx, " + symbolTable.at(operand1).getInternalName(), ";move contenets of operand into ecx");
	emit("", "imul ", "eax, ecx", ";multiply ecx by eax");
}
void Compiler :: emitDivisionCode(string operand1, string operand2) {
	if (whichType(operand1) != INTEGER || whichType(operand2) != INTEGER) {
		processError("illegal type");
	}
	if (isTemporary(contentsOfAReg) == true && contentsOfAReg != operand2) {
		emit("", "mov", "[" + contentsOfAReg + "], eax", "; fill this in");
		symbolTable.at(contentsOfAReg).setAlloc(YES);
		contentsOfAReg = ""; // deassign it
	}
	if (isTemporary(contentsOfAReg) == false && contentsOfAReg != operand2) {
		contentsOfAReg = ""; // deassign it
	}
	if (contentsOfAReg != operand2) {
		emit("", "mov", "eax,[" + symbolTable.at(operand2).getInternalName() + "]", ";move contents of variables to eax");
	}
	// perform the register-memory divison
	emit("","cdq","","");
	emit("", "idiv", "dword [" + symbolTable.at(operand1).getInternalName() + "]", "; fill this in");
	
	
	//if operands are temporaroy deassign them
	if(isTemporary(operand1)){
		freeTemp();
	}
	if(isTemporary(operand2)){
		freeTemp();
	}
	//give a reg the next availbe temp
	contentsOfAReg = getTemp();
	//make a reg an integer
	symbolTable.at(contentsOfAReg).setDataType(INTEGER);
	
	pushOperand(contentsOfAReg);
	
}
void Compiler :: emitModuloCode(string operand1, string operand2) {
	if (whichType(operand1) != INTEGER || whichType(operand2) != INTEGER) {
		processError("illegal type");
	}
	if (isTemporary(contentsOfAReg) == true && contentsOfAReg != operand2) {
		emit("", "mov", "[" + contentsOfAReg + "], eax", "; fill this in");
		symbolTable.at(contentsOfAReg).setAlloc(YES);
		contentsOfAReg = ""; // deassign it
	}
	if (isTemporary(contentsOfAReg) == false && contentsOfAReg != operand2) {
		contentsOfAReg = ""; // deassign it
	}
	if (contentsOfAReg != operand2) {
		emit("", "mov", "eax,[" + symbolTable.at(operand2).getInternalName() + "]", ";move contents of variables to eax");
	}
	// perform the register-memory modular
	emit("","cdq","","");
	emit("", "idiv", "dword [" + symbolTable.at(operand1).getInternalName() + "]", "; fill this in");
	emit("", "xchg", "eax, edx", "");
	
	
	//if operands are temporaroy deassign them
	if(isTemporary(operand1)){
		freeTemp();
	}
	if(isTemporary(operand2)){
		freeTemp();
	}
	//give a reg the next availbe temp
	contentsOfAReg = getTemp();
	//make a reg an integer
	symbolTable.at(contentsOfAReg).setDataType(INTEGER);
	
	pushOperand(contentsOfAReg);
}
void Compiler :: emitOrCode(string operand1, string operand2) {
	if (whichType(operand1) != BOOLEAN || whichType(operand2) != BOOLEAN) {
		processError("illegal type");
	}
	if (isTemporary(contentsOfAReg) && contentsOfAReg != operand1 && contentsOfAReg != operand2) {
		emit("", "mov", "[" + contentsOfAReg + "], eax", "; store that temp into memory");
		symbolTable.at(contentsOfAReg).setAlloc(YES); // change the allocation to YES
		contentsOfAReg = "";
	}
	if (isTemporary(contentsOfAReg) == false && contentsOfAReg != operand1 && contentsOfAReg != operand2) {
		contentsOfAReg = ""; // deassign it
	}
	if (contentsOfAReg != operand1 || contentsOfAReg != operand2) {
		emit("", "mov", "eax,[" + symbolTable.at(operand2).getInternalName() + "]", ";move contents of variables to eax");
	}
	if (contentsOfAReg == operand1) {
		emit("", "OR", "eax, [" + symbolTable.at(operand2).getInternalName() + "]", "; fill this in");
	}
	//if operands are temporaroy deassign them
	if(isTemporary(operand1)){
		freeTemp();
	}
	if(isTemporary(operand2)){
		freeTemp();
	}
	//give a reg the next availbe temp
	contentsOfAReg = getTemp();
	//make a reg an integer
	symbolTable.at(contentsOfAReg).setDataType(INTEGER);
	
	pushOperand(contentsOfAReg); // push it to the stack
}
/*---------------------------TEMP STUFF -----------------------*/
void Compiler :: freeTemp(){
	currentTempNo--;
	if(currentTempNo < -1){
		processError("compiler error, current temp should not be less than <= -1");
	}
}

string Compiler:: getTemp(){
	string temp;
	currentTempNo++;
	temp = "T" + currentTempNo;
	if (currentTempNo > maxTempNo){
		insert(temp, UNKNOWN, VARIABLE, "", NO, 1);
	}
	maxTempNo++;
	return temp;
}

bool Compiler :: isTemporary(string s) const {
	if(s[0] == 'T'){
		return true;
	} 
	else {
		return false;
	}
}

void Compiler :: emitNegationCode(string operand1, string nothing){
	if(whichType(operand1) == INTEGER){
		emit("", "neg ", "eax", "; AReg = -AReg");
	}
	else {
		processError("\'-\' may only be used on type INTEGER");
	}
}

void Compiler :: emitNotCode(string operand1, string nothing){
	if(whichType(operand1) == BOOLEAN){
		emit("", "not ", "eax", "; AReg = -AReg");
	}
	else {
		processError("\"not\" may only be used on type BOOLEAN");
	}
}

void Compiler :: emitAndCode(string operand1, string operand2){
	if(whichType(operand1) != BOOLEAN || whichType(operand2) != BOOLEAN){
		processError("error only booleans may be used with \"and\" ");
	}
	if(isTemporary(contentsOfAReg) && contentsOfAReg != operand1 && contentsOfAReg != operand2){
		emit("", "mov", "[" + contentsOfAReg + "], eax", ";emit code to load operand2 into the A register" );
		symbolTable.at(contentsOfAReg).setAlloc(YES);
		contentsOfAReg = "";
	}
	if(contentsOfAReg != operand1 && contentsOfAReg != operand2){
		emit("", "mov", "eax,[" + symbolTable.at(operand2).getInternalName() + "]", ";move contents of variables to eax");
		contentsOfAReg = operand2;
	}
	if(contentsOfAReg == operand1){
		emit("", "and ", "eax, [" + symbolTable.at(operand2).getInternalName() + "]", ";" + contentsOfAReg + "and " + operand2);
	}
	else{
		//emit("", "mov ", "eax, [" + operand1 +"]", ";Areg  = " + operand1 );
		emit("", "and ", "eax, [" + symbolTable.at(operand1).getInternalName() + "]", ";" + contentsOfAReg + "and " + operand2);
	}

	//if operands are temporaroy deassign them
	if(isTemporary(operand1)){
		freeTemp();
	}
	if(isTemporary(operand2)){
		freeTemp();
	}
	//give a reg the next availbe temp
	contentsOfAReg = getTemp();
	//make a reg an integer
	symbolTable.at(contentsOfAReg).setDataType(INTEGER);
	
	pushOperand(contentsOfAReg);
}


void Compiler :: emitEqualityCode(string operand1, string operand2){
	string label1, label2;
	label1 = getLabel();
	label2 = getLabel();
	// types of operands are not the same
	if (whichType(operand1) != whichType(operand2)) {
		processError("incompatible types");
	}
	// if the A Register holds a temp not operand1 nor operand2
	if (isTemporary(contentsOfAReg) && (contentsOfAReg != operand1 || contentsOfAReg != operand2)) {
		emit("", "mov", "[" + contentsOfAReg + "], eax", "; store that temp into memory"); // store the temp into memory
		symbolTable.at(contentsOfAReg).setAlloc(YES); // change the allocation to YES
		contentsOfAReg = ""; // deassign it
	}
	// if the A register holds a non-temp not operand2 nor operand1
	if (isTemporary(contentsOfAReg) == false && (contentsOfAReg != operand1 || contentsOfAReg != operand2)) {
		contentsOfAReg = ""; // deassign it
	}
	if (contentsOfAReg != operand1 && contentsOfAReg != operand2) {
		emit("", "mov", "eax,[" + symbolTable.at(operand2).getInternalName() + "]", ";move contents of variables to eax"); // load operand2 into the A register
	}
	emit("", "cmp", "eax, [" + symbolTable.at(operand1).getInternalName() + "]","; fill this in"); // perform a register-memory compare
	emit("", "je", label1, ""); // jump if equal to the next available Ln (call getLabel)
	emit("", "mov", "eax, [FALSE]", "; load FALSE"); //  load FALSE into the A register
	emit("", "jmp", label2, "; unconditional jump"); // perform an unconditional jump to the next label (call getLabel should be L(n+1))
	emit(label1, "", "", ""); // emit code to label the next instruction with the first acquired label Ln
	emit("", "mov", "eax, [TRUE]", "; load TRUE"); //  load TRUE into the A register
	emit(label2, "", "", "");
	// inserting true and false into the symbol table
	if (symbolTable.count("true") == 0) {
		symbolTable.insert({ "true", SymbolTableEntry("TRUE", BOOLEAN, CONSTANT, "-1", YES, 1) });
	}
	if (symbolTable.count("false") == 0) {
		symbolTable.insert({ "false", SymbolTableEntry("FALSE", BOOLEAN, CONSTANT, "0", YES, 1) });
	}
	//if operands are temporaroy deassign them
	if(isTemporary(operand1)){
		freeTemp();
	}
	if(isTemporary(operand2)){
		freeTemp();
	}
	//give a reg the next availbe temp
	contentsOfAReg = getTemp();
	//make a reg an integer
	symbolTable.at(contentsOfAReg).setDataType(BOOLEAN);
	
	pushOperand(contentsOfAReg); // push it to the stack
}
void Compiler :: emitInequalityCode(string operand1, string operand2) {
	string label1, label2;
	label1 = getLabel();
	label2 = getLabel();
	// types of operands are not the same
	if (whichType(operand1) != whichType(operand2)) {
		processError("incompatible types");
	}
	// if the A Register holds a temp not operand1 nor operand2
	if (isTemporary(contentsOfAReg) && (contentsOfAReg != operand1 || contentsOfAReg != operand2)) {
		emit("", "mov", "[" + contentsOfAReg + "], eax", "; store that temp into memory"); // store the temp into memory
		symbolTable.at(contentsOfAReg).setAlloc(YES); // change the allocation to YES
		contentsOfAReg = ""; // deassign it
	}
	// if the A register holds a non-temp not operand2 nor operand1
	if (isTemporary(contentsOfAReg) == false && (contentsOfAReg != operand1 || contentsOfAReg != operand2)) {
		contentsOfAReg = ""; // deassign it
	}
	if (contentsOfAReg != operand1 && contentsOfAReg != operand2) {
		emit("", "mov", "eax,[" + symbolTable.at(operand2).getInternalName() + "]", ";move contents of variables to eax"); // load operand2 into the A register
	}
	emit("", "cmp", "eax, [" + symbolTable.at(operand1).getInternalName() + "]","; fill this in"); // perform a register-memory compare
	emit("", "jne", label1, ""); // jump if equal to the next available Ln (call getLabel)
	emit("", "mov", "eax, [FALSE]", "; load FALSE"); //  load FALSE into the A register
	emit("", "jmp", label2, "; unconditional jump"); // perform an unconditional jump to the next label (call getLabel should be L(n+1))
	emit(label1, "", "", ""); // emit code to label the next instruction with the first acquired label Ln
	emit("", "mov", "eax, [TRUE]", "; load TRUE"); //  load TRUE into the A register
	emit(label2, "", "", "");
	// inserting true and false into the symbol table
	if (symbolTable.count("true") == 0) {
		symbolTable.insert({ "true", SymbolTableEntry("TRUE", BOOLEAN, CONSTANT, "-1", YES, 1) });
	}
	if (symbolTable.count("false") == 0) {
		symbolTable.insert({ "false", SymbolTableEntry("FALSE", BOOLEAN, CONSTANT, "0", YES, 1) });
	}
	//if operands are temporaroy deassign them
	if(isTemporary(operand1)){
		freeTemp();
	}
	if(isTemporary(operand2)){
		freeTemp();
	}
	//give a reg the next availbe temp
	contentsOfAReg = getTemp();
	//make a reg an integer
	symbolTable.at(contentsOfAReg).setDataType(BOOLEAN);
	
	pushOperand(contentsOfAReg); // push it to the stack
}
string Compiler::getLabel() {
	static int labelCount = -1;
	string currentLabel;
	labelCount++; // increment the number of labels by 1
	currentLabel = ".L" + to_string(labelCount);
	return currentLabel; // Return the current label
}

//op2 lhs op1 rhs
void Compiler :: emitLessThanCode(string operand1, string operand2){
	string label1 = getLabel();
	string label2 = getLabel();

	if(whichType(operand1) != INTEGER || whichType(operand2) != INTEGER){
		processError("error only integers may be used with \"<=\" ");
	}
	//Call is temp
	if(isTemporary(contentsOfAReg) && contentsOfAReg != operand1 && contentsOfAReg != operand2){
		emit("", "mov", "[" + contentsOfAReg + "], eax", ";emit code to load operand2 into the A register" );
		symbolTable.at(contentsOfAReg).setAlloc(YES);
		contentsOfAReg = "";
	}
	if(contentsOfAReg != operand1 && contentsOfAReg != operand2){
		emit("", "mov", "eax,[" + symbolTable.at(operand2).getInternalName() + "]", ";move contents of variables to eax");
		contentsOfAReg = operand2;
	}
	else{
		emit("", "cmp ", "eax, [" + symbolTable.at(operand1).getInternalName() + "]", ";compare eax and " +symbolTable.at(operand1).getInternalName());
		emit("", "jl ", label1, ";if eax is less than" + symbolTable.at(operand1).getInternalName());
		emit("", "jmp ", label2, ";if eax is not less than" +symbolTable.at(operand1).getInternalName());
		emit(label1 + ":", "", "" ,"");
		emit("", "mov ", "eax, [true]", ";assign eax value of true");
		emit(label2 + ":", "", "" ,"");
		emit("", "mov ", "eax, [false]", ";assign eax value of false");
	}

	//if operands are temporaroy deassign them
	if(isTemporary(operand1)){
		freeTemp();
	}
	if(isTemporary(operand2)){
		freeTemp();
	}
	//give a reg the next availbe temp
	contentsOfAReg = getTemp();
	//make a reg an integer
	symbolTable.at(contentsOfAReg).setDataType(INTEGER);
	
	pushOperand(contentsOfAReg);

}

void Compiler :: emitLessThanOrEqualToCode(string operand1, string operand2){
	string label1 = getLabel();
	string label2 = getLabel();

	if(whichType(operand1) != INTEGER || whichType(operand2) != INTEGER){
		processError("error only integers may be used with \"<=\" ");
	}
	//Call is temp
	if(isTemporary(contentsOfAReg) && contentsOfAReg != operand1 && contentsOfAReg != operand2){
		emit("", "mov", "[" + contentsOfAReg + "], eax", ";emit code to load operand2 into the A register" );
		symbolTable.at(contentsOfAReg).setAlloc(YES);
		contentsOfAReg = "";
	}
	if(contentsOfAReg != operand1 && contentsOfAReg != operand2){
		emit("", "mov", "eax,[" + symbolTable.at(operand2).getInternalName() + "]", ";move contents of variables to eax");
		contentsOfAReg = operand2;
	}
	else{
		emit("", "cmp ", "eax, [" + symbolTable.at(operand1).getInternalName() + "]", ";compare eax and " +symbolTable.at(operand1).getInternalName());
		emit("", "jle ", label1, ";if eax is less than" + symbolTable.at(operand1).getInternalName());
		emit("", "jmp ", label2, ";if eax is not less than" +symbolTable.at(operand1).getInternalName());
		emit(label1 + ":", "", "" ,"");
		emit("", "mov ", "eax, [true]", ";assign eax value of true");
		emit(label2 + ":", "", "" ,"");
		emit("", "mov ", "eax, [false]", ";assign eax value of false");
	}

	//if operands are temporaroy deassign them
	if(isTemporary(operand1)){
		freeTemp();
	}
	if(isTemporary(operand2)){
		freeTemp();
	}
	//give a reg the next availbe temp
	contentsOfAReg = getTemp();
	//make a reg an integer
	symbolTable.at(contentsOfAReg).setDataType(INTEGER);
	
	pushOperand(contentsOfAReg);

}
void Compiler :: emitGreaterThanCode(string operand1, string operand2) {
	string label1, label2;
	label1 = getLabel();
	label2 = getLabel();
	// types of operands are not the same
	if (whichType(operand1) != whichType(operand2)) {
		processError("incompatible types");
	}
	// if the A Register holds a temp not operand1 nor operand2
	if (isTemporary(contentsOfAReg) && (contentsOfAReg != operand1 || contentsOfAReg != operand2)) {
		emit("", "mov", "[" + contentsOfAReg + "], eax", "; store that temp into memory"); // store the temp into memory
		symbolTable.at(contentsOfAReg).setAlloc(YES); // change the allocation to YES
		contentsOfAReg = ""; // deassign it
	}
	// if the A register holds a non-temp not operand2 nor operand1
	if (isTemporary(contentsOfAReg) == false && (contentsOfAReg != operand1 || contentsOfAReg != operand2)) {
		contentsOfAReg = ""; // deassign it
	}
	if (contentsOfAReg != operand1 && contentsOfAReg != operand2) {
		emit("", "mov", "eax,[" + symbolTable.at(operand2).getInternalName() + "]", ";move contents of variables to eax"); // load operand2 into the A register
	}
	emit("", "cmp", "eax, [" + symbolTable.at(operand1).getInternalName() + "]","; fill this in"); // perform a register-memory compare
	emit("", "jg", label1, ""); // jump if greater than and the next available Ln (call getLabel)
	emit("", "mov", "eax, [FALSE]", "; load FALSE"); //  load FALSE into the A register
	emit("", "jmp", label2, "; unconditional jump"); // perform an unconditional jump to the next label (call getLabel should be L(n+1))
	emit(label1, "", "", ""); // emit code to label the next instruction with the first acquired label Ln
	emit("", "mov", "eax, [TRUE]", "; load TRUE"); //  load TRUE into the A register
	emit(label2, "", "", "");
	// inserting true and false into the symbol table
	if (symbolTable.count("true") == 0) {
		symbolTable.insert({ "true", SymbolTableEntry("TRUE", BOOLEAN, CONSTANT, "-1", YES, 1) });
	}
	if (symbolTable.count("false") == 0) {
		symbolTable.insert({ "false", SymbolTableEntry("FALSE", BOOLEAN, CONSTANT, "0", YES, 1) });
	}
	//if operands are temporaroy deassign them
	if(isTemporary(operand1)){
		freeTemp();
	}
	if(isTemporary(operand2)){
		freeTemp();
	}
	//give a reg the next availbe temp
	contentsOfAReg = getTemp();
	//make a reg an integer
	symbolTable.at(contentsOfAReg).setDataType(BOOLEAN);
	
	pushOperand(contentsOfAReg); // push it to the stack
}
void Compiler :: emitGreaterThanOrEqualToCode(string operand1, string operand2) {
	string label1, label2;
	label1 = getLabel();
	label2 = getLabel();
	// types of operands are not the same
	if (whichType(operand1) != whichType(operand2)) {
		processError("incompatible types");
	}
	// if the A Register holds a temp not operand1 nor operand2
	if (isTemporary(contentsOfAReg) && (contentsOfAReg != operand1 || contentsOfAReg != operand2)) {
		emit("", "mov", "[" + contentsOfAReg + "], eax", "; store that temp into memory"); // store the temp into memory
		symbolTable.at(contentsOfAReg).setAlloc(YES); // change the allocation to YES
		contentsOfAReg = ""; // deassign it
	}
	// if the A register holds a non-temp not operand2 nor operand1
	if (isTemporary(contentsOfAReg) == false && (contentsOfAReg != operand1 || contentsOfAReg != operand2)) {
		contentsOfAReg = ""; // deassign it
	}
	if (contentsOfAReg != operand1 && contentsOfAReg != operand2) {
		emit("", "mov", "eax,[" + symbolTable.at(operand2).getInternalName() + "]", ";move contents of variables to eax"); // load operand2 into the A register
	}
	emit("", "cmp", "eax, [" + symbolTable.at(operand1).getInternalName() + "]","; fill this in"); // perform a register-memory compare
	emit("", "jge", label1, ""); // jump if greater than or equal to the next available Ln (call getLabel)
	emit("", "mov", "eax, [FALSE]", "; load FALSE"); //  load FALSE into the A register
	emit("", "jmp", label2, "; unconditional jump"); // perform an unconditional jump to the next label (call getLabel should be L(n+1))
	emit(label1, "", "", ""); // emit code to label the next instruction with the first acquired label Ln
	emit("", "mov", "eax, [TRUE]", "; load TRUE"); //  load TRUE into the A register
	emit(label2, "", "", "");
	// inserting true and false into the symbol table
	if (symbolTable.count("true") == 0) {
		symbolTable.insert({ "true", SymbolTableEntry("TRUE", BOOLEAN, CONSTANT, "-1", YES, 1) });
	}
	if (symbolTable.count("false") == 0) {
		symbolTable.insert({ "false", SymbolTableEntry("FALSE", BOOLEAN, CONSTANT, "0", YES, 1) });
	}
	//if operands are temporaroy deassign them
	if(isTemporary(operand1)){
		freeTemp();
	}
	if(isTemporary(operand2)){
		freeTemp();
	}
	//give a reg the next availbe temp
	contentsOfAReg = getTemp();
	//make a reg an integer
	symbolTable.at(contentsOfAReg).setDataType(BOOLEAN);
	
	pushOperand(contentsOfAReg); // push it to the stack
}
