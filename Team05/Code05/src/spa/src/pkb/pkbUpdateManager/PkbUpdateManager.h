#pragma once


#include <pkb/pkbRepository/PkbRepository.h>
#include <pkb/pkbRepository/design_objects/entities/PkbEntity.h>
#include <pkb/pkbRepository/design_objects/relationships/PkbRelationship.h>

#include <sp/dataclasses/cfg/CFGNode.h>
#include <sp/dataclasses/design_objects/Entity.h>
#include <sp/dataclasses/design_objects/Pattern.h>
#include <sp/dataclasses/design_objects/Relationship.h>

#include <memory>
#include <string>
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
    */
    shared_ptr<PkbEntity> externalEntityToPkbEntity(Entity entity);

    /*
        Converts external relationship object to a PKB relationship.
    */
    shared_ptr<PkbRelationship> externalRelationshipToPkbRelationship(Relationship relationship);


    /*
        Converts external pattern object to a PKB pattern.
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
    void addRelationships(
        vector<Relationship> relationships, shared_ptr<PkbRepository> repository);

    /*
        Updates the repository by reference.
    */
    void addPatterns(vector<Pattern> patterns, shared_ptr<PkbRepository> repository);

    /*
        Updates the repository by reference.
    */
    void addCfgs(vector<shared_ptr<CFGNode>> rootNodes, shared_ptr<PkbRepository> repository);
};
