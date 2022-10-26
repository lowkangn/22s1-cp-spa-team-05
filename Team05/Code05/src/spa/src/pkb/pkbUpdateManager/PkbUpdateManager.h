#pragma once


#include <sp/dataclasses/design_objects/Entity.h>
#include <sp/dataclasses/design_objects/Relationship.h>
#include <sp/dataclasses/design_objects/Pattern.h>
#include <sp/dataclasses/cfg/CFGNode.h>

#include <pkb/pkbRepository/design_objects/entities/PkbEntity.h>
#include <pkb/pkbRepository/design_objects/relationships/PkbRelationship.h>
#include <pkb/pkbRepository/PkbRepository.h>

#include <memory>
#include <vector>

const string EXTERNAL_PATTERN_DELIMITER = " ";


using namespace std;

/*
	Handles all preprocessing logic before storage and forwarding to the pkbrepository.
*/
class PkbUpdateManager {
	
private:
	
	// ==================== helper functions ====================
	/*
		Converts an SP entity to a pkb entity with the correct underlying behaviour.
		TODO: consider refactoring this. For now, we have a dependency on an external type from the SP. this was done
			for speed of development, and also the two-way coupling breaks abstraction in a minimal and arguably trivial way.
			Better practice would be to have an external contract dataclass, SP convert to that and pass into the PKB.
			Because it's currently not necessary, we don't do that.
	*/
	shared_ptr<PkbEntity> externalEntityToPkbEntity(Entity entity);

	/*
		Converts external relationship object to a PKB relationship.
		TODO: consider refactoring this. For now, we have a dependency on an external type from the SP. this was done
			for speed of development, and also the two-way coupling breaks abstraction in a minimal and arguably trivial way.
			Better practice would be to have an external contract dataclass, SP convert to that and pass into the PKB.
			Because it's currently not necessary, we don't do that.
	*/
	shared_ptr<PkbRelationship> externalRelationshipToPkbRelationship(Relationship relationship);


	/*
		Converts external pattern object to a PKB pattern.
		TODO: consider refactoring this. For now, we have a dependency on an external type from the SP. this was done
			for speed of development, and also the two-way coupling breaks abstraction in a minimal and arguably trivial way.
			Better practice would be to have an external contract dataclass, SP convert to that and pass into the PKB.
			Because it's currently not necessary, we don't do that.
	*/
	vector<shared_ptr<PkbPattern>> externalPatternToPkbPattern(Pattern pattern);

public: 
	/*
		Updates the repository by reference.
	*/
	void addEntities(vector<Entity> entities, shared_ptr<PkbRepository> repository);

	/*
		Updates the repository by reference.
	*/
	void addRelationships(vector<Relationship> relationships, shared_ptr<PkbRepository> repository);

	/*
		Updates the repository by reference.
	*/
	void addPatterns(vector<Pattern> patterns, shared_ptr<PkbRepository> repository);

	/*
		Updates the repository by reference.
	*/
	void addCfg(shared_ptr<CFGNode> rootNode, shared_ptr<PkbRepository> repository);

};