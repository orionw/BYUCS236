#include <algorithm>
#include "Lexer.h"
#include "DatalogProgram.h"
#include "Table.h"
using namespace std;


 int main(int argc, char *argv[]) {
	 Lexer* parse = new Lexer(argv[1]);
	 DatalogProgram* program;
	 try {
		 program = new DatalogProgram(*parse);
		 //program->outputResults();
		 Database db(program);
		 Graph g(program);
		 g.reverseDependencies(program);
		 g.getReversePostOrder();
		 g.getSCC();
		 cout << endl << "Rule Evaluation" << endl;
		 db.processRules(program, g.SCC);
		 cout << endl << "Query Evaluation" << endl;
		 db.executeQueries(program);
		 delete program;
	 }
	 catch (const std::runtime_error&){
		 // Error occured.  Failure will be output where it threw the error
	 }
	 delete parse;

return 0;             


 }