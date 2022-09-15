#include "catch.hpp"
#include <sp/SourceProcessor.h>
#include <istream>
#include <sstream>
#include <pkb/PKB.h>
#include <pkb/interfaces/PKBUpdateHandler.h>
#include <pkb/table_managers/PkbEntityTable.h>
#include <pkb/table_managers/PkbRelationshipTable.h>

#include <memory>


using namespace std;
/*
TEST_CASE("Test Source Processor") {

	auto test = [](string program, vector<PkbEntity>  expectedEntities, vector<PkbRelationship> expectedRelationships) {

		// ===== given ======
		// stream
		stringstream ss(program);
		istream& stream = ss;
		
		// sp
		SourceProcessor sp = SourceProcessor();

		// pkb
		shared_ptr<PKBUpdateHandler> pkb = ProgramKnowledgeBase::getInstance();

		// ===== when =====
		// parse, extract and update
		sp.tokenizeParseExtractAndUpdatePkb(stream, pkb);

		// ===== then =====
		// pkb shoud be equal to expected
		// for now, we only support modifies and entity tracking, and its VERY manual
		shared_ptr<ProgramKnowledgeBase> pkb_whole = ProgramKnowledgeBase::getInstance(); // get by reference
		unordered_map<int, PkbRelationship> modifiesTable = pkb_whole->_getModifiesTable()->getMap();
		unordered_map<int, PkbEntity> entitiesTable = pkb_whole->_getEntitiesTable()->getMap();

		// No way of equality comparison
		// TODO refactor everything pkb
		// check that all relationships inside

	};
	
	SECTION("Basic program") {
		string program = "procedure someprocedure {\nx = 1;\nread y;}";
	
	}
	
}
*/

TEST_CASE("Test SP extraction of Entities and Relationships") {
	auto testExtract = [](string program, vector<Entity> expectedEntities, vector<Relationship> expectedRelationships) {
		// Given
		stringstream ss(program);
		istream& stream = ss;

		SourceProcessor sourceProcessor = SourceProcessor();
		PKB pkb = PKB();
		shared_ptr<PKBUpdateHandler> pkbUpdateHandler = pkb.getUpdateHandler();

		sourceProcessor.tokenizeParseExtractAndUpdatePkb(stream, pkbUpdateHandler);

		for (int i = 0; i < expectedEntities.size(); i++) {
			REQUIRE(pkb.containsEntity(expectedEntities[i]));
		}

		for (int i = 0; i < expectedRelationships.size(); i++) {
			REQUIRE(pkb.containsRelationship(expectedRelationships[i]));
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
		Entity programEntity = Entity::createProgramEntity();
		Entity procedureMainEntity = Entity::createProcedureEntity(mainToken);

		Entity xEntity = Entity::createVariableEntity(1, xTokenMain);
		Entity assignEntity = Entity::createAssignEntity(1);
		Entity constEntity = Entity::createConstantEntity(1, constToken);

		Entity yEntity = Entity::createVariableEntity(2, yToken);
		Entity readEntity = Entity::createReadEntity(2);

		vector<Entity> expectedEntities = vector<Entity>{ programEntity, procedureMainEntity, xEntity, assignEntity, constEntity, readEntity, yEntity };

		// Create relationships
		Relationship procedureXRelationship = Relationship{ procedureMainEntity, xEntity, RelationshipType::MODIFIES };
		Relationship procedureYRelationship = Relationship{ procedureMainEntity, yEntity, RelationshipType::MODIFIES };
		Relationship assignRelation = Relationship{ assignEntity, xEntity, RelationshipType::MODIFIES };
		Relationship readRelation = Relationship{ readEntity, yEntity, RelationshipType::MODIFIES };

		vector<Relationship> expectedRelationships = vector<Relationship>{ procedureXRelationship, procedureYRelationship, assignRelation, readRelation };

		testExtract(program, expectedEntities, expectedRelationships);
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

		// tokens for full program
		Token rootNodeToken = Token::getPlaceHolderToken();

		// Create entities
		Entity programEntity = Entity::createProgramEntity();
		Entity procedureMainEntity = Entity::createProcedureEntity(mainToken);
		Entity procedureReadYEntity = Entity::createProcedureEntity(readYToken);

		Entity xEntity = Entity::createVariableEntity(1, xTokenMain);
		Entity assignEntity = Entity::createAssignEntity(1);
		Entity constEntity = Entity::createConstantEntity(1, constToken);

		Entity yEntity = Entity::createVariableEntity(2, yToken);
		Entity readEntity = Entity::createReadEntity(2);

		vector<Entity> expectedEntities = vector<Entity>{ programEntity, procedureMainEntity, procedureReadYEntity, xEntity, assignEntity, constEntity, readEntity, yEntity };

		// Create relationships
		Relationship procedureXRelationship = Relationship{ procedureMainEntity, xEntity, RelationshipType::MODIFIES };
		Relationship procedureYRelationship = Relationship{ procedureReadYEntity, yEntity, RelationshipType::MODIFIES };
		Relationship assignRelation = Relationship{ assignEntity, xEntity, RelationshipType::MODIFIES };
		Relationship readRelation = Relationship{ readEntity, yEntity, RelationshipType::MODIFIES };

		vector <Relationship> expectedRelationships = vector<Relationship>{ procedureXRelationship, procedureYRelationship, assignRelation, readRelation };
	
		testExtract(program, expectedEntities, expectedRelationships);

	}
}
