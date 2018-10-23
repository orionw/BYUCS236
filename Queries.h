#pragma once
#include <string>
#include <list>
#include <vector>
#include <set>
#include <algorithm>
#include <istream>
#include <sstream>
#include "Lexer.h"
#include "Parameter.h"
#include "Utilities.h"
#include "Predicates.h"
using namespace std;

// Forward Declaration
void printExpressionOrParameter(vector<Query*>*& queries, int i, int j, int x);
string printExpressionOrParameterString(vector<Query*>*& queries, int i, int j, int x);

class QueryItem {
public:
	string table;
	vector<string> parameters;
	QueryItem(string nameVal, vector<string> listOfParams) {
		table = nameVal;
		parameters = listOfParams;
	}
	string toString() {
		string query;
		query += table + "(";
		for (unsigned int i = 0; i < parameters.size() - 1; i++) {
			query += parameters.at(i) + ",";
		}
		query += parameters.at(parameters.size() - 1);
		query += ")? ";
		return query;
	}

};

class Query {
public:
	Predicate* firstPart;
	~Query() {
		delete firstPart;
	}

	Query(Lexer& lex) {
		//super(lex);
		try {
			firstPart = nullptr;
			firstPart = new Predicate(lex);
			checkFor(lex, "Q_MARK");
		}
		catch (const std::runtime_error&) {
			if (firstPart) {
				delete firstPart;
			}
			throw std::runtime_error("Deleted");
		}
	}


};

class Queries {
public:
	vector<Query*>* queries;

	~Queries() {
		for (unsigned int i = 0; i < queries->size(); i++) {
			delete queries->at(i);
		}
		delete queries;
	}
	Queries(Lexer& lex) {
		queries = nullptr;
		checkFor(lex, "QUERIES");
		checkFor(lex, "COLON");
		try {
			queries = new vector<Query*>();
			do {
				queries->push_back(new Query(lex));
			} while (lex.getCurrentToken().getTokenType() == "ID");
		}
		catch (const std::runtime_error&) {
			if (queries) {
				for (unsigned int i = 0; i < queries->size(); i++) {
					delete queries->at(i);
				}
				delete queries;
			}
			throw std::runtime_error("Deleted");
		}

	}

	void toString() {
		cout << "Queries(" + to_string(queries->size()) + "):" << endl;
		for (unsigned int i = 0; i < queries->size(); i++) {
			cout << "  " << queries->at(i)->firstPart->id->value << "(";
			for (unsigned int j = 0; j < queries->at(i)->firstPart->parameters->size() - 1; j++) {
				// implement other parameters/expressions etc.
				printExpressionOrParameter(queries, i, j, 0);
				if (j != queries->at(i)->firstPart->parameters->size() - 1) {
					cout << ",";
				}
			}
			printExpressionOrParameter(queries, i, queries->at(i)->firstPart->parameters->size() - 1, 0);
			cout << ")?" << endl;
		}
	}

	vector<QueryItem> getQueries() {
		string toReturn = "";
		string tableName;
		vector<QueryItem> queryList;
		for (unsigned int i = 0; i < queries->size(); i++) {
			// get table to search
			tableName = queries->at(i)->firstPart->id->value;
			vector<string> result;
			for (unsigned int j = 0; j < queries->at(i)->firstPart->parameters->size() - 1; j++) {
				result.push_back(printExpressionOrParameterString(queries, i, j, 0));
				if (j != queries->at(i)->firstPart->parameters->size() - 1) {
					toReturn += ",";
				}
			}
			result.push_back(printExpressionOrParameterString(queries, i, queries->at(i)->firstPart->parameters->size() - 1, 0));
			QueryItem query(tableName, result);
			queryList.push_back(query);
		}
		return queryList;
	}

};


// Utility for printing Queries
void printExpressionOrParameter(vector<Query*>*& queries, int i, int j, int x) {
	// modify to do Queries
	if (queries->at(i)->firstPart->parameters->at(j)->isExp) {
		Expression* expString = (Expression *)queries->at(i)->firstPart->parameters->at(j);
		cout << expString->toString();
	}
	else {
		cout << queries->at(i)->firstPart->parameters->at(j)->toString();
	}
}

// Utility for printing Queries
string printExpressionOrParameterString(vector<Query*>*& queries, int i, int j, int x) {
	// modify to do Queries
	if (queries->at(i)->firstPart->parameters->at(j)->isExp) {
		Expression* expString = (Expression *)queries->at(i)->firstPart->parameters->at(j);
		return expString->toString();
	}
	else {
		return queries->at(i)->firstPart->parameters->at(j)->toString();
	}
}
