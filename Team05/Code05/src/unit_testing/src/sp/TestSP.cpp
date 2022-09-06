#include "catch.hpp"
#include <sp/SourceProcessor.h>
#include <istream>
#include <sstream>
#include <pkb/interfaces/PKBUpdateHandler.h>
#include <pkb/design_objects/PkbEntity.h>
#include <pkb/design_objects/PkbRelationship.h>
#include <pkb/pkb_object/PKB.h>
#include <memory>


using namespace std;
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