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
using namespace std;


class Rule {
public:
	HeadPredicate* headPredicate;
	vector<Predicate*>* predicateList;
	~Rule() {
		delete headPredicate;
		for (unsigned int i = 0; i < predicateList->size(); i++) {
			delete predicateList->at(i);
		}
		delete predicateList;
	}


	Rule(Lexer& lex) {
		try {
			headPredicate = nullptr;
			predicateList = nullptr;
			headPredicate = new HeadPredicate(lex);
			checkFor(lex, "COLON_DASH");


			predicateList = new vector<Predicate*>();
			while (true) {
				predicateList->push_back(new Predicate(lex));
				if (lex.getCurrentToken().getTokenType() != "COMMA") break;
				lex.advanceTokens();
			}

			checkFor(lex, "PERIOD");
		}
		catch (const std::runtime_error&) {
			if (predicateList) {
				for (unsigned int i = 0; i < predicateList->size(); i++) {
					delete predicateList->at(i);
				}
				delete predicateList;
			}
			if (headPredicate) {
				delete headPredicate;
			}
			throw std::runtime_error("Deleted");
		}
	}
};


// forward declaration of utility function
void printExpressionOrParameterRules(vector<Rule*>*& rules, int ruleNum, int j, int x);

void printExpressionOrParameterHead(vector<Rule*>*& rules, int ruleNum, int j, int x) {
	if (rules->at(ruleNum)->headPredicate->ids->at(j)->isExp) {
		Expression* expString = (Expression *)rules->at(ruleNum)->headPredicate->ids->at(j);
		cout << expString->toString();
	}
	else {
		cout << rules->at(ruleNum)->headPredicate->ids->at(j)->toString();
	}
}

void printExpressionOrParameter(vector<Rule*>*& rules, int ruleNum, int j, int x) {
	if (rules->at(ruleNum)->predicateList->at(j)->parameters->at(x)->isExp) {
		Expression* expString = (Expression *)rules->at(ruleNum)->predicateList->at(j)->parameters->at(x);
		cout << expString->toString();
	}
	else {
		cout << rules->at(ruleNum)->predicateList->at(j)->parameters->at(x)->toString();
	}
}



void printRulesAre(vector<Rule*>*& rules, int ruleNum) {
	for (unsigned int j = 0; j < rules->at(ruleNum)->predicateList->size(); j++) {
		// each predicate in the list
		cout << rules->at(ruleNum)->predicateList->at(j)->id->toString() << "(";
		for (unsigned int x = 0; x < rules->at(ruleNum)->predicateList->at(j)->parameters->size(); x++) {
			// each parameter in the predicate
			printExpressionOrParameterRules(rules, ruleNum, j, x);
			if (x != rules->at(ruleNum)->predicateList->at(j)->parameters->size() - 1) {
				cout << ",";
			}
		}
		cout << ")";
		if (j != rules->at(ruleNum)->predicateList->size() - 1) {
			cout << ",";
		}
	}
}

class Rules {
public:
	vector<Rule*>* rules;
	Rules(Lexer& lex) {
		try {
			rules = nullptr;
			rules = new vector<Rule* >();

			checkFor(lex, "RULES");
			checkFor(lex, "COLON");
			while (lex.getCurrentToken().getTokenType() == "ID") {
				rules->push_back(new Rule(lex));
			}
		}
		catch (const std::runtime_error&) {
			if (rules) {
				for (unsigned int i = 0; i < rules->size(); i++) {
					delete rules->at(i);
				}
				delete rules;
			}
			throw std::runtime_error("Deleted");
		}
	}
	~Rules() {
		for (unsigned int i = 0; i < rules->size(); i++) {
			delete rules->at(i);
		}
		delete rules;
	}
	void toString() {
		cout << "Rules(" + to_string(rules->size()) + "):" << endl;
		for (unsigned int i = 0; i < rules->size(); i++) {
			// get's headpredicate
			cout << "  " << rules->at(i)->headPredicate->idIs->value << "(";
			for (unsigned int j = 0; j < rules->at(i)->headPredicate->ids->size(); j++) {
				// for each parameter in headpredicate
				printExpressionOrParameterHead(rules, i, j, 0);
				if (j != rules->at(i)->headPredicate->ids->size() - 1) {
					cout << ",";
				}
			}
			// last parameter of headPredicate
			cout << ") :- ";
			// get predicateList and parameters
			printRulesAre(rules, i);
			cout << "." << endl;
		}
	}
};

void printExpressionOrParameterRules(vector<Rule*>*& rules, int ruleNum, int j, int x) {
	if (rules->at(ruleNum)->predicateList->at(j)->parameters->at(x)->isExp) {
		Expression* expString = (Expression *)rules->at(ruleNum)->predicateList->at(j)->parameters->at(x);
		cout << expString->toString();
	}
	else {
		cout << rules->at(ruleNum)->predicateList->at(j)->parameters->at(x)->toString();
	}
}



