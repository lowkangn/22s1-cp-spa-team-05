#include "catch.hpp"

#include <qps/query/clause/AffectsClause.h>
#include <qps/query/clause/AffectsTClause.h>

#include <sp/SourceProcessor.h>

using namespace std;

// =============== UNIT TESTS ====================

TEST_CASE("AffectsClause: test equals") {
	auto testEquals = [](shared_ptr<RelationshipClause> other, bool expected) {
		// given
		ClauseArgument lhs = ClauseArgument::createStmtArg("s1");
		ClauseArgument rhs = ClauseArgument::createStmtArg("s2");
		AffectsClause affectsClause = AffectsClause(lhs, rhs);

		// when
		bool actual = affectsClause.equals(other);

		// then
		REQUIRE(actual == expected);
	};

	ClauseArgument firstStmtArg = ClauseArgument::createStmtArg("s1");
	ClauseArgument secondStmtArg = ClauseArgument::createStmtArg("s2");
	ClauseArgument lineArg = ClauseArgument::createLineNumberArg("1");

	SECTION("Equal") {
		shared_ptr<RelationshipClause> affectsClauseAgain(new AffectsClause(firstStmtArg, secondStmtArg));
		testEquals(affectsClauseAgain, true);
	}

	SECTION("Not equal, same clause type different arguments") {
		shared_ptr<RelationshipClause> affectsClauseDiffLhs(new AffectsClause(secondStmtArg, secondStmtArg));
		testEquals(affectsClauseDiffLhs, false);

		shared_ptr<RelationshipClause> affectsClauseDiffRhs(new AffectsClause(firstStmtArg, firstStmtArg));
		testEquals(affectsClauseDiffRhs, false);

		shared_ptr<RelationshipClause> affectsClauseDiffArgType(new AffectsClause(firstStmtArg, lineArg));
		testEquals(affectsClauseDiffArgType, false);
	}

	SECTION("Not equal, AffectsClause vs AffectsTClause") {
		shared_ptr<RelationshipClause> affectsTClause(new AffectsTClause(firstStmtArg, secondStmtArg));
		testEquals(affectsTClause, false);
	}
}

TEST_CASE("AffectsTClause: test equals") {
	auto testEquals = [](const shared_ptr<RelationshipClause> other, bool expected) {
		// given
		ClauseArgument lhs = ClauseArgument::createStmtArg("s1");
		ClauseArgument rhs = ClauseArgument::createStmtArg("s2");
		AffectsTClause affectsTClause = AffectsTClause(lhs, rhs);

		// when
		bool actual = affectsTClause.equals(other);

		// then
		REQUIRE(actual == expected);
	};

	ClauseArgument firstStmtArg = ClauseArgument::createStmtArg("s1");
	ClauseArgument secondStmtArg = ClauseArgument::createStmtArg("s2");
	ClauseArgument lineArg = ClauseArgument::createLineNumberArg("1");

	SECTION("Equal") {
		shared_ptr<RelationshipClause> affectsTClauseAgain(new AffectsTClause(firstStmtArg, secondStmtArg));
		testEquals(affectsTClauseAgain, true);
	}

	SECTION("Not equal, same clause type different arguments") {
		shared_ptr<RelationshipClause> affectsTClauseDiffLhs(new AffectsTClause(secondStmtArg, secondStmtArg));
		testEquals(affectsTClauseDiffLhs, false);

		shared_ptr<RelationshipClause> affectsTClauseDiffRhs(new AffectsTClause(firstStmtArg, firstStmtArg));
		testEquals(affectsTClauseDiffRhs, false);

		shared_ptr<RelationshipClause> affectsTClauseDiffArgType(new AffectsTClause(firstStmtArg, lineArg));
		testEquals(affectsTClauseDiffArgType, false);
	}

	SECTION("Not equal, AffectsClause vs AffectsTClause") {
		shared_ptr<RelationshipClause> affectsClause(new AffectsClause(firstStmtArg, secondStmtArg));
		testEquals(affectsClause, false);
	}
}

