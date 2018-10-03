#include <iostream>
#include <string>
#include <list>
#include <fstream>
#include <vector>
#include "token.h"
#include "InputStream.h"
#include "Lexer.h"
using namespace std;


	char Lexer::advance() {
		return istream->getNext();
	}

	Token Lexer::getCurrentToken() {
		return *tokenList->at(currentIntToken);
	}

	Token Lexer::peekNextToken() {
		return *tokenList->at(currentIntToken + 1);
	}


	void Lexer::startState() {
		state = START;
		currentChar = ' ';
		while (currentChar != EOF) {
			switch (state) {

			case START:
				// Deal with last character state things
				charSoFar = "";
				currentChar = advance();
				curLineNum = istream->getLineNumber();
				state = parseChar(currentChar);
				break;

			case PUNCUTATION:
				// token has been added -> go back to Start State
				state = START;
				break;

			case COLON:
				// get next character and see if it's a colon dash - else add the colon
				state = checkDash();
				break;

			case ID:
				// Add first char to string and get rest of ID string
				charSoFar += currentChar;
				state = getID();
				break;

			case ERROR:
				break;

			case STRING:
				// Add first char to string and get rest of the string
				charSoFar += currentChar;
				state = getString();
				break;

			case END_OF_STRING:
				state = checkEndOfString();
				break;

			case START_OF_COMMENT:
				state = getComment();
				break;
			case SINGLELINE_COMMENT:
				break;
			case MULTILINE_COMMENT:
				state = getMultiComment();
				break;
			case VERTICAL_BAR:
				state = getVerticalBar();
				break;

			default:
				createAndAddToken("UNDEFINED", charSoFar, curLineNum);
				state = START;
				break;
			}

		}
		createAndAddToken("EOF", "");
	}

	State Lexer::getVerticalBar() {
		if (currentChar == '#') {
			charSoFar += currentChar;
			createAndAddToken("COMMENT", charSoFar, curLineNum);
			//advance();
			return START;
		}
		else {
			charSoFar += currentChar;
			//char i = istream->peek();
			if (currentChar == '|') {
				currentChar = advance();
				return VERTICAL_BAR;
			}
			else {
				return MULTILINE_COMMENT;
			}
		}
	}

	State Lexer::getMultiComment() {
		while (istream->peek() != EOF && istream->peek() != '|') {
			charSoFar += advance();
		}
		if (istream->peek() == '|') {
			// Add |
			currentChar = advance();
			charSoFar += currentChar;
			// Give next function the currentChar of what's after the bar
			currentChar = advance();
			return VERTICAL_BAR;
		}
		return UNDEFINED;
	}

	State Lexer::getComment() {
		charSoFar += currentChar;
		if (istream->peek() == '|') {
			currentChar = advance();
			charSoFar += currentChar;
			return MULTILINE_COMMENT;
		}
		else {
			while (istream->peek() != EOF && istream->peek() != '\n') {
				charSoFar += advance();
				
			}
			if (istream->peek() == '\n') {
				//createAndAddToken("COMMENT", charSoFar);
				return START;
			}
			if (istream->peek() == EOF) {
				// just get back into while loop so it can kick itself out and end;
				return START;
			}
			return UNDEFINED;
		}
	}

	State Lexer::checkEndOfString() {
		if (currentChar == '\'' && istream->peek() != '\'') {
			// end of string
			createAndAddToken("STRING", charSoFar, curLineNum);
		}
		else {
			currentChar = advance();
			// false alarm - string contained a quote
			return STRING;
		}
		return START;
	}


	State Lexer::getString() {
		while (istream->peek() != EOF && istream->peek() != '\'') {
			charSoFar += advance();
		}
		if (istream->peek() == '\'') {
			// Could be end of string
			charSoFar += advance();
			return END_OF_STRING;
		}
		else {
			// returned EOF
			return UNDEFINED;
		}

	}



	State Lexer::getID() {
		while (istream->peek() != '\n' && istream->peek() != EOF && !isspace(istream->peek())) {
			if (isalnum((unsigned) istream->peek()) || istream->peek() == '_') {
				currentChar = advance();
				charSoFar += currentChar;
			}
			else {
				break;
				// undefined character
			/*	createAndAddToken("ID", charSoFar);
				return START;*/
			}
		}
		// ID string has ended (because of space, newline or EOF)
		// If special token add it
		if (!isSpecialID()) {
			// if not a "FACTS", "SCHEMES" etc. add an ID token
			createAndAddToken("ID", charSoFar);
		}
		return START;
	}

	bool Lexer::isSpecialID() {
		if (charSoFar == "Facts") {
			createAndAddToken("FACTS", charSoFar);
		}
		else if (charSoFar == "Queries") {
			createAndAddToken("QUERIES", charSoFar);
		}
		else if (charSoFar == "Schemes") {
			createAndAddToken("SCHEMES", charSoFar);
		}
		else if (charSoFar == "Rules") {
			createAndAddToken("RULES", charSoFar);
		}
		else {
			// ID string not special
			return false;
		}
		return true;
	}

	State Lexer::checkDash() {
		if (istream->peek() == '-') {
			createAndAddToken("COLON_DASH", charSoFar + advance());
		}
		else {
			createAndAddToken("COLON", charSoFar);
		}
		return START;
	}

	State Lexer::parseChar(char currentChar) {
		if (isPunct(currentChar)) {
			return PUNCUTATION;
		}
		else if (isGrammarPunct(currentChar)) {
			return PUNCUTATION;
		}
		else if (isWhitespace(currentChar)) {
			return START;
		}
		else if (isColon(currentChar)) {
			return COLON;
		}
		else if (isalpha((unsigned) currentChar)) {
			return ID;
		}
		else if (currentChar == '\'') {
			return STRING;
		}
		else if (currentChar == '#') {
			return START_OF_COMMENT;
		}
		else {
			charSoFar += currentChar;
			return UNDEFINED;
		}
	}


	Lexer::Lexer(string filename) {
		currentIntToken = 0;
		istream = new InputStream(filename);
		tokenList = new vector<Token*>;
		startState();
		exitAndReturn();
	}


	void Lexer::exitAndReturn() {
		/*for (signed int i = 0; i < (signed) tokenList->size(); i++) {
			cout << tokenList->at(i)->toString() << endl;
		}
		cout << "Total Tokens = " + to_string(tokenList->size()) << endl;*/
	}

	bool Lexer::isColon(char currentChar) {
		if (currentChar == ':') {
			charSoFar += currentChar;
			return true;
		}
		return false;
	}

	string Lexer::parenType(char currentChar) {
		string type;
		if (currentChar == '(') {
			type = "LEFT_PAREN";
		}
		if (currentChar == ')') {
			type = "RIGHT_PAREN";
		}
		else if (currentChar == '*') {
			type = "MULTIPLY";
		}
		else if (currentChar == '+') {
			type = "ADD";
		}
		return type;
	}

	bool Lexer::isGrammarPunct(char currentChar) {
		string type;
		if (currentChar == '.') {
			type = "PERIOD";
		}
		else if (currentChar == ',') {
			type = "COMMA";
		}
		else if (currentChar == '?') {
			type = "Q_MARK";
		}
		// Put value into string to match function type
		string output = "";
		output += currentChar;
		if (type != "") {
			createAndAddToken(type, output);
		}
		return type != "";

	}

	bool Lexer::isPunct(char currentChar) {
		string type;

		if (currentChar == '(' || currentChar == ')' || currentChar == '*' || currentChar == '+') {
			type = parenType(currentChar);
		}
	
		// Put value into string to match function type
		string output = "";
		output += currentChar;
		if (type != "") {
			createAndAddToken(type, output);
		}
		return type != "";
	}

	bool Lexer::isWhitespace(char currentChar) {
		if (currentChar == '\n' || currentChar == ' ' || currentChar =='\r') {
			return true;
		}
		return false;
	}

	void Lexer::createAndAddToken(string type, string value, int lineNum) {
		value = "\"" + value + "\"";
		if (lineNum == 0) {
			lineNum = istream->getLineNumber();
		}
		Token* addToken = new Token(type, value, lineNum);
		tokenList->push_back(addToken);
	}

	/*

	3 Start states
	1.  If you get the current character and in the start state (switch state) and it is a one char you create the token, advcance in the string
	add the char token to the list of tokens (8 single character tokens) and then go back to the start state
	(created a function with the character and token type)
	2. Start and you see a colon you go to the state -> initialize a new token with line number etc. and then go to state Colon or Colon Dash
	If you see a dash add the dash to the token character and set the token type to colon dash and create that token and add it to the list
	Go back to start state;

	Can use if statement > "a" < "Z' etc.
	Get a character and it is a string and it isn't punction or -1 then create the String token until you see a single quote.  Then go to a state "End of String?"
	Advance therer - if it is a single quote again then add it to the string and keep going.  If there was only one create the String token

	If you see a letter - add it to the charSoFar and then get the line number - token type = ID -> keep appending info if it is alphabet or digit
	When you run out of letters or digits.  If charSoFar == "Facts" or Queries etc. change the TokenType.  Then go back to the Start state after adding it

	If you see a # set it as a comment, add line number, advance.
	Single line comment as long as there is no end of line \n or EOF then keep adding it - Mlutliline you see a vertical bar you keep adding until EOF or verticalBar and Comment


	Try block and if there is an undefined throw an error and quit

	Start state see an undefined char go to that
	*/

