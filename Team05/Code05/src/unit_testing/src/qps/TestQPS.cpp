#include "catch.hpp"

#include <qps/QPS.h>

namespace {
	/* Corresponds to the following SIMPLE source (with line numbers)
		procedure main {
	1:		while (v > 1) {
	2:			y = x + 1;
	3:			if (b > 0) then {
	4:				read x;
				} else {
	5:				read y;
	6:				read stmt;
				}
	7:			z = 1;
			}
	8:		print x;
		}

	For which the following relationships hold:
	Parent: (1,2), (1,3), (1,7), (3,4), (3,5), (3,6)
	Parent*: Parent, (1,4), (1,5), (1,6)
	Follows: (1,8), (2,3), (3,7), (5,6)
	Follows*: Follows, (2,7)
	ModifiesS: (1,y), (1,x), (1,stmt), (1,z), (2,y), (3,x), (3,y), (3, stmt), (4,x), (5,y), (6,stmt), (7,z)
	ModifiesP: (main, rhsM) where rhsM is an rhs of ModifiesS
	UsesS: (1,v), (1,x), (1,b), (2,x), (3,b) (8,x)
	UsesP: (main, rhsU) where rhsU is an rhs of UsesS
	*/

	// Initialise statement entities
	Entity w1 = Entity::createWhileEntity(1);
	Entity a2 = Entity::createAssignEntity(2);
	Entity i3 = Entity::createIfEntity(3);
	Entity r4 = Entity::createReadEntity(4);
	Entity r5 = Entity::createReadEntity(5);
	Entity r6 = Entity::createReadEntity(6);
	Entity a7 = Entity::createAssignEntity(7);
	Entity p8 = Entity::createPrintEntity(8);

	// Initialise non-statement entities
	Entity main = Entity::createProcedureEntity(Token::createNameOrKeywordToken("main"));
	Entity v1 = Entity::createVariableEntity(1, Token::createNameOrKeywordToken("v"));
	Entity constOne1 = Entity::createConstantEntity(1, Token::createIntegerToken("1"));
	Entity y2 = Entity::createVariableEntity(2, Token::createNameOrKeywordToken("y"));
	Entity x2 = Entity::createVariableEntity(2, Token::createNameOrKeywordToken("x"));
	Entity constOne2 = Entity::createConstantEntity(2, Token::createIntegerToken("1"));
	Entity b3 = Entity::createVariableEntity(3, Token::createNameOrKeywordToken("b"));
	Entity constZero3 = Entity::createConstantEntity(3, Token::createIntegerToken("0"));
	Entity x4 = Entity::createVariableEntity(4, Token::createNameOrKeywordToken("x"));
	Entity y5 = Entity::createVariableEntity(5, Token::createNameOrKeywordToken("y"));
	Entity stmt6 = Entity::createVariableEntity(6, Token::createNameOrKeywordToken("stmt"));
	Entity z7 = Entity::createVariableEntity(7, Token::createNameOrKeywordToken("z"));
	Entity constOne7 = Entity::createConstantEntity(7, Token::createIntegerToken("1"));
	Entity x8 = Entity::createVariableEntity(8, Token::createNameOrKeywordToken("x"));

	// Initialise parent relationships
	Relationship parentW1A2 = Relationship::createParentRelationship(w1, a2);
	Relationship parentW1I3 = Relationship::createParentRelationship(w1, i3);
	Relationship parentW1A7 = Relationship::createParentRelationship(w1, a7);
	Relationship parentI3R4 = Relationship::createParentRelationship(i3, r4);
	Relationship parentI3R5 = Relationship::createParentRelationship(i3, r5);
	Relationship parentI3R6 = Relationship::createParentRelationship(i3, r6);

	// Initialise parentStar relationships
	Relationship parentStarW1A2 = Relationship::createParentTRelationship(w1, a2);
	Relationship parentStarW1I3 = Relationship::createParentTRelationship(w1, i3);
	Relationship parentStarW1A7 = Relationship::createParentTRelationship(w1, a7);
	Relationship parentStarI3R4 = Relationship::createParentTRelationship(i3, r4);
	Relationship parentStarI3R5 = Relationship::createParentTRelationship(i3, r5);
	Relationship parentStarI3R6 = Relationship::createParentTRelationship(i3, r6);
	Relationship parentStarW1R4 = Relationship::createParentTRelationship(w1, r4);
	Relationship parentStarW1R5 = Relationship::createParentTRelationship(w1, r5);
	Relationship parentStarW1R6 = Relationship::createParentTRelationship(w1, r6);

	// Initialise Modifies relationships -- only statements required for iter 1
	Relationship modifiesW1Y2 = Relationship::createModifiesRelationship(w1, y2);
	Relationship modifiesW1X4 = Relationship::createModifiesRelationship(w1, x4);
	Relationship modifiesW1Y5 = Relationship::createModifiesRelationship(w1, y5);
	Relationship modifiesW1Stmt6 = Relationship::createModifiesRelationship(w1, stmt6);
	Relationship modifiesW1Z7 = Relationship::createModifiesRelationship(w1, z7);

	Relationship modifiesA2Y2 = Relationship::createModifiesRelationship(a2, y2);

	Relationship modifiesI3X4 = Relationship::createModifiesRelationship(i3, x4);
	Relationship modifiesI3Y5 = Relationship::createModifiesRelationship(i3, y5);
	Relationship modifiesI3Stmt6 = Relationship::createModifiesRelationship(i3, stmt6);

	Relationship modifiesR4X4 = Relationship::createModifiesRelationship(r4, x4);
	Relationship modifiesR5Y5 = Relationship::createModifiesRelationship(r5, y5);
	Relationship modifiesR6Stmt6 = Relationship::createModifiesRelationship(r6, stmt6);

