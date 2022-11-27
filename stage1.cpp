//Esai Barron and Victor Obioma
//CS 4301
//Stage 1

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
                            "begin", "end", "true", "false", "not", "and", "or", "mod", "div", "and", "or", "read", "write"};

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
			} else {
				if (isupper(name[0])) {
					cout << "Temps created" << endl;
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
	if (token != "begin") {
		processError("keyword \"begin\" expected");
	}
	execStmts();
	if (token != "end") {
		cout << "I WANT TO DIE " << token << endl;
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
//come back to this and add cuntion calls for emit
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
void Compiler::pushOperand(string name){ //push name onto operandStk//if name is a literal, also create a symbol table entry for it
    if (isLiteral(name) && symbolTable.count(name) != 1){
        insert(name, whichType(name), CONSTANT, name, YES, 1);
    }
    operandStk.push(name);
}
string Compiler::popOperand() {
	if (!operandStk.empty()) {
		string topElement = operandStk.top();
		operandStk.pop();
		return topElement;
	} else {
		//cout << "DEBUG popOperand "<<operandStk.top() << endl; this caused a core dump
		processError("compiler error; operand stack underflow operand");
	}
	return "";
}

//fix this for edge case, when you get something thst is already in the symbol table
void Compiler :: pushOperator(string op){
	cout << "DEBUG pushOPRTR: " << op << endl;
    operatorStk.push(op);
}

string Compiler:: popOperator(){

   if(operatorStk.empty() == false){
        string topOfStack = operatorStk.top();
        operatorStk.pop();
        return topOfStack;
   }
   else{
	   //cout << "DEBUG popOperator "<<operandStk.top() << endl;
        processError("compiler error; operator stack underflow operator");
   }
   //dummy return
   return "";
}

/* -------------------- END OF PUSH AND POP ---------------------------*/

void Compiler :: part(){
	cout << "partbegin" << endl;
	if(token == "not"){
		nextToken();
		cout << "part1" << endl;
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
			cout << "partpushOperand1 " + token << endl;
			pushOperand(token);
			nextToken();
		}
		else if(isNonKeyId(token)){
			code("not", token);
			nextToken();
		}
		else{
			processError("a non boolean operator cannot follow \"not\" goofy");
		}
	}
	else if(token == "+"){
		nextToken();
		cout << "part2" << endl;
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
			cout << "partpushOperand2 " + token << endl;
			pushOperand(token);
			nextToken();
		}
		else{
			processError("must have an integer valte after \'+\' GOPFY");
		}
	}
	else if(token == "-"){
		nextToken();
		cout << "part3" << endl;
		if(token == "("){
			nextToken();
			express();
			//nextToken();
			if(token != ")"){
				processError("no closing parentheses to match opening.");
			}
			cout << "<<<<<<<<<<<<<< DID WE GET TO - IN PART " << endl << endl;
			code("neg", popOperand());
			nextToken();
		}
		else if(isInteger(token)){
			cout << "partpushOperand3 " + token << endl;
			pushOperand("-" + token);
			nextToken();
		}
		else if(isNonKeyId(token)){
			code("neg", token);
			nextToken();
		}
		else{
			processError("Integer value must follow \"-\"");
		}
	}
	else if(token == "(")
	{
		cout << "part5" << endl;
			nextToken();
			express();
			//nextToken();
			if(token != ")"){
				processError("no closing parentheses to match opening.");
			}
			nextToken();
	}
	else if(isInteger(token) || isBoolean(token) || isNonKeyId(token)){
		cout << "partpushOperand4 " + token << endl;
		pushOperand(token);
		cout << "part4" << endl;
		nextToken();
	}

}
void Compiler::factors() {
	cout << "TESTING FROM FACTORS: " << token << endl;
	if (token == "*" || token == "div" || token == "mod" || token == "and") {
		pushOperator(token);

		nextToken();
		cout << "TOKEN IN FACTORS: " + token << endl;
		part();
		string popOprt, lhs, rhs;
		popOprt = popOperator();
		lhs = popOperand();
		rhs = popOperand();
		code(popOprt, lhs, rhs);

		factors();
	}
	cout << "factors end" << endl;
}
void Compiler::express() {
	cout << "It got to express " << token << endl;
	term();
	cout << "call to term in express success" << endl;
	expresses();
	cout << "call to expresses in express success" << endl;
}
void Compiler::factor() {
	part();
	factors();
	cout << "factor end" << endl;
}
void Compiler::term() {
	cout << "made it to term " << token << endl;
	cout << "top of operator stk bruh: " << operatorStk.top() << endl;
	factor();
	cout << "finished factor" << endl;
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
		cout << "EXPRESS: " << token << endl;
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
	cout << "LATEST TESTING: " << token << endl;


		pushOperand(token);
		nextToken();

		if(token != ":="){
			//cout << "DEBUG IN ASSIGNSTMT: " << token << endl;
			processError("cannot have an assignment statment without \" := \" ");
		}
		pushOperator(token);
		nextToken();
		if(token != "not" && token != "+" && token != "-" && isNonKeyId(token) == false && token != "(" && isLiteral(token) == false){
			processError("error, illegal token in assignment statement");
		}
		express();
		cout << "finishedExpress!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << endl;
		if(token != ";"){
			//cout << "DEBUG IN ASSIGNSTMT2: " << token << endl;
			processError("one of \"*\", \"and\", \"div\", \"mod\", \")\", \"+\", \"-\", \";\", \"<\", \"<=\", \"<>\", \"=\", \">\", \">=\", or \"or\" expected");
		}
		
		string popOprt, lhs, rhs;
		popOprt = popOperator();
		lhs = popOperand();
		rhs = popOperand();
		code(popOprt, lhs, rhs);

}

//fix this
void Compiler :: execStmt(){
	if(token == "read"){
		readStmt();
	}
	else if(token == "write"){
		writeStmt();
	}
	else if (isNonKeyId(token)){ //dont jump ahead too far
		assignStmt();
	}
}

//also fix this
void Compiler :: execStmts(){
	while(token != "end"){
		execStmt();
		nextToken();
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
				processError("cannot read varaibles of this type (boolean)");
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
   //cout << "ASSIGN CODE DEBUG!!!!!!!!!!!!!!!!!" << isLiteral(operand2) << " BRUH " <<whichType(operand1) << endl;
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
	    //cout << "ASSIGN CODE DEBUG!!!!!!!!!!!!!!!!!" << isLiteral(operand2) << " BRUH " <<whichType(operand1) << endl;
	  emit("", "mov", "eax,[" + symbolTable.at(operand1).getInternalName() + "]", "; AReg = " + operand1);
	   contentsOfAReg = operand1;
   }
   emit("", "mov", "[" + symbolTable.at(operand2).getInternalName() + "],eax", "; " + operand2 + " = AReg"  );
   contentsOfAReg = operand2;
   if (isTemporary(operand1) == true) {
      freeTemp();
   }

   //emit("Done","" , "", ""  );
}

//op2 + op1
void Compiler :: emitAdditionCode(string operand1, string operand2){

	cout << "\nadd ops " << operand1 << " " << operand2 << endl;
   if (whichType(operand1) != INTEGER || whichType(operand2) != INTEGER) { //maybe use symbol tabel.at
      processError("binary '+' requires integer operands");
   }
   if (isTemporary(contentsOfAReg) == true && (contentsOfAReg != operand1 && contentsOfAReg != operand2)) {
      emit("", "mov", "[" +symbolTable.at(contentsOfAReg).getInternalName() + "],eax", "; deassign AReg" ); // store that temp into memory
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
	if(isTemporary(contentsOfAReg) && contentsOfAReg != operand1 && contentsOfAReg != operand2){
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
      emit("", "mov", "[" +symbolTable.at(contentsOfAReg).getInternalName() + "],eax", ";store temp in memory" ); // store that temp into memory
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
	cout << "<<<<<<<<<<<<<< WE MADE IT TO NEGATION CODE" << operand1 << endl;
	if (whichType(operand1) != INTEGER) {
		processError("illegal type");
	}
	if (isTemporary(contentsOfAReg) && contentsOfAReg != operand1) {
		emit("", "mov", "[" + contentsOfAReg + "],eax", "; store that temp into memory");
		symbolTable.at(contentsOfAReg).setAlloc(YES); // change the allocation to YES
		contentsOfAReg = "";
	}
	if (isTemporary(contentsOfAReg) == false && contentsOfAReg != operand1) {
		contentsOfAReg = ""; // deassign it
	}
	else{
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
}

void Compiler :: emitNotCode(string operand1, string nothing){
	if (whichType(operand1) != BOOLEAN) {
		processError("Unary 'not' may only be used with boolean operand");
	}
	if (isTemporary(contentsOfAReg) && contentsOfAReg != operand1) {
		emit("", "mov","[" + contentsOfAReg + "],eax", "; store that temp into memory");
		symbolTable.at(contentsOfAReg).setAlloc(YES); // change the allocation to YES
		contentsOfAReg = "";
	}
	if (isTemporary(contentsOfAReg) == false && contentsOfAReg != operand1) {
		contentsOfAReg = ""; // deassign it
	}
	else{
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
	if (isTemporary(contentsOfAReg) && (contentsOfAReg != operand1 && contentsOfAReg != operand2)) {
		emit("", "mov", "[" + contentsOfAReg + "],eax", "; store that temp into memory"); // store the temp into memory
		symbolTable.at(contentsOfAReg).setAlloc(YES); // change the allocation to YES
		contentsOfAReg = ""; // deassign it
	}
	// if the A register holds a non-temp not operand2 nor operand1
	if (isTemporary(contentsOfAReg) == false && (contentsOfAReg != operand1 && contentsOfAReg != operand2)) {
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
	if (isTemporary(contentsOfAReg) && (contentsOfAReg != operand1 && contentsOfAReg != operand2)) {
		emit("", "mov", "[" + contentsOfAReg + "],eax", "; store that temp into memory"); // store the temp into memory
		symbolTable.at(contentsOfAReg).setAlloc(YES); // change the allocation to YES
		contentsOfAReg = ""; // deassign it
	}
	// if the A register holds a non-temp not operand2 nor operand1
	if (isTemporary(contentsOfAReg) == false && (contentsOfAReg != operand1 && contentsOfAReg != operand2)) {
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

void Compiler :: emitOrCode(string operand1, string operand2) {
	string label1 = getLabel();
	string label2 = getLabel();
	if (whichType(operand1) != BOOLEAN || whichType(operand2) != BOOLEAN) {
		processError("illegal type");
	}
	if (isTemporary(contentsOfAReg) && contentsOfAReg != operand1 && contentsOfAReg != operand2) {
		emit("", "mov", "[" + contentsOfAReg + "],eax", "; store that temp into memory");
		symbolTable.at(contentsOfAReg).setAlloc(YES); // change the allocation to YES
		contentsOfAReg = "";
	}
	if (isTemporary(contentsOfAReg) == false && contentsOfAReg != operand1 && contentsOfAReg != operand2) {
		contentsOfAReg = ""; // deassign it
	}
	if (contentsOfAReg != operand1 || contentsOfAReg != operand2) {
		emit("", "mov", "eax,[" + symbolTable.at(operand2).getInternalName() + "]", "; AReg = " + operand2);
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
		processError("error only booleans may be used with \"and\" ");
	}
	if(isTemporary(contentsOfAReg) && contentsOfAReg != operand1 && contentsOfAReg != operand2){
		emit("", "mov","[" + contentsOfAReg + "],eax", ";emit code to load operand2 into the A register" );
		symbolTable.at(contentsOfAReg).setAlloc(YES);
		contentsOfAReg = "";
	}
	if(contentsOfAReg != operand1 && contentsOfAReg != operand2){
		emit("", "mov", "eax,[" + symbolTable.at(operand2).getInternalName() + "]", "; AReg = " + operand2);
		contentsOfAReg = operand2;
	}
	if(contentsOfAReg == operand1){
		emit("", "and ", "eax,[" + symbolTable.at(operand2).getInternalName() + "]", "; AReg = " + operand1 + " and first " + operand2);
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
		emit("", "mov", "[" + contentsOfAReg + "],eax", ";emit code to load operand2 into the A register" );
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
		emit("", "mov", "[" + contentsOfAReg + "],eax", ";emit code to load operand2 into the A register" );
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
	symbolTable.at(contentsOfAReg).setDataType(BOOLEAN);

	pushOperand(contentsOfAReg);

}

void Compiler :: emitGreaterThanCode(string operand1, string operand2) {
	cout << "KANYEEEEEEEEEEEEEEEEE " << operand1 << " AND " << operand2 << " contents of areg: " << contentsOfAReg<<endl; 
	string label1, label2;
	label1 = getLabel();
	label2 = getLabel();
	// types of operands are not the same
	if (whichType(operand1) != INTEGER || whichType(operand2) != INTEGER) {
		cout << "-----------GREATER THAN DEBUG ----------------------" << whichType(operand1) << "       " << whichType(operand2) << endl << endl;
		processError("Only integers may be used with '>'");
	}
	// if the A Register holds a temp not operand1 nor operand2
	if (isTemporary(contentsOfAReg) && (contentsOfAReg != operand1 && contentsOfAReg != operand2)) {
		emit("", "mov", "[" + contentsOfAReg + "],eax", "; store that temp into memory"); // store the temp into memory
		symbolTable.at(contentsOfAReg).setAlloc(YES); // change the allocation to YES
		contentsOfAReg = ""; // deassign it
	}
	// if the A register holds a non-temp not operand2 nor operand1
	if (isTemporary(contentsOfAReg) == false && (contentsOfAReg != operand1 && contentsOfAReg != operand2)) {
		contentsOfAReg = ""; // deassign it
	}
	if (contentsOfAReg != operand1 && contentsOfAReg != operand2) {
		emit("", "mov", "eax,[" + symbolTable.at(operand2).getInternalName() + "]", ";move contents of variables to eaxzzs"); // load operand2 into the A register
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
		cout << "THIS BETTER NOT BE THE PROBLEM BRO__________________________" << endl;
		processError("Only integers may be used with '>='");
	}
	// if the A Register holds a temp not operand1 nor operand2
	if (isTemporary(contentsOfAReg) && (contentsOfAReg != operand1 && contentsOfAReg != operand2)) {
		emit("", "mov", "[" + contentsOfAReg + "],eax", "; store that temp into memory"); // store the temp into memory
		symbolTable.at(contentsOfAReg).setAlloc(YES); // change the allocation to YES
		contentsOfAReg = ""; // deassign it
	}
	// if the A register holds a non-temp not operand2 nor operand1
	if (isTemporary(contentsOfAReg) == false && (contentsOfAReg != operand1 && contentsOfAReg != operand2)) {
		contentsOfAReg = ""; // deassign it
	}
	if (contentsOfAReg != operand1 && contentsOfAReg != operand2) {
		emit("", "mov", "eax,[" + symbolTable.at(operand2).getInternalName() + "]", ";move contents of variables to eax"); // load operand2 into the A register
	}
	emit("", "cmp", "eax, [" + symbolTable.at(operand1).getInternalName() + "]","; fill this in"); // perform a register-memory compare
	emit("", "jge", label1, ""); // jump if greater than or equal to the next available Ln (call getLabel)
	emit("", "mov", "eax, [FALSE]", "; load FALSE"); //  load FALSE into the A register
	emit("", "jmp", label2, "; unconditional jump"); // perform an unconditional jump to the next label (call getLabel should be L(n+1))
	emit(label1 + ":", "", "", ""); // emit code to label the next instruction with the first acquired label Ln
	emit("", "mov", "eax, [TRUE]", "; load TRUE"); //  load TRUE into the A register
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
