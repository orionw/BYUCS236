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
			throw std::runtime_error("Expecting token of type " + type + " but found " +  lex.getCurrentToken().getTokenType());
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
};

void deleteExpOrParam(Parameter* param);


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


Parameter* createParameter(Lexer& lex);

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

	string toString() {
		Expression* left = (Expression*) leftParameter;
		Expression* right = (Expression*) rightParameter;
		if (leftParameter->isExp) {
			if (rightParameter->isExp) {
				return "(" + left->toString() + op->toString() + right->toString() + ")";
			}
			return "(" + left->toString() + op->toString() + rightParameter->toString() + ")";
		}
		else if (rightParameter->isExp) {
			return "(" + leftParameter->toString() + op->toString() + right->toString() + ")";
		}
		else {
			return "(" + leftParameter->toString() + op->toString() + rightParameter->toString() + ")";
		}
	}
};


void deleteExpOrParam(Parameter* param) {
	if (param->isExp) {
		Expression * exp = (Expression*) param;
		delete exp;
	}
	else {
		delete param;
	}
}


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
void printExpressionOrParameter(vector<Rule*>*& rules, int ruleNum, int j, int x) {
	if (rules->at(ruleNum)->predicateList->at(j)->parameters->at(x)->isExp) {
		Expression* expString = (Expression *)rules->at(ruleNum)->predicateList->at(j)->parameters->at(x);
		cout << expString->toString();
	}
	else {
		cout << rules->at(ruleNum)->predicateList->at(j)->parameters->at(x)->toString();
	}
}

void printExpressionOrParameterHead(vector<Rule*>*& rules, int ruleNum, int j, int x) {
	if (rules->at(ruleNum)->headPredicate->ids->at(j)->isExp) {
		Expression* expString = (Expression *)rules->at(ruleNum)->headPredicate->ids->at(j);
		cout << expString->toString();
	}
	else {
		cout << rules->at(ruleNum)->headPredicate->ids->at(j)->toString();
	}
}

void printRulesAre(vector<Rule*>*& rules, int ruleNum) {
	for (unsigned int j = 0; j < rules->at(ruleNum)->predicateList->size(); j++) {
		// each predicate in the list
		cout << rules->at(ruleNum)->predicateList->at(j)->id->toString() << "(";
		for (unsigned int x = 0; x < rules->at(ruleNum)->predicateList->at(j)->parameters->size(); x++) {
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
	 int stage;

	 DatalogProgram(Lexer& lex) {
		 try {
			 schemes = new Schemes(lex);
		 }
		 catch (const std::runtime_error&) {
			 throw std::runtime_error("Deleted");
		 }
		 try {
			 facts = new Facts(lex);
		 }
		 catch (const std::runtime_error&) {
			 delete schemes;
			 throw std::runtime_error("Deleted");
		 }
		 try {
			 rules = new Rules(lex);
		 }
		 catch (const std::runtime_error&) {
			 delete schemes;
			 delete facts;
			 throw std::runtime_error("Deleted");
		 }
		 try {
			 queries = new Queries(lex);
		 }
		 catch (const std::runtime_error&) {
			 delete schemes;
			 delete facts;
			 delete rules;
			 throw std::runtime_error("Deleted");

		 }
	 }

	 ~DatalogProgram() {
		 delete schemes;
		 delete facts;
		 delete rules;
		 delete queries;
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
		 // insert facts into domainList
		 for (unsigned int i = 0; i < facts->factList->size(); i++) {
			 for (unsigned int j = 0; j < facts->factList->at(i)->listOfStrings->size(); j++) {
				 domains.insert(facts->factList->at(i)->listOfStrings->at(j)->toString());
			 }
		 }
		 // start printing domains
		 cout << "Domain(" << to_string(domains.size()) << "):" << endl;
		 const auto separator = "\n";
		 const auto* sep = "";
		 for (string value : domains) {
			 cout << sep << "  " << value;
			 sep = separator;
		 }
	 }


 };

 int main(int argc, char *argv[]) {
	 FIRST_OF_PARAMETER.insert("ID");
	 FIRST_OF_PARAMETER.insert("STRING");
	 FIRST_OF_PARAMETER.insert("LEFT_PAREN");
	 Lexer* parse = new Lexer(argv[1]);
	 DatalogProgram* program;
	 try {
		 program = new DatalogProgram(*parse);
		 program->outputResults();
		 delete program;
	 }
	 catch (const std::runtime_error&){
		 // Error occured.  Failure will be output where it threw the error
	 }
	 delete parse;

	

return 0;             


 }