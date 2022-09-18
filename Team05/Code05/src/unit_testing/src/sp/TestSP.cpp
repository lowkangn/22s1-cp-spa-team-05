#include "catch.hpp"
#include <sstream>
#include <memory>
#include <vector>

#include<sp/SourceProcessor.h>
#include <sp/lexer/Lexer.h>
#include <sp/parser/SimpleSyntaxParserManager.h>
#include <sp/design_extractor/DesignExtractorManager.h>
#include <sp/design_extractor/EntityExtractor.h>
#include <sp/design_extractor/PatternExtractor.h>
#include <sp/design_extractor/ModifiesExtractor.h>
#include <sp/design_extractor/UsesExtractor.h>
#include <sp/design_extractor/ParentExtractor.h>
#include <sp/design_extractor/ParentTExtractor.h>
#include <sp/design_extractor/FollowsExtractor.h>
#include <sp/design_extractor/FollowsTExtractor.h>


#include <sp/dataclasses/tokens/Token.h>
#include <sp/dataclasses/ast/AST.h>
#include <sp/SPException.h>

#include <string>
#include <list>
#include <vector>
#include <sstream>
#include <memory>

using namespace std;


// Util
bool compareEntity(Entity entityOne, Entity entityTwo) {
	if (entityOne.getLine() == entityTwo.getLine()) {
		return entityOne.getString() > entityTwo.getString();
	}
	else {
		return entityOne.getLine() > entityTwo.getLine();
	}
};

bool comparePattern(Pattern patternOne, Pattern patternTwo) {
	if (patternOne.getLine() == patternTwo.getLine()) {
		return patternOne.getRhs() > patternTwo.getRhs();
	}
	else {
		return patternOne.getLine() > patternTwo.getLine();
	}
}

bool compareRelationship(Relationship relationshipOne, Relationship relationshipTwo) {
	if (relationshipOne.getType() == relationshipTwo.getType()) {
		return relationshipOne.getRhs().getLine() > relationshipTwo.getRhs().getLine();
	}
	return relationshipOne.getType() > relationshipTwo.getType();
}

