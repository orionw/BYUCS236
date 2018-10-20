#pragma once
#include <string>
#include <list>
#include <vector>
#include <set>
#include <algorithm>
#include "Lexer.h"
#include "Parameter.h"
#include "Utilities.h"

class Scheme {
public:
	Id* id;
	vector<Id*>* ids;
	~Scheme() {
		delete id;
		for (unsigned int i = 0; i < ids->size(); i++) {
			delete ids->at(i);
		}
		delete ids;
	}


	Scheme(Lexer& lex) {
		try {
			id = nullptr;
			ids = nullptr;
			id = new Id(lex);
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
			if (id) {
				delete id;
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

class Schemes {
public:
	~Schemes() {
		for (unsigned int i = 0; i < listOfSchemes->size(); i++) {
			delete listOfSchemes->at(i);
		}
		delete listOfSchemes;
	}

	Schemes(Lexer& lex) {
		try {
			listOfSchemes = nullptr;
			listOfSchemes = new vector<Scheme*>;
			checkFor(lex, "SCHEMES");
			checkFor(lex, "COLON");

			do {
				listOfSchemes->push_back(new Scheme(lex));
			} while (lex.getCurrentToken().getTokenType() == "ID");
		}
		catch (const std::runtime_error&) {
			if (listOfSchemes) {
				for (unsigned int i = 0; i < listOfSchemes->size(); i++) {
					delete listOfSchemes->at(i);
				}
				delete listOfSchemes;
			}
			throw std::runtime_error("Deleted");
		}
	}
	void toString() {
		cout << "Schemes(" + to_string(listOfSchemes->size()) + "):" << endl;
		for (unsigned int i = 0; i < listOfSchemes->size(); i++) {
			cout << "  " << listOfSchemes->at(i)->id->value << "(";
			for (unsigned int j = 0; j < listOfSchemes->at(i)->ids->size() - 1; j++) {
				cout << listOfSchemes->at(i)->ids->at(j)->value << ",";
			}
			cout << listOfSchemes->at(i)->ids->at(listOfSchemes->at(i)->ids->size() - 1)->value << ")" << endl;
		}
	}
	vector<Scheme*>* listOfSchemes;

};
