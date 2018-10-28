#pragma once
#include <vector>
#include <algorithm>
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

	// gets vector pairs of column mappings
	vector<vector<int>> getJoinColumnsToKeep(Header headerToJoin) {
		vector<vector<int>> sameColumns;
		for (unsigned int col = 0; col < header.size(); col++) {
			for (unsigned int colToJoin = 0; colToJoin < headerToJoin.size(); colToJoin++) {
				if (header.at(col) == headerToJoin.at(colToJoin)) {
					// add the pair of same column
					vector<int> pair;
					pair.push_back(col);
					pair.push_back(colToJoin);
					sameColumns.push_back(pair);
				}
			}
		}
		return sameColumns;
	}

	vector<vector<int>> getJoinColumnsToKeep(vector<string> headerToJoin) {
		vector<vector<int>> sameColumns;
		for (unsigned int col = 0; col < header.size(); col++) {
			for (unsigned int colToJoin = 0; colToJoin < headerToJoin.size(); colToJoin++) {
				if (header.at(col) == headerToJoin.at(colToJoin)) {
					// add the pair of same column
					vector<int> pair;
					pair.push_back(col);
					pair.push_back(colToJoin);
					sameColumns.push_back(pair);
				}
			}
		}
		return sameColumns;
	}

	bool satisfies(Row row, Row rowToJoin, vector<vector<int>>& columnsToKeep) {
		// for each matching pair of columns in the join
		for (unsigned int pair = 0; pair < columnsToKeep.size(); pair++) {
			int col1 = columnsToKeep.at(pair).at(0);
			int col2 = columnsToKeep.at(pair).at(1);
			// if the two columns are NOT the same
			if (!(row.at(col1) == rowToJoin.at(col2))) {
				return false;
			}
		}
		// all columns to match were matched
		return true;
	}

	Row joined(Row row, Row rowToJoin, vector<int>& columnsToKeep) {
		for (unsigned int colToJoin = 0; colToJoin < rowToJoin.size(); colToJoin++) {
			if (find(columnsToKeep.begin(), columnsToKeep.end(), colToJoin) != columnsToKeep.end()) {
				row.push_back(rowToJoin.at(colToJoin));
			}
		}
		return row;
	}

	Header getJoinedHeader(Header header, Header headerToJoin, vector<int>& columnsToKeep) {
		for (unsigned int colToJoin = 0; colToJoin < headerToJoin.size(); colToJoin++) {
			if (find(columnsToKeep.begin(), columnsToKeep.end(), colToJoin) != columnsToKeep.end()) {
				header.push_back(headerToJoin.at(colToJoin));
			}
		}
		return header;
	}

	// utility function to return a vector full of indices (columns) in the 2nd table to keep
	vector<int> getJoinIndices(Header row, Header rowToJoin, vector<vector<int>>& columnsToKeep) {
		vector<int> colIndices;
		for (unsigned int column = 0; column < rowToJoin.size(); column++) {
			bool joinedCol = false;
			for (unsigned int pair = 0; pair < columnsToKeep.size(); pair++) {
				if (column == columnsToKeep.at(pair).at(1)) {
					joinedCol = true;
				}
			}
			if (!joinedCol) {
				colIndices.push_back(column);
			}
		}
		return colIndices;
	}



	Table join(Table toJoin) {
		Table joinResult;
		vector<vector<int>> columnsToKeep = getJoinColumnsToKeep(toJoin.header);
		vector<int> columnJoinIndices = getJoinIndices(header, toJoin.header, columnsToKeep);
		joinResult.header = getJoinedHeader(header, toJoin.header, columnJoinIndices);
		joinResult.name = name;
		for (Row row : rows) {
			for (Row rowToJoin : toJoin.rows) {
				if (satisfies(row, rowToJoin, columnsToKeep)) {
					joinResult.rows.push_back(joined(row, rowToJoin, columnJoinIndices));
				}
			}
		}
		return joinResult;
	}

	// keep only the columns needed for the head predicate
	Table filter(QueryItem head, Table joined) {
		Table newRelation;
		newRelation.name = joined.name;

		// keep the schema for the headPredicate
		for (string param : head.parameters) {
			newRelation.header.push_back(param);
		}
		// Get the mapping of the columns we need
		vector<vector<int>> columnsToKeep = getJoinColumnsToKeep(head.parameters);
		for (Row row : joined.rows) {
			Row filtered;
			for (int i = 0; i < columnsToKeep.size(); i++) {
				// this should push the columns back in the right order
				filtered.push_back(row.at(columnsToKeep.at(i).at(1)));
			}
			newRelation.rows.push_back(filtered);
		}
		return newRelation;
	}

};

// Databases are just a collection of tables
class Database {
public:
	map<string, Table> tables;
	bool newAdd;

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

	Table executeQueries(DatalogProgram* program, vector<QueryItem> queriesToDo = vector<QueryItem>(), bool toPrint = true) {
		Table newRelation;
		vector<QueryItem> queriesToExecute;

		// size of optional param is 0
		if (!queriesToDo.size()) {
			queriesToExecute = program->queries->getQueries();
		}
		else {
			queriesToExecute = queriesToDo;
		}

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
			if (toPrint) {
				newRelation.toPrint(query);
			}
		}
		// need to put this where the new rule gets added
		return newRelation;
	}

	Table process(QueryItem rule, bool& ruleAdded, DatalogProgram*& program) {
		Table newRelation;
		vector<QueryItem> ruleToDo;
		ruleToDo.push_back(rule);
		newRelation = executeQueries(program, ruleToDo, false);
		if (newAdd = true) {
			ruleAdded = true;
		}
		return newRelation;
	}

	void processRules(DatalogProgram* program) {
		vector<RuleItem> rulesToProcess = program->rules->getRules();
		bool ruleAdded = true;
		vector<Table> interimTables;
		Table joined;
		while (ruleAdded) {
			// get the tables from each rule query
			for (unsigned int i = 0; i < rulesToProcess.size(); i++) {
				for (unsigned int j = 0; j < rulesToProcess.at(i).predicates.size(); j++) {
					ruleAdded = false;
					// process the j-th predicate query
					interimTables.push_back(process(rulesToProcess.at(i).predicates.at(j), ruleAdded, program));
				}
				// join the tables from each query
				joined = interimTables.at(0);
				for (unsigned int i = 1; i < interimTables.size(); i++) {
					joined = joined.join(interimTables.at(i));
				}
				Table renamed = joined.filter(rulesToProcess.at(i).headPredicate, joined);
			}
		}
	}

	~Database() {};
};
