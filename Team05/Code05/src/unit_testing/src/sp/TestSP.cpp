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
}

TEST_CASE("Test Source Processor : extractEntity") {
	auto test = [](string sourceProgram, vector<Entity> expectedEntites) {
		SourceProcessor sp = SourceProcessor();
		sp.initialize(sourceProgram);

		vector<Entity> entities = sp.extractEntities();

		sort(entities.begin(), entities.end(), compareEntity);

		REQUIRE(entities.size() == expectedEntites.size());

		for (int i = 0; i < entities.size(); i++) {
			Entity entityExtracted = entities[i];
			Entity entityExpected = expectedEntites[i];
			bool check = entities[i].equals(expectedEntites[i]);
			REQUIRE(entities[i].equals(expectedEntites[i]));
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