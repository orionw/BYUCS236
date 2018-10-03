#include <iostream>
#include <string>
#include <list>
#include <fstream>
#include <vector>
#include <set>
#include <algorithm>
#include "Lexer.h"
using namespace std;

set<string> FIRST_OF_PARAMETER;


void checkType(Lexer& lex, string type) {
	if (lex.getCurrentToken().getTokenType() != type) {
			cout << "Failure!" << endl;
			cout << "  " << lex.getCurrentToken().toString() << endl;
			throw std::invalid_argument("Expecting token of type " + type + " but found " +  lex.getCurrentToken().getTokenType());
	}
}

void checkFor(Lexer& lex, string type) {
	checkType(lex, type);
	lex.advanceTokens();
}





class Parameter {
public:
	string value;
	bool isExp;

	bool isInFirstOf(Lexer& lex) {
		return FIRST_OF_PARAMETER.find(lex.getCurrentToken().getTokenType()) != FIRST_OF_PARAMETER.end();
	}

	Parameter() {
		isExp = false;
	}

	string toString() {
		return value;
	}

	/*Parameter(Lexer& lex) {



		if (lex.getCurrentToken().getTokenType() == "ID") {

			result = new Id(lex);
		}
		else if (lex.getCurrentToken().getTokenType() == "STRING") {
			checkType(lex, "STRING");
			value = lex.getCurrentToken().getValue();
			lex.advanceTokens();
		}
		else if (lex.getCurrentToken().getTokenType() == "STRING") {

			result = new Expression(lex);
		}
		else {
			throw new exception(
				"Looking for a Parameter but found ");
		}
	}*/



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
			throw new runtime_error(
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


Parameter* createParameter(Lexer& lex);

class Expression : public Parameter
{
public:
	Parameter* leftParameter;
	Operator* op;
	Parameter* rightParameter;

	Expression(Lexer& lex) {
		checkFor(lex, "LEFT_PAREN");

		leftParameter = createParameter(lex);
		op = new Operator(lex);
		rightParameter = createParameter(lex);

		checkFor(lex, "RIGHT_PAREN");
		isExp = true;
	}

	string toString() {
		return "(" + leftParameter->toString() + op->toString() + rightParameter->toString() + ")";
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



Parameter* createParameter(Lexer& lex) {
	Parameter* result;
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
		throw new exception(
			"Looking for a Parameter but found ");
	}
	return result;
}



class HeadPredicate {
public:
	Id* idIs;
	vector<Id*>* ids;
	;


	HeadPredicate(Lexer& lex) {
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
};


class Predicate {
public:
	Id* id;
	vector<Parameter*>* parameters;

	Predicate(Lexer& lex) {
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
};

class Query {
public:
	Predicate* firstPart;

	Query(Lexer& lex) {
		//super(lex);
		firstPart = new Predicate(lex);
		checkFor(lex, "Q_MARK");
	}


};
class Scheme {
public:
	Id* id;
	vector<Id*>* ids;
	;


	Scheme(Lexer& lex) {
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
};

class Schemes {
	 public:

	 Schemes(Lexer& lex) {
			listOfSchemes = new vector<Scheme*>;
			checkFor(lex, "SCHEMES");
			checkFor(lex, "COLON");

			do {
				listOfSchemes->push_back(new Scheme(lex));
			} while (lex.getCurrentToken().getTokenType() == "ID");
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



class Fact {
public:
	Id* id;
	vector<DLString*>* listOfStrings;
	;


	Fact(Lexer& lex) {
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
};

class Rule {
public:
	HeadPredicate* headPredicate;
	vector<Predicate*>* predicateList;
	;


	Rule(Lexer& lex) {
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
};




class Facts {
public:
	vector<Fact*>* factList;
	

	Facts(Lexer& lex) {
		checkFor(lex, "FACTS");
		checkFor(lex, "COLON");

		factList = new vector<Fact* >();
		while (lex.getCurrentToken().getTokenType() == "ID") {
			factList->push_back(new Fact(lex));
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
		for (int x = 0; x < rules->at(ruleNum)->predicateList->at(j)->parameters->size(); x++) {
			// each parameter in the predicate
			printExpressionOrParameter(rules, ruleNum, j, x);
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
			rules = new vector<Rule* >();

			checkFor(lex, "RULES");
			checkFor(lex, "COLON");
			while (lex.getCurrentToken().getTokenType() == "ID") {
				rules->push_back(new Rule(lex));
			}
		}
	 void toString() {
		 // TODO fix doubles
		 cout << "Rules(" + to_string(rules->size()) + "):" << endl;
		 for (unsigned int k = 0; k < rules->size(); k++) {
			 // k is the number of rules
					// gets first part headpredicate
			 for (unsigned int i = 0; i < rules->size(); i++) {
				 cout << "  " << rules->at(i)->headPredicate->idIs->value << "(";
				 for (unsigned int j = 0; j < rules->at(i)->headPredicate->ids->size() - 1; j++) {
					 cout << rules->at(i)->headPredicate->ids->at(j)->toString() << ",";
				 }
				 cout << rules->at(i)->headPredicate->ids->at(rules->at(i)->headPredicate->ids->size() - 1)->toString() << ") :- ";
			 }
				 printRulesAre(rules, k);
			 
			 cout << "." << endl;
		 }
	 }
};


void printExpressionOrParameter(vector<Query*>*& queries, int i, int j, int x) {
	// modify to do Queries
	if (queries->at(i)->firstPart->parameters->at(j)->isExp) {
		Expression* expString = (Expression *) queries->at(i)->firstPart->parameters->at(j);
		cout << expString->toString();
	}
	else {
		cout << queries->at(i)->firstPart->parameters->at(j)->toString();
	}
}

 class Queries {
 public:
	 vector<Query*>* queries;


	 Queries(Lexer& lex) {
			checkFor(lex, "QUERIES");
			checkFor(lex, "COLON");

			queries = new vector<Query*>();
			do {
				queries->push_back(new Query(lex));
			} while (lex.getCurrentToken().getTokenType() == "ID");
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
			 cout  << ")?" << endl;
		 }
	 }
 };



 class DatalogProgram {
 public:
	 Schemes* schemes;
	 Facts* facts;
	 Rules* rules;
	 Queries* queries;

	 DatalogProgram(Lexer& lex) {
		 schemes = new Schemes(lex);
		 facts = new Facts(lex);
		 rules = new Rules(lex);
		 queries = new Queries(lex);
	 }

	 void outputResults() {
		 cout << "Success!" << endl;
		 schemes->toString();
		 facts->toString();
		 rules->toString();
		 queries->toString();
		 getDomains();
	 }

	 void getDomains() {
		 set<string> domains;
		 for (unsigned int i = 0; i < facts->factList->size(); i++) {
			 //cout << "  " << facts->factList->at(i)->id->value << "(";
			 for (unsigned int j = 0; j < facts->factList->at(i)->listOfStrings->size(); j++) {
				 domains.insert(facts->factList->at(i)->listOfStrings->at(j)->toString());
			 }
		 }
		 cout << "Domain(" << to_string(domains.size()) << "):" << endl;
		 const auto separator = "\n";
		 const auto* sep = "";
		 for (string value : domains) {
			 cout << sep << "  " << value;
			 sep = separator;
		 }
		 //cout << endl;
		 //TODO do I need this?
	 }


 };

 int main(int argc, char *argv[]) {
	 FIRST_OF_PARAMETER.insert("ID");
	 FIRST_OF_PARAMETER.insert("STRING");
	 FIRST_OF_PARAMETER.insert("LEFT_PAREN");
	 Lexer* parse = new Lexer("TestCase.txt");
	 try {
		 DatalogProgram* program = new DatalogProgram(*parse);
		 program->outputResults();
	 }
	 catch (const std::invalid_argument& e){
		 // Error occured.  Failure will be output where it threw the error
	 }
	/* delete parse;
	 delete program;*/

return 0;             


 }