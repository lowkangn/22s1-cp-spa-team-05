#pragma once
#include <vector>
#include <string>
#include <istream>

using namespace std;

class FileIOManager {

public:
	/*
		Reads in a file stream by reference.
	*/
	void readLinesFromFile(ifstream& stream, const string& filename);
};