// =============== INTEGRATION TESTS ====================
// this prevents other files from using these variables
namespace {
	/* Corresponds to the following SIMPLE source (with line numbers)
		procedure main {
			1.	x = 0;
			2.	i = 5;
			3.	while (i != 0) {
			4.		x = x + 2 * y;
			5.	    call test;
			6.		i = i - 1;
				 }
			7.	if (x == 1) then {
			8.	    x = x + 1;
				} else {
			9.		z = 1;
				}
			10.	z = z + x + i;
			11. read z;
			12. x = x * y + z;
		}

		procedure test {
			13. y = x / 2;
			14. if (y < 5) then {
			15.		while (y < 5) {
			16.			x = x - y;
			17.			y = y + 1;
					}
				} else {
			18.		x = y;
				}
			19. x = x + y;
			20. v = y + 2;
			21. y = v + x;
		}
	*/

	string programString = "procedure main {\n   x = 0;\n   i = 5;\n   while (i != 0) {\n   \tx = x + 2 * y;\n   \tcall test;\n   \ti = i - 1;\n   }\n   if (x == 1) then {\n   \tx = x + 1;\n   } else {\n   \tz = 1;\n   }\n   z = z + x + i;\n   read z;\n   x = x * y + z;\n   }\n\nprocedure test {\n   y = x / 2;\n   if (y < 5) then {\n   \twhile (y < 5) {\n   \t\tx = x - y;\n    \t\ty = y + 1;\n   \t}\n   } else {\n   \tx = y;\n   }\n   x = x + y;\n   v = y + 2;\n   y = v + x;\n   }\n";

	// Initialise corresponding PQLEntities and PQLRelationships
	PQLEntity pqlA1 = PQLEntity::generateStatement(1);
	PQLEntity pqlA2 = PQLEntity::generateStatement(2);
	PQLEntity pqlA4 = PQLEntity::generateStatement(4);
	PQLEntity pqlA6 = PQLEntity::generateStatement(6);
	PQLEntity pqlA8 = PQLEntity::generateStatement(8);
	PQLEntity pqlA9 = PQLEntity::generateStatement(9);
	PQLEntity pqlA10 = PQLEntity::generateStatement(10);
	PQLEntity pqlA12 = PQLEntity::generateStatement(12);
	PQLEntity pqlA13 = PQLEntity::generateStatement(13);
	PQLEntity pqlA14 = PQLEntity::generateStatement(14);
	PQLEntity pqlA16 = PQLEntity::generateStatement(16);
	PQLEntity pqlA17 = PQLEntity::generateStatement(17);
	PQLEntity pqlA18 = PQLEntity::generateStatement(18);
	PQLEntity pqlA19 = PQLEntity::generateStatement(19);
	PQLEntity pqlA20 = PQLEntity::generateStatement(20);
	PQLEntity pqlA21 = PQLEntity::generateStatement(21);

	PQLRelationship pqlAffectsA1A4 = PQLRelationship(pqlA1, pqlA4);
	PQLRelationship pqlAffectsA1A8 = PQLRelationship(pqlA1, pqlA8);
	PQLRelationship pqlAffectsA1A10 = PQLRelationship(pqlA1, pqlA10);
	PQLRelationship pqlAffectsA1A12 = PQLRelationship(pqlA1, pqlA12);
	PQLRelationship pqlAffectsA2A6 = PQLRelationship(pqlA2, pqlA6);
	PQLRelationship pqlAffectsA2A10 = PQLRelationship(pqlA2, pqlA10);
	PQLRelationship pqlAffectsA6A6 = PQLRelationship(pqlA6, pqlA6);
	PQLRelationship pqlAffectsA6A10 = PQLRelationship(pqlA6, pqlA10);
	PQLRelationship pqlAffectsA8A10 = PQLRelationship(pqlA8, pqlA10);
	PQLRelationship pqlAffectsA8A12 = PQLRelationship(pqlA8, pqlA12);
	PQLRelationship pqlAffectsA9A10 = PQLRelationship(pqlA9, pqlA10);

