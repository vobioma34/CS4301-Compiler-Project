//Esai Barron and Victor Obioma
//CS 4301
//Stage 2

#include <stage2.h>
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
	listingFile << "STAGE1:          " << "ESAI BARRON, VICTOR OBIOMA         " << ctime(&result) << endl;
	listingFile << "LINE NO.              " << "SOURCE STATEMENT" << "\r\n";
	listingFile << endl;
	//lineNo++;
	//listingFile << setw(6) << lineNo << "|";

}


/*---------------------------------------PARSER----------------------------------------------------*/
//"new int main() - Womack 2022"
void Compiler::parser() {




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

//need to be fixed
string Compiler :: nextToken() {
	token = "";
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
			

			//handle compound operators here
			if(ch == ':'){
				token += ch;
				nextChar();
				if(ch == '='){
					token += ch;
					nextChar();
				}
			}

			else if(ch == '<'){
				token += ch;
				nextChar();
				if(ch == '='){
					token += ch;
					nextChar();
				}
				else if(ch == '>'){
					token += ch;
					nextChar();
				}
			}

			else if(ch == '>'){
				token += ch;
				nextChar();
				if(ch == '='){
					token += ch;
					nextChar();
				}
			}
			else {
				token = ch;
				nextChar();
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
                            "begin", "end", "true", "false", "not", "and", "or", "mod", "div", "and", "or", "read", "write", "if", "then", "else", "while", "do", "repeat", "until"};

    //go through keywords array and see if s matches nay key words
    for(uint i = 0; i < sizeof(keywords)/sizeof(keywords[0]); i++){ //change hardcoded 10 and ask Dr.Motl
        if (s == keywords[i]) {
            return true; //if s matches a keyword, return true
        }
    }
    return false; //s did not match any keywords
}
//determines if c is a special symbol, true is it is a special symbol otherwise false
bool Compiler :: isSpecialSymbol(char c) const {
    char specialSymbols[] = {':', ',', ';', '=', '+', '-', '.', '(', ')', '*', '>', '<'};

    for(uint i = 0; i < sizeof(specialSymbols)/sizeof(specialSymbols[0]); i++){
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
	if(isLiteral(name) && !isNonKeyId(name)){
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
			processError("reference to undefined symbol " + name);
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
			} 
			else if( name == "FALSE" || name == "TRUE"){
				if(name == "FALSE") {
					symbolTable.insert({ "false", SymbolTableEntry("FALSE", inType, inMode, inValue, inAlloc, inUnits) });
				}
				else 
					symbolTable.insert({ "true", SymbolTableEntry("TRUE", inType, inMode, inValue, inAlloc, inUnits) });
			}
			else{
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


	x = nextToken();


	if (isNonKeyId(token) == false) {


		processError("program name expected");
	}

	nextToken();

	if (token != ";") {

		processError("semicolon expected");
	}

	nextToken();

	code("program", x);

	insert(x, PROG_NAME, CONSTANT, x, NO, 0);
}
void Compiler::beginEndStmt() // token should be "begin"
{
	static stack<int> begins;
	begins.push(1);

	// This function may be change for stage2 material.
	string variable;
	// Must be modify for stage2 material
	//cout << token << endl; <- begin
	if (token != "begin") 
	{
		processError("keyword \"begin\" expected");
	}
	nextToken();
	//cout << token << endl; <- read
	// Modify these if condition for the new stage 2 material
	if(isNonKeyId(token) || token == "read" || token == "write" || token == "if" || token == "while" || token == "repeat" || token == "until" || token == "begin") 
	{
		execStmts();
	}
	// Modify this if condition for the new stage 2 material
	if (token != "end" && !isNonKeyId(token) && token != "read" && token != "write" && token != "if" && token != "while" && token != "repeat" && token != "until" && token != "begin") 
	{
		processError("keyword \"begin\", \"end\", NON_KEY_ID, \"read\", \"write\", \"if\", \"while\", \"repeat\", or \"until\" expected");
	}
	variable = nextToken();
	// This may be correct or incorrect
	if (variable != "." && variable != ";") {
		//cout << "HI" << endl;
		processError("period or semicolon expected");
	}
	if(token == "." && begins.size() > 1){
		processError("ends exepcted the final 'end' must have ';' ");
	}
	if(token == "." && begins.size() == 1){
		code("end", ".", "");
	}
	if(token != ";"){
		nextToken();
	}
	begins.pop();
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
//come back to this and add funtion calls for emit
void Compiler :: code(string op, string operand1, string operand2) {
	// Need to add some else if for stage2
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
		emitAdditionCode(operand1, operand2);
	}
	else if (op == "-") { // this must be binary '-'
		// call to the emitSubtractionCode() function
		emitSubtractionCode(operand1, operand2);
	}
	else if (op == "neg") { // this must be unary '-'
		// call to the emitNegationCode() function
		emitNegationCode(operand1, operand2);
	}
	else if (op == "not") {
		// call to the emitNotCode() function
		emitNotCode(operand1, operand2);
	}
	else if (op == "*") {
		// call to the emitMultiplicationCode() function
		emitMultiplicationCode(operand1, operand2);
	}
	else if (op == "div") {
		// call to the emitDivisionCode() function
		emitDivisionCode(operand1, operand2);
	}
	else if (op == "mod") {
		// call to the emitModuloCode() function
		emitModuloCode(operand1, operand2);
	}
	else if (op == "and") {
		// call to the emitAndCode() function
		emitAndCode(operand1, operand2);
	}
	else if (op == "or") {
		// call to the emitOrCode() function
		emitOrCode(operand1, operand2);
	}
	else if (op == "=") {
		// call to the emitEqualityCode() function
		emitEqualityCode(operand1, operand2);
	}
	else if (op == ":=") {
		emitAssignCode(operand1, operand2);
	}
	else if (op == "<>") {
		// call to the emitInequalityCode() function
		emitInequalityCode(operand1, operand2);
	}
	else if (op == "<") {
		// call to the emitLessThanCode() function
		emitLessThanCode(operand1, operand2);
	}
	else if (op == "<=") {
		// call to the emitLessThanOrEqualToCode() function
		emitLessThanOrEqualToCode(operand1, operand2);
	}
	else if (op == ">") {
		// call to the emitGreaterThanCode() function, get these from victor
		emitGreaterThanCode(operand1, operand2);
	}
	else if (op == ">=") {
		// call to the emitGreaterThanOrEqualCode() function, get these from victor
		emitGreaterThanOrEqualToCode(operand1, operand2);
	} 
	else if (op == "while") {
		// call to the emitWhileCode() function
		emitWhileCode(operand1, operand2);
	} 
	else if (op == "do") {
		// call to the emitDoCode() function
		emitDoCode(operand1, operand2);
	}
	else  if (op == "post_while") { // the string may be incorrect but it's a start
		// call to the emitPostWhileCode() function
		emitPostWhileCode(operand1, operand2);
	}
	else if (op == "repeat") {
		// call to the emitRepeatCode() function
		emitRepeatCode(operand1, operand2);
	}
	else if (op == "then") {
		// call to the emitThenCode() function
		emitThenCode(operand1,operand2);
	}
	else if (op == "else") {
		// call to the emitElseCode() function
		emitElseCode(operand1, operand2);
	}
	else if (op == "post_if") {
		// call to the emitPostIfCode() function
		emitPostIfCode(operand1, operand2);
	}
	else if (op == "until") {
		// call to the emitUntilCode() function
		emitUntilCode(operand1, operand2);
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

			 if((*it).second.getValue() == "false" ){
			 	emit((*it).second.getInternalName(), "dd", "0", ("; "+(*it).first));
			 }
			 else if ((*it).second.getValue() == "true"){
				emit((*it).second.getInternalName(), "dd", "-1", ("; "+(*it).first));
			 }
			 else if(isNonKeyId((*it).second.getValue()) == true){ //THIS MAY BE A PROBLEM!!!!!!!!!!
				 //string temp = (*it).second.getValue();
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

/* --------------- PUSH AND POP OPERTAIONS ----------------------- */
void Compiler::pushOperand(string name) { 

	//add mason data set

   if(name[0] == '.' && name[1] == 'L')
   {
        operandStk.push(name);
   }
   else if(isLiteral(name) && symbolTable.count(name) == 0)
   {
        if(name == "true")
            insert("TRUE", BOOLEAN, CONSTANT, name, YES, 1);
        else if(name == "false")
            insert("FALSE", BOOLEAN, CONSTANT, name, YES, 1);
        else
            insert(name, whichType(name), CONSTANT, name, YES, 1);

        operandStk.push(name);
   }
   else if(!(isLiteral(name)) && symbolTable.count(name) == 0)
   {
       processError("reference to undefined symbol: " + name);
   }
   else 
       operandStk.push(name);
	cout << "pushed: " + name << endl;
}
string Compiler::popOperand() {
	if (!operandStk.empty()) {
		string topElement = operandStk.top();
		operandStk.pop();
		cout << topElement << endl; 
		return topElement;
	} else {
		processError("compiler error; operand stack underflow operand");
	}
	return "";
}

//fix this for edge case, when you get something thst is already in the symbol table
void Compiler :: pushOperator(string op){
    operatorStk.push(op);
}

string Compiler:: popOperator(){

   if(operatorStk.empty() == false){
        string topOfStack = operatorStk.top();
        operatorStk.pop();
        return topOfStack;
   }
   else{
        processError("compiler error; operator stack underflow operator");
   }
   //dummy return
   return "";
}

/* -------------------- END OF PUSH AND POP ---------------------------*/

void Compiler :: part(){
	if(token == "not"){
		nextToken();
		if(token == "("){
			nextToken();
			express();
			//nextToken();
			if(token != ")"){
				processError("no closing parentheses to match opening.");
			}
			code("not", popOperand());
			nextToken();
		}
		else if(isBoolean(token)){
			pushOperand((token == "true" ? "false" : "true")); 
			nextToken();
		}
		else if(isNonKeyId(token)){
			code("not", token);
			nextToken();
		}
		else{
			processError("Expected NON_KEY_ID, '(', or boolean literal after \"not\"");
		}
	}
	else if(token == "+"){
		nextToken();
		if(token == "("){
			nextToken();
			express();
			//nextToken();
			if(token != ")"){
				processError("no closing parentheses to match opening.");
			}

			nextToken();
		}
		else if(isInteger(token) || isNonKeyId(token)){
			pushOperand(token);
			nextToken();
		}
		else{
			processError("must have an integer value after '+'");
		}
	}
	else if(token == "-"){
		nextToken();
		if(token == "("){
			nextToken();
			express();
			//nextToken();
			if(token != ")"){
				processError("no closing parentheses to match opening.");
			}
			code("neg", popOperand());
			nextToken();
			}
			else if(isNonKeyId(token)){
			code("neg", token);
			nextToken();
		}
		else if(isInteger(token) && isLiteral(token)){
			pushOperand("-" + token);
			nextToken();
		}
		else{
			processError("Integer value must follow \"-\"");
		}
	}
	else if(token == "(")
	{
			nextToken();
			express();
			//nextToken();
			if(token != ")"){
				processError("no closing parentheses to match opening.");
			}
			nextToken();
	}
	else if(isInteger(token) || isBoolean(token) || isNonKeyId(token)){
		pushOperand(token);
		nextToken();
	}

}
void Compiler::factors() {
	if (token == "*" || token == "div" || token == "mod" || token == "and") {
		pushOperator(token);

		nextToken();
		part();
		string popOprt, lhs, rhs;
		popOprt = popOperator();
		lhs = popOperand();
		rhs = popOperand();
		code(popOprt, lhs, rhs);

		factors();
	}
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
		nextToken();
		factor();
		string popOprt, lhs, rhs;
		popOprt = popOperator();
		lhs = popOperand();
		rhs = popOperand();
		code(popOprt, lhs, rhs);
		terms();
	}

}
void Compiler::expresses() {
	if (token == "=" || token == "<>" || token == "<=" || token == ">=" || token == "<" || token == ">") {
		pushOperator(token);
		nextToken();
		term();
		string popOprt, lhs, rhs;
		popOprt = popOperator();
		lhs = popOperand();
		rhs = popOperand();
		code(popOprt, lhs, rhs);
		expresses();
	}

}

void Compiler :: writeStmt(){

	string writeOut, temp;
    temp = "temp";
    if(token != "write")
        processError("Keyword \"write\" expected, recieved: " + token);
    nextToken();
    if(token!= "(")
        processError("Token '(' not found, required for write statements");
    nextToken();
    if(isNonKeyId(token) == false)
        processError("NON_KEY_ID expected, recieved: " +token);
    writeOut = ids();

    //nextToken();
    if(token!=")")
        processError("Token ')' not found, must have closing ')' for write statements found: " + token);
    nextToken();
    if(token!=";")
        processError("Token ';' not found, must have for concluding statements");
    uint i = 0;
    while(temp != "")
    {
        temp = "";
        while(writeOut[i] != ',' && i < writeOut.length())
        {
            temp += writeOut[i];
            i++;
        }
        i++;
        if(temp != "")
            code("write", temp);
    }
}

void Compiler :: readStmt(){

	string readIn, temp;
    temp = "temp";
    if(token != "read")
        processError("Keyword \"read\" expected, recieved: " + token);
    nextToken();
    if(token!= "(")
        processError("Token '(' not found, required for read statements");
    nextToken();
    if(isNonKeyId(token) == false)
        processError("NON_KEY_ID expected, recieved: " +token);
    readIn = ids();

    //nextToken();
    if(token!=")")
        processError("Token ')' not found, must have closing ')' for read statements found: " + token);
    nextToken();
    if(token!=";")
        processError("Token ';' not found, must have for concluding statements");
    uint i = 0;
    while(temp != "")
    {
        temp = "";
        while(readIn[i] != ',' && i < readIn.length())
        {
            temp += readIn[i];
            i++;
        }
        i++;
        if(temp != "")
            code("read", temp);
    }
}

void Compiler :: assignStmt(){

		pushOperand(token);
		nextToken();

		if(token != ":="){
			processError("cannot have an assignment statment without \" := \" ");
		}
		pushOperator(token);
		nextToken();
		if(token != "not" && token != "+" && token != "-" && isNonKeyId(token) == false && token != "(" && isLiteral(token) == false){
			processError("error, illegal token in assignment statement");
		}
		express();
		if(token != ";"){
			processError("one of \"*\", \"and\", \"div\", \"mod\", \")\", \"+\", \"-\", \";\", \"<\", \"<=\", \"<>\", \"=\", \">\", \">=\", or \"or\" expected");
		}
		
		string popOprt, lhs, rhs;
		popOprt = popOperator();
		lhs = popOperand();
		rhs = popOperand();
		code(popOprt, lhs, rhs);
}

void Compiler :: execStmt(){
	if(token == "read")
	{
		readStmt(); // call to the readStmt function
	}
	else if(token == "write")
	{
		writeStmt(); // call to the writeStmt function
	}
	else if (isNonKeyId(token)) 
	{ 
		assignStmt(); // call to the assignStmt function
	} 
	else if (token == "if") 
	{
		ifStmt(); // call to the ifStmt function 
	}
	else if (token == "while") 
	{
		whileStmt(); // call to the whileStmt function
	}
	else if (token == "repeat") 
	{
		repeatStmt(); // call to the repeatStmt function
	}
	else if (token == ";") 
	{
		nullStmt(); // call to the nullStmt function
	}
	else if (token == "begin") 
	{
		beginEndStmt(); // call to the beginEndStmt function
	}
	else
	{
		processError("non_key_id, \"read\", \"write\", \"begin\", \"if\", \"while\", \"repeat\", or \";\" expected");
	}
}

void Compiler :: execStmts(){
	// check this function for stage 2 material; may be incorrect
	while(token != "end" && token != "until"){
		execStmt();
		if(token == ";"){
			nextToken();
		}
		//execStmts();
	}
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
	temp = "T" + to_string(currentTempNo); //DO NOT FORGET THE to_string()
	if (currentTempNo > maxTempNo){
		insert(temp, UNKNOWN, VARIABLE, "", NO, 1);
		maxTempNo++;
	}

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
/*------------------------- END OF TEMP STUFF -----------------*/


/*---------------------------------------------------------- EMITS------------ -----------------*/
void Compiler :: emitReadCode(string operand, string operand2) {
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
			else if (whichType(name) == BOOLEAN) {
				processError("can't read variables of this type");
			}
			else if (symbolTable.at(name).getMode() != VARIABLE){
				processError("attempting to read to a read-only location");
			}
			else{
				emit("", "call", "ReadInt", "; read int; value placed in eax");
				emit("", "mov", "[" + symbolTable.at(name).getInternalName() + "],eax", "; store eax at " + operand);
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
			if (name != contentsOfAReg) {
				emit("", "mov", "eax,[" + symbolTable.at(name).getInternalName() + "]", "; load " + name + " in eax");
				contentsOfAReg = name;
			}
         if (whichType(name) == INTEGER || whichType(name) == BOOLEAN) {
				emit("", "call", "WriteInt", "; write int in eax to standard out");
			}
			emit("", "call", "Crlf", "; write \\r\\n to standard out");
		}
	}
}

//op2 = op1
void Compiler :: emitAssignCode(string operand1, string operand2){
   if (whichType(operand1) != whichType(operand2)) {
      processError("incompatible types for operator ':='");
   }
   if (symbolTable.at(operand2).getMode() != VARIABLE) {
      processError("symbol on left-hand side of assignment must have a storage mode of VARIABLE");
   }
   if (operand1 == operand2) {
      return;
   }
   if (operand1 != contentsOfAReg) {
	  emit("", "mov", "eax,[" + symbolTable.at(operand1).getInternalName() + "]", "; AReg = " + operand1);
	  contentsOfAReg = operand1;
   }
   emit("", "mov", "[" + symbolTable.at(operand2).getInternalName() + "],eax", "; " + operand2 + " = AReg"  );
   contentsOfAReg = operand2;
   if (isTemporary(operand1) == true) {
      freeTemp();
   }
}

//op2 + op1
void Compiler :: emitAdditionCode(string operand1, string operand2){
   if (whichType(operand1) != INTEGER || whichType(operand2) != INTEGER) { //maybe use symbol tabel.at
      processError("binary '+' requires integer operands");
   }
   if (isTemporary(contentsOfAReg) == true && (contentsOfAReg != operand1 && contentsOfAReg != operand2)) {
      emit("", "mov", "[" +symbolTable.at(contentsOfAReg).getInternalName() + "],eax", "; deassign AReg" ); // deassign AReg
		symbolTable.at(contentsOfAReg).setAlloc(YES); // change the allocation to YES
		contentsOfAReg = ""; // deassign it
   }
   if (isTemporary(contentsOfAReg) == false && (contentsOfAReg != operand1 && contentsOfAReg != operand2)) {
      contentsOfAReg = ""; // deassign it
   }
   if (contentsOfAReg != operand1 && contentsOfAReg != operand2) { // Neither of the operands equal the contents of the A register
      emit("", "mov", "eax,[" + symbolTable.at(operand2).getInternalName() + "]", "; AReg = " + operand2); // emit code to load operand2 into the A register

	  contentsOfAReg = operand2;
   }
   // addition step
   if(contentsOfAReg == operand1){
	    emit("","add", "eax,[" + symbolTable.at(operand2).getInternalName() + "]", "; AReg = " + operand1 + " + " + operand2);
   }
   else{
	    emit("","add", "eax,[" + symbolTable.at(operand1).getInternalName() + "]", "; AReg = " + operand2 + " + " + operand1);
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
		processError("error only integers may be used with \"-\" ");
	}
	//Call is temp
	if(isTemporary(contentsOfAReg)  && contentsOfAReg != operand2){
		emit("", "mov", "[" + contentsOfAReg + "],eax", "; deassign AReg");
		symbolTable.at(contentsOfAReg).setAlloc(YES);
		contentsOfAReg = "";
	}
	if(contentsOfAReg != operand2){
		emit("", "mov","eax,[" + symbolTable.at(operand2).getInternalName() + "]", "; AReg = " + operand2);
		contentsOfAReg = operand2;
	}
	//emit code for subtraction
	emit("", "sub", "eax,[" + symbolTable.at(operand1).getInternalName() + "]", "; AReg = " + operand2 + " - " +operand1 );

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


void Compiler:: emitMultiplicationCode(string operand1, string operand2) {
   // need to check on this function's code
   if (whichType(operand1) != INTEGER || whichType(operand2) != INTEGER) { //maybe use symbol tabel.at
      processError("Only integers may be used with '*' operator");
   }
   if (isTemporary(contentsOfAReg) == true && (contentsOfAReg != operand1 && contentsOfAReg != operand2)) {
		emit("", "mov", "[" +symbolTable.at(contentsOfAReg).getInternalName() + "],eax", "; deassign AReg" ); // deassign AReg
		symbolTable.at(contentsOfAReg).setAlloc(YES); // change the allocation to YES
		contentsOfAReg = ""; // deassign it
   }
   if (isTemporary(contentsOfAReg) == false && (contentsOfAReg != operand1 && contentsOfAReg != operand2)) {
      contentsOfAReg = ""; // deassign it
   }
   if (contentsOfAReg != operand1 && contentsOfAReg != operand2) { // Neither of the operands equal the contents of the A register
      emit("", "mov", "eax,[" + symbolTable.at(operand2).getInternalName() + "]", "; AReg = " + operand2); // emit code to load operand2 into the A register
	  contentsOfAReg = operand2;
   }
	//emit code for multiplication
	if(contentsOfAReg == operand1){
		emit("", "imul", "dword [" + symbolTable.at(operand2).getInternalName() + "]", "; AReg = " + operand1 + " * " + operand2);
	}
	else {
		emit("", "imul", "dword [" + symbolTable.at(operand1).getInternalName() + "]", "; AReg = " + operand2 + " * " + operand1);
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

//get this from victor
void Compiler :: emitDivisionCode(string operand1, string operand2) {
	if (whichType(operand1) != INTEGER || whichType(operand2) != INTEGER) {
		processError("Only integers may be used with 'div'");
	}
	if (isTemporary(contentsOfAReg) == true && contentsOfAReg != operand2) {
		emit("", "mov", "[" + contentsOfAReg + "],eax", "; deassign AReg");
		symbolTable.at(contentsOfAReg).setAlloc(YES);
		contentsOfAReg = ""; // deassign it
	}
	if (isTemporary(contentsOfAReg) == false && contentsOfAReg != operand2) {
		contentsOfAReg = ""; // deassign it
	}
	if (contentsOfAReg != operand2) {
		emit("", "mov", "eax,[" + symbolTable.at(operand2).getInternalName() + "]", "; AReg = " + operand2);
	}
	// perform the register-memory divison
	emit("","cdq","","; sign extend dividend from eax to edx:eax");
	emit("", "idiv", "dword [" + symbolTable.at(operand1).getInternalName() + "]", "; AReg = " + operand2 + " div " + operand1);


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
		processError("Only integers may be used with 'mod'");
	}
	if (isTemporary(contentsOfAReg) == true && contentsOfAReg != operand2) {
		emit("", "mov", "[" + contentsOfAReg + "],eax", "; fill this in");
		symbolTable.at(contentsOfAReg).setAlloc(YES);
		contentsOfAReg = ""; // deassign it
	}
	if (isTemporary(contentsOfAReg) == false && contentsOfAReg != operand2) {
		contentsOfAReg = ""; // deassign it
	}
	if (contentsOfAReg != operand2) {
		emit("", "mov", "eax,[" + symbolTable.at(operand2).getInternalName() + "]", "; AReg = " + operand2);
	}
	// perform the register-memory modular
	emit("","cdq","","; sign extend dividend from eax to edx:eax");
	emit("", "idiv", "dword [" + symbolTable.at(operand1).getInternalName() + "]", "; AReg = " + operand2 + " div " + operand1);
	emit("", "xchg", "eax,edx", "; exchange quotient and remainder");


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


//these  may need more stuff added, such as mov eax, operandX
void Compiler :: emitNegationCode(string operand1, string nothing){
	if (whichType(operand1) != INTEGER) {
		processError("Only integer values may be used with negation.");
	}
	if (isTemporary(contentsOfAReg) && contentsOfAReg != operand1) {
		emit("", "mov", "[" + contentsOfAReg + "],eax", "; deassign AReg");
		symbolTable.at(contentsOfAReg).setAlloc(YES); // change the allocation to YES
		contentsOfAReg = "";
	}
	if (isTemporary(contentsOfAReg) == false && contentsOfAReg != operand1) {
		contentsOfAReg = ""; // deassign it
	}
	if(contentsOfAReg != operand1){
		emit("", "mov", "eax,[" + symbolTable.at(operand1).getInternalName() + "]", "; AReg = " + operand1);
		contentsOfAReg = operand1;
	}
	
	emit("", "neg ", "eax", "; AReg = -AReg");
		//if operands are temporaroy deassign them
	if(isTemporary(operand1)){
		freeTemp();
	}
	//give a reg the next availbe temp
	contentsOfAReg = getTemp();
	//make a reg an integer
	symbolTable.at(contentsOfAReg).setDataType(INTEGER);

	pushOperand(contentsOfAReg); // push it to the stack

}

void Compiler :: emitNotCode(string operand1, string nothing){
	if (whichType(operand1) != BOOLEAN) {
		processError("Unary 'not' may only be used with boolean operand");
	}
	if (isTemporary(contentsOfAReg) && contentsOfAReg != operand1) {
		emit("", "mov","[" + contentsOfAReg + "],eax", "; deassign AReg");
		symbolTable.at(contentsOfAReg).setAlloc(YES); // change the allocation to YES
		contentsOfAReg = "";
	}
	if (isTemporary(contentsOfAReg) == false && contentsOfAReg != operand1) {
		contentsOfAReg = ""; // deassign it
	}
	if(contentsOfAReg != operand1){
		emit("", "mov", "eax,[" + symbolTable.at(operand1).getInternalName() + "]", "; AReg = " + operand1);
		contentsOfAReg = operand1;
	}
		emit("", "not", "eax", "; AReg = !AReg");
		//if operands are temporaroy deassign them
	if(isTemporary(operand1)){
		freeTemp();
	}

	//give a reg the next availbe temp
	contentsOfAReg = getTemp();
	//make a reg an integer
	symbolTable.at(contentsOfAReg).setDataType(BOOLEAN);

	pushOperand(contentsOfAReg); // push it to the stack
}


void Compiler :: emitEqualityCode(string operand1, string operand2){
	string label1, label2;
	label1 = getLabel();
	label2 = getLabel();
	// types of operands are not the same
	if (whichType(operand1) != whichType(operand2)) {
		processError("Equality operator must ne used with matching data types");
	}
	// if the A Register holds a temp not operand1 nor operand2
	if (isTemporary(contentsOfAReg) && (contentsOfAReg != operand1 && contentsOfAReg != operand2)) {
		emit("", "mov", "[" + contentsOfAReg + "],eax", "; deassign AReg"); // store the temp into memory
		symbolTable.at(contentsOfAReg).setAlloc(YES); // change the allocation to YES
		contentsOfAReg = ""; // deassign it
	}
	// if the A register holds a non-temp not operand2 nor operand1
	if (isTemporary(contentsOfAReg) == false && (contentsOfAReg != operand1 && contentsOfAReg != operand2)) {
		contentsOfAReg = ""; // deassign it
	}
	if (contentsOfAReg != operand1 && contentsOfAReg != operand2) {
		emit("", "mov", "eax,[" + symbolTable.at(operand2).getInternalName() + "]", "; AReg = " + operand2); // load operand2 into the A register
	}
	if(contentsOfAReg == operand1){
		emit("", "cmp", "eax,[" + symbolTable.at(operand2).getInternalName() + "]","; compare " + operand1 + " and " + operand2);
	}
	else{
		emit("", "cmp", "eax,[" + symbolTable.at(operand1).getInternalName() + "]","; compare " + operand2 + " and " + operand1);
	}
	
	emit("", "je", label1, "; if " + operand2 + " = " + operand1 + " then jump to set eax to TRUE" ); // jump if equal to the next available Ln (call getLabel)
	emit("", "mov", "eax,[FALSE]", "; else set eax to FALSE"); //  load FALSE into the A register
	emit("", "jmp", label2, "; unconditionally jump"); // perform an unconditional jump to the next label (call getLabel should be L(n+1))
	emit(label1 + ":", "", "", ""); // emit code to label the next instruction with the first acquired label Ln
	emit("", "mov", "eax,[TRUE]", "; set eax to TRUE"); //  load TRUE into the A register
	emit(label2 + ":", "", "", "");
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
		processError("Inequality operator must be used with matching types.");
	}
	// if the A Register holds a temp not operand1 nor operand2
	if (isTemporary(contentsOfAReg) && (contentsOfAReg != operand1 && contentsOfAReg != operand2)) {
		emit("", "mov", "[" + contentsOfAReg + "],eax", "; deassign AReg"); // store the temp into memory
		symbolTable.at(contentsOfAReg).setAlloc(YES); // change the allocation to YES
		contentsOfAReg = ""; // deassign it
	}
	// if the A register holds a non-temp not operand2 nor operand1
	if (isTemporary(contentsOfAReg) == false && (contentsOfAReg != operand1 && contentsOfAReg != operand2)) {
		contentsOfAReg = ""; // deassign it
	}
	if (contentsOfAReg != operand1 && contentsOfAReg != operand2) {
		emit("", "mov", "eax,[" + symbolTable.at(operand2).getInternalName() + "]", "; AReg = " + operand2); // load operand2 into the A register
	}
	emit("", "cmp", "eax,[" + symbolTable.at(operand1).getInternalName() + "]","; compare " + operand2 + " and " + operand1); // perform a register-memory compare
	emit("", "jne", label1, "; if " + operand2 + " <> " + operand1 +" then jump to set eax to TRUE"); // jump if equal to the next available Ln (call getLabel)
	emit("", "mov", "eax,[FALSE]", "; else set eax to FALSE"); //  load FALSE into the A register
	emit("", "jmp", label2, "; unconditionally jump"); // perform an unconditional jump to the next label (call getLabel should be L(n+1))
	emit(label1 + ":", "", "", ""); // emit code to label the next instruction with the first acquired label Ln
	emit("", "mov", "eax,[TRUE]", "; set eax to TRUE"); //  load TRUE into the A register
	emit(label2 + ":", "", "", "");
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

void Compiler :: emitOrCode(string operand1, string operand2) {
	if (whichType(operand1) != BOOLEAN || whichType(operand2) != BOOLEAN) {
		processError("illegal type");
	}
	if (isTemporary(contentsOfAReg) && contentsOfAReg != operand1 && contentsOfAReg != operand2) {
		emit("", "mov", "[" + contentsOfAReg + "],eax", "; deassign AReg");
		symbolTable.at(contentsOfAReg).setAlloc(YES); // change the allocation to YES
		contentsOfAReg = "";
	}
	if (isTemporary(contentsOfAReg) == false && contentsOfAReg != operand1 && contentsOfAReg != operand2) {
		contentsOfAReg = ""; // deassign it
	}
	if (contentsOfAReg != operand1 && contentsOfAReg != operand2) {
		emit("", "mov", "eax,[" + symbolTable.at(operand2).getInternalName() + "]", "; AReg = " + operand2);
		contentsOfAReg = operand2;
	}
	if (contentsOfAReg == operand1) {
		emit("", "or", "eax,[" + symbolTable.at(operand2).getInternalName() + "]", "; AReg = " + operand1 + " or " + operand2);
	}
	else {
		emit("", "or", "eax,[" + symbolTable.at(operand1).getInternalName() + "]", "; AReg = " + operand2 + " or " + operand1);
	}
	//emit("TEST", "TEST", "TEST", "TEST");
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

void Compiler :: emitAndCode(string operand1, string operand2){
	if(whichType(operand1) != BOOLEAN || whichType(operand2) != BOOLEAN){
		processError("binary 'and' requires boolean operands");
	}
	if(isTemporary(contentsOfAReg) && contentsOfAReg != operand1 && contentsOfAReg != operand2){
		emit("", "mov","[" + contentsOfAReg + "],eax", "; deassign AReg" );
		symbolTable.at(contentsOfAReg).setAlloc(YES);
		contentsOfAReg = "";
	}
	if(contentsOfAReg != operand1 && contentsOfAReg != operand2){
		emit("", "mov", "eax,[" + symbolTable.at(operand2).getInternalName() + "]", "; AReg = " + operand2);
		contentsOfAReg = operand2;
	}
	if(contentsOfAReg == operand1){
		emit("", "and ", "eax,[" + symbolTable.at(operand2).getInternalName() + "]", "; AReg = " + operand1 + " and " + operand2);
	}
	else{
		//emit("", "mov ", "eax, [" + operand1 +"]", ";Areg  = " + operand1 );
		emit("", "and ", "eax,[" + symbolTable.at(operand1).getInternalName() + "]", "; AReg = " + operand2 + " and " + operand1);
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

	pushOperand(contentsOfAReg);
}
//op2 lhs op1 rhs
void Compiler :: emitLessThanCode(string operand1, string operand2){
	string label1 = getLabel();
	string label2 = getLabel();

	if(whichType(operand1) != INTEGER || whichType(operand2) != INTEGER){
		processError("error only integers may be used with \"<\" ");
	}
	//Call is temp
	if(isTemporary(contentsOfAReg) && contentsOfAReg != operand1 && contentsOfAReg != operand2){
		emit("", "mov", "[" + contentsOfAReg + "],eax", "; deassign AReg" );
		symbolTable.at(contentsOfAReg).setAlloc(YES);
		contentsOfAReg = "";
	}
	if(contentsOfAReg != operand2){
		emit("", "mov", "eax,[" + symbolTable.at(operand2).getInternalName() + "]", "; AReg = " + operand2);
		contentsOfAReg = operand2;
	}
	emit("", "cmp ", "eax,[" + symbolTable.at(operand1).getInternalName() + "]", "; compare " + operand2 + " and " + operand1);
	emit("", "jl ", label1, "; if " + operand2 + " < " + operand1 + " then jump to set eax to TRUE");
	emit("", "mov ", "eax,[FALSE]", "; else set eax to FALSE");
	emit("", "jmp ", label2, "; unconditionally jump");
	emit(label1 + ":", "", "" ,"");
	emit("", "mov ", "eax,[TRUE]", "; set eax to TRUE");
	emit(label2 + ":", "", "" ,"");

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
		emit("", "mov", "[" + contentsOfAReg + "],eax", "; deassign AReg" );
		symbolTable.at(contentsOfAReg).setAlloc(YES);
		contentsOfAReg = "";
	}
	if(contentsOfAReg != operand1 && contentsOfAReg != operand2){
		emit("", "mov", "eax,[" + symbolTable.at(operand2).getInternalName() + "]", "; AReg = " + operand2);
		contentsOfAReg = operand2;
	}
		emit("", "cmp ", "eax,[" + symbolTable.at(operand1).getInternalName() + "]", "; compare " + operand2 + " and " + operand1);
		emit("", "jle ", label1, "; if " + operand2 + " <= " +operand1 + " then jump to set eax to TRUE" );
		emit("", "mov ", "eax,[FALSE]", "; else set eax to FALSE");
		emit("", "jmp ", label2, "; unconditionally jump");
		emit(label1 + ":", "", "" ,"");
		emit("", "mov ", "eax,[TRUE]", "; set eax to TRUE");
		emit(label2 + ":", "", "" ,"");
		

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

	pushOperand(contentsOfAReg);

}

void Compiler :: emitGreaterThanCode(string operand1, string operand2) {
	string label1, label2;
	label1 = getLabel();
	label2 = getLabel();
	// types of operands are not the same
	if (whichType(operand1) != INTEGER || whichType(operand2) != INTEGER) {
		processError("Only integers may be used with '>'");
	}
	// if the A Register holds a temp not operand1 nor operand2
	if (isTemporary(contentsOfAReg) && (contentsOfAReg != operand1 && contentsOfAReg != operand2)) {
		emit("", "mov", "[" + contentsOfAReg + "],eax", "; deassign AReg"); // store the temp into memory
		symbolTable.at(contentsOfAReg).setAlloc(YES); // change the allocation to YES
		contentsOfAReg = ""; // deassign it
	}
	// if the A register holds a non-temp not operand2 nor operand1
	if (isTemporary(contentsOfAReg) == false && (contentsOfAReg != operand1 && contentsOfAReg != operand2)) {
		contentsOfAReg = ""; // deassign it
	}
	if (contentsOfAReg != operand1 && contentsOfAReg != operand2) {
		emit("", "mov", "eax,[" + symbolTable.at(operand2).getInternalName() + "]", "; AReg = " + operand2); // load operand2 into the A register
	}
	emit("", "cmp", "eax,[" + symbolTable.at(operand1).getInternalName() + "]","; compare " + operand2 + " and " + operand1); // perform a register-memory compare
	emit("", "jg", label1, "; if " + operand2 + " > " + operand1 + " then jump to set eax to TRUE"); // jump if greater than and the next available Ln (call getLabel)
	emit("", "mov", "eax,[FALSE]", "; else set eax to FALSE"); //  load FALSE into the A register
	emit("", "jmp", label2, "; unconditionally jump"); // perform an unconditional jump to the next label (call getLabel should be L(n+1))
	emit(label1 + ":", "", "", ""); // emit code to label the next instruction with the first acquired label Ln
	emit("", "mov", "eax,[TRUE]", "; set eax to TRUE"); //  load TRUE into the A register
	emit(label2 + ":", "", "", "");
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
	if (whichType(operand1) != INTEGER || whichType(operand2) != INTEGER) {
		processError("Only integers may be used with '>='");
	}
	// if the A Register holds a temp not operand1 nor operand2
	if (isTemporary(contentsOfAReg) && (contentsOfAReg != operand1 && contentsOfAReg != operand2)) {
		emit("", "mov", "[" + contentsOfAReg + "],eax", "; deassign AReg"); // store the temp into memory
		symbolTable.at(contentsOfAReg).setAlloc(YES); // change the allocation to YES
		contentsOfAReg = ""; // deassign it
	}
	// if the A register holds a non-temp not operand2 nor operand1
	if (isTemporary(contentsOfAReg) == false && (contentsOfAReg != operand1 && contentsOfAReg != operand2)) {
		contentsOfAReg = ""; // deassign it
	}
	if (contentsOfAReg != operand1 && contentsOfAReg != operand2) {
		emit("", "mov", "eax,[" + symbolTable.at(operand2).getInternalName() + "]", "; AReg = " + operand2); // load operand2 into the A register
		contentsOfAReg = operand2;
	}
	emit("", "cmp", "eax,[" + symbolTable.at(operand1).getInternalName() + "]","; compare " + operand2 + " and " + operand1); // perform a register-memory compare
	emit("", "jge", label1, "; if " + operand2 + " >= " + operand1 + " then jump to set eax to TRUE"); // jump if greater than or equal to the next available Ln (call getLabel)
	emit("", "mov", "eax,[FALSE]", "; else set eax to FALSE"); //  load FALSE into the A register
	emit("", "jmp", label2, "; unconditionally jump"); // perform an unconditional jump to the next label (call getLabel should be L(n+1))
	emit(label1 + ":", "", "", ""); // emit code to label the next instruction with the first acquired label Ln
	emit("", "mov", "eax,[TRUE]", "; set eax to TRUE"); //  load TRUE into the A register
	emit(label2 + ":", "", "", "");
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
/*------------------ STAGE 2 STUFF -------------------------*/
bool Compiler :: isLabel(string s) const {
	// Copy and pasted from the isTemporary() function with the appropriate modifications
	//cout << s.at(0) << " HEY" << endl;
	if(s.at(1) == 'L'){
		return true;
	}
	else {
		return false;
	}
}
void Compiler :: ifStmt() 
{
	bool nestedIf = false;

	if (token == "if") // if the token received is the "if" keyword
	{
		nextToken();
		express();
		//nextToken(); // call to the next token
		if (token != "then") // if the token received is the "then" keyword
		{
			processError("Keyword 'then' expcteded");
		}

		code("then", popOperand(), "");
		nextToken();
		if(token == "if"){
			nestedIf = true;
		}
		
		execStmt();

		if(nestedIf == false){
			nextToken();
		}
		
		if(token == "else"){
			code("else", popOperand(), "");
			nextToken();
			elsePt();
			code("post_if", popOperand(), "");
		}
		else{
			code("post_if",popOperand(), "");
		}
		
	}
}
void Compiler :: elsePt() 
{

		execStmt();

}
void Compiler :: whileStmt() // token should be while
{
	string lhs, rhs;

    if (token == "while") {
		code("while","","");
		nextToken();
		express();
		if (token != "do") {
			processError("Keyword 'do' expected.");
		}

		code("do", popOperand(), "");
		nextToken();
		execStmt();
		lhs = popOperand();
		rhs = popOperand();
		code("post_while", lhs, rhs);
	}
}
void Compiler :: repeatStmt() 
{
	string lhs, rhs;
	if (token == "repeat") {

		code("repeat","","");
		nextToken();
		execStmts();

		if (token != "until") {
			processError("keyword 'until' expected");
		
		}
		nextToken();
		express();
		lhs = popOperand();
		rhs = popOperand();
		code("until", lhs, rhs);
		nextToken();
	}
}
void Compiler :: nullStmt() 
{
	// may be incorrect for this function
	if (token != ";") 
	{
		processError("Token ';' not found, must have for concluding statements");
	}
}
/*----------------------- EMITS FOR STAGE 2 -----------------------------*/
void Compiler :: emitWhileCode(string operand1, string operand2) 
{
	string tempLabel; // created a string variable called tempLabel
	tempLabel = getLabel(); // assign next label to tempLabel
	emit(tempLabel + ":","","","; while"); // emit instruction to label this point of object code as tempLabel
	cout << "while label" + tempLabel << endl;
	pushOperand(tempLabel); // push tempLabel into operandStk
	cout << "AA" << endl;
	contentsOfAReg = ""; // deassign operands from all registers
}
void Compiler :: emitDoCode(string operand1, string operand2) 
{
	string tempLabel; // created a string variable called tempLabel
	if (whichType(operand1) != BOOLEAN) { // if the datatype of operand1 is not boolean
		processError("while predicate must be of type boolean"); // error message
	}
	cout << "BB" << endl;
	tempLabel = getLabel(); // assign next label to tempLabel
	if (contentsOfAReg != operand1) { // if operand1 is not in the A Register
		emit("","mov","eax,[" + symbolTable.at(operand1).getInternalName() + "]","; AReg = " + operand1); // emit instruction to move operand1 to the A register
		contentsOfAReg = operand1;
	}
	cout << "CC" << endl;
	emit("", "cmp", "eax,0", "; compare eax to 0"); // emit instruction to compare the A register to zero (false)
	emit("", "je", tempLabel, "; if " + operand1 + " is false then jump to end while TEST"); // emit code to branch to tempLabel if the compare indicates equality
	cout << "DD" << endl;
	pushOperand(tempLabel); // push tempLabel onto operandStk
	if (isTemporary(operand1)) { // if operand1 is a temp
		freeTemp(); // free operand's name for reuse
	}
	cout << "EE" << endl;
	contentsOfAReg = ""; // deassign operands from all registers
}
void Compiler :: emitPostWhileCode(string operand1, string operand2) 
{
	cout << "FF" << endl;
	emit("","jmp",operand2,"; end while"); // emit instruction which branches unconditionally to the beginning of the loop, i.e., to the value of operand2
	cout << "GG" << endl;
	emit(operand1 + ":","","",""); // emit instruction which labels this point of the object code with the argument operand1
	contentsOfAReg = ""; // deassign operands from all registers
}
void Compiler :: emitRepeatCode(string operand1, string operand2) 
{
	string tempLabel; // created a string variable called tempLabel
	tempLabel = getLabel(); // assign next label to tempLabel
	emit(tempLabel + ":","","","; repeat"); // emit instruction to label this point in the object code with the value of tempLabel
	pushOperand(tempLabel); // push tempLabel into operandStk
	contentsOfAReg = ""; // deassign operands from all registers
}
void Compiler :: emitElseCode(string operand1, string operand2)
{
	string tempLabel; // created a string variable called tempLabel
	tempLabel = getLabel(); // assign next label to tempLabel
	emit("", "jmp", tempLabel, "; unconditionally jump"); // emit instruction to branch unconditionally to tempLabel
	emit(operand1 + ":", "", "", ""); // emit instruction to label this point of object code with the argument operand1
	pushOperand(tempLabel); // push tempLabel onto operandStk
	contentsOfAReg = ""; // deassign operands from all registers
}
void Compiler :: emitPostIfCode(string operand1, string operand2)
{
	emit(operand1 + ":","","","; end if"); // emit instruction to label this point of object code with the argument operand1
	contentsOfAReg = ""; // deassign operands from all registers
}
void Compiler :: emitThenCode(string operand1, string operand2)
{
	string tempLabel; // created a string variable called tempLabel
	if (whichType(operand1) != BOOLEAN) { // if the data type of operand1 is not boolean
		processError("if predicate must be of type boolean"); // error message
	}
	tempLabel = getLabel(); // assign next label to tempLabel
	if (contentsOfAReg != operand1) { // if the contents of the A Register is not operand1
		emit("","mov","eax,[" + symbolTable.at(operand1).getInternalName() + "]","; AReg = " + operand1); // emit instruction to move operand1 to the A register
	}
	emit("", "cmp", "eax,0", "; compare eax to 0"); // emit instruction to compare the A register to zero (false)
	emit("", "je", tempLabel, "; if " + operand1 + " is false then jump to end of if"); // emit code to branch to tempLabel if the compare indicates equality
	pushOperand(tempLabel); //  push tempLabel onto operandStk so that it can be referenced when emitElseCode() or emitPostIfCode() is called
	if (isTemporary(operand1) == true) { // if operand1 is a temp then
		freeTemp(); // free it for reuse 
	}
	contentsOfAReg = ""; // deassign operands from all registers
}
void Compiler :: emitUntilCode(string operand1, string operand2)
{
	if (whichType(operand1) != BOOLEAN) { // if the data type of operand1 is not boolean
		processError("if predicate must be of type boolean"); // error message
	}
	if (contentsOfAReg != operand1) { // if the contents of a A Register is not operand1
		emit("", "mov", "eax,[" + symbolTable.at(operand1).getInternalName() + "]", "; AReg = " + operand1); // emit instruction to move operand1 to the A register
	}
	emit("","cmp","eax,0","; compare eax to 0"); // emit instruction to compare the A register to zero (false)
	emit("", "je", operand2, "; if " + operand1 + " is false then jump to end while"); // emit code to branch to operand2 if the compare indicates equality
	if (isTemporary(operand1) == true) { // if the operand1 is a temp then
		freeTemp(); // free for reuse
	}
	contentsOfAReg = ""; // deassign operands from all registers
}
