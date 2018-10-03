#pragma once
#include <iostream>
#include <string>
#include <list>
#include <fstream>
#include <vector>
#include "token.h"
using namespace std;


//enum TokenType {
//	QUERY, SCHEMES, FACTS, COMMA, PERIOD, LEFT_PAREN, RIGHT_PAREN, Q_MARK, COLON, COLON_DASH, RULES, ADD,
//	MULTIPLY, COMMENT, UNDEFINED, STRING, WHITESPACE, END_OF_FILE, NONE
//};

class Token {

public:
	string type;
	string value;
	int line;
	string toString() {
		return ("(" + type + "," + value + "," + to_string(line) + ")");
	}
	string getValue() {
		// remove quotation marks for lab 2
		return value.substr(1, value.size() - 2);
	}
	string getTokenType() {
		return  type;
	}
	Token(string typeOf, string values, int lineNum) {
		type = typeOf;
		value = values;
		line = lineNum;
	}
	Token() {}
};
