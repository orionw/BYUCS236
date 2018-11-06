#pragma once
#include <vector>
#include <string>

class Header : public vector<string> {};
// have Row and Header just be a vector of string
class Row : public vector<string> {
	/* needed for set comparison */
	bool operator == (const Row& other) const {
		for (unsigned int i = 0; i < this->size(); i++) {
			if (!other.size() && other.at(i) == "" && this->at(i) != other.at(i)) {
				return false;
			}
		}
		return true;
	}
public:
	string toString(Header header) {
		string result = "  ";
		for (unsigned int i = 0; i < this->size(); i++) {
			if (i > 0) {
				result += ", ";
			}
			result += header.at(i);
			result += "=";
			result += this->at(i);
		}
		return result;
	}
};
