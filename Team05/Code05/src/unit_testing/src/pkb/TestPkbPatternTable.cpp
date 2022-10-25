#include <catch.hpp>

#include <pkb/pkbRepository/design_objects/entities/PkbStatementEntity.h>
#include <pkb/pkbRepository/table_managers/PkbPatternTable.h>
#include <pkb/pkbRepository/design_objects/patterns/PkbAssignPattern.h>

#include <vector>
#include <string>
#include <assert.h>
using namespace std;

TEST_CASE("PkbPatternTable::add and ::get works correctly") {
	auto test = [](vector<shared_ptr<PkbPattern>> expectedPatterns, vector<string> keys) {
		// given
		PkbPatternTable table;

		// when 
		for (shared_ptr<PkbPattern> p : expectedPatterns) {
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
	
	SECTION("One entry for ASSIGN pattern type") {
		vector<string> assignStrings = {
			"x",
			"x+1"
		};
		vector<shared_ptr<PkbPattern>> patterns = {
			PkbAssignPattern::createAssignPattern(1, assignStrings),
		};
		vector<string> keys = {
			string("xx+1") + string("1"),
		};
		test(patterns, keys);
	};
	SECTION("One entry for IF pattern type") {
		vector<string> ifString = {
			"x"
		};
		vector<shared_ptr<PkbPattern>> patterns = {
			PkbIfPattern::createIfPattern(1, ifString)
		};
		vector<string> keys = {
			string("x") + string("1"),
		};
		test(patterns, keys);
	}
	

}

TEST_CASE("PkbPatternTable::getAll works correctly") {
	auto test = [](vector<shared_ptr<PkbPattern>> expectedPatterns) {
		// given
		PkbPatternTable table;

		// when 
		for (shared_ptr<PkbPattern> p : expectedPatterns) {
			// add
			table.add(p);
		}

		// then 
		vector<shared_ptr<PkbPattern>> all = table.getAll();
		for (int i = 0; i < expectedPatterns.size(); i++) {
			// retrieval by key matches expectation
			REQUIRE(find(all.begin(), all.end(), expectedPatterns[i]) != all.end());
		}
	};

	SECTION("Assign pattern entries") {
		vector<string> assignStringsFirst = {
			"x",
			"x+1"
		};

		vector<string> assignStringsSecond = {
			"y",
			"z+20"
		};
		vector<shared_ptr<PkbPattern>> patterns = {
			PkbAssignPattern::createAssignPattern(1, assignStringsFirst),
			PkbAssignPattern::createAssignPattern(2, assignStringsSecond),
		};
		test(patterns);
	};

	SECTION("If pattern entries") {
		vector<string> assignStringsFirst = {
			"x",
			"x+1"
		};

		vector<string> assignStringsSecond = {
			"y",
			"z+20"
		};
		vector<shared_ptr<PkbPattern>> patterns = {
			PkbAssignPattern::createAssignPattern(1, assignStringsFirst),
			PkbAssignPattern::createAssignPattern(2, assignStringsSecond),
		};
		test(patterns);
	};

	SECTION("If pattern entries") {
		vector<string> ifStringsFirst = {
			"x",
		};

		vector<string> ifStringsSecond = {
			"count",
		};

		vector<shared_ptr<PkbPattern>> patterns = {
			PkbIfPattern::createIfPattern(1, ifStringsFirst),
			PkbIfPattern::createIfPattern(2, ifStringsSecond),
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

TEST_CASE("Test ::getAllThatMatchPostFixStrings for PKB Statement Pattern") {
	auto test = [](vector<string> postFixStrings, vector<shared_ptr<PkbPattern>> toAdd, vector<shared_ptr<PkbPattern>> expected) {
		// given
		PkbPatternTable table;
		for (shared_ptr<PkbPattern> p : toAdd) {
			table.add(p);
		}

		// when 
		vector<shared_ptr<PkbPattern>> matching = table.getAllThatMatchPostFixStrings(postFixStrings);

		// then
		REQUIRE(matching.size() == expected.size());
		for (shared_ptr<PkbPattern> e : expected) {
			bool found = false;
			for (shared_ptr<PkbPattern> m : matching) {
				if (m->equals(e)) {
					found = true;
					break;
				}
			}
			REQUIRE(found);
		}

	};

	/*
		ASSIGN STATEMENT PATTERNS:
		x = 2 * x + y // is x, 2x*y+ in postfix
		y = 3 / y - 2 // is 3y/2- in postfix
		z = x + y // is xy+ in postfix
		z = x // is x in postfix
		y = x + y // is xy+ in postfix
	*/
	shared_ptr<PkbPattern> line1 = PkbAssignPattern::createAssignPattern(1, vector<string>({ " x ", " 2 x * y + " }));
	shared_ptr<PkbPattern> line2 = PkbAssignPattern::createAssignPattern(2, vector<string>({ " y ", " 3 y / 2 - " }));
	shared_ptr<PkbPattern> line3 = PkbAssignPattern::createAssignPattern(3, vector<string>({ " z ", " x y + " }));
	shared_ptr<PkbPattern> line4 = PkbAssignPattern::createAssignPattern(4, vector<string>({ " z ", " x " }));
	shared_ptr<PkbPattern> line5 = PkbAssignPattern::createAssignPattern(3, vector<string>({ " y ", " x y + " }));
	vector<shared_ptr<PkbPattern>> assignPatternToAdd = {
		line1, line2, line3, line4, line5
	};

	SECTION("Exact match for statement pattern") {
		vector<string> postFixStrings = {
			"x", "2 x * y +",
		};
		vector<shared_ptr<PkbPattern>> expected = {
			line1
		};
		test(postFixStrings, assignPatternToAdd, expected);
	}

	SECTION("LHS Wildcard match for statement pattern") {
		vector<string> postFixStrings = {
			"_", "x y +",
		};
		vector<shared_ptr<PkbPattern>> expected = {
			line3, line5
		};
		test(postFixStrings, assignPatternToAdd, expected);
	}

	SECTION("RHS Wildcard match for statement pattern") {
		vector<string> postFixStrings = {
			"z", "_",
		};
		vector<shared_ptr<PkbPattern>> expected = {
			line3, line4
		};
		test(postFixStrings, assignPatternToAdd, expected);
	}


	SECTION("RHS sandwich wildcard match, variable (statement pattern)") {
		vector<string> postFixStrings = {
			"_", "_x_",
		};
		vector<shared_ptr<PkbPattern>> expected = {
			line1, line3, line4, line5
		};
		test(postFixStrings, assignPatternToAdd, expected);
	}
}

TEST_CASE("Test ::getVariableMatch for PKB If Pattern") {
	auto test = [](string conditions, vector<shared_ptr<PkbPattern>> toAdd, vector<shared_ptr<PkbPattern>> expected) {
		// given
		PkbPatternTable table;
		for (shared_ptr<PkbPattern> p : toAdd) {
			table.add(p);
		}

		// when 
		vector<shared_ptr<PkbPattern>> matching = table.getVariableMatch(conditions);

		// then
		REQUIRE(matching.size() == expected.size());
		for (shared_ptr<PkbPattern> e : expected) {
			bool found = false;
			for (shared_ptr<PkbPattern> m : matching) {
				if (m->equals(e)) {
					found = true;
					break;
				}
			}
			REQUIRE(found);
		}
	};

	/*
		IF STATEMENT PATTERNS:
		if (x == 1)
		if ((x == 1) && (y == 2))
		if ((x == 1) && (y == 2) || (yz == 3))
		if (a == 5)
	*/
	// if (x == 1)
	shared_ptr<PkbPattern> if1 = PkbIfPattern::createIfPattern(6, vector<string>({ "x" }));

	// if ((x == 1) && (y == 2))
	shared_ptr<PkbPattern> if2 = PkbIfPattern::createIfPattern(7, vector<string>({ "x" }));
	shared_ptr<PkbPattern> if3 = PkbIfPattern::createIfPattern(7, vector<string>({ "y" }));

	// if ((x == 1) && (y == 2) || (yz == 3))
	shared_ptr<PkbPattern> if4 = PkbIfPattern::createIfPattern(8, vector<string>({ "x" }));
	shared_ptr<PkbPattern> if5 = PkbIfPattern::createIfPattern(8, vector<string>({ "y" }));
	shared_ptr<PkbPattern> if6 = PkbIfPattern::createIfPattern(8, vector<string>({ "yz" }));

	// if (a == 5)
	shared_ptr<PkbPattern> if7 = PkbIfPattern::createIfPattern(6, vector<string>({ "a" }));

	vector<shared_ptr<PkbPattern>> ifPatternToAdd = {
		if1, if2, if3, if4, if5, if6, if7
	};

	SECTION("Exact match for one if pattern only") {
		string conditions = "a";
		vector<shared_ptr<PkbPattern>> expected = {
			if7
		};
		test(conditions, ifPatternToAdd, expected);
	}

	SECTION("Multiple match") {
		string conditions = "x";
		vector<shared_ptr<PkbPattern>> expected = {
			if1,
			if2,
			if4
		};
		test(conditions, ifPatternToAdd, expected);
	}

	SECTION("No match") {
		string conditions = "z";
		vector<shared_ptr<PkbPattern>> expected = { };
		test(conditions, ifPatternToAdd, expected);
	}

	SECTION("Match y exactly but not yz") {
		string conditions = "y";
		vector<shared_ptr<PkbPattern>> expected = { if3, if5 };
		test(conditions, ifPatternToAdd, expected);
	}

	SECTION("Match wildcard") {
		string conditions = "_";
		vector<shared_ptr<PkbPattern>> expected = {
			if1,
			if2,
			if3,
			if4,
			if5,
			if6,
			if7
		};
		test(conditions, ifPatternToAdd, expected);
	}
}

//TEST_CASE("PkbPatternTable throws when we try to add duplicate items") {
//	auto test = [](vector<shared_ptr<PkbAssignPattern>> alreadyInTable, shared_ptr<PkbAssignPattern> toAdd) {
//		// given
//		PkbPatternTable table;
//		for (shared_ptr<PkbAssignPattern> e : alreadyInTable) {
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
//		vector<shared_ptr<PkbAssignPattern>> patterns = {
//			PkbAssignPattern::createAssignPattern(1, strings),
//		};
//		shared_ptr<PkbAssignPattern> toAdd = shared_ptr<PkbAssignPattern>(PkbAssignPattern::createAssignPattern(1, strings));
//		test(patterns, toAdd);
//	};
//
//
//}
