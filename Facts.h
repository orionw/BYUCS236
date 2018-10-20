#pragma once
#include <string>
#include <list>
#include <vector>
#include <set>
#include <algorithm>
#include "Lexer.h"
#include "Parameter.h"
#include "Utilities.h"
#include "Predicates.h"
#include "Rules.h"
using namespace std;



class Fact {
public:
	Id* id;
	vector<DLString*>* listOfStrings;
	~Fact() {
		delete id;
		for (unsigned int i = 0; i < listOfStrings->size(); i++) {
			delete listOfStrings->at(i);
		}
		delete listOfStrings;
	}


	Fact(Lexer& lex) {

		try {
			id = nullptr;
			listOfStrings = nullptr;
			id = new Id(lex);
			checkFor(lex, "LEFT_PAREN");

			listOfStrings = new vector<DLString*>();
			while (true) {
				listOfStrings->push_back(new DLString(lex));
				if (lex.getCurrentToken().getTokenType() != "COMMA") break;
				lex.advanceTokens();
			}
			checkFor(lex, "RIGHT_PAREN");
			checkFor(lex, "PERIOD");
		}
		catch (const std::runtime_error&) {
			if (id) {
				delete id;
			}
			if (listOfStrings) {
				for (unsigned int i = 0; i < listOfStrings->size(); i++) {
					delete listOfStrings->at(i);
				}
				delete listOfStrings;
			}
			throw std::runtime_error("Deleted");
		}
	}
};



class Facts {
public:
	vector<Fact*>* factList;

	~Facts() {
		for (unsigned int i = 0; i < factList->size(); i++) {
			delete factList->at(i);
		}
		delete factList;
	}
	Facts(Lexer& lex) {
		checkFor(lex, "FACTS");
		checkFor(lex, "COLON");
		try {
			factList = nullptr;
			factList = new vector<Fact* >();
			while (lex.getCurrentToken().getTokenType() == "ID") {
				factList->push_back(new Fact(lex));
			}
		}
		catch (const std::runtime_error&) {
			if (factList) {
				for (unsigned int i = 0; i < factList->size(); i++) {
					delete factList->at(i);
				}
				delete factList;
			}
			throw std::runtime_error("Deleted");
		}
	}

	void toString() {
		cout << "Facts(" + to_string(factList->size()) + "):" << endl;
		for (unsigned int i = 0; i < factList->size(); i++) {
			cout << "  " << factList->at(i)->id->value << "(";
			for (unsigned int j = 0; j < factList->at(i)->listOfStrings->size() - 1; j++) {
				cout << factList->at(i)->listOfStrings->at(j)->toString() << ",";
			}
			cout << factList->at(i)->listOfStrings->at(factList->at(i)->listOfStrings->size() - 1)->toString() << ")." << endl;
		}
	}
};

