#pragma once
#include <vector>
#include <algorithm>
#include <string>
#include <map>
#include <set>
#include <stack>
#include <iterator>
#include "DatalogProgram.h"
#include "header.h"
using namespace std;

class graphNode {
public:
	int number;
	set<int> dependents;
	bool visited;
	bool reversed;
	RuleItem thisRule;
	graphNode(RuleItem rule, int num, set<int> depend, DatalogProgram* program) {
		number = num;
		for (unsigned int i = 0; i < rule.predicates.size(); i++) {
			dependents = depend;
			thisRule = rule;
			visited = false;
			reversed = false;
		}
	}

};

class Graph {
public:
	set<int> tempIndex;
	int numRules;
	int currNodeNum;
	vector<set<int>> depList;
	vector<RuleItem> rulesToProcess;
	map<int, graphNode> graph;
	vector<set<int>> SCC;
	vector<set<int>> reverse;
	vector<QueryItem> queriesToGraph;
	vector<int> postOrder;


	bool predicateInRule(vector<RuleItem> rules, QueryItem pred) {
		tempIndex.clear();
		for (unsigned int i = 0; i < rules.size(); i++) {
			if (pred.table == rules.at(i).headPredicate.table) {
				tempIndex.insert(i);
			}
		}
		return (tempIndex.size() != 0);
	}

	set<int> getDependents(vector<RuleItem> rules, int i) {
		set<int> toReturn;
		RuleItem current = rules.at(i);
		for (unsigned int j = 0; j < current.predicates.size(); j++) {
			QueryItem predToFind = current.predicates.at(j);
			if (predicateInRule(rules, predToFind)) {
				for (int dependent : tempIndex) {
					toReturn.insert(dependent);
				}
			}
		}
		return toReturn;
	}

	void printDependencies() {
		cout << "Dependency Graph" << endl;
		for (int i = 0; i < numRules; i++) {
			set<int> currentSet;
			depList.push_back(currentSet);
			cout << "R" << to_string(i) << ":";
			int count = 0;
			for (int child : graph.at(i).dependents) {
				if (count) {
					cout << ",";
				}
				cout << "R" << to_string(child);
				depList.at(i).insert(child);
				count++;
			}
			cout << endl;
		}
	}

	void reverseDependencies(DatalogProgram* program) {
		// create empty sets
		for (unsigned int i = 0; i < depList.size(); i++) {
			set<int> emptySet;
			reverse.push_back(emptySet);
		}
		// add reverse dependencies
		for (unsigned int i = 0; i < depList.size(); i++) {
			// for each dependent
			for (int child : depList.at(i)) {
				// add to the revere set (indexed at the child) the parent
				reverse.at(child).insert(i);
			}
		}
	}

	Graph(DatalogProgram* program) {
		rulesToProcess = program->rules->getRules();
		numRules = rulesToProcess.size();
		// create graph
		for (unsigned int i = 0; i < rulesToProcess.size(); i++) {
			set<int> depend = getDependents(rulesToProcess, i);
			graphNode node(rulesToProcess.at(i), i, depend, program);
			graph.insert(pair<int, graphNode>(i, node));
		}
		// print the output of the dependency graph
		printDependencies();
	}

	void getReversePostOrder() {
		map<int, bool> visitedPost;
		bool pushed = false;
		stack<int> DFS;
		for (unsigned int i = 0; i < reverse.size(); i++) {
			// if not visited
			if (visitedPost.find(i) == visitedPost.end()) {
				visitedPost[i] = true;
				DFS.push(i);
				while (!DFS.empty()) {
					for (int child : reverse.at(DFS.top())) {
						// hasn't been visited so add it
						if (visitedPost.find(child) == visitedPost.end()) {
							pushed = true;
							DFS.push(child);
							visitedPost[child] = true;
						}
					}
					// check if anything was added
					if (!pushed) {
						// it is next on the post order
						postOrder.push_back(DFS.top());
						DFS.pop();
					}
					else {
						// reset counter
						pushed = false;
					}
				}
			}
		}
	}


	void getSCC() {
		/*
		The first SCC is found by running a depth-first search on the original dependency graph 
		starting from the node with the largest post-order number. 
		Any node visited during the DFS is part of the SCC.*/
		map<int, bool> visitedSCC;
		bool pushed = false;
		stack<int> DFS;
		int SSCCount = -1;
		for (int i = (int) postOrder.size() - 1; i >= 0; i--) {
			// if not visited
			if (visitedSCC.find(i) == visitedSCC.end()) {
				// new SCC, push back a new set
				SSCCount++;
				set<int> currentSet;
				SCC.push_back(currentSet);
				// mark the node as visited and push it
				DFS.push(i);
				// begin the depth search
				while (!DFS.empty()) {
					int topNode = DFS.top();
					DFS.pop();
					// hasn't been visited so add it
					if (visitedSCC.find(topNode) == visitedSCC.end()) {
						visitedSCC[topNode] = true;
						for (int child : depList.at(topNode)) {
							pushed = true;
							DFS.push(child);
						}
					}
					// check if anything was added
					if (!pushed) {
						// it is next on the post order
						SCC.at(SSCCount).insert(topNode);
					}
					else {
						// reset counter
						pushed = false;
					}
				}
			}
		}
	}


};