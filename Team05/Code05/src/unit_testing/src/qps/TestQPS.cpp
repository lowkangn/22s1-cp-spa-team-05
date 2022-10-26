#include "catch.hpp"

#include <qps/QPS.h>

namespace {
	/* Corresponds to the following SIMPLE source (with line numbers)
		procedure main {
	1:		while (v > 1 && t == 2) {
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
	9.		call y;
		}
		procedure x {
	10.		x = x / 1 + 2;
	11.		call main;
		}
		procedure y {
	12.		print main;
	13:		if (c == 0 && d == 1 || e == 2) {
	14:			print f;		
			} //else ommitted for brevity
		}

	For which the following relationships hold:
	Parent: (1,2), (1,3), (1,7), (3,4), (3,5), (3,6)
	Parent*: Parent, (1,4), (1,5), (1,6)
	Follows: (1,8), (2,3), (3,7), (5,6), (8,9), (10,11)
	Follows*: Follows, (1,9), (2,7)
	ModifiesS: (1,y), (1,x), (1,stmt), (1,z), (2,y), (3,x), (3,y), (3, stmt), (4,x), (5,y), (6,stmt), (7,z),
				(11, all of the previous RHS), (10,x)
	ModifiesP: (main, rhsM), (x,rhsM), where rhsM is an rhs of ModifiesS
	UsesS: (1,v), (1,x), (1,b), (2,x), (3,b), (7,b) (8,x), (9, y), (11, all of the previous RHS), (10, x), (12, main)
	UsesP: (main,v), (main,x), (main,b), (main, main), (y,main) (x, all of the previous RHS)
	PatternAssign: 2(y, _"x"_), 2(y, _"1"_), 2(y, _"0"_), 2(y, _"1*0"_), 2(y, _"x+1*0"_), 2(y, "x+1*0"),
				   7(z, _"0"_), 7(z, _"1000"_), 7(z, _"1000-0"_), 7(z, _"1000-0+b"_), 7(z, "1000-0+b") 
				   10(x, _"x"_), 10(x, _"1"_), 10(x, _"2"_), 10(x, _"x/1"_), 10(x, _"x/1+2"_), 10(x, "x/1+2")
	CallsAttribute: (9, y), (11, main)
	PatternWhile: 1("v", _), 1("t", _)
	PatternIf: 3("b", _, _), 13("c", _, _), 13("d", _, _), 13("e", _, _)
	*/

	// Initialise statement entities
	Entity w1 = Entity::createWhileEntity(1);
	Entity a2 = Entity::createAssignEntity(2);
	Entity i3 = Entity::createIfEntity(3);
	Entity r4 = Entity::createReadEntity(4);
	Entity r5 = Entity::createReadEntity(5);
	Entity r6 = Entity::createReadEntity(6);
	Entity a7 = Entity::createAssignEntity(7);
	Entity p8 = Entity::createPrintEntity(8);\
	Entity call9 = Entity::createCallEntity(9);
	Entity a10 = Entity::createAssignEntity(10);
	Entity call11 = Entity::createCallEntity(11);
	Entity p12 = Entity::createPrintEntity(12);
	Entity i13 = Entity::createIfEntity(13);
	Entity p14 = Entity::createPrintEntity(14);

	// Initialise tokens for non-statement entities
	Token mainToken = Token::createNameOrKeywordToken("main");
	Token vToken = Token::createNameOrKeywordToken("v");
	Token tToken = Token::createNameOrKeywordToken("t");
	Token constOneToken = Token::createIntegerToken("1");
	Token yToken = Token::createNameOrKeywordToken("y");
	Token xToken = Token::createNameOrKeywordToken("x");
	Token bToken = Token::createNameOrKeywordToken("b");
	Token cToken = Token::createNameOrKeywordToken("c");
	Token dToken = Token::createNameOrKeywordToken("d");
	Token eToken = Token::createNameOrKeywordToken("e");
	Token fToken = Token::createNameOrKeywordToken("f");
	Token constZeroToken = Token::createIntegerToken("0");
	Token stmtToken = Token::createNameOrKeywordToken("stmt");
	Token zToken = Token::createNameOrKeywordToken("z");
	Token constOneThousandToken = Token::createIntegerToken("1000");
	Token constTwoToken = Token::createIntegerToken("2");

