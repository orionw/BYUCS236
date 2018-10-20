#pragma once
#include <string>
#include "Utilities.h"
#include "Operator.h"
using namespace std;

// Forward Declarations
Parameter* createParameter(Lexer& lex);
void deleteExpOrParam(Parameter* param);


class Parameter {
public:
	string value;
	bool isExp;

	Parameter() {
		isExp = false;
	}

	string toString() {
		return value;
	}
};

class Id : public Parameter
{
public:

	Id(Lexer& lex) {
		checkType(lex, "ID");
		value = lex.getCurrentToken().getValue();
		lex.advanceTokens();
	}
};



class DLString : public Parameter
{
public:
	DLString(Lexer& lex) {
		checkType(lex, "STRING");
		value = lex.getCurrentToken().getValue();
		lex.advanceTokens();
	}
};

class Expression : public Parameter
{
public:
	Parameter* leftParameter;
	Operator* op;
	Parameter* rightParameter;

	~Expression() {
		deleteExpOrParam(leftParameter);
		deleteExpOrParam(rightParameter);
		delete op;
	}


	Expression(Lexer& lex) {
		try {
			op = nullptr;
			leftParameter = nullptr;
			rightParameter = nullptr;
			checkFor(lex, "LEFT_PAREN");

			leftParameter = createParameter(lex);
			op = new Operator(lex);
			rightParameter = createParameter(lex);

			checkFor(lex, "RIGHT_PAREN");
			isExp = true;
		}
		catch (const std::runtime_error&) {
			if (op) {
				delete op;
			}
			if (leftParameter) {
				deleteExpOrParam(leftParameter);
			}
			if (rightParameter) {
				deleteExpOrParam(rightParameter);
			}
			throw std::runtime_error("Deleted");
		}
	}
};


Parameter* createParameter(Lexer& lex) {
	Parameter* result = nullptr;
	if (lex.getCurrentToken().getTokenType() == "ID") {
		result = (Parameter*) new Id(lex);
	}
	else if (lex.getCurrentToken().getTokenType() == "STRING") {

		result = (Parameter*) new DLString(lex);
	}
	else if (lex.getCurrentToken().getTokenType() == "LEFT_PAREN") {
		result = (Parameter*) new Expression(lex);
	}
	else {
		cout << "Failure!" << endl;
		cout << "  " << lex.getCurrentToken().toString() << endl;
		throw runtime_error(
			"Looking for a Parameter but found ");
	}
	return result;
}


void deleteExpOrParam(Parameter* param) {
	if (param->isExp) {
		Expression * exp = (Expression*)param;
		delete exp;
	}
	else {
		delete param;
	}
}


