#pragma once

#include <string>
#include <vector>

using namespace std;

class StringSplitter {
public:

	/*
	Utility method to split string by delimiter, useful for operations like splitting if/while patterns
	e.g. if (x == 1 and y == 2 and z == 3) -> "x y z" in SP -> Array [x, y, z] which can be used for PKB operations
	*/
	static vector<string> splitStringByDelimiter(string s, string delimiter) {
		size_t pos_start = 0, pos_end, delim_len = delimiter.length();
		string token;
		vector<string> res;

		while ((pos_end = s.find(delimiter, pos_start)) != string::npos) {
			token = s.substr(pos_start, pos_end - pos_start);
			pos_start = pos_end + delim_len;
			res.push_back(token);
		}

		res.push_back(s.substr(pos_start));
		return res;
	}
};