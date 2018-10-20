#pragma once
#include<string>

using namespace std;

class Operator {
public:
	string op;

	Operator(Lexer& lex) {
		if (lex.getCurrentToken().getTokenType() == "ADD") {
			op = "ADD";
		}
		else if (lex.getCurrentToken().getTokenType() == "MULTIPLY") {
			op = "MULTIPLY";
		}
		else {
			cout << "Failure!" << endl;
			cout << "  " << lex.getCurrentToken().toString() << endl;
			throw runtime_error(
				"Expecting operator but found " + lex.getCurrentToken().getTokenType());
		}
		lex.advanceTokens();
	}

	string toString() {
		if (op == "ADD") {
			return "+";
		}
		else {
			return "*";
		}
	}

};