	// Initialise non-statement entities
	Entity mainProc = Entity::createProcedureEntity(mainToken);
	Entity v1 = Entity::createVariableEntity(1, vToken);
	Entity t1 = Entity::createVariableEntity(1, tToken);
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
	Entity y9 = Entity::createVariableEntity(9, yToken);

	Entity xProc = Entity::createProcedureEntity(xToken);
	Entity x10Lhs = Entity::createVariableEntity(10, xToken);
	Entity x10Rhs = Entity::createVariableEntity(10, xToken);
	Entity constOne10 = Entity::createConstantEntity(10, constOneToken);
	Entity constTwo10 = Entity::createConstantEntity(10, constTwoToken);

	Entity yProc = Entity::createProcedureEntity(yToken);
	Entity main12 = Entity::createVariableEntity(12, mainToken);
	Entity c13 = Entity::createVariableEntity(13, cToken);
	Entity d13 = Entity::createVariableEntity(13, dToken);
	Entity e13 = Entity::createVariableEntity(13, eToken);
	Entity f14 = Entity::createVariableEntity(14, fToken);

	// Initialise parent relationships
	Relationship parentW1A2 = Relationship::createParentRelationship(w1, a2);
	Relationship parentW1I3 = Relationship::createParentRelationship(w1, i3);
	Relationship parentW1A7 = Relationship::createParentRelationship(w1, a7);
	Relationship parentI3R4 = Relationship::createParentRelationship(i3, r4);
	Relationship parentI3R5 = Relationship::createParentRelationship(i3, r5);
	Relationship parentI3R6 = Relationship::createParentRelationship(i3, r6);
	Relationship parentI13P14 = Relationship::createParentRelationship(i13, p14);

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
	Relationship parentStarI13P14 = Relationship::createParentTRelationship(i13, p14);

	// Initialise Modifies relationships -- statements (except calls)
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
	Relationship modifiesA10X10 = Relationship::createModifiesRelationship(a10, x10Lhs);

	// Initialise Modifies relationships -- calls
	Relationship modifiesC11Y2 = Relationship::createModifiesRelationship(call11, y2);
	Relationship modifiesC11X4 = Relationship::createModifiesRelationship(call11, x4);
	Relationship modifiesC11Y5 = Relationship::createModifiesRelationship(call11, y5);
	Relationship modifiesC11Stmt6 = Relationship::createModifiesRelationship(call11, stmt6);
	Relationship modifiesC11Z7 = Relationship::createModifiesRelationship(call11, z7);
	
	// Initialise Modifies relationships -- procedures
	Relationship modifiesMainY2 = Relationship::createModifiesRelationship(mainProc, y2);
	Relationship modifiesMainX4 = Relationship::createModifiesRelationship(mainProc, x4);
	Relationship modifiesMainY5 = Relationship::createModifiesRelationship(mainProc, y5);
	Relationship modifiesMainStmt6 = Relationship::createModifiesRelationship(mainProc, stmt6);
	Relationship modifiesMainZ7 = Relationship::createModifiesRelationship(mainProc, z7);

	Relationship modifiesXProcY2 = Relationship::createModifiesRelationship(xProc, y2);
	Relationship modifiesXProcX4 = Relationship::createModifiesRelationship(xProc, x4);
	Relationship modifiesXProcY5 = Relationship::createModifiesRelationship(xProc, y5);
	Relationship modifiesXProcStmt6 = Relationship::createModifiesRelationship(xProc, stmt6);
	Relationship modifiesXProcZ7 = Relationship::createModifiesRelationship(xProc, z7);
	Relationship modifiesXProcX10 = Relationship::createModifiesRelationship(xProc, x10Lhs);

	// Initialise Uses relationships -- statements (except calls)
	Relationship usesW1V1 = Relationship::createUsesRelationship(w1, v1);
	Relationship usesW1T1 = Relationship::createUsesRelationship(w1, t1);
	Relationship usesW1X2 = Relationship::createUsesRelationship(w1, x2);
	Relationship usesW1B3 = Relationship::createUsesRelationship(w1, b3);

	Relationship usesA2X2 = Relationship::createUsesRelationship(a2, x2);
	Relationship usesI3B3 = Relationship::createUsesRelationship(i3, b3);
	Relationship usesA7B7 = Relationship::createUsesRelationship(a7, b7);

