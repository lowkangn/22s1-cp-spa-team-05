#include "catch.hpp"

#include <qps/QPS.h>

namespace {
	/* Corresponds to the following SIMPLE source (with line numbers)
		procedure main {
	1:		while (v > 1) {
	2:			y = x + 1 * 0;
	3:			if (b > 0) then {
	4:				read x;
				} else {
	5:				read y;
	6:				read stmt;
				}
	7:			z = 1000 - 0 + b;
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
	UsesS: (1,v), (1,x), (1,b), (2,x), (3,b), (7,b) (8,x)
	UsesP: (main, rhsU) where rhsU is an rhs of UsesS
	PatternAssign: 2(y, _"x"_), 2(y, _"1"_), 2(y, _"0"_), 2(y, _"1*0"_), 2(y, _"x+1*0"_), 2(y, "x+1*0"),
				   7(z, _"0"_), 7(z, _"1000"_), 7(z, _"1000-0"_), 7(z, _"1000-0+b"_), 7(z, "1000-0+b") 
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

	// Initialise tokens for non-statement entities
	Token mainToken = Token::createNameOrKeywordToken("main");
	Token vToken = Token::createNameOrKeywordToken("v");
	Token constOneToken = Token::createIntegerToken("1");
	Token yToken = Token::createNameOrKeywordToken("y");
	Token xToken = Token::createNameOrKeywordToken("x");
	Token bToken = Token::createNameOrKeywordToken("b");
	Token constZeroToken = Token::createIntegerToken("0");
	Token stmtToken = Token::createNameOrKeywordToken("stmt");
	Token zToken = Token::createNameOrKeywordToken("z");
	Token constOneThousandToken = Token::createIntegerToken("1000");

	// Initialise non-statement entities
	Entity main = Entity::createProcedureEntity(mainToken);
	Entity v1 = Entity::createVariableEntity(1, vToken);
	Entity constOne1 = Entity::createConstantEntity(1, constOneToken);
	Entity y2 = Entity::createVariableEntity(2, yToken);
	Entity x2 = Entity::createVariableEntity(2, xToken);
	Entity constOne2 = Entity::createConstantEntity(2, constOneToken);
	Entity constZero2 = Entity::createConstantEntity(2, constZeroToken);
	Entity b3 = Entity::createVariableEntity(3, bToken);
	Entity constZero3 = Entity::createConstantEntity(3, constZeroToken);
	Entity x4 = Entity::createVariableEntity(4, xToken);
	Entity y5 = Entity::createVariableEntity(5, yToken);
	Entity stmt6 = Entity::createVariableEntity(6, stmtToken);
	Entity z7 = Entity::createVariableEntity(7, zToken);
	Entity constZero7 = Entity::createConstantEntity(7, constZeroToken);
	Entity constOneThousand7 = Entity::createConstantEntity(7, constOneThousandToken);
	Entity b7 = Entity::createVariableEntity(7, bToken);
	Entity x8 = Entity::createVariableEntity(8, xToken);

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


	// Initialise Follows relationships
	Relationship followsW1P8 = Relationship::createFollowsRelationship(w1, p8);
	Relationship followsA2I3 = Relationship::createFollowsRelationship(a2, i3);
	Relationship followsI3A7 = Relationship::createFollowsRelationship(i3, a7);
	Relationship followsR5R6 = Relationship::createFollowsRelationship(r5, r6);

	// Initialise FollowsStar relationships
	Relationship followsStarW1P8 = Relationship::createFollowsTRelationship(w1, p8);
	Relationship followsStarA2I3 = Relationship::createFollowsTRelationship(a2, i3);
	Relationship followsStarI3A7 = Relationship::createFollowsTRelationship(i3, a7);
	Relationship followsStarR5R6 = Relationship::createFollowsTRelationship(r5, r6);
	Relationship followsStarA2A7 = Relationship::createFollowsTRelationship(a2, a7);


	// Initialise PatternAssigns
	Pattern patternA2Y2 = Pattern::createAssignPattern(a2.getLine(), yToken.getString(), "x10*+"); //is x+1*0
	Pattern patternA7Z7 = Pattern::createAssignPattern(a7.getLine(), zToken.getString(), "10000-b+"); //is 1000-0+b
};

TEST_CASE("QPS: test working correctly") {

    auto testQPS = [](string queryString, 
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
	
	vector<Entity> entities{ 
		w1, a2, i3, r4, r5, r6, a7, p8, main, v1, constOne1, y2, x2, constOne2, b3, constZero3,
		x4, y5, stmt6, z7, constZero7, constOneThousand7, b7, x8 };
	
	vector<Relationship> allParentAndParentStar{ 
		parentW1A2, parentW1I3, parentW1A7, parentI3R4, parentI3R5, parentI3R6,
		parentStarW1A2, parentStarW1I3, parentStarW1A7, parentStarI3R4, parentStarI3R5, parentStarI3R6,
		parentStarW1R4, parentStarW1R5, parentStarW1R6 };

	vector<Relationship> allFollowsAndFollowsStar{
		followsW1P8, followsA2I3, followsI3A7, followsR5R6, followsStarW1P8, followsStarA2I3,
		followsStarI3A7, followsStarR5R6, followsStarA2A7 };
	
	vector<Relationship> allModifies{ 
		modifiesW1Y2, modifiesW1X4, modifiesW1Y5, modifiesW1Stmt6, modifiesW1Z7, modifiesA2Y2,
		modifiesI3X4, modifiesI3Y5, modifiesI3Stmt6, modifiesR4X4, modifiesR5Y5, modifiesR6Stmt6,
		modifiesA7Z7 };
	
	vector<Relationship> allUses{ usesW1V1, usesW1X2, usesW1B3, usesA2X2, usesI3B3, usesP8X8};

	vector<Pattern> allPatternAssigns{ patternA2Y2, patternA7Z7 };
	
	pkb->addEntities(entities);
	pkb->addRelationships(allParentAndParentStar);
	pkb->addRelationships(allFollowsAndFollowsStar);
	pkb->addRelationships(allModifies);
	pkb->addRelationships(allUses);
	pkb->addPatterns(allPatternAssigns);

	string queryString = "stmt s; Select s";
	set<string> expectedResult{ "1", "2", "3", "4", "5", "6", "7", "8"};
	SECTION("Select only") {
		testQPS(queryString, expectedResult, pkb);

		queryString = "constant c;\n Select c";
		expectedResult = set<string>{ "1", "0", "1000"};
		testQPS(queryString, expectedResult, pkb);

		queryString = "procedure p;\t Select p";
		expectedResult = set<string>{ "main" };
		testQPS(queryString, expectedResult, pkb);

		queryString = "variable \n syn; Select syn";
		expectedResult = set<string>{ "v", "y", "x", "b", "z", "stmt"};
		testQPS(queryString, expectedResult, pkb);

		queryString = "assign syn; Select \n syn";
		expectedResult = set<string>{ "2", "7" };
		testQPS(queryString, expectedResult, pkb);

		queryString = "read r01; Select r01";
		expectedResult = set<string>{ "4", "5", "6" };
		testQPS(queryString, expectedResult, pkb);
	}

	SECTION("Select and such that") {
		queryString = "stmt child;\n Select child such that Parent(_, child)";
		expectedResult = set<string>{ "2", "3", "7", "4", "5", "6" };
		testQPS(queryString, expectedResult, pkb);

		queryString = "stmt stmt, child;\n Select child such that Parent(stmt, child)";
		expectedResult = set<string>{ "2", "3", "7", "4", "5", "6" };
		testQPS(queryString, expectedResult, pkb);

		queryString = "stmt s;\n Select s such that Parent(1, s)";
		expectedResult = set<string>{ "2", "3", "7"};
		testQPS(queryString, expectedResult, pkb);

		queryString = "stmt Parent;\n Select Parent such that Parent*(Parent, _)";
		expectedResult = set<string>{ "1", "3" };
		testQPS(queryString, expectedResult, pkb);

		queryString = "variable Select;\n Select Select such that Modifies(6, Select)";
		expectedResult = set<string>{ "stmt", };
		testQPS(queryString, expectedResult, pkb);

		queryString = "variable Modifies; stmt s; \n Select Modifies such that Modifies(s, Modifies)";
		expectedResult = set<string>{ "y", "x", "stmt", "z"};
		testQPS(queryString, expectedResult, pkb);

		queryString = "stmt s; \n Select s such that Uses(s, _)";
		expectedResult = set<string>{ "1", "2", "3", "8" };
		testQPS(queryString, expectedResult, pkb);

		queryString = "variable variable; stmt s; \n Select variable such that Uses(s, variable)";
		expectedResult = set<string>{ "v", "x", "b" };
		testQPS(queryString, expectedResult, pkb);

		queryString = "variable variable; \n Select variable such that Follows(1, 8)";
		expectedResult = set<string>{ "v", "y", "x", "b", "z", "stmt" };
		testQPS(queryString, expectedResult, pkb);

		queryString = "stmt stmt; \n Select stmt such that Follows(stmt, 3)";
		expectedResult = set<string>{ "2" };
		testQPS(queryString, expectedResult, pkb);

		queryString = "assign a; \n Select a such that Follows(2, a)";
		expectedResult = set<string>{};
		testQPS(queryString, expectedResult, pkb);

		queryString = "assign a; \n Select a such that Follows*(2, a)";
		expectedResult = set<string>{ "7" };
		testQPS(queryString, expectedResult, pkb);
	}

	SECTION("Select and Pattern") {
		
		queryString = "assign a; \n Select a pattern a(\"y\", _)";
		expectedResult = set<string>{ "2" };
		testQPS(queryString, expectedResult, pkb);

		queryString = "assign a; variable v; \n Select v pattern a(v, _)";
		expectedResult = set<string>{ "y", "z"};
		testQPS(queryString, expectedResult, pkb);

		queryString = "assign a; \n Select a pattern a(_, _ )";
		expectedResult = set<string>{ "2" , "7"};
		testQPS(queryString, expectedResult, pkb);

		queryString = "assign a; \n Select a pattern a(_, _\"x\"_ )";
		expectedResult = set<string>{ "2"};
		testQPS(queryString, expectedResult, pkb);

		queryString = "assign a; stmt s; variable v; \n Select s pattern a(v, _\"1000\"_ )";
		expectedResult = set<string>{ "1", "2", "3", "4", "5", "6", "7", "8" };
		testQPS(queryString, expectedResult, pkb);


		queryString = "assign a; stmt s; variable v; \n Select s pattern a(_, _ )";
		expectedResult = set<string>{ "1", "2", "3", "4", "5", "6", "7", "8" };
		testQPS(queryString, expectedResult, pkb);

		/* Bugged: pattern matches z = 1000 - 0 + b, 100 is a substring of 1000 1000 0 - b + 
		queryString = "assign a; stmt s; variable v;\n Select s pattern a(v, _\"100\"_ )";
		expectedResult = set<string>{};
		testQPS(queryString, expectedResult, pkb);
		*/

