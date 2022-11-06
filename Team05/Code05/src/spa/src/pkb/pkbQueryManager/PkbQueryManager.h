#pragma once

#include <assert.h>

#include <pkb/interfaces/PKBQueryHandler.h>
#include <pkb/pkbQueryManager/PkbEntityQueryHelper.h>
#include <pkb/pkbQueryManager/PkbPatternQueryHelper.h>
#include <pkb/pkbQueryManager/PkbRelationshipQueryHelper.h>
#include <pkb/pkbRepository/PkbRepository.h>
#include <pkb/pkbRepository/design_objects/entities/PkbProcedureEntity.h>
#include <pkb/pkbRepository/design_objects/entities/PkbStatementEntity.h>

#include <qps/query/clause/ClauseArgument.h>
#include <qps/query/clause/PQLEntity.h>
#include <qps/query/clause/PQLPattern.h>
#include <qps/query/clause/PQLRelationship.h>

#include <memory>
#include <string>
#include <vector>

using namespace std;

class PkbQueryManager {
private:
    // ==================== attributes ====================
    PkbEntityQueryHelper entityHelper;
    PkbRelationshipQueryHelper relationshipHelper;
    PkbPatternQueryHelper patternHelper;


    // ==================== helper functions ====================

    // ******************** conversion ********************
    /*
        Converts an internal Pkb entity to a pql entity used in the qps.
    */
    PQLEntity pkbEntityToQpsPqlEntity(shared_ptr<PkbEntity> entity) {
        // based on entity type, use pqlentity api
        if (entity->isProcedure()) {
            return PQLEntity::generateProcedure(entity->getIdentifier());
        } else if (entity->isVariable()) {
            return PQLEntity::generateVariable(entity->getIdentifier());
        } else if (entity->isStatement()) {
            return PQLEntity::generateStatement(entity->getLineNumber());
        } else if (entity->isConstant()) {
            return PQLEntity::generateConstant(entity->getValue());
        } else {
            throw PkbException("Unknown PkbEntity type being passed to QPS!");
        }
    }

    /*
        Maps the external facing type to an internal corresponding type.
    */
    PkbRelationshipType pkbTrackedRelationshipTypeToInternalType(PKBTrackedRelationshipType t) {
        switch (t) {
        case PKBTrackedRelationshipType::NEXTSTAR:
            return PkbRelationshipType::NEXTSTAR;
        case PKBTrackedRelationshipType::AFFECTS:
            return PkbRelationshipType::AFFECTS;
        case PKBTrackedRelationshipType::AFFECTSSTAR:
            return PkbRelationshipType::AFFECTSSTAR;
        case PKBTrackedRelationshipType::NEXT:
            return PkbRelationshipType::NEXT;
        case PKBTrackedRelationshipType::FOLLOWS:
            return PkbRelationshipType::FOLLOWS;
        case PKBTrackedRelationshipType::FOLLOWSSTAR:
            return PkbRelationshipType::FOLLOWSSTAR;
        case PKBTrackedRelationshipType::MODIFIES:
            return PkbRelationshipType::MODIFIES;
        case PKBTrackedRelationshipType::PARENT:
            return PkbRelationshipType::PARENT;
        case PKBTrackedRelationshipType::PARENTSTAR:
            return PkbRelationshipType::PARENTSTAR;
        case PKBTrackedRelationshipType::USES:
            return PkbRelationshipType::USES;
        case PKBTrackedRelationshipType::CALLS:
            return PkbRelationshipType::CALLS;
        case PKBTrackedRelationshipType::CALLSSTAR:
            return PkbRelationshipType::CALLSSTAR;
        case PKBTrackedRelationshipType::CALLSTMTATTRIBUTE:
            return PkbRelationshipType::CALLSTMTATTRIBUTE;
        default:
            throw PkbException("Type cannot be mapped! e.g. ALL");
        }
    }

    /*
        Maps the external facing type to an internal corresponding type.
    */
    PkbStatementType pkbTrackedStatmentTypeToInternalType(PKBTrackedStatementType t) {
        switch (t) {
        case PKBTrackedStatementType::ASSIGN:
            return PkbStatementType::ASSIGN;
        case PKBTrackedStatementType::CALL:
            return PkbStatementType::CALL;
        case PKBTrackedStatementType::IF:
            return PkbStatementType::IF;
        case PKBTrackedStatementType::WHILE:
            return PkbStatementType::WHILE;
        case PKBTrackedStatementType::PRINT:
            return PkbStatementType::PRINT;
        case PKBTrackedStatementType::READ:
            return PkbStatementType::READ;
        default:
            throw PkbException("Type cannot be mapped! e.g. ALL");
        }
    }

    /*
        Converts an internal pkb pattern to a pql pattern used in the qps.
    */
    PQLPattern pkbPatternToPqlPattern(shared_ptr<PkbPattern> pattern);

public:
    // ******************** retrieval ********************
    // -------------------- all --------------------
    /*
        Retrieves all procedure entities.
    */
    vector<PQLEntity> retrieveAllProcedureEntities(shared_ptr<PkbRepository> repository);

    /*
        Retrieves all statement entities, regardless of type.
    */
    vector<PQLEntity> retrieveAllStatementEntities(shared_ptr<PkbRepository> repository);

    /*
        Retrieves all variables.
    */
    vector<PQLEntity> retrieveAllVariables(shared_ptr<PkbRepository> repository);

    /*
        Retrieves all constants.
    */
    vector<PQLEntity> retrieveAllConstants(shared_ptr<PkbRepository> repository);

    // -------------------- by type --------------------
    /*
        Retrieves all statement entities of a specified type.
    */
    vector<PQLEntity> retrieveStatementEntitiesByType(
        PKBTrackedStatementType pkbTrackedStatementType, shared_ptr<PkbRepository> repository);

    // -------------------- by identifier --------------------
    /*
        Retrieves all statement entities by line number of a specified type.
    */
    optional<PQLEntity> retrieveStatementByLineNumberAndType(
        int lineNumber, PKBTrackedStatementType pkbTrackedStatementType,
        shared_ptr<PkbRepository> repository);


    /*
        Retrieves all procedure entities by name.
    */
    optional<PQLEntity> retrieveProcedureEntityByName(
        string procedureName, shared_ptr<PkbRepository> repository);


    /*
        Retrieves all variables by a name.
    */
    optional<PQLEntity> retrieveVariableByName(string name, shared_ptr<PkbRepository> repository);

    /*
        Retrieves all constants by a value.
    */
    optional<PQLEntity> retrieveConstantByValue(int value, shared_ptr<PkbRepository> repository);

    // -------------------- specifics --------------------
    /*
        Retrieves all relationships by a lhs, rhs for relationships of a specified type.
    */
    vector<PQLRelationship> retrieveRelationshipByTypeAndLhsRhs(
        PKBTrackedRelationshipType relationshipType,
        ClauseArgument lhs,
        ClauseArgument rhs,
        shared_ptr<PkbRepository> repository,
        bool optimized);

    /*
        Retrieves statements by lhs and rhs for Assign, If or While Patterns
    */
    vector<PQLPattern> retrievePatterns(
        PKBTrackedStatementType statementType, ClauseArgument lhs, ClauseArgument rhs,
        shared_ptr<PkbRepository> repository);
};
