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

		REQUIRE(extractedEntities.size() == expectedEntities.size());
		REQUIRE(extractedRelationships.size() == expectedRelationships.size());
		REQUIRE(extractedPatterns.size() == expectedPatterns.size());

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

		for (int i = 0; i < extractedPatterns.size(); i++) {
			bool isInExpectedPatterns = false;
			for (int j = 0; j < expectedPatterns.size(); j++) {
				isInExpectedPatterns |= extractedPatterns[i].equals(expectedPatterns[j]);
			}
			REQUIRE(isInExpectedPatterns);
		}

	};

	SECTION("Basic program") {
		string program = "procedure main {\nx = 1;\nread y;}";
		// Create tokens
		Token mainToken = Token{ "main", TokenType::NAME_OR_KEYWORD };
		Token xTokenMain = Token{ "x", TokenType::NAME_OR_KEYWORD };
		Token assignToken = Token{ "=", TokenType::OPERATOR };
		Token constToken = Token{ "1", TokenType::INTEGER };
		Token readToken = Token{ "read", TokenType::NAME_OR_KEYWORD };
		Token yToken = Token{ "y", TokenType::NAME_OR_KEYWORD };

		// Create entities
		Entity procedureMainEntity = Entity::createProcedureEntity(mainToken);

		Entity xEntity = Entity::createVariableEntity(1, xTokenMain);
		Entity assignEntity = Entity::createAssignEntity(1);
		Entity constEntity = Entity::createConstantEntity(1, constToken);

		Entity yEntity = Entity::createVariableEntity(2, yToken);
		Entity readEntity = Entity::createReadEntity(2);

		vector<Entity> expectedEntities = vector<Entity>{ procedureMainEntity, xEntity, assignEntity, constEntity, readEntity, yEntity };

		// Create relationships
		Relationship procedureXRelationship = Relationship{ procedureMainEntity, xEntity, RelationshipType::MODIFIES };
		Relationship procedureYRelationship = Relationship{ procedureMainEntity, yEntity, RelationshipType::MODIFIES };
		Relationship assignRelation = Relationship{ assignEntity, xEntity, RelationshipType::MODIFIES };
		Relationship readRelation = Relationship{ readEntity, yEntity, RelationshipType::MODIFIES };

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
		Token mainToken = Token{ "main", TokenType::NAME_OR_KEYWORD };
		Token xTokenMain = Token{ "x", TokenType::NAME_OR_KEYWORD };
		Token assignToken = Token{ "=", TokenType::OPERATOR };
		Token constToken = Token{ "1", TokenType::INTEGER };

		// readY
		Token readYToken = Token{ "readY", TokenType::NAME_OR_KEYWORD };
		Token readToken = Token{ "read", TokenType::NAME_OR_KEYWORD };
		Token yToken = Token{ "y", TokenType::NAME_OR_KEYWORD };

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
		Relationship procedureXRelationship = Relationship{ procedureMainEntity, xEntity, RelationshipType::MODIFIES };
		Relationship procedureYRelationship = Relationship{ procedureReadYEntity, yEntity, RelationshipType::MODIFIES };
		Relationship assignRelation = Relationship{ assignEntity, xEntity, RelationshipType::MODIFIES };
		Relationship readRelation = Relationship{ readEntity, yEntity, RelationshipType::MODIFIES };

		vector <Relationship> expectedRelationships = vector<Relationship>{ procedureXRelationship, procedureYRelationship, assignRelation, readRelation };

		vector<Pattern> expectedPatterns{ Pattern::createAssignPattern(1, "x", "1") };

		testExtract(program, expectedEntities, expectedRelationships, expectedPatterns);

	}
}
