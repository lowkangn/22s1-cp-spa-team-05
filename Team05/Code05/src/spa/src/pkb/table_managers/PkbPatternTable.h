#pragma once

#include <pkb/design_objects/patterns/PkbStatementPattern.h>

#include <unordered_map>
#include <vector>
#include <string>
#include <memory>
#include <iostream>
using namespace std;

const string WILDCARD_CHAR = "_";
const string REGEX_MATCH_ANY = ".*"; // can be 0
const string PLUS_CHAR = "+";
const string MULT_CHAR = "*";
const string OR_CHAR = "|";
const string OPEN_BRACKET_CHAR = "(";
const string CLOSED_BRACKET_CHAR = ")";
const string SPACE_DELIMITER = " ";

class PkbPatternTable {
private:
	unordered_map<string, shared_ptr<PkbStatementPattern>> table;

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

	/*
		Creates a regex string from a string.

		The expected patterns are:
		_something_, something, _
	*/
	static string createRegexStringFromString(string s) {
		

		// replace all special characters with escaped characters
		PkbPatternTable::replaceAll(s, PLUS_CHAR, string("\\") + PLUS_CHAR);
		PkbPatternTable::replaceAll(s, MULT_CHAR, string("\\") + MULT_CHAR);
		PkbPatternTable::replaceAll(s, OR_CHAR, string("\\") + OR_CHAR);
		PkbPatternTable::replaceAll(s, OPEN_BRACKET_CHAR, string("\\") + OPEN_BRACKET_CHAR);
		PkbPatternTable::replaceAll(s, CLOSED_BRACKET_CHAR, string("\\") + CLOSED_BRACKET_CHAR);

		// replace WILDCARD CHAR with .* regex pattern by reference
		int initialLength = s.size();
		PkbPatternTable::replaceAll(s, WILDCARD_CHAR, REGEX_MATCH_ANY);

		// we validate the input by checking the change in length
		// we expect exactly 1 (+1 char) or 2 (+2 char) wildcards e.g.:
		// _x_ -> x, _ -> '', x -> x
		int afterLength = s.size();
		if (afterLength - initialLength == 1) {
			// one wildcard replaced
			// do nothing
		}
		else if (afterLength - initialLength == 2) {
			// two wildcards replaced
			// insert space before the wildcards
			s.insert(2, SPACE_DELIMITER);
			s.insert(s.size() - 2, SPACE_DELIMITER);

		} else if (afterLength == initialLength) {
			// no wildcards, prepend and append space
			s = SPACE_DELIMITER + s + SPACE_DELIMITER;
		}
		else {
			// more than two characters added
			throw PkbException(string("Tried to construct regex string but too many wildards replaced! Got ") + s);
		}

		return s;
	}

	/*
		Replaces all instances of a substring to a new string by reference.
	*/
	static void replaceAll(string& str, const std::string& from, const std::string& to) {
		if (from.empty())
			return;
		size_t start_pos = 0;
		while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
			str.replace(start_pos, from.length(), to);
			start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
		}
	}

};