#include <algorithm>
#include "Lexer.h"
#include "DatalogProgram.h"
using namespace std;



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