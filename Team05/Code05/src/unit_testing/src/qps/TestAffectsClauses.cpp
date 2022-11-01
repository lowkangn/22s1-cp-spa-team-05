#include "catch.hpp"

#include <qps/query/clause/AffectsClause.h>
#include <qps/query/clause/AffectsTClause.h>

#include <sp/SourceProcessor.h>

#include <list>

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
			9.	    z = 1;
				}
			10.	z = z + x + i;
			11. read z;
			12. x = x * y + z;
		}

		procedure test {
			13. y = x;
			14. x = 5;
			15. print y;
		}
	*/

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

	PQLRelationship pqlAffectsA1A4 = PQLRelationship(pqlA1, pqlA4);
	PQLRelationship pqlAffectsA2A6 = PQLRelationship(pqlA4, pqlA6);

	// Clause Arguments
	ClauseArgument firstStmtArg = ClauseArgument::createStmtArg("s1");
	ClauseArgument secondStmtArg = ClauseArgument::createStmtArg("s2");
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
	string programString = "procedure main {\n   x = 0;\n   i = 5;\n   while (i != 0) {\n   \tx = x + 2 * y;\n   \tcall test;\n   \ti = i - 1;\n   }\n   if (x == 1) then {\n   \tx = x + 1;\n   } else {\n   \tz = 1;\n   }\n   z = z + x + i;\n   read z;\n   x = x * y + z;\n   }\n\nprocedure test {\n   y = x;\n   x = 5;\n   print y;\n   }\n";
	//string programString = "procedure main {\n read x;\n y = x;\n y = y / 3;\n }\n\n procedure test {\n y = 1;\n }\n"; 
	stringstream stream(programString);
	SourceProcessor sp(stream);


	// ------ PKB ------ 
	shared_ptr<PKB> pkb = shared_ptr<PKB>(new PKB());
	sp.extractAllAndAddToPkb(pkb);


	// ------ QPS ------ 
	AffectsClause clause = AffectsClause(firstStmtArg, secondStmtArg);
	vector<PQLRelationship> expectedRetrievedFromPkb = { pqlAffectsA1A4 };
	RelationshipClauseResult expectedClauseResult = RelationshipClauseResult(firstStmtArg, secondStmtArg,
		expectedRetrievedFromPkb);

	SECTION("Two stmt synoynms - all affects results") {
		testExecute(clause, expectedClauseResult, pkb);
	}
};