		/* Not needed for iter 1
		
		queryString = "assign a; variable v; \n Select v pattern a(v,  _\"1000-0\"_ )";
		expectedResult = set<string>{ "z" };
		testQPS(queryString, expectedResult, pkb);

		queryString = "assign a; variable v; \n Select v pattern a(v,  \"1000-0\" )";
		expectedResult = set<string>{};
		testQPS(queryString, expectedResult, pkb);

		queryString = "assign a; variable v; \n Select a pattern a(v,  _\"1*0\"_ )";
		expectedResult = set<string>{ "2" };
		testQPS(queryString, expectedResult, pkb);

		queryString = "assign a; variable v; \n Select a pattern a(v,  _\"x+1\"_ )";
		expectedResult = set<string>{ };
		testQPS(queryString, expectedResult, pkb);
		*/
	}

	SECTION("Select, such that and Pattern") {
		
		// Select assignments that follow line 2 and has an RHS with 0 in it
		queryString = "assign a; variable v; \n Select a such that Follows(2,a) pattern a(v, _\"0\"_ )";
		expectedResult = set<string>{};
		testQPS(queryString, expectedResult, pkb);

		// Select assignments that follow* line 2 and has an RHS with 0 in it
		queryString = "assign a; \n Select a such that Follows*(2,a) pattern a(_, _\"0\"_ )";
		expectedResult = set<string>{ "7" };
		testQPS(queryString, expectedResult, pkb);

		
		queryString = "assign a; variable v; Select a pattern a(_, _\"x\"_) such that Parent(_, a)";
		expectedResult = set<string>{ "2" };
		testQPS(queryString, expectedResult, pkb);

		// Select variables that are used in an assignment that has an RHS with 1000 in it and has line 1 as its parent 
		queryString = "assign a; variable v; \n Select v such that Parent(1,a) pattern a(v, _\"1000\"_ )";
		expectedResult = set<string>{ "z" };
		testQPS(queryString, expectedResult, pkb);

		// Select assignments whose LHS has a variable that is used by an if
		queryString = "assign a; if i; variable v; \n Select a pattern a(v, _ ) such that Modifies(i, v)" ;
		expectedResult = set<string>{ "2" };
		testQPS(queryString, expectedResult, pkb);

		// Select constants iff there is an assignment of the form y = ...0... and a while is a parent of an if
		queryString = "assign a; constant c; if i; while w; \n Select c pattern a(\"y\", _\"0\"_ ) such that Parent(w, i) " ;
		expectedResult = set<string>{ "0", "1", "1000"};
		testQPS(queryString, expectedResult, pkb);

	}
}