TEST_CASE("Test Source Processor : extractEntity") {
	auto test = [](string sourceProgram, vector<Entity> expectedEntites) {
		stringstream ss(sourceProgram);

		SourceProcessor sp = SourceProcessor(ss);
		
		vector<Entity> entities = sp.extractEntities();

		sort(entities.begin(), entities.end(), compareEntity);

		REQUIRE(entities.size() == expectedEntites.size());

		for (int i = 0; i < entities.size(); i++) {
			Entity expectedEntity = entities[i];
			bool check = find(expectedEntites.begin(), expectedEntites.end(), expectedEntity) != expectedEntites.end();
			REQUIRE(check);
		}
	};

	SECTION("Test Milestone 1 program") {
		/*
			procedure main {
			read x;
			print x;
			y = 0;
			if ((!(x == 0)) && (y < 1)) then {
				while (y >= 0) {
					x = x + 10;
					y = y - 10;
				} 
			} else {
				y = x % 2 + y;
				z = 5 * x - y;
				z = z / 1 * 3;
			}
		}
		*/

		string program = "procedure main{\n\tread x; \n\tprint x; \n\ty = 0; \n    if ((!(x == 0)) && (y < 1)) then {\n\t\twhile(y >= 0) { \n\t\t\tx = x + 10; \n\t\t\ty = y - 10; \n\t\t } \n\t} else { \n\t\ty = x % 2 + y; \n\t\tz = 5 * x - y; \n\t\tz = z / 1 * 3; \n\t }\n}";

		vector<Entity> expected{
			Entity::createProcedureEntity(Token::createNameOrKeywordToken("main")),

			Entity::createReadEntity(1),
			Entity::createVariableEntity(1, Token::createNameOrKeywordToken("x")),

			Entity::createPrintEntity(2),
			Entity::createVariableEntity(2, Token::createNameOrKeywordToken("x")),

			Entity::createAssignEntity(3),
			Entity::createVariableEntity(3, Token::createNameOrKeywordToken("y")),
			Entity::createConstantEntity(3, Token::createIntegerToken("0")),

			Entity::createIfEntity(4),
			Entity::createExpressionEntity(4, Token::createAndToken()),

			Entity::createExpressionEntity(4, Token::createEqualityToken()),
			Entity::createVariableEntity(4, Token::createNameOrKeywordToken("x")),
			Entity::createConstantEntity(4, Token::createIntegerToken("0")),

			Entity::createExpressionEntity(4, Token::createLessThanToken()),
			Entity::createVariableEntity(4, Token::createNameOrKeywordToken("y")),
			Entity::createConstantEntity(4, Token::createIntegerToken("1")),

			Entity::createWhileEntity(5),
			Entity::createExpressionEntity(5, Token::createGreaterThanEqualToken()),
			Entity::createVariableEntity(5, Token::createNameOrKeywordToken("y")),
			Entity::createConstantEntity(5, Token::createIntegerToken("0")),

			Entity::createAssignEntity(6),
			Entity::createVariableEntity(6, Token::createNameOrKeywordToken("x")),
			Entity::createExpressionEntity(6, Token::createPlusToken()),
			Entity::createVariableEntity(6, Token::createNameOrKeywordToken("x")),
			Entity::createConstantEntity(6, Token::createIntegerToken("10")),

			Entity::createAssignEntity(7),
			Entity::createVariableEntity(7, Token::createNameOrKeywordToken("y")),
			Entity::createExpressionEntity(7, Token::createMinusToken()),
			Entity::createVariableEntity(7, Token::createNameOrKeywordToken("y")),
			Entity::createConstantEntity(7, Token::createIntegerToken("10")),

			Entity::createAssignEntity(8),
			Entity::createVariableEntity(8, Token::createNameOrKeywordToken("y")),
			Entity::createExpressionEntity(8, Token::createModulusToken()),
			Entity::createVariableEntity(8, Token::createNameOrKeywordToken("x")),
			Entity::createExpressionEntity(8, Token::createPlusToken()),
			Entity::createConstantEntity(8, Token::createIntegerToken("2")),
			Entity::createVariableEntity(8, Token::createNameOrKeywordToken("y")),

			Entity::createAssignEntity(9),
			Entity::createVariableEntity(9, Token::createNameOrKeywordToken("z")),
			Entity::createExpressionEntity(9, Token::createMultiplyToken()),
			Entity::createConstantEntity(9, Token::createIntegerToken("5")),
			Entity::createExpressionEntity(9, Token::createMinusToken()),
			Entity::createVariableEntity(9, Token::createNameOrKeywordToken("x")),
			Entity::createVariableEntity(9, Token::createNameOrKeywordToken("y")),


			Entity::createAssignEntity(10),
			Entity::createVariableEntity(10, Token::createNameOrKeywordToken("z")),
			Entity::createExpressionEntity(10, Token::createDivideToken()),
			Entity::createVariableEntity(10, Token::createNameOrKeywordToken("z")),
			Entity::createExpressionEntity(10, Token::createMultiplyToken()),
			Entity::createConstantEntity(10, Token::createIntegerToken("1")),
			Entity::createConstantEntity(10, Token::createIntegerToken("3")),
		};

		sort(expected.begin(), expected.end(), compareEntity);

		test(program, expected);
	}
}

TEST_CASE("Test Source Processor : extractPattern") {
	auto test = [](string sourceProgram, vector<Pattern> expectedPatterns) {
		stringstream ss(sourceProgram);

		SourceProcessor sp = SourceProcessor(ss);

		vector<Pattern> patterns = sp.extractPatterns();

		sort(patterns.begin(), patterns.end(), comparePattern);

		REQUIRE(patterns.size() == expectedPatterns.size());

		for (int i = 0; i < patterns.size(); i++) {
			Pattern patternExtracted = patterns[i];
			Pattern patternExpected = expectedPatterns[i];
			bool check = patterns[i].equals(expectedPatterns[i]);
			REQUIRE(patterns[i].equals(expectedPatterns[i]));
		}
	};

	SECTION("Test Milestone 1 program") {
		/*
			procedure main {
			read x;
			print x;
			y = 0;
			if ((!(x == 0)) && (y < 1)) then {
				while (y >= 0) {
					x = x + 10;
					y = y - 10;
				}
			} else {
				y = x % 2 + y;
				z = 5 * x - y;
				z = z / 1 * 3;
			}
		}
		*/

		string program = "procedure main{\n\tread x; \n\tprint x; \n\ty = 0; \n    if ((!(x == 0)) && (y < 1)) then {\n\t\twhile(y >= 0) { \n\t\t\tx = x + 10; \n\t\t\ty = y - 10; \n\t\t } \n\t} else { \n\t\ty = x % 2 + y; \n\t\tz = 5 * x - y; \n\t\tz = z / 1 * 3; \n\t }\n}";

		vector<Pattern> expected{
			Pattern::createAssignPattern(3,"y", "0"),
			Pattern::createIfPattern(4,"x y"),
			Pattern::createWhilePattern(5,"y"),
			Pattern::createAssignPattern(6,"x", "x10+"),
			Pattern::createAssignPattern(7,"y", "y10-"),
			Pattern::createAssignPattern(8,"y", "x2%y+"),
			Pattern::createAssignPattern(9,"z", "5x*y-"),
			Pattern::createAssignPattern(10,"z", "z1/3*"),
		};

		sort(expected.begin(), expected.end(), comparePattern);

		test(program, expected);
	}
}

