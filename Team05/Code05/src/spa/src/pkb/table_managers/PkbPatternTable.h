#pragma once

#include <pkb/design_objects/patterns/PkbStatementPattern.h>

#include <unordered_map>
#include <vector>
#include <string>
#include <memory>
using namespace std;

const string WILDCARD_CHAR = "_";
const string REGEX_MATCH_ANY = ".*";

class PkbPatternTable {
private:
	unordered_map<string, shared_ptr<PkbStatementPattern>> table;
	/*
		Creates a regex string from a string.

		The expected patterns are: 
		_something_, something, _
	*/
	string createRegexStringFromString(string s) {
		int initialLength = s.size();
		
		// replace WILDCARD CHAR with .* regex pattern by reference
		this->replaceAll(s, WILDCARD_CHAR, REGEX_MATCH_ANY);
		

		// we validate the input by checking the change in length
		// we expect exactly 1 (+1 char) or 2 (+2 char) wildcards e.g.:
		// _x_ -> x, _ -> '', x -> x
		int afterLength = s.size();
		if ((afterLength - 2) > initialLength) {
			// more than two characters added
			throw PkbException(string("Tried to construct regex string but too many wildards replaced! Got ") + s);
		}
		return s;
	}

	/*
		Repalces all instances of a substring to a new string by reference.
	*/
	void replaceAll(string& str, const std::string& from, const std::string& to) {
		if (from.empty())
			return;
		size_t start_pos = 0;
		while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
			str.replace(start_pos, from.length(), to);
			start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
		}
	}

	/*
		Splits a string by a string delimiter
	*/
	vector<string> splitStringByDelimiter(const string& s, const string& delimiter) {
		vector<string> tokens;
		size_t prev = 0, pos = 0;

		// use a do while loop to find the delimiter position
		do {
			// first, we find the delimiter position starting from prev to end
			pos = s.find(delimiter, prev);
			if (pos == string::npos) { // == npos -> not found
				pos = s.length(); // so we set pos of delimiter to to end
			}
			string token = s.substr(prev, pos - prev); // and then we cut out the substring from prev start to pos found
			if (!token.empty()) { // if not an empty string extracted
				tokens.push_back(token); // add to output
			}
			prev = pos + delimiter.length(); //continue, but start at the part after the delimiter
		} while (pos < s.length() && prev < s.length()); // do while within bounds
		return tokens;
	
	}

public:

	PkbPatternTable() {};

	/*
		Adds the pattern and returns the key used to store it.
		Throws an error if a duplicate object is attempted to be stored.
	*/
	string add(shared_ptr<PkbStatementPattern> pattern);

	/*
		Gets an item by a specific key. If the item is not present,
		we return NULL.
	*/
	shared_ptr<PkbStatementPattern> get(string& key);

	/*
		Returns all items.
	*/
	vector<shared_ptr<PkbStatementPattern>> getAll();

	/*
		Given all a vector of strings to match, returns the results.
	*/
	vector<shared_ptr<PkbStatementPattern>> getAllThatMatchPostFixStrings(vector<string> postFixStrings);


};