	Relationship modifiesA7Z7 = Relationship::createModifiesRelationship(a7, z7);


	// Initialise Uses relationships -- only statements required for iter 1
	Relationship usesW1V1 = Relationship::createUsesRelationship(w1, v1);
	Relationship usesW1X2 = Relationship::createUsesRelationship(w1, x2);
	Relationship usesW1B3 = Relationship::createUsesRelationship(w1, b3);

	Relationship usesA2X2 = Relationship::createUsesRelationship(a2, x2);
	Relationship usesI3B3 = Relationship::createUsesRelationship(i3, b3);
	Relationship usesP8X8 = Relationship::createUsesRelationship(p8, x8);

	//TODO: Add Follows and FollowsStar relationships

};

TEST_CASE("QPS: test working correctly") {

    auto testEvaluate = [](string queryString, 
		set<string> expectedResult, 
		shared_ptr<PKB> pkb) {
        // ----- given -----
        QPS qps = QPS();
		list<string> autoTesterResults;

        // ----- when -----
		qps.evaluate(queryString, shared_ptr<PKBQueryHandler>(pkb));
		qps.projectResults(autoTesterResults);

        // ----- then -----
		REQUIRE(qps.getResults() == expectedResult);

		set<string> autoTesterSet{};
		for (string s : autoTesterResults) {
			autoTesterSet.insert(s);
		}
		REQUIRE(autoTesterSet == expectedResult);
    };

	// ------ PKB ------ 
	shared_ptr<PKB> pkb = shared_ptr<PKB>(new PKB());
	vector<Entity> entities{ w1, a2, i3, r4, r5, r6, a7, p8, main, v1, constOne1,
		y2, x2, constOne2, b3, constZero3, x4, y5, stmt6, z7, constOne7, x8 };
	vector<Relationship> allParentAndParentStar{ 
		parentW1A2, parentW1I3, parentW1A7, parentI3R4, parentI3R5, parentI3R6,
		parentStarW1A2, parentStarW1I3, parentStarW1A7, parentStarI3R4, parentStarI3R5, parentStarI3R6,
		parentStarW1R4, parentStarW1R5, parentStarW1R6 };
	vector<Relationship> allModifies{ 
		modifiesW1Y2, modifiesW1X4, modifiesW1Y5, modifiesW1Stmt6, modifiesW1Z7, modifiesA2Y2,
		modifiesI3X4, modifiesI3Y5, modifiesI3Stmt6, modifiesR4X4, modifiesR5Y5, modifiesR6Stmt6,
		modifiesA7Z7 };
	vector<Relationship> allUses{ usesW1V1, usesW1X2, usesW1B3, usesA2X2, usesI3B3, usesP8X8};
	pkb->addEntities(entities);
	pkb->addRelationships(allParentAndParentStar);
	pkb->addRelationships(allModifies);
	pkb->addRelationships(allUses);


	string queryString = "stmt s; Select s";
	set<string> expectedResult{ "1", "2", "3", "4", "5", "6", "7", "8"};
	SECTION("Select only") {
		testEvaluate(queryString, expectedResult, pkb);

		queryString = "constant c;\n Select c";
		expectedResult = set<string>{ "1", "0" };
		testEvaluate(queryString, expectedResult, pkb);

		queryString = "procedure p;\t Select p";
		expectedResult = set<string>{ "main" };
		testEvaluate(queryString, expectedResult, pkb);

		queryString = "variable \n syn; Select syn";
		expectedResult = set<string>{ "v", "y", "x", "b", "z", "stmt"};
		testEvaluate(queryString, expectedResult, pkb);

		queryString = "assign syn; Select \n syn";
		expectedResult = set<string>{ "2", "7" };
		testEvaluate(queryString, expectedResult, pkb);

		queryString = "read r01; Select r01";
		expectedResult = set<string>{ "4", "5", "6" };
		testEvaluate(queryString, expectedResult, pkb);
	}

	SECTION("Select and such that") {
		queryString = "stmt child;\n Select child such that Parent(_, child)";
		expectedResult = set<string>{ "2", "3", "7", "4", "5", "6" };
		testEvaluate(queryString, expectedResult, pkb);

		queryString = "stmt stmt, child;\n Select child such that Parent(stmt, child)";
		expectedResult = set<string>{ "2", "3", "7", "4", "5", "6" };
		testEvaluate(queryString, expectedResult, pkb);

		queryString = "stmt s;\n Select s such that Parent(1, s)";
		expectedResult = set<string>{ "2", "3", "7"};

		queryString = "stmt Parent;\n Select Parent such that Parent*(Parent, _)";
		expectedResult = set<string>{ "1", "3" };
		testEvaluate(queryString, expectedResult, pkb);

		queryString = "variable Select;\n Select Select such that Modifies(6, Select)";
		expectedResult = set<string>{ "stmt", };
		testEvaluate(queryString, expectedResult, pkb);

		queryString = "variable Modifies; stmt s; \n Select Modifies such that Modifies(s, Modifies)";
		expectedResult = set<string>{ "y", "x", "stmt", "z"};
		testEvaluate(queryString, expectedResult, pkb);

		queryString = "stmt s; \n Select s such that Uses(s, _)";
		expectedResult = set<string>{ "1", "2", "3", "8" };
		testEvaluate(queryString, expectedResult, pkb);

		queryString = "variable variable; stmt s; \n Select variable such that Uses(s, variable)";
		expectedResult = set<string>{ "v", "x", "b" };
		testEvaluate(queryString, expectedResult, pkb);

		//TODO: Follows/FollowsT
	}

	SECTION("Select and Pattern") {
		//TODO
	}

	SECTION("Select, such that and Pattern") {
		//TODO
	}
}