	PQLRelationship pqlAffectsA13A16 = PQLRelationship(pqlA13, pqlA16);
	PQLRelationship pqlAffectsA13A17 = PQLRelationship(pqlA13, pqlA17);
	PQLRelationship pqlAffectsA13A18 = PQLRelationship(pqlA13, pqlA18);
	PQLRelationship pqlAffectsA13A19 = PQLRelationship(pqlA13, pqlA19);
	PQLRelationship pqlAffectsA13A20 = PQLRelationship(pqlA13, pqlA20);
	PQLRelationship pqlAffectsA16A16 = PQLRelationship(pqlA16, pqlA16);
	PQLRelationship pqlAffectsA16A19 = PQLRelationship(pqlA16, pqlA19);
	PQLRelationship pqlAffectsA17A16 = PQLRelationship(pqlA17, pqlA16);
	PQLRelationship pqlAffectsA17A17 = PQLRelationship(pqlA17, pqlA17);
	PQLRelationship pqlAffectsA17A19 = PQLRelationship(pqlA17, pqlA19);
	PQLRelationship pqlAffectsA17A20 = PQLRelationship(pqlA17, pqlA20);
	PQLRelationship pqlAffectsA18A19 = PQLRelationship(pqlA18, pqlA19);
	PQLRelationship pqlAffectsA19A21 = PQLRelationship(pqlA19, pqlA21);
	PQLRelationship pqlAffectsA20A21 = PQLRelationship(pqlA20, pqlA21);

	PQLRelationship pqlAffectsTA13A21 = PQLRelationship(pqlA13, pqlA21);
	PQLRelationship pqlAffectsTA16A21 = PQLRelationship(pqlA16, pqlA21);
	PQLRelationship pqlAffectsTA17A21 = PQLRelationship(pqlA17, pqlA21);
	PQLRelationship pqlAffectsTA18A21 = PQLRelationship(pqlA18, pqlA21);

	// Clause Arguments
	ClauseArgument firstStmtArg = ClauseArgument::createStmtArg("s1");
	ClauseArgument secondStmtArg = ClauseArgument::createStmtArg("s2");
	ClauseArgument assignArg = ClauseArgument::createAssignArg("a");
	ClauseArgument secondAssignArg = ClauseArgument::createAssignArg("a1");
	ClauseArgument readArg = ClauseArgument::createReadArg("r");
	ClauseArgument ifArg = ClauseArgument::createIfArg("i");
	ClauseArgument printArg = ClauseArgument::createPrintArg("pp");

	ClauseArgument lineOneArg = ClauseArgument::createLineNumberArg("1");
	ClauseArgument lineThreeArg = ClauseArgument::createLineNumberArg("3");
	ClauseArgument lineTwoArg = ClauseArgument::createLineNumberArg("2");
	ClauseArgument lineSixArg = ClauseArgument::createLineNumberArg("6");
	ClauseArgument lineNineArg = ClauseArgument::createLineNumberArg("9");
	ClauseArgument lineTenArg = ClauseArgument::createLineNumberArg("10");
	ClauseArgument lineTwelveArg = ClauseArgument::createLineNumberArg("12");
	ClauseArgument lineThirteenArg = ClauseArgument::createLineNumberArg("13");
	ClauseArgument lineSixteenArg = ClauseArgument::createLineNumberArg("16");
	ClauseArgument lineEighteenArg = ClauseArgument::createLineNumberArg("18");
	ClauseArgument lineTwentyArg = ClauseArgument::createLineNumberArg("20");
	ClauseArgument lineTwentyOneArg = ClauseArgument::createLineNumberArg("21");
};

