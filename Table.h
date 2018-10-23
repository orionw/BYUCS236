#pragma once
#include <vector>
#include <string>
#include <map>
#include <set>
#include "DatalogProgram.h"
#include "header.h"
using namespace std;


class Table {
public:
	string name;
	Header header;
	vector<Row> rows;
	// TODO implement this
	void toPrint(QueryItem query) {
		cout << query.toString();
		if (rows.size() != 0) {
			cout << "Yes(" << to_string(rows.size()) << ")" << endl;
			for (unsigned int row = 0; row < rows.size(); row++) {
				for (unsigned int col = 0; col < query.parameters.size() - 1; col++) {
					cout << query.parameters.at(col) << "='" << rows.at(row).at(col) << "'" << ",";
				}
				cout << query.parameters.at(query.parameters.size() - 1) << "='" << rows.at(row).at(query.parameters.size() - 1) << "'" << endl;
			}
		}
		else {
			cout << "No" << endl;
		}
	}

	void addRow(vector<string> row) {
		Row currentRow;
		for (unsigned int j = 0; j < row.size(); j++) {
			currentRow.push_back(row.at(j));
		}
		rows.push_back(currentRow);
	}

	void initializeRow(vector<DLString*>* row) {
		Row currentRow;
		for (unsigned int j = 0; j < row->size(); j++) {
			currentRow.push_back(row->at(j)->value);
		}
		rows.push_back(currentRow);
	}

	// Gets header
	Table(Schemes* schemes, int i) {
		// grab the headPredicate for the name of the table
		name = schemes->listOfSchemes->at(i)->id->value;
		// get the column names for the table
		header = schemes->getParameters(i);
	}

	// select is the where clause - keeps rows that have the condition
	// param to match - string is col1
	// make another function with a string and a colname
	// will call select on each one in the query list SK(A, B, c, D) go select(A,B) select(A,C), seleect(A,D)
	Table select(int col, string paramToMatch) {
		Table newRelation;
		for (unsigned int row = 0; row < rows.size(); row++) {
			if (rows.at(row).at(col) == paramToMatch) {
				// add row to the new table
				newRelation.rows.push_back(rows.at(row));
			}
		}
		return newRelation;
	}

	Table select(int col1, int col2) {
		Table newRelation;
		// for (scan through all rows)
		// if (the same col1 == col2
		// new relation add the tuple = newTable.addRow()
		return newRelation;
	}

	//Project(list of columns to keep) {
		//Table newRelation;
		//newRlation has same name as previous
		//newSchema
			//for loop keep the ones in the order given;
		/*for (each row) {
				// keep this schema and truples if match the input
		}*/

	Table rename(Table changeName, string newName) {
		changeName.name = name;
		return changeName;
		return changeName;
	}

	Table() {}

};

// Databases are just a collection of tables
class Database {
public:
	map<string, Table> tables;

	// initialize the database with the schemes and facts
	Database(DatalogProgram* program) {
		// create table names
		for (unsigned int i = 0; i < program->schemes->listOfSchemes->size(); i++) {
			// create table columns and names
			tables[program->schemes->listOfSchemes->at(i)->id->value] = Table(program->schemes, i);
		}
		for (unsigned int i = 0; i < program->facts->factList->size(); i++) {
			string tableName = program->facts->factList->at(i)->id->value;
			tables[tableName].initializeRow(program->facts->factList->at(i)->listOfStrings);
		}

	}


	void executeQueries(DatalogProgram* program) {
		vector<Table> results;
		Table newRelation;
		vector<QueryItem> queriesToExecute = program->queries->getQueries();
		for (QueryItem query : queriesToExecute) {
			// for each parameter
			for (unsigned int i = 0; i < query.parameters.size(); i++) {
				// for each parameter ahead of it
				for (unsigned int j = i + 1; j < query.parameters.size(); j++) {
					// if there is a constant, perform a select
					if (query.parameters.at(i).find("'") != -1) {
							newRelation = tables[query.table].select(i, query.parameters.at(i));
					}
					// the two parameters are both the same varaible.  Perform a select
					else if (query.parameters.at(i) == query.parameters.at(j)) {
						newRelation = tables[query.table].select(i, j);
					}
				}
			}
			/*if (newRelation.name == "") {
				newRelation = tables[query.table];
			}*/
			newRelation.toPrint(query);
		}
		results.push_back(newRelation);
	}

	~Database() {};
};
