#pragma once
#include <vector>
#include <string>
#include <istream>

using namespace std;

class FileIOManager {

public:
	/*
		Reads lines from a specified file into a vector of strings.
	*/
	istream readLinesFromFile(string filename);
};