	Relationship usesP8X8 = Relationship::createUsesRelationship(p8, x8);
	
	Relationship usesA10X10 = Relationship::createUsesRelationship(a10, x10Rhs);
	Relationship usesP12Main12 = Relationship::createUsesRelationship(p12, main12);

	Relationship usesI13C13 = Relationship::createUsesRelationship(i13, c13);
	Relationship usesI13D13 = Relationship::createUsesRelationship(i13, d13);
	Relationship usesI13E13 = Relationship::createUsesRelationship(i13, e13);

	Relationship usesP14F14 = Relationship::createUsesRelationship(p14, f14);

	// Initialise Uses relationships -- calls (call y)
	Relationship usesC9Main12 = Relationship::createUsesRelationship(call9, main12);
	Relationship usesC9C13 = Relationship::createUsesRelationship(call9, c13);
	Relationship usesC9D13 = Relationship::createUsesRelationship(call9, d13);
	Relationship usesC9E13 = Relationship::createUsesRelationship(call9, e13);
	Relationship usesC9F14 = Relationship::createUsesRelationship(call9, f14);
	
	// Initialise Uses relationships -- calls (call main)
	Relationship usesC11V1 = Relationship::createUsesRelationship(call11, v1);
	Relationship usesC11T1 = Relationship::createUsesRelationship(call11, t1);
	Relationship usesC11X2 = Relationship::createUsesRelationship(call11, x2);
	Relationship usesC11B3 = Relationship::createUsesRelationship(call11, b3);
	Relationship usesC11B7 = Relationship::createUsesRelationship(call11, b7);
	Relationship usesC11X8 = Relationship::createUsesRelationship(call11, x8);
	Relationship usesC11Main12 = Relationship::createUsesRelationship(call11, main12);

	// Initialise Uses relationships -- procedures (procedure main)
	Relationship usesMainV1 = Relationship::createUsesRelationship(mainProc, v1);
	Relationship usesMainT1 = Relationship::createUsesRelationship(mainProc, t1);
	Relationship usesMainX2 = Relationship::createUsesRelationship(mainProc, x2);
	Relationship usesMainB3 = Relationship::createUsesRelationship(mainProc, b3);
	Relationship usesMainB7 = Relationship::createUsesRelationship(mainProc, b7);
	Relationship usesMainX8 = Relationship::createUsesRelationship(mainProc, x8);
	Relationship usesMainMain12 = Relationship::createUsesRelationship(mainProc, main12);

	// Initialise Uses relationships -- procedures (procedure x)
	Relationship usesXProcX10 = Relationship::createUsesRelationship(xProc, x10Rhs);
	Relationship usesXProcV1 = Relationship::createUsesRelationship(xProc, v1);
	Relationship usesXProcT1 = Relationship::createUsesRelationship(xProc, t1);
	Relationship usesXProcX2 = Relationship::createUsesRelationship(xProc, x2);
	Relationship usesXProcB3 = Relationship::createUsesRelationship(xProc, b3);
	Relationship usesXProcB7 = Relationship::createUsesRelationship(xProc, b7);
	Relationship usesXProcX8 = Relationship::createUsesRelationship(xProc, x8);
	Relationship usesXProcMain12 = Relationship::createUsesRelationship(xProc, main12);
	Relationship usesXProcC13 = Relationship::createUsesRelationship(xProc, c13);
	Relationship usesXProcD13 = Relationship::createUsesRelationship(xProc, d13);
	Relationship usesXProcE13 = Relationship::createUsesRelationship(xProc, e13);
	Relationship usesXProcF14 = Relationship::createUsesRelationship(xProc, f14);

	// Initialise Uses relationships -- procedures (procedure y)
	Relationship usesYProcMain12 = Relationship::createUsesRelationship(yProc, main12);
	Relationship usesYProcC13 = Relationship::createUsesRelationship(yProc, c13);
	Relationship usesYProcD13 = Relationship::createUsesRelationship(yProc, d13);
	Relationship usesYProcE13 = Relationship::createUsesRelationship(yProc, e13);
	Relationship usesYProcF14 = Relationship::createUsesRelationship(yProc, f14);

