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

	Table() {}

	void toPrint(QueryItem query) {
		cout << query.toString();
		if (rows.size() != 0) {
			cout << "Yes(" << to_string(rows.size()) << ")";
			if (header.size() != 0) {
				cout << endl;
				for (unsigned int row = 0; row < rows.size(); row++) {
					cout << " ";
					for (unsigned int col = 0; col < header.size() - 1; col++) {
						cout << " " << header.at(col) << "=" << rows.at(row).at(col) << ",";
					}
					cout << " " << header.at(header.size() - 1) << "=" << rows.at(row).at(header.size() - 1) << endl;
				}
			}
			else {
				// no params to print
				cout << endl;
			}
		}
		else {
			// no matches
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
		newRelation.header = header;
		newRelation.name = name;
		return newRelation;
	}

	Table select(int col1, int col2) {
		Table newRelation;
		for (unsigned int row = 0; row < rows.size(); row++) {
			if (rows.at(row).at(col1) == rows.at(row).at(col2)) {
				// add row to the new table
				newRelation.rows.push_back(rows.at(row));
			}
		}
		newRelation.header = header;
		newRelation.name = name;
		return newRelation;
	}

	Table project(QueryItem query) {
		Table newRelation;
		//newRlation has same name as previous

		// init empty rows
		for (unsigned int a = 0; a < rows.size(); a++) {
			Row row;
			newRelation.rows.push_back(row);
		}
		map<string, int> seen;
			//for loop keep the ones in the order given;
		for (unsigned int i = 0; i < query.parameters.size(); i++) {
			// if it is a constant erase it from the rows and header
			if (query.parameters.at(i).find("'") == string::npos && seen.find(query.parameters.at(i)) == seen.end()) {
				seen[query.parameters.at(i)] = 1;
				newRelation.header.push_back(query.parameters.at(i));
				for (unsigned int j = 0; j < rows.size(); j++) {
					newRelation.rows.at(j).push_back(rows.at(j).at(i));
				}
			}
		}

		// get only unique rows
		sort(newRelation.rows.begin(), newRelation.rows.end());
		newRelation.rows.erase(unique(newRelation.rows.begin(), newRelation.rows.end()), newRelation.rows.end());
		
		// assign the varaibles to the new table and return
		newRelation.name = name;
		return newRelation;
	}

	Table rename(QueryItem query) {
		Table newRelation;
		for (unsigned int i = 0; i < header.size(); i++) {
			header.at(i) = query.parameters.at(i);
		}
		return newRelation;
	}

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
			newRelation = tables[query.table];
			// for each parameter
			for (unsigned int i = 0; i < query.parameters.size(); i++) {
				// if there is a constant, perform a select
				if (query.parameters.at(i).find("'") != string::npos) {
					newRelation = newRelation.select(i, query.parameters.at(i));
				}
				for (unsigned int j = i + 1; j < query.parameters.size(); j++) {
					// the two parameters are both the same variable.  Perform a select
					if (query.parameters.at(i) == query.parameters.at(j)) {
						newRelation = newRelation.select(i, j);
					}
				}
			}
			newRelation = newRelation.project(query);
			newRelation.toPrint(query);
			results.push_back(newRelation);
		}
	}

	~Database() {};
};
