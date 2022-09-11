#pragma once
#include <string>
#include <vector>
#include "../../qps/query/clause/ClauseArgument.h"
#include "../../qps/query/clause/ClauseResult.h"
#include "../../qps/query/clause/PQLEntity.h"
#include "../../qps/query/clause/PQLRelationship.h"

enum class PKBTrackedStatementType {
    ALL,
    STATEMENT,
    READ,
    PRINT,
    WHILE,
    IF,
    ASSIGN,
};

enum class PKBTrackedRelationshipType {
    Follows,
    FollowsStar,
    Parent,
    ParentStar,
    Uses,
    UsesStar,
    Modifies,
    ModifieStar,
};


/*
 * Interface used by QPS to retrieve data from PKB
 */
class PKBQueryHandler {
public:
    PKBQueryHandler() {};

    // ===== procedure =====

    /*
        Retrieves procedure entities by name. 

        NOTE: we should refactor such that it does not return a PQLEntity, but instead a ProcedurePQLEntity for safety.
        We can always up cast as needed, but down casting is bad practice.
    */
    vector<PQLEntity> retrieveProcedureEntitiesByName(string procedureName);

    /*
        Retrieves all procedure entities. 
        
        NOTE: we should refactor such that it does not return a PQLEntity, but instead a ProcedurePQLEntity for safety.
        We can always up cast as needed, but down casting is bad practice.
    */
    vector<PQLEntity> retrieveAllProcedureEntities();

    // ===== statements and specific statement types =====

    /*
        Retrieves statement entities by line number specified. A specific request type, defined 
        here (rather than in the QPS) is so that the API constract is well defined on the PKB side.
        This reduces unnecessary coupling between the two systems. 

        Note that we parametrize both line number and type so that if you're looking for 
        a specific type constrained to line number, it's much easier, than making a query by 
        type and then filtering on your own.
    */
    vector<PQLEntity> retrieveStatementEntitiesByLineNumber(int lineNumber, PKBTrackedStatementType pkbTrackedStatementType);

    /*
        Retrieves statement entities of a specific type.
    */
    vector<PQLEntity> retrieveStatementEntitiesByType(PKBTrackedStatementType pkbTrackedStatementType);
    /*
        Retrieves all statement entities.
    */
    vector<PQLEntity> retrieveAllStatementEntities();

    // ===== Variables and constants =====
    /*
        Retrieves all stored constants.
    */
    vector<PQLEntity> retrieveAllConstants();

    /*
        Retrieves all stored variables.
    */
    vector<PQLEntity> retrieveAllVariables();

    /*
        Retrieves variables by name.
    */
    vector<PQLEntity> retrieveVariablesByName(string name);


    // ===== Relationships =====
    /*
        Retrieves all relationships of a specified supported type and a lhs and rhs. 
    */
    vector<PQLRelationship> retrieveRelationshipByTypeAndLhsRhs(PKBTrackedRelationshipType relationshipType, ClauseArgument lhs, ClauseArgument rhs);
    
    /*
        Retrieves all relationships of a specified type.
    */
    vector<PQLRelationship> retrieveRelationshipByType(PKBTrackedRelationshipType relationshipType);

    
};
