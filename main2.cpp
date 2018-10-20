#include <iostream>
#include <string>
#include <list>
#include <fstream>
#include <vector>
#include <set>
#include <algorithm>
#include "Lexer.h"
#include "Parameter.h"
#include "Utilities.h"
#include "Schemes.h"
#include "Predicates.h"
#include "Rules.h"
#include "Facts.h"
#include "Queries.h"
using namespace std;



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