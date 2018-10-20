#pragma once
class Parameter;
class Expression;
class DLString;
class Id;

void checkType(Lexer& lex, string type) {
	if (lex.getCurrentToken().getTokenType() != type) {
		cout << "Failure!" << endl;
		cout << "  " << lex.getCurrentToken().toString() << endl;
		throw std::runtime_error("Expecting token of type " + type + " but found " + lex.getCurrentToken().getTokenType());
	}
}

void checkFor(Lexer& lex, string type) {
	checkType(lex, type);
	lex.advanceTokens();
}