	// Initialise Follows relationships
	Relationship followsW1P8 = Relationship::createFollowsRelationship(w1, p8);
	Relationship followsA2I3 = Relationship::createFollowsRelationship(a2, i3);
	Relationship followsI3A7 = Relationship::createFollowsRelationship(i3, a7);
	Relationship followsR5R6 = Relationship::createFollowsRelationship(r5, r6);
	Relationship followsP8C9 = Relationship::createFollowsRelationship(p8, call9);
	Relationship followsA10C11 = Relationship::createFollowsRelationship(a10, call11);
	Relationship followsP12I13 = Relationship::createFollowsRelationship(p12, i13);

	// Initialise FollowsStar relationships
	Relationship followsStarW1P8 = Relationship::createFollowsTRelationship(w1, p8);
	Relationship followsStarA2I3 = Relationship::createFollowsTRelationship(a2, i3);
	Relationship followsStarI3A7 = Relationship::createFollowsTRelationship(i3, a7);
	Relationship followsStarR5R6 = Relationship::createFollowsTRelationship(r5, r6);
	Relationship followsStarP8C9 = Relationship::createFollowsTRelationship(p8, call9);
	Relationship followsStarA10C11 = Relationship::createFollowsTRelationship(a10, call11);
	Relationship followsStarP12I13 = Relationship::createFollowsTRelationship(p12, i13);

	Relationship followsStarW1C9 = Relationship::createFollowsTRelationship(w1, call9);
	Relationship followsStarA2A7 = Relationship::createFollowsTRelationship(a2, a7);
	Relationship followsStarW1I9 = Relationship::createFollowsTRelationship(w1, i13);


	// Initialise Calls Attribute relationships
	Relationship callsAttributeC9YProc = Relationship::createCallStmtAttributeRelationship(call9, yProc);
	Relationship callsAttributeC11Main = Relationship::createCallStmtAttributeRelationship(call11, mainProc);

	// Initialise PatternAssigns
	Pattern patternA2Y2 = Pattern::createAssignPattern(a2.getLine(), " " + yToken.getString() + " ", " x 1 0 * + "); //is x+1*0
	Pattern patternA7Z7 = Pattern::createAssignPattern(a7.getLine(), " " + zToken.getString() + " ", " 1000 0 - b + "); //is 1000-0+b
	Pattern patternA10X10 = Pattern::createAssignPattern(a10.getLine(), " " + xToken.getString() + " ", " x 1 / 2 + "); //is x/1+2

	// Initialise PatternWhile
	Pattern patternWhileV1 = Pattern::createWhilePattern(w1.getLine(), vToken.getString());
	Pattern patternWhileT1 = Pattern::createWhilePattern(w1.getLine(), tToken.getString());

