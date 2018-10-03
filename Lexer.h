#pragma once
#include <iostream>
#include <string>
#include <list>
#include <fstream>
#include <vector>
#include "token.h"
#include "InputStream.h"
using namespace std;

enum State { START, ID, ERROR, COLON, PUNCUTATION, END_OF_STRING, STRING, START_OF_COMMENT, SINGLELINE_COMMENT, MULTILINE_COMMENT, VERTICAL_BAR, UNDEFINED};


class Lexer {
public:
	vector<Token*>* tokenList;
	int currentIntToken;
	Token currentToken;
	int lineNum;
	int curLineNum;
	string charSoFar;
	char currentChar;
	InputStream* istream;
	char advance();
	void advanceTokens() {
		currentIntToken++;
	}
	Token getCurrentToken();
	Token peekNextToken();
	State parseChar(char value);
	void startState();
	State getVerticalBar();
	State getMultiComment();
	State getComment();
	State checkEndOfString();
	void exitAndReturn();
	void createAndAddToken(string type, string value, int lineNum = 0);
	bool isPunct(char currentChar);
	string parenType(char currentChar);
	bool isGrammarPunct(char currentChar);
	bool isString(char currentChar);
	bool isComment(char currentChar);
	bool isWhitespace(char currentChar);
	bool isColon(char currentChar);
	State checkDash();
	State getID();
	bool isSpecialID();
	State getString();


	State state;
	Lexer(string filename);
	~Lexer() {
		delete istream;
		for (unsigned int i = 0; i < tokenList->size(); i++) {
			delete tokenList->at(i);
		}
		delete tokenList;
	}
};
