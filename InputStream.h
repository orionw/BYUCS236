#pragma once
#pragma once
#include <iostream>
#include <string>
#include <list>
#include <fstream>
#include <vector>
#include "token.h"
using namespace std;



class InputStream {
public:
	char currentChar;
	int currentLine = 1;
	int charPointer;
	int getLineNumber() {
		return currentLine;
	}

	char getNext() {
		if (file->peek() == '\n') {
			currentLine++;
		}
		return file->get();
	}
	char peek() {
		return file->peek();
	}
	ifstream* file;

	InputStream(string filename) {
		file = new ifstream;
		file->open(filename);
		if (!*file) {
			cerr << "Unable to open file";
		}
	}
	~InputStream() {
		file->close();
		delete file;
	}

};
