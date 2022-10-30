#include "catch.hpp"

#include <pkb/pkbRepository/design_objects/entities/PkbProcedureEntity.h>
#include <pkb/pkbRepository/design_objects/entities/PkbVariableEntity.h>
#include <pkb/pkbRepository/design_objects/entities/PkbStatementEntity.h>
#include <pkb/pkbRepository/design_objects/relationships/PkbCallsRelationship.h>
#include <pkb/pkbRepository/design_objects/relationships/PkbCallsStarRelationship.h>
#include <pkb/pkbRepository/design_objects/relationships/PkbFollowsRelationship.h>
#include <pkb/pkbRepository/design_objects/relationships/PkbFollowsStarRelationship.h>
#include <pkb/pkbRepository/design_objects/relationships/PkbModifiesRelationship.h>
#include <pkb/pkbRepository/design_objects/relationships/PkbParentRelationship.h>
#include <pkb/pkbRepository/design_objects/relationships/PkbParentStarRelationship.h>
#include <pkb/pkbRepository/design_objects/relationships/PkbUsesRelationship.h>
#include <pkb/pkbRepository/design_objects/relationships/PkbNextStarRelationship.h>
#include <pkb/pkbRepository/design_objects/relationships/PkbNextRelationship.h>
#include <pkb/pkbRepository/table_managers/PkbPatternTable.h>
#include <pkb/PkbException.h>
#include <pkb/PKB.h>
#include <pkb/pkbRepository/design_objects/graphs/PkbGraphNode.h>
#include <pkb/pkbRepository/design_objects/graphs/PkbControlFlowGraphNode.h>
#include <sp/dataclasses/design_objects/Pattern.h>
#include <sp/dataclasses/design_objects/Relationship.h>
#include <sp/dataclasses/design_objects/Entity.h>
#include <sp/SourceProcessor.h>

#include <algorithm>
#include <memory>
#include <string>
using namespace std;



