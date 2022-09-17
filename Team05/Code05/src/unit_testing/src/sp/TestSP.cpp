#include "catch.hpp"

#include <sp/SourceProcessor.h>
#include <istream>
#include <sstream>
#include <pkb/interfaces/PKBUpdateHandler.h>
#include <pkb/PKB.h>
#include <pkb/table_managers/PkbEntityTable.h>
#include <pkb/table_managers/PkbRelationshipTable.h>
#include <pkb/table_managers/PkbPatternTable.h>
#include <sp/dataclasses/design_objects/Relationship.h>
#include <sp/dataclasses/tokens/Token.h>
#include <sp/lexer/Lexer.h>
#include <sp/parser/SimpleSyntaxParserManager.h>
#include <sp/design_extractor/DesignExtractorManager.h>
#include <sp/design_extractor/ModifiesExtractor.h>

#include <memory>


using namespace std;

TEST_CASE("Test SP extraction of Entities and Relationships") {
	auto testExtract = [](string program, vector<Entity> expectedEntities, vector<Relationship> expectedRelationships, vector<Pattern> expectedPatterns) {
		// Given
		stringstream ss(program);
		istream& stream = ss;

		Lexer lexer = Lexer();
		list<Token> tokens = lexer.tokenize(stream);

		ParserManager parser = ParserManager(tokens);
		shared_ptr<ASTNode> programTree = parser.parse();
		shared_ptr<EntityExtractor> entityExtractor(new EntityExtractor());
		shared_ptr<PatternExtractor> patternExtractor(new PatternExtractor());
		shared_ptr<Extractor<Relationship>> modifiesExtractor = shared_ptr<Extractor<Relationship>>(new ModifiesExtractor());

		// put relationship extractors into a vector
		vector<shared_ptr<Extractor<Relationship>>> relationExtractors = vector<shared_ptr<Extractor<Relationship>>>{ modifiesExtractor };

		// create manager
		DesignExtractorManager extractor = DesignExtractorManager(*entityExtractor, *patternExtractor, relationExtractors);

		vector<Entity> extractedEntities = extractor.extractEntities(programTree);
		vector<Relationship> extractedRelationships = extractor.extractRelationships(programTree);
		vector<Pattern> extractedPatterns = extractor.extractPatterns(programTree);

		//REQUIRE(extractedEntities.size() == expectedEntities.size());
		//REQUIRE(extractedRelationships.size() == expectedRelationships.size());
		//REQUIRE(extractedPatterns.size() == expectedPatterns.size());

		for (int i = 0; i < extractedEntities.size(); i++) {
			bool isInExpectedEntities = false;
			for (int j = 0; j < expectedEntities.size(); j++) {
				isInExpectedEntities |= extractedEntities[i].equals(expectedEntities[j]);
			}
			REQUIRE(isInExpectedEntities);
		}

		for (int i = 0; i < extractedRelationships.size(); i++) {
			bool isInExpectedRelationships = false;
			for (int j = 0; j < expectedRelationships.size(); j++) {
				isInExpectedRelationships |= extractedRelationships[i].equals(expectedRelationships[j]);
			}
			REQUIRE(isInExpectedRelationships);
		}
		/*
		for (int i = 0; i < extractedPatterns.size(); i++) {
			bool isInExpectedPatterns = false;
			for (int j = 0; j < expectedPatterns.size(); j++) {
				isInExpectedPatterns |= extractedPatterns[i].equals(expectedPatterns[j]);
			}
			REQUIRE(isInExpectedPatterns);
		}
		*/
	};

	SECTION("Basic program") {
		string program = "procedure main {\nx = 1;\nread y;}";
		// Create tokens
		Token mainToken = Token::createNameOrKeywordToken("main");
		Token xTokenMain = Token::createNameOrKeywordToken("x");
		Token assignToken = Token::createEqualsToken();
		Token constToken = Token::createIntegerToken("1");
		Token readToken = Token::createReadToken();
		Token yToken = Token::createNameOrKeywordToken("y");

		// Create entities
		Entity procedureMainEntity = Entity::createProcedureEntity(mainToken);

		Entity xEntity = Entity::createVariableEntity(1, xTokenMain);
		Entity assignEntity = Entity::createAssignEntity(1);
		Entity constEntity = Entity::createConstantEntity(1, constToken);

		Entity yEntity = Entity::createVariableEntity(2, yToken);
		Entity readEntity = Entity::createReadEntity(2);

		vector<Entity> expectedEntities = vector<Entity>{ procedureMainEntity, xEntity, assignEntity, constEntity, readEntity, yEntity };

		// Create relationships
		Relationship procedureXRelationship = Relationship::createModifiesRelationship(procedureMainEntity, xEntity);
		Relationship procedureYRelationship = Relationship::createModifiesRelationship(procedureMainEntity, yEntity);
		Relationship assignRelation = Relationship::createModifiesRelationship(assignEntity, xEntity);
		Relationship readRelation = Relationship::createModifiesRelationship(readEntity, yEntity);

		vector<Relationship> expectedRelationships = vector<Relationship>{ procedureXRelationship, procedureYRelationship, assignRelation, readRelation };
		
		vector<Pattern> expectedPatterns{ Pattern::createAssignPattern(1, "x", "1") };

		testExtract(program, expectedEntities, expectedRelationships, expectedPatterns);
	}
	
	SECTION("Multi-procedure program") {
		/*
			procedure main {
				1. x = 1
			}

			procedure readY {
				2. read y
			}
		*/
	
		string program = "procedure main {\nx=1;\n}\n procedure readY {\nread y;\n}";
		// Create tokens
		// main
		Token mainToken = Token::createNameOrKeywordToken("main");
		Token xTokenMain = Token::createNameOrKeywordToken("x");
		Token assignToken = Token::createEqualsToken();
		Token constToken = Token::createIntegerToken("1");

		// readY
		Token readYToken = Token::createNameOrKeywordToken("readY");
		Token readToken = Token::createReadToken();
		Token yToken = Token::createNameOrKeywordToken("y");

		// Create entities
		Entity procedureMainEntity = Entity::createProcedureEntity(mainToken);
		Entity procedureReadYEntity = Entity::createProcedureEntity(readYToken);

		Entity statementListMain = Entity::createStmtLstEntity();
		Entity statementListReadY = Entity::createStmtLstEntity();

		Entity xEntity = Entity::createVariableEntity(1, xTokenMain);
		Entity assignEntity = Entity::createAssignEntity(1);
		Entity constEntity = Entity::createConstantEntity(1, constToken);

		Entity yEntity = Entity::createVariableEntity(2, yToken);
		Entity readEntity = Entity::createReadEntity(2);

		Entity fakeEntity = Entity::createReadEntity(500);


		vector<Entity> expectedEntities = vector<Entity>{ procedureMainEntity, procedureReadYEntity, xEntity, assignEntity, constEntity, readEntity, yEntity };

		// Create relationships
		Relationship procedureXRelationship = Relationship::createModifiesRelationship(procedureMainEntity, xEntity);
		Relationship procedureYRelationship = Relationship::createModifiesRelationship(procedureReadYEntity, yEntity);
		Relationship assignRelation = Relationship::createModifiesRelationship(assignEntity, xEntity);
		Relationship readRelation = Relationship::createModifiesRelationship(readEntity, yEntity);

		vector <Relationship> expectedRelationships = vector<Relationship>{ procedureXRelationship, procedureYRelationship, assignRelation, readRelation };

		vector<Pattern> expectedPatterns{ Pattern::createAssignPattern(1, "x", "1") };

		testExtract(program, expectedEntities, expectedRelationships, expectedPatterns);

	}
	
	SECTION("Multi-procedure program with if-else and while statements") {
		/*
		procedure main {
			1. x = 3;
			2. read y;
			3. while ( x != 0 ) {
			4.     x = x - 1;
			5.     print y;
			}
			6. if (y > 5) then {
			7.     x = y;
			} else {
			8.     y = x;
			}
		}
		*/
	
		string program = "procedure main {\nx = 3;\nread y;\nwhile ( x != 0 ) {\nx = x - 1;\nprint y;\n}\nif (y > 5) then {\nx = y;\n} else {\ny = x;\n}\n}";

		// Tokens
		Token mainToken = Token::createNameOrKeywordToken("main");
		Token xToken = Token::createNameOrKeywordToken("x");
		Token yToken = Token::createNameOrKeywordToken("y");
		Token assignToken = Token::createEqualsToken();
		Token notEqualToken = Token::createNotEqualsToken();
		Token greaterToken = Token::createGreaterThanToken();
		Token minusToken = Token::createMinusToken();
		Token constThreeToken = Token::createIntegerToken("3");
		Token constZeroToken = Token::createIntegerToken("0");
		Token constOneToken = Token::createIntegerToken("1");
		Token constFiveToken = Token::createIntegerToken("5");
		Token readToken = Token::createReadToken();
		Token whileToken = Token::createWhileToken();
		Token printToken = Token::createPrintToken();
		Token ifToken = Token::createIfToken();
		Token stmtListToken = Token::createPlaceholderToken();

		// Creating Entities
		Entity procedureEntity = Entity::createProcedureEntity(mainToken);

		Entity assign1Entity = Entity::createAssignEntity(1);
		Entity x1Entity = Entity::createVariableEntity(1, xToken);
		Entity y2Entity = Entity::createVariableEntity(2, yToken);
		Entity readEntity = Entity::createReadEntity(2);

		Entity whileEntity = Entity::createWhileEntity(3);

		Entity assign4Entity = Entity::createAssignEntity(4);
		Entity x4Entity = Entity::createVariableEntity(4, xToken);
		Entity printEntity = Entity::createPrintEntity(5);
		Entity y5Entity = Entity::createVariableEntity(5, yToken);

		Entity ifEntity = Entity::createIfEntity(6);

		Entity assign7Entity = Entity::createAssignEntity(7);
		Entity x7Entity = Entity::createVariableEntity(7, xToken);

		Entity assign8Entity = Entity::createAssignEntity(8);
		Entity y8Entity = Entity::createVariableEntity(8, yToken);

		vector<Entity> expectedEntities = {
			procedureEntity, assign1Entity, x1Entity, y2Entity, readEntity, whileEntity, assign4Entity,
			x4Entity, printEntity, y5Entity, ifEntity, x7Entity, assign8Entity, y8Entity
		};

		// Creating relationships
		Relationship mainModifiesX1 = Relationship::createModifiesRelationship(procedureEntity, x1Entity);
		Relationship mainModifiesY2 = Relationship::createModifiesRelationship(procedureEntity, y2Entity);
		Relationship mainModifiesX4 = Relationship::createModifiesRelationship(procedureEntity, x4Entity);
		Relationship mainModifiesX7 = Relationship::createModifiesRelationship(procedureEntity, x7Entity);
		Relationship mainModifiesY8 = Relationship::createModifiesRelationship(procedureEntity, y8Entity);

		Relationship readModifiesX4 = Relationship::createModifiesRelationship(readEntity, y2Entity);

		Relationship whileModifiesX4 = Relationship::createModifiesRelationship(whileEntity, x4Entity);

		Relationship assign1ModifiesX1 = Relationship::createModifiesRelationship(assign1Entity, x1Entity);

		Relationship assign4ModifiesX4 = Relationship::createModifiesRelationship(assign4Entity, x4Entity);

		Relationship ifModifiesX7 = Relationship::createModifiesRelationship(ifEntity, x7Entity);
		Relationship ifModifiesY8 = Relationship::createModifiesRelationship(ifEntity, y8Entity);

		Relationship assign7ModifiesX7 = Relationship::createModifiesRelationship(assign7Entity, x7Entity);
		Relationship assign8ModifiesY8 = Relationship::createModifiesRelationship(assign8Entity, y8Entity);

		vector<Relationship> expectedRelationships = vector<Relationship>{ mainModifiesX1, mainModifiesY2, mainModifiesX4, mainModifiesX7,
																	mainModifiesY8, assign1ModifiesX1, readModifiesX4, whileModifiesX4, assign4ModifiesX4,
																	ifModifiesX7, ifModifiesY8, assign7ModifiesX7, assign8ModifiesY8 };

		vector<Pattern> expectedPattern = vector<Pattern>{};
		testExtract(program, expectedEntities, expectedRelationships, expectedPattern);
	}
	
}