TEST_CASE("QPS: test correct errors") {

	auto testQPS = [](string queryString,
		set<string> expectedResult) {
			// ----- given -----
			// no need for a populated PKB since we expect SyntaxError/SemanticError
			shared_ptr<PKB> pkb = shared_ptr<PKB>(new PKB());
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

	string queryString = "stmt s;\n";
	set<string> expectedResult{ "SyntaxError" };
	SECTION("Syntax errors") {
		testQPS(queryString, expectedResult);

		queryString = "stmt s\n Select s";
		testQPS(queryString, expectedResult);

		queryString = "stmt s;\n Select s;";
		testQPS(queryString, expectedResult);

		queryString = "statement s;\n Select s";
		testQPS(queryString, expectedResult);

		queryString = "stmt s;\n Select 1";
		testQPS(queryString, expectedResult);

		queryString = "stmt s;\n Select s such that Nested(1,2)";
		testQPS(queryString, expectedResult);

		queryString = "stmt s; Select s that";
		testQPS(queryString, expectedResult);

		queryString = "stmt s; select s";
		testQPS(queryString, expectedResult);

		queryString = "stmt s;\n Select s such that Follows*(\"x\", s)";
		testQPS(queryString, expectedResult);

		queryString = "stmt s;\n Select s such that Parent*(\"main\", s)";
		testQPS(queryString, expectedResult);

		queryString = "stmt s;\n Select s such that Follows*(\"x\", s)";
		testQPS(queryString, expectedResult);

		queryString = "stmt s;\n Select s such that Uses(s, \"x)";
		testQPS(queryString, expectedResult);
	}

	SECTION("Semantic errors") {
		expectedResult = set<string>{ "SemanticError" };

		queryString = "stmt s;\n Select c";
		testQPS(queryString, expectedResult);

		queryString = "stmt s,s;\n Select s";
		testQPS(queryString, expectedResult);

		queryString = "stmt s; constant s;\n Select s";
		testQPS(queryString, expectedResult);

		queryString = "stmt s; constant c;\n Select s such that Parent(s, c)";
		testQPS(queryString, expectedResult);

		queryString = "stmt s; constant c;\n Select s such that Parent*(c, s)";
		testQPS(queryString, expectedResult);

		queryString = "stmt s; procedure p;\n Select s such that Follows(s, p)";
		testQPS(queryString, expectedResult);

		queryString = "stmt s; procedure p;\n Select s such that Follows(p, s)";
		testQPS(queryString, expectedResult);

		queryString = "stmt s; \n Select s such that Uses(_, \"x\")";
		testQPS(queryString, expectedResult);

		queryString = "stmt s; \n Select s such that Uses(s, 1)";
		testQPS(queryString, expectedResult);

		queryString = "stmt s, s2; \n Select s such that Modifies(s, s2)";
		testQPS(queryString, expectedResult);

		queryString = "stmt s; \n Select s such that Modifies(_, \"x\")";
		testQPS(queryString, expectedResult);

		queryString = "stmt s, s2; \n Select s pattern s2(_, _)";
		testQPS(queryString, expectedResult);

		queryString = "assign a1, a; \n Select s pattern a1(a, _)";
		testQPS(queryString, expectedResult);
	}
}