TEST_CASE("Test add and get patterns") {
	auto test = [](PKBTrackedStatementType statementType, ClauseArgument lhs, ClauseArgument rhs, vector<PQLPattern> expectedPatterns, vector<Pattern> toAdd) {

		// given
		PKB pkb;

		// when
		pkb.addPatterns(toAdd);

		vector<PQLPattern> all;

		vector<PQLPattern> patterns = pkb.retrievePatterns(statementType, lhs, rhs);
		for (int i = 0; i < patterns.size(); i++) {
			all.push_back(patterns[i]);
		}

		// then 
		REQUIRE(expectedPatterns.size() == all.size());
		for (PQLPattern e : expectedPatterns) {
			bool found = false;
			for (PQLPattern p : all) {
				if (p == e) {
					found = true;
					break;
				}
			}
			REQUIRE(found);
		}
	};

	// some patterns to add
	/*
		ASSIGN PATTERNS:
		x = 2 * x + y // is x, 2x*y+ in postfix
		y = 3 / y - 2 // is 3y/2- in postfix
		z = x + y // is xy+ in postfix
		z = x // is x in postfix
		y = x + y // is xy+ in postfix
		y = x + y // repeated on a different line

		IF PATTERNS:
		if (x == 1)
		if ((x == 0) && (y == 1))
		if ((x == 0) && (y == 1) || (z == 2))
		if (a != 3)

		WHILE PATTERNS:
		while (i > 0)
		while ((i > 0) || (j == 0))
		while ((i > 0) || (j == 0) && (k == 1))
	*/
	Pattern p1 = Pattern::createAssignPattern(1, " x ", " 2 x * y + ");
	Pattern p2 = Pattern::createAssignPattern(2, " y ", " 3 y / 2 - ");
	Pattern p3 = Pattern::createAssignPattern(3, " z ", " x y + ");
	Pattern p4 = Pattern::createAssignPattern(4, " z ", " x ");
	Pattern p5 = Pattern::createAssignPattern(5, " y ", " x y + ");
	Pattern p6 = Pattern::createAssignPattern(6, " y ", " x y + ");
	Pattern p7 = Pattern::createAssignPattern(7, " ab ", " 1000 ");

	// if (x == 1)
	Pattern p8 = Pattern::createIfPattern(8, "x");

	// if ((x == 0) && (y == 1))
	Pattern p9 = Pattern::createIfPattern(9, "x y");

	// if ((x == 0) && (y == 1) || (yz == 2))
	Pattern p10 = Pattern::createIfPattern(10, "x y yz");

	// if (a != 3)
	Pattern p11 = Pattern::createIfPattern(11, "a");

	// while (i > 0)
	Pattern p12 = Pattern::createWhilePattern(12, "i");

	// while ((i > 0) || (j == 0))
	Pattern p13 = Pattern::createWhilePattern(13, "i j");

	// while ((i > 0) || (j == 0) && (k == 1))
	Pattern p14 = Pattern::createWhilePattern(14, "i j jk");


	vector<Pattern> toAdd = {
		p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14
	};

	SECTION("lhs and rhs wildcard should get all assign patterns") {
		// lhs wildcard, // rhs also
		ClauseArgument lhs = ClauseArgument::createWildcardArg();
		ClauseArgument rhs = ClauseArgument::createWildcardArg();
		vector<PQLPattern> expectedPatterns = {
			PQLPattern::generateAssignPattern(1, "x"),
			PQLPattern::generateAssignPattern(2, "y"),
			PQLPattern::generateAssignPattern(3, "z"),
			PQLPattern::generateAssignPattern(4, "z"),
			PQLPattern::generateAssignPattern(5, "y"),
			PQLPattern::generateAssignPattern(6, "y"),
			PQLPattern::generateAssignPattern(7, "ab"),
		};
		test(PKBTrackedStatementType::ASSIGN, lhs, rhs, expectedPatterns, toAdd);
	}

	SECTION("lhs wildcard should get all if patterns") {
		ClauseArgument lhs = ClauseArgument::createWildcardArg();

		// rhs doesn't matter for if patterns
		ClauseArgument rhs = ClauseArgument::createWildcardArg();
		// TODO for PQL: change generateAssignPattern method name to generatePattern
		vector<PQLPattern> expectedPatterns = {
			PQLPattern::generateAssignPattern(8, "x"),
			PQLPattern::generateAssignPattern(9, "x"),
			PQLPattern::generateAssignPattern(9, "y"),
			PQLPattern::generateAssignPattern(10, "x"),
			PQLPattern::generateAssignPattern(10, "y"),
			PQLPattern::generateAssignPattern(10, "yz"),
			PQLPattern::generateAssignPattern(11, "a"),
		};
		test(PKBTrackedStatementType::IF, lhs, rhs, expectedPatterns, toAdd);

	}

	SECTION("lhs wildcard should get all while patterns") {
		ClauseArgument lhs = ClauseArgument::createWildcardArg();

		// rhs doesn't matter for while patterns
		ClauseArgument rhs = ClauseArgument::createWildcardArg();
		vector<PQLPattern> expectedPatterns = {
			PQLPattern::generateAssignPattern(12, "i"),
			PQLPattern::generateAssignPattern(13, "i"),
			PQLPattern::generateAssignPattern(13, "j"),
			PQLPattern::generateAssignPattern(14, "i"),
			PQLPattern::generateAssignPattern(14, "j"),
			PQLPattern::generateAssignPattern(14, "jk"),
		};
		test(PKBTrackedStatementType::WHILE, lhs, rhs, expectedPatterns, toAdd);

	}

	SECTION("specific lhs variable for if patterns") {
		ClauseArgument lhs = ClauseArgument::createStringLiteralArg("x");

		// rhs doesn't matter for if patterns
		ClauseArgument rhs = ClauseArgument::createWildcardArg();

		vector<PQLPattern> expectedPatterns = {
			PQLPattern::generateAssignPattern(8, "x"),
			PQLPattern::generateAssignPattern(9, "x"),
			PQLPattern::generateAssignPattern(10, "x")
		};
		test(PKBTrackedStatementType::IF, lhs, rhs, expectedPatterns, toAdd);

	}

	SECTION("specific lhs variable retrieved with proper exact regex match for if patterns") {
		// i.e. get if (..y..) but not if (..yz..)
		ClauseArgument lhs = ClauseArgument::createStringLiteralArg("y");

		// rhs doesn't matter for if patterns
		ClauseArgument rhs = ClauseArgument::createWildcardArg();

		vector<PQLPattern> expectedPatterns = {
			PQLPattern::generateAssignPattern(9, "y"),
			PQLPattern::generateAssignPattern(10, "y")
		};
		test(PKBTrackedStatementType::IF, lhs, rhs, expectedPatterns, toAdd);

	}

	SECTION("specific lhs variable retrieved with proper exact regex match for while patterns") {
		// i.e. get while (..j..) but not while (..jk..)
		ClauseArgument lhs = ClauseArgument::createStringLiteralArg("j");

		// rhs doesn't matter for if patterns
		ClauseArgument rhs = ClauseArgument::createWildcardArg();

		vector<PQLPattern> expectedPatterns = {
			PQLPattern::generateAssignPattern(13, "j"),
			PQLPattern::generateAssignPattern(14, "j")
		};
		test(PKBTrackedStatementType::WHILE, lhs, rhs, expectedPatterns, toAdd);

	}

	SECTION("lhs wildcard, rhs specific") {
		// lhs wildcard, // rhs specific
		ClauseArgument lhs = ClauseArgument::createWildcardArg();
		ClauseArgument rhs = ClauseArgument::createPatternStringArg("x y +");
		vector<PQLPattern> expectedPatterns = {
			PQLPattern::generateAssignPattern(3, "z"),
			PQLPattern::generateAssignPattern(5, "y"),
			PQLPattern::generateAssignPattern(6, "y"),
		};
		test(PKBTrackedStatementType::ASSIGN, lhs, rhs, expectedPatterns, toAdd);
	}

	SECTION("lhs specific, rhs sandwich wilcard") {
		// lhs specific, rhs sandwich wildcard _x_
		ClauseArgument lhs = ClauseArgument::createWildcardArg();
		ClauseArgument rhs = ClauseArgument::createPatternStringWithWildcardsArg("_x_");
		vector<PQLPattern> expectedPatterns = {
			PQLPattern::generateAssignPattern(1, "x"),
			PQLPattern::generateAssignPattern(3, "z"),
			PQLPattern::generateAssignPattern(4, "z"),
			PQLPattern::generateAssignPattern(5, "y"),
			PQLPattern::generateAssignPattern(6, "y"),
		};
		test(PKBTrackedStatementType::ASSIGN, lhs, rhs, expectedPatterns, toAdd);
	}

	SECTION("Synonyms should behave like wildcards") {
		// lhs synonym, rhs specific
		ClauseArgument lhs = ClauseArgument::createVariableArg("v");
		ClauseArgument rhs = ClauseArgument::createPatternStringArg("x y +");
		vector<PQLPattern> expectedPatterns = {
			PQLPattern::generateAssignPattern(3, "z"),
			PQLPattern::generateAssignPattern(5, "y"),
			PQLPattern::generateAssignPattern(6, "y"),
		};
		test(PKBTrackedStatementType::ASSIGN, lhs, rhs, expectedPatterns, toAdd);
	}

	SECTION("Can handle identical patterns on different lines") {
		// lhs synonym, rhs specific
		ClauseArgument lhs = ClauseArgument::createPatternStringArg("y");
		ClauseArgument rhs = ClauseArgument::createPatternStringArg("x y +");
		vector<PQLPattern> expectedPatterns = {
			PQLPattern::generateAssignPattern(5, "y"),
			PQLPattern::generateAssignPattern(6, "y"),

		};
		test(PKBTrackedStatementType::ASSIGN, lhs, rhs, expectedPatterns, toAdd);
	}

	SECTION("Can handle substring matching") {
		// 100 is a substring of 1000 (pattern7), but should not be matched
		ClauseArgument lhs = ClauseArgument::createPatternStringArg("ab");
		ClauseArgument rhs = ClauseArgument::createPatternStringArg("100");
		vector<PQLPattern> expectedPatterns = {
		};
		test(PKBTrackedStatementType::ASSIGN, lhs, rhs, expectedPatterns, toAdd);
	}

}