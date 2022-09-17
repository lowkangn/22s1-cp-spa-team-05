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

}

TEST_CASE("Test ::createRegexStringFromString") {

}

TEST_CASE("Test ::getAllThatMatchPostFixStrings") {

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
