#pragma once

#include <pkb/interfaces/PKBQueryHandler.h>
#include <pkb/interfaces/PKBUpdateHandler.h>
#include <pkb/pkbQueryManager/PkbQueryManager.h>
#include <pkb/pkbRepository/PkbRepository.h>
#include <pkb/pkbRepository/design_objects/entities/PkbStatementEntity.h>
#include <pkb/pkbRepository/graph_managers/PkbGraphManager.h>
#include <pkb/pkbRepository/table_managers/PkbEntityTable.h>
#include <pkb/pkbRepository/table_managers/PkbPatternTable.h>
#include <pkb/pkbRepository/table_managers/PkbRelationshipTable.h>
#include <pkb/pkbUpdateManager/PkbUpdateManager.h>

#include <qps/query/clause/ClauseArgument.h>
#include <qps/query/clause/PQLEntity.h>
#include <qps/query/clause/PQLRelationship.h>

#include <sp/dataclasses/design_objects/Entity.h>
#include <sp/dataclasses/design_objects/Pattern.h>
#include <sp/dataclasses/design_objects/Relationship.h>

#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

class PKB : public PKBQueryHandler, public PKBUpdateHandler {
private:
    // ======================== attributes ==============================
    shared_ptr<PkbRepository> repository = make_shared<PkbRepository>();
    PkbUpdateManager updateManager;
    PkbQueryManager queryManager;
    bool optimized = false;

public:
    explicit PKB(bool optimized = false) {
        this->optimized = optimized;
    }

    // ============================== Update handler API ==============================

    /*
        Add extracted relationships from the SP.
    */
    void addRelationships(vector<Relationship> relationships) override;

    /*
        Add extracted patterns from the SP.
    */
    void addPatterns(vector<Pattern> patterns) override;

    /*
        Add extracted entities from the SP.
    */
    void addEntities(vector<Entity> entities) override;

    /*
        Add the control flow graphs from the SP.
    */
    void addCfgs(vector<shared_ptr<CFGNode>> rootNodes) override;

    // ============================== Retrieve handler API ==============================

    // ******************** ALL ********************

    /*
        Retrieves all procedure entities.
    */
    vector<PQLEntity> retrieveAllProcedureEntities() override;

    /*
        Retrieves all statement entities, regardless of type.
    */
    vector<PQLEntity> retrieveAllStatementEntities() override;

    /*
        Retrieves all variables.
    */
    vector<PQLEntity> retrieveAllVariables() override;

    /*
        Retrieves all constants.
    */
    vector<PQLEntity> retrieveAllConstants() override;

    // ******************** BY TYPE ********************

    /*
        Retrieves all statement entities of a specified type.
    */
    vector<PQLEntity> retrieveStatementEntitiesByType(
        PKBTrackedStatementType pkbTrackedStatementType) override;

    // ******************** BY IDENTIFIER ********************

    /*
        Retrieves all statement entities by line number of a specified type.
    */
    optional<PQLEntity> retrieveStatementByLineNumberAndType(
        int lineNumber,
        PKBTrackedStatementType pkbTrackedStatementType) override;

    /*
        Retrieves all procedure entities by name.
    */
    optional<PQLEntity> retrieveProcedureEntityByName(string procedureName) override;


    /*
        Retrieves all variables by a name.
    */
    optional<PQLEntity> retrieveVariableByName(string name) override;

    /*
        Retrieves all constants by a value.
    */
    optional<PQLEntity> retrieveConstantByValue(int value) override;

    // ******************** BY DETAIL ********************

    /*
        Retrieves all relationships by a lhs, rhs for relationships of a specified type.
    */
    vector<PQLRelationship> retrieveRelationshipByTypeAndLhsRhs(
        PKBTrackedRelationshipType relationshipType,
        ClauseArgument lhs,
        ClauseArgument rhs) override;


    /*
        Retrieves statements by lhs and rhs for Assign Patterns
    */
    vector<PQLPattern> retrievePatterns(
        PKBTrackedStatementType statementType,
        ClauseArgument lhs,
        ClauseArgument rhs) override;

    // ============================== State management ==============================
    void resetCache() {
        this->repository->resetCache();
    }
};
