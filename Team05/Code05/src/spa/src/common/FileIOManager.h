#pragma once
#include <vector>
#include <string>

using namespace std;

class FileIOManager {

public:
	/*
		Reads lines from a specified file into a vector of strings.
	*/
	vector<string> readLinesFromFile(string filename);
};