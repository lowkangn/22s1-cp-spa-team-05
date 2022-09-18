#include <catch.hpp>

#include <pkb/design_objects/entities/PkbStatementEntity.h>
#include <pkb/table_managers/PkbPatternTable.h>
#include <pkb/design_objects/patterns/PkbStatementPattern.h>

#include <vector>
#include <string>
#include <assert.h>
using namespace std;

TEST_CASE("PkbPatternTable::add and ::get works correctly") {
	auto test = [](vector<shared_ptr<PkbStatementPattern>> expectedPatterns, vector<string> keys) {
		// given
		PkbPatternTable table;

		// when 
		for (shared_ptr<PkbStatementPattern> p : expectedPatterns) {
			// add
			table.add(p);
		}

		// then 
		assert(expectedPatterns.size() == keys.size());
		for (int i = 0; i < keys.size(); i++) {
			// retrieval by key matches expectation
			REQUIRE(expectedPatterns[i] == table.get(keys[i]));
		}
	};
	// shared
	vector<string> strings = {
			"x",
			"x+1"
	};
	SECTION("One entry per pattern type") {
		vector<shared_ptr<PkbStatementPattern>> patterns = {
			PkbStatementPattern::createAssignPattern(1, strings),
		};
		vector<string> keys = {
			string("xx+1") + string("1"),
		};
		test(patterns, keys);
	};
	

}

TEST_CASE("PkbPatternTable::getAll works correctly") {
	auto test = [](vector<shared_ptr<PkbStatementPattern>> expectedPatterns) {
		// given
		PkbPatternTable table;

		// when 
		for (shared_ptr<PkbStatementPattern> p : expectedPatterns) {
			// add
			table.add(p);
		}

		// then 
		vector<shared_ptr<PkbStatementPattern>> all = table.getAll();
		for (int i = 0; i < expectedPatterns.size(); i++) {
			// retrieval by key matches expectation
			REQUIRE(find(all.begin(), all.end(), expectedPatterns[i]) != all.end());
		}
	};
	// shared
	vector<string> strings = {
			"x",
			"x+1"
	};
	SECTION("One entry per pattern type") {
		vector<shared_ptr<PkbStatementPattern>> patterns = {
			PkbStatementPattern::createAssignPattern(1, strings),
		};
		test(patterns);
	};

}

TEST_CASE("Test ::replaceAll") {
	auto test = [](string s, string from, string to, string expected) {
		// given, when
		PkbPatternTable::replaceAll(s, from, to);

		// then
		REQUIRE(s == expected);

	};

	string s = "hereisfromhereistohereisfromhereisto";
	string from = "from";
	string to = "to";
	string expected = "hereistohereistohereistohereisto";

	SECTION("Empty strings, from") {

		// from is empty
		from = "";
		expected = "hereisfromhereistohereisfromhereisto";
		test(s, from, to, expected);


	}
	SECTION("Empty strings, to") {

		// to is empty
		to = "";
		expected = "hereishereistohereishereisto";
		test(s, from, to, expected);

	}

	SECTION("Empty strings, s") {

		// s is empty
		s = "";
		expected = "";
		test(s, from, to, expected);

	}

	SECTION("from == to") {
		from = "to";
		expected = "hereisfromhereistohereisfromhereisto";
		test(s, from, to, expected);
	}

	SECTION("Normal valid case") {
		s = "_x1*y+_"; // is _x * 1 + y_
		from = "_";
		to = ".*";
		expected = ".*x1*y+.*";
		test(s, from, to, expected);
	}

	SECTION("Normal valid case, no wildcards") {
		s = "x1*y+"; // is _x * 1 + y_
		from = "_";
		to = ".*";
		expected = "x1*y+";
		test(s, from, to, expected);
	}
}

TEST_CASE("Test ::createRegexStringFromString") {
	auto test = [](string s, string expected, bool throws) {
		if (!throws) {
			// given, when
			string out = PkbPatternTable::createRegexStringFromString(s);

			// then
			REQUIRE(out == expected);
		}
		else {
			REQUIRE_THROWS_AS(PkbPatternTable::createRegexStringFromString(s), PkbException);
		}
		

	};

	SECTION("Normal use cases") {
		test("_x 1 * y +_", ".* x 1 \\* y \\+ .*", false); // _x*1+y_
		test("_", ".*", false); // _
	}

	SECTION("Invalid pattern string passed in") {

		test("_x1_*y+_", "", true); // too many wildcards
	}

	SECTION("Validates number of wildcard replacements") {

		test("_x1_*y+_", "", true); // too many wildcards
	}
}

