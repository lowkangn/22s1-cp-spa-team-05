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
		int initialLength = s.size();

		// replace WILDCARD CHAR with .* regex pattern by reference
		PkbPatternTable::replaceAll(s, WILDCARD_CHAR, REGEX_MATCH_ANY);

		// replace all special characters with escaped characters


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