TEST_CASE("Test Source Processor : extractRelations") {
	auto test = [](string sourceProgram, vector<Relationship> expectedRelations) {
		stringstream ss(sourceProgram);

		SourceProcessor sp = SourceProcessor(ss);

		vector<Relationship> relations = sp.extractRelations();

		sort(relations.begin(), relations.end(), compareRelationship);

		//REQUIRE(relations.size() == expectedRelations.size());

		for (int i = 0; i < expectedRelations.size(); i++) {
			Relationship expectedRelation = expectedRelations[i];
			bool check = find(relations.begin(), relations.end(), expectedRelation) != relations.end();
			if (!check) {
				int x = 1;
			}
			REQUIRE(find(relations.begin(), relations.end(), expectedRelation) != relations.end());
		}
	};

	SECTION("Test Milestone 1 program") {
		/*
			procedure main {
			read x;
			print x;
			y = 0;
			if ((!(x == 0)) && (y < 1)) then {
				while (y >= 0) {
					x = x + 10;
					y = y - 10;
				}
			} else {
				y = x % 2 + y;
				z = 5 * x - y;
				z = z / 1 * 3;
			}
		}
		*/

		string program = "procedure main{\n\tread x; \n\tprint x; \n\ty = 0; \n    if ((!(x == 0)) && (y < 1)) then {\n\t\twhile(y >= 0) { \n\t\t\tx = x + 10; \n\t\t\ty = y - 10; \n\t\t } \n\t} else { \n\t\ty = x % 2 + y; \n\t\tz = 5 * x - y; \n\t\tz = z / 1 * 3; \n\t }\n}";

		Token procedureToken = Token::createNameOrKeywordToken("main");
		Token xToken = Token::createNameOrKeywordToken("x");
		Token yToken = Token::createNameOrKeywordToken("y");
		Token zToken = Token::createNameOrKeywordToken("z");
		Token zeroToken = Token::createIntegerToken("0");
		Token oneToken = Token::createIntegerToken("1");
		Token twoToken = Token::createIntegerToken("2");
		Token threeToken = Token::createIntegerToken("3");
		Token fiveToken = Token::createIntegerToken("5");
		Token tenToken = Token::createIntegerToken("10");

		vector<Relationship> expectedFollows {
			Relationship::createFollowsRelationship(Entity::createReadEntity(1), Entity::createPrintEntity(2)),
			Relationship::createFollowsRelationship(Entity::createPrintEntity(2), Entity::createAssignEntity(3)),
			Relationship::createFollowsRelationship(Entity::createAssignEntity(3), Entity::createIfEntity(4)),

			Relationship::createFollowsRelationship(Entity::createAssignEntity(6), Entity::createAssignEntity(7)),
			Relationship::createFollowsRelationship(Entity::createAssignEntity(8), Entity::createAssignEntity(9)),
			Relationship::createFollowsRelationship(Entity::createAssignEntity(9), Entity::createAssignEntity(10)),
		};
		vector<Relationship> expectedFollowT {
			Relationship::createFollowsTRelationship(Entity::createReadEntity(1), Entity::createPrintEntity(2)),
			Relationship::createFollowsTRelationship(Entity::createReadEntity(1), Entity::createAssignEntity(3)),
			Relationship::createFollowsTRelationship(Entity::createReadEntity(1), Entity::createIfEntity(4)),

			Relationship::createFollowsTRelationship(Entity::createPrintEntity(2), Entity::createAssignEntity(3)),
			Relationship::createFollowsTRelationship(Entity::createPrintEntity(2), Entity::createIfEntity(4)),

			Relationship::createFollowsTRelationship(Entity::createAssignEntity(3), Entity::createIfEntity(4)),

			Relationship::createFollowsTRelationship(Entity::createAssignEntity(6), Entity::createAssignEntity(7)),
			Relationship::createFollowsTRelationship(Entity::createAssignEntity(8), Entity::createAssignEntity(9)),
			Relationship::createFollowsTRelationship(Entity::createAssignEntity(8), Entity::createAssignEntity(10)),
			Relationship::createFollowsTRelationship(Entity::createAssignEntity(9), Entity::createAssignEntity(10)),
		};

		vector<Relationship> expectedParent {
			Relationship::createParentRelationship(Entity::createIfEntity(4), Entity::createWhileEntity(5)),
			Relationship::createParentRelationship(Entity::createIfEntity(4), Entity::createAssignEntity(8)),
			Relationship::createParentRelationship(Entity::createIfEntity(4), Entity::createAssignEntity(9)),
			Relationship::createParentRelationship(Entity::createIfEntity(4), Entity::createAssignEntity(10)),

			Relationship::createParentRelationship(Entity::createWhileEntity(5), Entity::createAssignEntity(6)),
			Relationship::createParentRelationship(Entity::createWhileEntity(5), Entity::createAssignEntity(7)),
		};

		vector<Relationship> expectedParentT {
			Relationship::createParentTRelationship(Entity::createIfEntity(4), Entity::createWhileEntity(5)),
			Relationship::createParentTRelationship(Entity::createIfEntity(4), Entity::createAssignEntity(6)),
			Relationship::createParentTRelationship(Entity::createIfEntity(4), Entity::createAssignEntity(7)),
			Relationship::createParentTRelationship(Entity::createIfEntity(4), Entity::createAssignEntity(8)),
			Relationship::createParentTRelationship(Entity::createIfEntity(4), Entity::createAssignEntity(9)),
			Relationship::createParentTRelationship(Entity::createIfEntity(4), Entity::createAssignEntity(10)),

			Relationship::createParentTRelationship(Entity::createWhileEntity(5), Entity::createAssignEntity(6)),
			Relationship::createParentTRelationship(Entity::createWhileEntity(5), Entity::createAssignEntity(7)),
		};



		vector<Relationship> expectedModifies{
			Relationship::createModifiesRelationship(Entity::createProcedureEntity(procedureToken), Entity::createVariableEntity(1, xToken)),
			Relationship::createModifiesRelationship(Entity::createProcedureEntity(procedureToken), Entity::createVariableEntity(3, yToken)),
			Relationship::createModifiesRelationship(Entity::createProcedureEntity(procedureToken), Entity::createVariableEntity(6, xToken)),
			Relationship::createModifiesRelationship(Entity::createProcedureEntity(procedureToken), Entity::createVariableEntity(7, yToken)),
			Relationship::createModifiesRelationship(Entity::createProcedureEntity(procedureToken), Entity::createVariableEntity(8, yToken)),
			Relationship::createModifiesRelationship(Entity::createProcedureEntity(procedureToken), Entity::createVariableEntity(9, zToken)),
			Relationship::createModifiesRelationship(Entity::createProcedureEntity(procedureToken), Entity::createVariableEntity(10, zToken)),

			Relationship::createModifiesRelationship(Entity::createReadEntity(1), Entity::createVariableEntity(1, xToken)),
			Relationship::createModifiesRelationship(Entity::createAssignEntity(3), Entity::createVariableEntity(3, yToken)),
			Relationship::createModifiesRelationship(Entity::createAssignEntity(6), Entity::createVariableEntity(6, xToken)),
			Relationship::createModifiesRelationship(Entity::createAssignEntity(7), Entity::createVariableEntity(7, yToken)),
			Relationship::createModifiesRelationship(Entity::createAssignEntity(8), Entity::createVariableEntity(8, yToken)),
			Relationship::createModifiesRelationship(Entity::createAssignEntity(9), Entity::createVariableEntity(9, zToken)),
			Relationship::createModifiesRelationship(Entity::createAssignEntity(10), Entity::createVariableEntity(10, zToken)),

			Relationship::createModifiesRelationship(Entity::createIfEntity(4), Entity::createVariableEntity(6, xToken)),
			Relationship::createModifiesRelationship(Entity::createIfEntity(4), Entity::createVariableEntity(7, yToken)),
			Relationship::createModifiesRelationship(Entity::createIfEntity(4), Entity::createVariableEntity(8, yToken)),
			Relationship::createModifiesRelationship(Entity::createIfEntity(4), Entity::createVariableEntity(9, zToken)),
			Relationship::createModifiesRelationship(Entity::createIfEntity(4), Entity::createVariableEntity(10, zToken)),

			Relationship::createModifiesRelationship(Entity::createWhileEntity(5), Entity::createVariableEntity(6, xToken)),
			Relationship::createModifiesRelationship(Entity::createWhileEntity(5), Entity::createVariableEntity(7, yToken)),

		};

		vector<Relationship> expectedUses{
			Relationship::createUsesRelationship(Entity::createProcedureEntity(procedureToken), Entity::createVariableEntity(2, xToken)),
			Relationship::createUsesRelationship(Entity::createProcedureEntity(procedureToken), Entity::createVariableEntity(4, xToken)),
			Relationship::createUsesRelationship(Entity::createProcedureEntity(procedureToken), Entity::createVariableEntity(4, yToken)),
			Relationship::createUsesRelationship(Entity::createProcedureEntity(procedureToken), Entity::createVariableEntity(5, yToken)),
			Relationship::createUsesRelationship(Entity::createProcedureEntity(procedureToken), Entity::createVariableEntity(6, xToken)),
			Relationship::createUsesRelationship(Entity::createProcedureEntity(procedureToken), Entity::createVariableEntity(7, yToken)),
			Relationship::createUsesRelationship(Entity::createProcedureEntity(procedureToken), Entity::createVariableEntity(8, xToken)),
			Relationship::createUsesRelationship(Entity::createProcedureEntity(procedureToken), Entity::createVariableEntity(8, yToken)),
			Relationship::createUsesRelationship(Entity::createProcedureEntity(procedureToken), Entity::createVariableEntity(9, xToken)),
			Relationship::createUsesRelationship(Entity::createProcedureEntity(procedureToken), Entity::createVariableEntity(9, yToken)),
			Relationship::createUsesRelationship(Entity::createProcedureEntity(procedureToken), Entity::createVariableEntity(10, zToken)),

			Relationship::createUsesRelationship(Entity::createPrintEntity(2), Entity::createVariableEntity(2, xToken)),

			Relationship::createUsesRelationship(Entity::createIfEntity(4), Entity::createVariableEntity(4, xToken)),
			Relationship::createUsesRelationship(Entity::createIfEntity(4), Entity::createVariableEntity(4, yToken)),
			Relationship::createUsesRelationship(Entity::createIfEntity(4), Entity::createVariableEntity(5, yToken)),
			Relationship::createUsesRelationship(Entity::createIfEntity(4), Entity::createVariableEntity(6, xToken)),
			Relationship::createUsesRelationship(Entity::createIfEntity(4), Entity::createVariableEntity(7, yToken)),
			Relationship::createUsesRelationship(Entity::createIfEntity(4), Entity::createVariableEntity(8, xToken)),
			Relationship::createUsesRelationship(Entity::createIfEntity(4), Entity::createVariableEntity(8, yToken)),
			Relationship::createUsesRelationship(Entity::createIfEntity(4), Entity::createVariableEntity(9, xToken)),
			Relationship::createUsesRelationship(Entity::createIfEntity(4), Entity::createVariableEntity(9, yToken)),
			Relationship::createUsesRelationship(Entity::createIfEntity(4), Entity::createVariableEntity(10, zToken)),

			Relationship::createUsesRelationship(Entity::createWhileEntity(5), Entity::createVariableEntity(5, yToken)),
			Relationship::createUsesRelationship(Entity::createWhileEntity(5), Entity::createVariableEntity(6, xToken)),	
			Relationship::createUsesRelationship(Entity::createWhileEntity(5), Entity::createVariableEntity(7, yToken)),
			Relationship::createUsesRelationship(Entity::createAssignEntity(6), Entity::createVariableEntity(6, xToken)),
			Relationship::createUsesRelationship(Entity::createAssignEntity(7), Entity::createVariableEntity(7, yToken)),

			Relationship::createUsesRelationship(Entity::createAssignEntity(8), Entity::createVariableEntity(8, xToken)),	
			Relationship::createUsesRelationship(Entity::createAssignEntity(8), Entity::createVariableEntity(8, yToken)),	
			Relationship::createUsesRelationship(Entity::createAssignEntity(9), Entity::createVariableEntity(9, xToken)),	
			Relationship::createUsesRelationship(Entity::createAssignEntity(9), Entity::createVariableEntity(9, yToken)),	
			Relationship::createUsesRelationship(Entity::createAssignEntity(10), Entity::createVariableEntity(10, zToken)),	
		};

		vector<Relationship> expected;
		expected.insert(expected.end(), expectedFollows.begin(), expectedFollows.end());
		expected.insert(expected.end(), expectedFollowT.begin(), expectedFollowT.end());
		expected.insert(expected.end(), expectedParent.begin(), expectedParent.end());
		expected.insert(expected.end(), expectedParentT.begin(), expectedParentT.end());
		expected.insert(expected.end(), expectedModifies.begin(), expectedModifies.end());
		expected.insert(expected.end(), expectedUses.begin(), expectedUses.end());

		sort(expected.begin(), expected.end(), compareRelationship);

		test(program, expected);
	}
}