TEST_CASE("Test ::getAllThatMatchPostFixStrings") {
	auto test = [](vector<string> postFixStrings, vector<shared_ptr<PkbStatementPattern>> toAdd, vector<shared_ptr<PkbStatementPattern>> expected) {
		// given
		PkbPatternTable table;
		for (shared_ptr<PkbStatementPattern> p : toAdd) {
			table.add(p);
		}

		// when 
		vector<shared_ptr<PkbStatementPattern>> matching = table.getAllThatMatchPostFixStrings(postFixStrings);

		// then
		REQUIRE(matching.size() == expected.size());
		for (shared_ptr<PkbStatementPattern> e : expected) {
			bool found = false;
			for (shared_ptr<PkbStatementPattern> m : matching) {
				if (m->equals(e)) {
					found = true;
					break;
				}
			}
			REQUIRE(found);
		}

	};

	// some patterns
	/*
		x = 2 * x + y // is x, 2x*y+ in postfix
		y = 3 / y - 2 // is 3y/2- in postfix
		z = x + y // is xy+ in postfix
		z = x // is x in postfix
		y = x + y // is xy+ in postfix
	*/
	shared_ptr<PkbStatementPattern> line1 = PkbStatementPattern::createAssignPattern(1, vector<string>({ " x ", " 2 x * y + " }));
	shared_ptr<PkbStatementPattern> line2 = PkbStatementPattern::createAssignPattern(2, vector<string>({ " y ", " 3 y / 2 - " }));
	shared_ptr<PkbStatementPattern> line3 = PkbStatementPattern::createAssignPattern(3, vector<string>({ " z ", " x y + " }));
	shared_ptr<PkbStatementPattern> line4 = PkbStatementPattern::createAssignPattern(4, vector<string>({ " z ", " x " }));
	shared_ptr<PkbStatementPattern> line5 = PkbStatementPattern::createAssignPattern(3, vector<string>({ " y ", " x y + " }));
	vector<shared_ptr<PkbStatementPattern>> toAdd = {
		line1, line2, line3, line4, line5
	};
	SECTION("Exact match") {
		vector<string> postFixStrings = {
			"x", "2 x * y +",
		};
		vector<shared_ptr<PkbStatementPattern>> expected = {
			line1
		};
		test(postFixStrings, toAdd, expected);
	}

	SECTION("LHS Wildcard match") {
		vector<string> postFixStrings = {
			"_", "x y +",
		};
		vector<shared_ptr<PkbStatementPattern>> expected = {
			line3, line5
		};
		test(postFixStrings, toAdd, expected);
	}

	SECTION("RHS Wildcard match") {
		vector<string> postFixStrings = {
			"z", "_",
		};
		vector<shared_ptr<PkbStatementPattern>> expected = {
			line3, line4
		};
		test(postFixStrings, toAdd, expected);
	}


	SECTION("RHS sandwich wildcard match, variable") {
		vector<string> postFixStrings = {
			"_", "_x_",
		};
		vector<shared_ptr<PkbStatementPattern>> expected = {
			line1, line3, line4, line5
		};
		test(postFixStrings, toAdd, expected);
	}

}


//TEST_CASE("PkbPatternTable throws when we try to add duplicate items") {
//	auto test = [](vector<shared_ptr<PkbStatementPattern>> alreadyInTable, shared_ptr<PkbStatementPattern> toAdd) {
//		// given
//		PkbPatternTable table;
//		for (shared_ptr<PkbStatementPattern> e : alreadyInTable) {
//			// add
//			table.add(e);
//		}
//		// when & then
//		REQUIRE_THROWS_AS(table.add(toAdd), PkbException);
//
//	};
//	vector<string> strings = {
//			"x",
//			"x+1"
//	};
//	SECTION("One entry per pattern type") {
//		vector<shared_ptr<PkbStatementPattern>> patterns = {
//			PkbStatementPattern::createAssignPattern(1, strings),
//		};
//		shared_ptr<PkbStatementPattern> toAdd = shared_ptr<PkbStatementPattern>(PkbStatementPattern::createAssignPattern(1, strings));
//		test(patterns, toAdd);
//	};
//
//
//}
