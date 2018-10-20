#pragma once
#include <vector>
#include <string>
#include <map>
#include <set>
#include "DatalogProgram.h"
using namespace std;

// have Row and Header just be a vector of string
class Row : public vector<string> {};
class Header : public vector<string> {};

class Table {
public:
	//For the Database constructor:
	//    Database(Schema schema)
	Table(Header header) {
		void addRow(Row row);
	}

	string name;
	Header header;
	Row rows;
};

// Databases are just a collection of tables
class Database {
public:
	map<string, Table> tables;
	DatalogProgram a;
	Database(DatalogProgram program) {
		a = program;
		//for (int i = 0; i < program.schemes->listOfSchemes->size(); i++) {
		//	cout << "Hi";
		//}
	}
	//	for every Scheme, schema, in datalogProgram.getSchemas()
	//		tables.put(schema.getName(), new Table(schema))
	//		//add the “Table(Schema s)” constructor to Database
	//		for every Fact, fact, in datalogProgram.getFacts()
	//			get the factName from the fact
	//			use it to get the corresponding Table, table, from the
	//			tables attribute above
	//			create a new Row and add it to the table
	//			table.add(new Row(fact))
	//			//add the “Row(Fact fact) constructor to Row
	//			//add “void add(Row row)” to Table 

};
