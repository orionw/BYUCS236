#pragma once
#include <string>
#include <list>
#include <vector>
#include <set>
#include <algorithm>
#include "Lexer.h"
#include "Parameter.h"
#include "Utilities.h"


class HeadPredicate {
public:
	Id* idIs;
	vector<Id*>* ids;
	~HeadPredicate() {
		delete idIs;
		for (unsigned int i = 0; i < ids->size(); i++) {
			delete ids->at(i);
		}
		delete ids;
	}


	HeadPredicate(Lexer& lex) {
		try {
			idIs = nullptr;
			ids = nullptr;
			idIs = new Id(lex);
			checkFor(lex, "LEFT_PAREN");

			ids = new vector<Id*>();
			while (true) {
				ids->push_back(new Id(lex));
				if (lex.getCurrentToken().getTokenType() != "COMMA") break;
				lex.advanceTokens();
			}

			checkFor(lex, "RIGHT_PAREN");
		}
		catch (const std::runtime_error&) {
			if (idIs) {
				delete idIs;
			}
			if (ids) {
				for (unsigned int i = 0; i < ids->size(); i++) {
					delete ids->at(i);
				}
				delete ids;
			}
			throw std::runtime_error("Deleted");
		}
	}
};


class Predicate {
public:
	Id* id;
	vector<Parameter*>* parameters;

	~Predicate() {
		delete id;
		for (unsigned int i = 0; i < parameters->size(); i++) {
			deleteExpOrParam(parameters->at(i));
		}
		delete parameters;
	}

	Predicate(Lexer& lex) {
		try {
			id = nullptr;
			parameters = nullptr;
			id = new Id(lex);
			checkFor(lex, "LEFT_PAREN");
			parameters = new vector<Parameter*>();


			while (true) {
				parameters->push_back(createParameter(lex));
				if (lex.getCurrentToken().getTokenType() != "COMMA") break;
				lex.advanceTokens();
			}

			checkFor(lex, "RIGHT_PAREN");
		}
		catch (const std::runtime_error&) {
			if (id) {
				delete id;
			}
			if (parameters) {
				for (unsigned int i = 0; i < parameters->size(); i++) {
					deleteExpOrParam(parameters->at(i));
				}
				delete parameters;
			}
			throw std::runtime_error("Deleted");
		}
	}
};