TEST_CASE("AffectsClause: test execute") {
	auto testExecute = [](AffectsClause affectsClause,
		RelationshipClauseResult expectedClauseResult,
		shared_ptr<PKB> pkb) {
			// given
			shared_ptr<PKBQueryHandler> pkbInterface = shared_ptr<PKBQueryHandler>(pkb);

			// when
			shared_ptr<RelationshipClauseResult> resPtr = affectsClause.execute(pkbInterface);
			RelationshipClauseResult actualClauseResult = *resPtr;

			// then
			REQUIRE(actualClauseResult == expectedClauseResult);

	};

	// ------ SP ------ 
	stringstream stream(programString);
    SourceProcessor sp = SourceProcessor();
	sp.initialize(stream);


	// ------ PKB ------ 
	shared_ptr<PKB> pkb = shared_ptr<PKB>(new PKB());
	sp.extractAllAndAddToPkb(pkb);


	// ------ QPS ------ 
	AffectsClause clause = AffectsClause(firstStmtArg, secondStmtArg);
	vector<PQLRelationship> expectedRetrievedFromPkb = {
			pqlAffectsA1A4, pqlAffectsA1A8, pqlAffectsA1A10, pqlAffectsA1A12, pqlAffectsA2A6, pqlAffectsA2A10, 
			pqlAffectsA6A6, pqlAffectsA6A10, pqlAffectsA8A10, pqlAffectsA8A12, pqlAffectsA9A10, pqlAffectsA13A16, 
			pqlAffectsA13A17, pqlAffectsA13A18, pqlAffectsA13A19, pqlAffectsA13A20, pqlAffectsA16A16, pqlAffectsA16A19, 
			pqlAffectsA17A16, pqlAffectsA17A17, pqlAffectsA17A19, pqlAffectsA17A20, pqlAffectsA18A19, pqlAffectsA19A21, 
			pqlAffectsA20A21
	};
	RelationshipClauseResult expectedClauseResult = RelationshipClauseResult(firstStmtArg, secondStmtArg,
		expectedRetrievedFromPkb);

	SECTION("Two stmt synoynms - all affects results") {
		testExecute(clause, expectedClauseResult, pkb);
	}

	SECTION("Other stmtRef synonyms - non empty results") {
		clause = AffectsClause(assignArg, secondAssignArg);
		expectedRetrievedFromPkb = {
				pqlAffectsA1A4, pqlAffectsA1A8, pqlAffectsA1A10, pqlAffectsA1A12, pqlAffectsA2A6, pqlAffectsA2A10,
				pqlAffectsA6A6, pqlAffectsA6A10, pqlAffectsA8A10, pqlAffectsA8A12, pqlAffectsA9A10, pqlAffectsA13A16,
				pqlAffectsA13A17, pqlAffectsA13A18, pqlAffectsA13A19, pqlAffectsA13A20, pqlAffectsA16A16, pqlAffectsA16A19,
				pqlAffectsA17A16, pqlAffectsA17A17, pqlAffectsA17A19, pqlAffectsA17A20, pqlAffectsA18A19, pqlAffectsA19A21,
				pqlAffectsA20A21
		};
		expectedClauseResult = RelationshipClauseResult(assignArg, secondAssignArg, expectedRetrievedFromPkb);
		testExecute(clause, expectedClauseResult, pkb);

		clause = AffectsClause(assignArg, assignArg);
		expectedRetrievedFromPkb = { pqlAffectsA6A6, pqlAffectsA16A16, pqlAffectsA17A17 };
		expectedClauseResult = RelationshipClauseResult(assignArg, assignArg, expectedRetrievedFromPkb);
		testExecute(clause, expectedClauseResult, pkb);

		clause = AffectsClause(firstStmtArg, firstStmtArg);
		expectedRetrievedFromPkb = { pqlAffectsA6A6, pqlAffectsA16A16, pqlAffectsA17A17 };
		expectedClauseResult = RelationshipClauseResult(firstStmtArg, firstStmtArg, expectedRetrievedFromPkb);
		testExecute(clause, expectedClauseResult, pkb);
	}

	SECTION("Other stmtRef synonyms - empty results") {
		expectedRetrievedFromPkb = {};

		clause = AffectsClause(assignArg, readArg);
		expectedClauseResult = RelationshipClauseResult(assignArg, readArg, expectedRetrievedFromPkb);
		testExecute(clause, expectedClauseResult, pkb);

		clause = AffectsClause(ifArg, assignArg);
		expectedClauseResult = RelationshipClauseResult(ifArg, assignArg, expectedRetrievedFromPkb);
		testExecute(clause, expectedClauseResult, pkb);

		clause = AffectsClause(firstStmtArg, printArg);
		expectedClauseResult = RelationshipClauseResult(firstStmtArg, printArg, expectedRetrievedFromPkb);
		testExecute(clause, expectedClauseResult, pkb);
	}

	SECTION("One line number and one stmtRef - non empty results") {
		clause = AffectsClause(lineTwoArg, assignArg);
		expectedRetrievedFromPkb = { pqlAffectsA2A6, pqlAffectsA2A10 };
		expectedClauseResult = RelationshipClauseResult(lineTwoArg, assignArg, expectedRetrievedFromPkb);
		testExecute(clause, expectedClauseResult, pkb);

		clause = AffectsClause(firstStmtArg, lineSixteenArg);
		expectedRetrievedFromPkb = { pqlAffectsA13A16, pqlAffectsA16A16, pqlAffectsA17A16 };
		expectedClauseResult = RelationshipClauseResult(firstStmtArg, lineSixteenArg, expectedRetrievedFromPkb);
		testExecute(clause, expectedClauseResult, pkb);
	}

	SECTION("One line number and one stmtRef - empty results") {
		expectedRetrievedFromPkb = {};

		clause = AffectsClause(firstStmtArg, lineNineArg);
		expectedClauseResult = RelationshipClauseResult(firstStmtArg, lineNineArg, expectedRetrievedFromPkb);
		testExecute(clause, expectedClauseResult, pkb);

		clause = AffectsClause(lineTwentyOneArg, assignArg);
		expectedClauseResult = RelationshipClauseResult(lineTwentyOneArg, assignArg, expectedRetrievedFromPkb);
		testExecute(clause, expectedClauseResult, pkb);
	}

	SECTION("Two line numbers - non empty results") {
		clause = AffectsClause(lineOneArg, lineTenArg);
		expectedRetrievedFromPkb = { pqlAffectsA1A10 };
		expectedClauseResult = RelationshipClauseResult(lineOneArg, lineTenArg, expectedRetrievedFromPkb);
		testExecute(clause, expectedClauseResult, pkb);

		clause = AffectsClause(lineSixArg, lineSixArg);
		expectedRetrievedFromPkb = { pqlAffectsA6A6 };
		expectedClauseResult = RelationshipClauseResult(lineSixArg, lineSixArg, expectedRetrievedFromPkb);
		testExecute(clause, expectedClauseResult, pkb);
	}

	SECTION("Two line numbers - empty results") {
		expectedRetrievedFromPkb = {};

		clause = AffectsClause(lineOneArg, lineTwoArg);
		expectedClauseResult = RelationshipClauseResult(lineOneArg, lineTwoArg, expectedRetrievedFromPkb);
		testExecute(clause, expectedClauseResult, pkb);

		clause = AffectsClause(lineTenArg, lineTwelveArg);
		expectedClauseResult = RelationshipClauseResult(lineTenArg, lineTwelveArg, expectedRetrievedFromPkb);
		testExecute(clause, expectedClauseResult, pkb);

		clause = AffectsClause(lineTwelveArg, lineThirteenArg);
		expectedClauseResult = RelationshipClauseResult(lineTwelveArg, lineThirteenArg, expectedRetrievedFromPkb);
		testExecute(clause, expectedClauseResult, pkb);
	}
};