	// Initialise PatternIf
	Pattern patternIf3 = Pattern::createIfPattern(i3.getLine(), bToken.getString());
	Pattern patternIf13C13 = Pattern::createIfPattern(i13.getLine(), cToken.getString());
	Pattern patternIf13D13 = Pattern::createIfPattern(i13.getLine(), dToken.getString());
	Pattern patternIf13E13 = Pattern::createIfPattern(i13.getLine(), eToken.getString());
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
		w1, a2, i3, r4, r5, r6, a7, p8, mainProc, v1, t1, constOne1, y2, x2, constOne2, b3, constZero3,
		x4, y5, stmt6, z7, constZero7, constOneThousand7, b7, x8, call9, xProc, a10, x10Lhs, x10Rhs, 
		constOne10, constTwo10, call11, yProc, p12, main12, i13, p14, c13, d13, e13, f14, };

	vector<Relationship> allParentAndParentStar{ 
		parentW1A2, parentW1I3, parentW1A7, parentI3R4, parentI3R5, parentI3R6,
		parentStarW1A2, parentStarW1I3, parentStarW1A7, parentStarI3R4, parentStarI3R5, parentStarI3R6,
		parentStarW1R4, parentStarW1R5, parentStarW1R6, parentStarI13P14, parentI13P14 };

	vector<Relationship> allFollowsAndFollowsStar{
		followsW1P8, followsA2I3, followsI3A7, followsR5R6, followsP8C9, followsA10C11, followsP12I13,
		followsStarW1P8, followsStarA2I3, followsStarI3A7, followsStarR5R6, followsStarP8C9, followsStarA10C11,
		followsStarW1C9, followsStarA2A7, followsStarP12I13 };

	vector<Relationship> allModifiesS{
		modifiesW1Y2, modifiesW1X4, modifiesW1Y5, modifiesW1Stmt6, modifiesW1Z7, modifiesA2Y2,
		modifiesI3X4, modifiesI3Y5, modifiesI3Stmt6, modifiesR4X4, modifiesR5Y5, modifiesR6Stmt6,
		modifiesA7Z7, modifiesA10X10,  
		modifiesC11Y2, modifiesC11X4, modifiesC11Y5, modifiesC11Stmt6, modifiesC11Z7, modifiesC11Z7 };
	
	vector<Relationship> allModifiesP{
		modifiesMainY2, modifiesMainX4, modifiesMainY5, modifiesMainStmt6, modifiesMainZ7, modifiesMainY2,
		modifiesMainX4, modifiesMainY5, modifiesMainStmt6, modifiesMainX4, modifiesMainY5, modifiesMainStmt6,
		modifiesMainZ7,
		modifiesXProcY2, modifiesXProcX4, modifiesXProcY5, modifiesXProcStmt6, modifiesXProcZ7, modifiesXProcX10, };

	vector<Relationship> allUsesS{ 
		usesW1V1, usesW1T1, usesW1X2, usesW1B3, usesA2X2, usesI3B3, usesA7B7, usesP8X8, usesA10X10, usesP12Main12, 
		usesC9Main12, usesC9C13, usesC9D13, usesC9E13, usesC9F14, usesC11V1, usesC11T1, usesC11X2, usesC11B3, 
		usesC11B7, usesC11X8, usesC11Main12, usesI13C13, usesI13D13, usesI13E13, usesP14F14 };

	vector<Relationship> allUsesP{
		usesMainV1, usesMainT1, usesMainX2, usesMainB3, usesMainB7, usesMainX8, usesMainMain12,
		usesXProcX10, usesXProcV1, usesXProcT1, usesXProcX2, usesXProcB3, usesXProcB7, usesXProcX8, usesXProcMain12,
		usesXProcC13, usesXProcD13, usesXProcE13, usesXProcF14,
		usesYProcMain12, usesYProcC13, usesYProcD13, usesYProcE13, usesYProcF14 };

	vector<Relationship> allCallStatementAttribute{ callsAttributeC9YProc, callsAttributeC11Main };

	vector<Pattern> allPatterns{ patternA2Y2, patternA7Z7, patternWhileT1, patternWhileV1, patternIf3,
								patternIf13C13, patternIf13D13, patternIf13E13 };

	
	pkb->addEntities(entities);
	pkb->addRelationships(allParentAndParentStar);
	pkb->addRelationships(allFollowsAndFollowsStar);
	pkb->addRelationships(allModifiesS);
	pkb->addRelationships(allModifiesP);
	pkb->addRelationships(allUsesS);
	pkb->addRelationships(allUsesP);
	pkb->addRelationships(allCallStatementAttribute);
	pkb->addPatterns(allPatterns);

	string queryString = "stmt s; Select s";
	set<string> expectedResult{ "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12", "13", "14"};
	SECTION("Select only") {
		testQPS(queryString, expectedResult, pkb);

		queryString = "constant c;\n Select c";
		expectedResult = set<string>{ "1", "0", "1000", "2"};
		testQPS(queryString, expectedResult, pkb);

		queryString = "procedure p;\t Select p";
		expectedResult = set<string>{ "main", "y", "x", };
		testQPS(queryString, expectedResult, pkb);

		queryString = "variable \n syn; Select syn";
		expectedResult = set<string>{ "v", "main", "x", "b", "y", "z", "stmt", "c", "d", "e", "f", "t"};
		testQPS(queryString, expectedResult, pkb);

		queryString = "assign syn; Select \n syn";
		expectedResult = set<string>{ "2", "7", "10"};
		testQPS(queryString, expectedResult, pkb);

		queryString = "read r01; Select r01";
		expectedResult = set<string>{ "4", "5", "6" };
		testQPS(queryString, expectedResult, pkb);
	}

	SECTION("Select and such that") {
		queryString = "stmt child;\n Select child such that Parent(_, child)";
		expectedResult = set<string>{ "2", "3", "7", "4", "5", "6", "14"};
		testQPS(queryString, expectedResult, pkb);

		queryString = "stmt stmt, child;\n Select child such that Parent(stmt, child)";
		expectedResult = set<string>{ "2", "3", "7", "4", "5", "6", "14"};
		testQPS(queryString, expectedResult, pkb);

		queryString = "stmt s;\n Select s such that Parent(1, s)";
		expectedResult = set<string>{ "2", "3", "7"};
		testQPS(queryString, expectedResult, pkb);

		queryString = "stmt Parent;\n Select Parent such that Parent* (Parent, _)";
		expectedResult = set<string>{ "1", "3", "13"};
		testQPS(queryString, expectedResult, pkb);

		queryString = "variable Select;\n Select Select such that Modifies(6, Select)";
		expectedResult = set<string>{ "stmt", };
		testQPS(queryString, expectedResult, pkb);

		queryString = "variable Modifies; stmt s; \n Select Modifies such that Modifies(s, Modifies)";
		expectedResult = set<string>{ "y", "x", "stmt", "z"};
		testQPS(queryString, expectedResult, pkb);

		queryString = "stmt s; \n Select s such that Uses(s, _)";
		expectedResult = set<string>{ "1", "2", "3", "7", "8", "9", "10", "11", "12", "13", "14"};
		testQPS(queryString, expectedResult, pkb);

		queryString = "variable variable; stmt s; \n Select variable such that Uses(s, variable)";
		expectedResult = set<string>{ "v", "t", "x", "b", "main", "c", "d", "e", "f"};
		testQPS(queryString, expectedResult, pkb);

		queryString = "variable variable; \n Select variable such that Follows(1, 8)";
		expectedResult = set<string>{ "v", "t", "y", "x", "b", "z", "stmt", "main", "c", "d", "e", "f" };
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
		
		// wildcard and wildcard surrounded integer / name
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
		expectedResult = set<string>{ "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12", "13", "14"};
		testQPS(queryString, expectedResult, pkb);

		queryString = "assign a; stmt s; variable v; \n Select s pattern a(_, _ )";
		expectedResult = set<string>{ "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12", "13", "14" };
		testQPS(queryString, expectedResult, pkb);

		queryString = "assign a; stmt s; variable v;\n Select s pattern a(v, _\"100\"_ )";
		expectedResult = set<string>{};
		testQPS(queryString, expectedResult, pkb);

		// wildcard surrounded including operator
		queryString = "assign a; variable v; \n Select v pattern a(v,  _\"1000-0\"_ )";
		expectedResult = set<string>{ "z" };
		testQPS(queryString, expectedResult, pkb);

		queryString = "assign a; variable v; \n Select a pattern a(v,  _\"1*0\"_ )";
		expectedResult = set<string>{ "2" };
		testQPS(queryString, expectedResult, pkb);

		queryString = "assign a; variable v; \n Select a pattern a(v,  _\"x+1\"_ )";
		expectedResult = set<string>{ };
		testQPS(queryString, expectedResult, pkb);

		// exact matches
		queryString = "assign a; variable v; \n Select v pattern a(v,  \"1000-0+b\" )";
		expectedResult = set<string>{ "z" };
		testQPS(queryString, expectedResult, pkb);

		queryString = "assign a; variable v; \n Select v pattern a(v,  \"1000-0\" )";
		expectedResult = set<string>{};
		testQPS(queryString, expectedResult, pkb);

		// brackets
		queryString = "assign a; variable v; \n Select v pattern a(v,  \"(1000-0)+((((b))))\" )";
		expectedResult = set<string>{ "z" };
		testQPS(queryString, expectedResult, pkb);

		queryString = "assign a; variable v; \n Select a pattern a(v,  _\"(x+1)*0\"_ )";
		expectedResult = set<string>{ };
		testQPS(queryString, expectedResult, pkb);

		// while patterns
		queryString = "while w; \n Select w pattern w(\"v\", _)";
		expectedResult = set<string>{ "1" };
		testQPS(queryString, expectedResult, pkb);

		// while patterns
		queryString = "while w; variable v; \n Select v pattern w(v, _)";
		expectedResult = set<string>{ "v", "t" };
		testQPS(queryString, expectedResult, pkb);

		queryString = "while w; \n Select w pattern w(\"t\", _)";
		expectedResult = set<string>{ "1" };
		testQPS(queryString, expectedResult, pkb);

		queryString = "while w; \n Select w pattern w(_, _)";
		expectedResult = set<string>{ "1" };
		testQPS(queryString, expectedResult, pkb);

		// if patterns
		queryString = "if i; \n Select i pattern i(\"b\", _, _)";
		expectedResult = set<string>{ "3" };
		testQPS(queryString, expectedResult, pkb);

		queryString = "if i; \n Select i pattern i(\"c\", _, _)";
		expectedResult = set<string>{ "13" };
		testQPS(queryString, expectedResult, pkb);

		queryString = "if i; \n Select i pattern i(\"d\", _, _)";
		expectedResult = set<string>{ "13" };
		testQPS(queryString, expectedResult, pkb);

		queryString = "if i; variable v; \n Select v pattern i(v, _, _)";
		expectedResult = set<string>{ "b", "c", "d", "e" };
		testQPS(queryString, expectedResult, pkb);

		queryString = "if i; \n Select i pattern i(_, _, _)";
		expectedResult = set<string>{ "3", "13" };
		testQPS(queryString, expectedResult, pkb);

	}

	SECTION("Select and with") {
		queryString = "stmt s; \n Select s with 1 = 1";
		expectedResult = set<string>{ "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12", "13", "14"};
		testQPS(queryString, expectedResult, pkb);

		queryString = "stmt s; \n Select s with 1 = 2";
		expectedResult = set<string>{};
		testQPS(queryString, expectedResult, pkb);

		queryString = "assign a; \n Select a with a.stmt# = 2";
		expectedResult = set<string>{ "2" };
		testQPS(queryString, expectedResult, pkb);

		queryString = "variable v; \n Select v with \"x\" = v.varName";
		expectedResult = set<string>{ "x" };
		testQPS(queryString, expectedResult, pkb);

		queryString = "procedure pro; \n Select pro with \"main\" = pro.procName";
		expectedResult = set<string>{ "main" };
		testQPS(queryString, expectedResult, pkb);

		queryString = "stmt s; constant c; \n Select <s,c> with c.value = s.stmt#";
		expectedResult = set<string>{ "1 1", "2 2" };
		testQPS(queryString, expectedResult, pkb);

		queryString = "read r; print pr; \n Select <r, \t pr> with r.varName = pr.varName";
		expectedResult = set<string>{ "4 8" };
		testQPS(queryString, expectedResult, pkb);

		queryString = "read r; call ca; \n Select ca with r.varName = ca.procName";
		expectedResult = set<string>{ "9" };
		testQPS(queryString, expectedResult, pkb);

		queryString = "print pr; procedure pro; \n Select pro with pr.varName = pro.procName";
		expectedResult = set<string>{ "x", "main" };
		testQPS(queryString, expectedResult, pkb);

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
		expectedResult = set<string>{ "0", "1", "1000", "2" };
		testQPS(queryString, expectedResult, pkb);
	}

	SECTION("Select, such that, Pattern and with") {

		queryString = "assign a; variable v; while w; Select a such that Parent(w,a) pattern a(v, _\"0\"_ ) with v.varName = \"z\"";
		expectedResult = set<string>{ "7" };
		testQPS(queryString, expectedResult, pkb);

		queryString = "assign a; \n Select a with a.stmt#=2 such that Follows*(1,a) pattern a(_, _\"0\"_ )";
		expectedResult = set<string>{};
		testQPS(queryString, expectedResult, pkb);

		queryString = "assign a; variable v; Select a with v.varName = \"main\" pattern a(_, _\"x\"_) such that Parent(_, a)";
		expectedResult = set<string>{ "2" };
		testQPS(queryString, expectedResult, pkb);

		queryString = "assign a; variable v; stmt s; while w; \n Select v such that Parent(s,a) with s.stmt# = w.stmt# pattern a(v, _\"1000\"_ )";
		expectedResult = set<string>{ "z" };
		testQPS(queryString, expectedResult, pkb);

		queryString = "assign a; if i; variable v; procedure pro; \n Select a pattern a(v, _ ) such that Modifies(i, v) with v.varName = pro.procName";
		expectedResult = set<string>{ "2" };
		testQPS(queryString, expectedResult, pkb);

		queryString = "assign a; if i; variable v; print pr; \n Select a pattern a(v, _ ) such that Modifies(i, v) with v.varName = pr.varName";
		expectedResult = set<string>{};
		testQPS(queryString, expectedResult, pkb);

		queryString = "assign a; constant c; if i; while w; read r; print pr; \n Select c pattern a(\"y\", _\"0\"_ ) such that Parent(w, i) with r.varName = pr.varName";
		expectedResult = set<string>{ "0", "1", "1000", "2" };
		testQPS(queryString, expectedResult, pkb);

		// Select statement with parent of if pattern with c
		queryString = "stmt s; if i; Select s pattern i(\"c\", _, _) with 1=1 such that Parent(i, s) ";
		expectedResult = set<string>{ "14" };
		testQPS(queryString, expectedResult, pkb);

		// Select statement with Parent* of while pattern with t
		queryString = "stmt s; while w; variable v; Select s pattern w(v, _) such that Parent*(w, s) with v.varName = \"t\"";
		expectedResult = set<string>{ "2", "3", "4", "5", "6", "7" };
		testQPS(queryString, expectedResult, pkb);

		//Select variables used by procedure 'y' if there is a while with a control variable
		queryString = "variable variable; procedure p; while w; \n Select variable such that Uses(p, variable) with p.procName=\"y\" pattern w(_, _)" ;
		expectedResult = set<string>{ "main", "c", "d", "e", "f" };
		testQPS(queryString, expectedResult, pkb);


	}

	SECTION("Select, multiple such that, multiple Pattern and multiple with") {

		// 1 group, non-empty
		queryString = "stmt s; if i; read r; while w; print p; assign a, a1; variable v, v1; procedure pro;\
			Select v1 such that Parent(i, r) such that Modifies(r, v) such that Follows(i, a) \
					 pattern a(v1, _\"1000 -   0\"_ ) with i.stmt# = i.stmt#  \
					 with r.varName = pro.procName such that Uses(p, v) \
			";
		expectedResult = set<string>{"z"};
		testQPS(queryString, expectedResult, pkb);

		// 2 groups, both non-empty
		queryString = "stmt s; if i; read r; while w; print p; assign a, a1; variable v, v1; call ca; \
			Select v1 such that Parent(i, r) such that Modifies(r, v) such that Follows(i, a) with a.stmt#=7 \
					 pattern a1(v1, _\"1 * 0\"_ ) such that Modifies(w, v1) with v1.varName = ca.procName  \
			";
		expectedResult = set<string>{ "y" };
		testQPS(queryString, expectedResult, pkb);
		
		// 2 groups, 1 empty
		queryString = "stmt s; if i; read r; while w; print p; assign a, a1; variable v, v1; call ca; \
			Select v1 such that Parent(i, r) such that Modifies(r, v) such that Follows(i, a) with a.stmt#=2 \
					 pattern a1(v1, _\"1 * 0\"_ ) such that Modifies(w, v1) with v1.varName = ca.procName  \
			";
		expectedResult = set<string>{ };
		testQPS(queryString, expectedResult, pkb);
		
		// 6 groups, all non-empty
		queryString = "stmt s; if i; read r; while w; print p; assign a, a1; variable v, v1; constant c; \
			Select c such that Parent(i, r) such that Modifies(s, v) such that Follows(w, p) \
					 pattern a1(v1, \"1000 - 0 + b\" ) such that Uses(a, \"x\") with 1 = 1 with \"x\"=\"x\"  \
			";
		expectedResult = set<string>{ "0", "1", "1000", "2" };
		testQPS(queryString, expectedResult, pkb);
		
		// 6 groups, 1 empty
		queryString = "stmt s; if i; read r; while w; print p; assign a, a1; variable v, v1; constant c; \
			Select c such that Parent(i, r) such that Modifies(s, v) such that Follows(w, p) \
					 pattern a1(v1, \"1000 - 0 + b\" ) such that Uses(a, \"v\") with 1 = 1 with \"x\"=\"y\" \
			";
		expectedResult = set<string>{ };
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

		queryString = "stmt s; \n Select s such that Uses(s, 1)";
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