#pragma once
#include <vector>
#include <string>
// have Row and Header just be a vector of string
class Row : public vector<string> {};
class Header : public vector<string> {};