TEST_CASE("AffectsTClause: test execute") {
	auto testExecute = [](AffectsTClause affectsTClause,
		RelationshipClauseResult expectedClauseResult,
		shared_ptr<PKB> pkb) {
			// given
			shared_ptr<PKBQueryHandler> pkbInterface = shared_ptr<PKBQueryHandler>(pkb);

			// when
			shared_ptr<RelationshipClauseResult> resPtr = affectsTClause.execute(pkbInterface);
			RelationshipClauseResult actualClauseResult = *resPtr;

			// then
			REQUIRE(actualClauseResult == expectedClauseResult);

	};

	// ------ SP ------ 
	stringstream stream(programString);
	SourceProcessor sp = SourceProcessor();
	sp.initialize(stream);


	// ------ PKB ------ 
	shared_ptr<PKB> pkb = shared_ptr<PKB>(new PKB());
	sp.extractAllAndAddToPkb(pkb);


	// ------ QPS ------ 
	AffectsTClause clause = AffectsTClause(firstStmtArg, secondStmtArg);
	vector<PQLRelationship> expectedRetrievedFromPkb = {
			pqlAffectsA1A4, pqlAffectsA1A8, pqlAffectsA1A10, pqlAffectsA1A12, pqlAffectsA2A6, pqlAffectsA2A10,
			pqlAffectsA6A6, pqlAffectsA6A10, pqlAffectsA8A10, pqlAffectsA8A12, pqlAffectsA9A10, pqlAffectsA13A16,
			pqlAffectsA13A17, pqlAffectsA13A18, pqlAffectsA13A19, pqlAffectsA13A20, pqlAffectsA16A16, pqlAffectsA16A19,
			pqlAffectsA17A16, pqlAffectsA17A17, pqlAffectsA17A19, pqlAffectsA17A20, pqlAffectsA18A19, pqlAffectsA19A21,
			pqlAffectsA20A21, pqlAffectsTA13A21, pqlAffectsTA16A21, pqlAffectsTA17A21, pqlAffectsTA18A21
	};
	RelationshipClauseResult expectedClauseResult = RelationshipClauseResult(firstStmtArg, secondStmtArg,
		expectedRetrievedFromPkb);

	SECTION("Two stmt synoynms - all affectsStar results") {
		testExecute(clause, expectedClauseResult, pkb);
	}

	SECTION("Other stmtRef synonyms - non empty results") {
		clause = AffectsTClause(assignArg, secondAssignArg);
		expectedRetrievedFromPkb = {
				pqlAffectsA1A4, pqlAffectsA1A8, pqlAffectsA1A10, pqlAffectsA1A12, pqlAffectsA2A6, pqlAffectsA2A10,
				pqlAffectsA6A6, pqlAffectsA6A10, pqlAffectsA8A10, pqlAffectsA8A12, pqlAffectsA9A10, pqlAffectsA13A16,
				pqlAffectsA13A17, pqlAffectsA13A18, pqlAffectsA13A19, pqlAffectsA13A20, pqlAffectsA16A16, pqlAffectsA16A19,
				pqlAffectsA17A16, pqlAffectsA17A17, pqlAffectsA17A19, pqlAffectsA17A20, pqlAffectsA18A19, pqlAffectsA19A21,
				pqlAffectsA20A21, pqlAffectsTA13A21, pqlAffectsTA16A21, pqlAffectsTA17A21, pqlAffectsTA18A21
		};
		expectedClauseResult = RelationshipClauseResult(assignArg, secondAssignArg, expectedRetrievedFromPkb);
		testExecute(clause, expectedClauseResult, pkb);

		clause = AffectsTClause(assignArg, assignArg);
		expectedRetrievedFromPkb = { pqlAffectsA6A6, pqlAffectsA16A16, pqlAffectsA17A17 };
		expectedClauseResult = RelationshipClauseResult(assignArg, assignArg, expectedRetrievedFromPkb);
		testExecute(clause, expectedClauseResult, pkb);

		clause = AffectsTClause(firstStmtArg, firstStmtArg);
		expectedRetrievedFromPkb = { pqlAffectsA6A6, pqlAffectsA16A16, pqlAffectsA17A17 };
		expectedClauseResult = RelationshipClauseResult(firstStmtArg, firstStmtArg, expectedRetrievedFromPkb);
		testExecute(clause, expectedClauseResult, pkb);
	}

	SECTION("Other stmtRef synonyms - empty results") {
		expectedRetrievedFromPkb = {};

		clause = AffectsTClause(assignArg, readArg);
		expectedClauseResult = RelationshipClauseResult(assignArg, readArg, expectedRetrievedFromPkb);
		testExecute(clause, expectedClauseResult, pkb);

		clause = AffectsTClause(ifArg, assignArg);
		expectedClauseResult = RelationshipClauseResult(ifArg, assignArg, expectedRetrievedFromPkb);
		testExecute(clause, expectedClauseResult, pkb);

		clause = AffectsTClause(firstStmtArg, printArg);
		expectedClauseResult = RelationshipClauseResult(firstStmtArg, printArg, expectedRetrievedFromPkb);
		testExecute(clause, expectedClauseResult, pkb);

        clause = AffectsTClause(lineThreeArg, firstStmtArg);
        expectedClauseResult = RelationshipClauseResult(lineThreeArg, firstStmtArg, expectedRetrievedFromPkb);
        testExecute(clause, expectedClauseResult, pkb);
	}

	SECTION("One line number and one stmtRef - non empty results") {
		clause = AffectsTClause(lineEighteenArg, assignArg);
		expectedRetrievedFromPkb = { pqlAffectsA18A19, pqlAffectsTA18A21 };
		expectedClauseResult = RelationshipClauseResult(lineEighteenArg, assignArg, expectedRetrievedFromPkb);
		testExecute(clause, expectedClauseResult, pkb);

		clause = AffectsTClause(firstStmtArg, lineTwentyOneArg);
		expectedRetrievedFromPkb = { pqlAffectsA19A21, pqlAffectsA20A21, pqlAffectsTA13A21, pqlAffectsTA16A21, 
				pqlAffectsTA17A21, pqlAffectsTA18A21 };
		expectedClauseResult = RelationshipClauseResult(firstStmtArg, lineTwentyOneArg, expectedRetrievedFromPkb);
		testExecute(clause, expectedClauseResult, pkb);
	}

	SECTION("One line number and one stmtRef - empty results") {
		expectedRetrievedFromPkb = {};

		clause = AffectsTClause(lineSixteenArg, lineEighteenArg);
		expectedClauseResult = RelationshipClauseResult(lineSixteenArg, lineEighteenArg, expectedRetrievedFromPkb);
		testExecute(clause, expectedClauseResult, pkb);

		clause = AffectsTClause(lineTwentyOneArg, assignArg);
		expectedClauseResult = RelationshipClauseResult(lineTwentyOneArg, assignArg, expectedRetrievedFromPkb);
		testExecute(clause, expectedClauseResult, pkb);
	}

	SECTION("Two line numbers - non empty results") {
		clause = AffectsTClause(lineEighteenArg, lineTwentyOneArg);
		expectedRetrievedFromPkb = { pqlAffectsTA18A21 };
		expectedClauseResult = RelationshipClauseResult(lineEighteenArg, lineTwentyOneArg, expectedRetrievedFromPkb);
		testExecute(clause, expectedClauseResult, pkb);

		clause = AffectsTClause(lineSixteenArg, lineSixteenArg);
		expectedRetrievedFromPkb = { pqlAffectsA16A16 };
		expectedClauseResult = RelationshipClauseResult(lineSixteenArg, lineSixteenArg, expectedRetrievedFromPkb);
		testExecute(clause, expectedClauseResult, pkb);
	}

	SECTION("Two line numbers - empty results") {
		expectedRetrievedFromPkb = {};

		clause = AffectsTClause(lineSixteenArg, lineEighteenArg);
		expectedClauseResult = RelationshipClauseResult(lineSixteenArg, lineEighteenArg, expectedRetrievedFromPkb);
		testExecute(clause, expectedClauseResult, pkb);

		clause = AffectsTClause(lineTwelveArg, lineThirteenArg);
		expectedClauseResult = RelationshipClauseResult(lineTwelveArg, lineThirteenArg, expectedRetrievedFromPkb);
		testExecute(clause, expectedClauseResult, pkb);
	}
};
