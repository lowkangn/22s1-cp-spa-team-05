#pragma once
#include <string>
#include <vector>
#include <qps/query/clause/ClauseArgument.h>
#include <qps/query/clause/PQLEntity.h>
#include <qps/query/clause/PQLRelationship.h>
#include <qps/query/clause/PQLPattern.h>

enum class PKBTrackedStatementType {
    ALL,
    READ,
    PRINT,
    WHILE,
    IF,
    ASSIGN,
    CALL,
};

enum class PKBTrackedRelationshipType {
    FOLLOWS,
    FOLLOWSSTAR,
    PARENT,
    PARENTSTAR,
    USES,
    MODIFIES,
};


/*
 * Interface used by QPS to retrieve data from PKB
 */
class PKBQueryHandler {
public:

    // ===== procedure =====

    /*
        Retrieves procedure entities by name. 

        NOTE: we should refactor such that it does not return a PQLEntity, but instead a ProcedurePQLEntity for safety.
        We can always up cast as needed, but down casting is bad practice.
    */
    virtual PQLEntity retrieveProcedureEntityByName(string procedureName) = 0;

    /*
        Retrieves all procedure entities. 
        
        NOTE: we should refactor such that it does not return a PQLEntity, but instead a ProcedurePQLEntity for safety.
        We can always up cast as needed, but down casting is bad practice.
    */
    virtual vector<PQLEntity> retrieveAllProcedureEntities() = 0;

    // ===== statements and specific statement types =====

    /*
        Retrieves statement entities by line number specified. A specific request type, defined 
        here (rather than in the QPS) is so that the API constract is well defined on the PKB side.
        This reduces unnecessary coupling between the two systems. 

        Note that we parametrize both line number and type so that if you're looking for 
        a specific type constrained to line number, it's much easier, than making a query by 
        type and then filtering on your own.
    */
    virtual PQLEntity retrieveStatementEntityByLineNumber(int lineNumber, PKBTrackedStatementType pkbTrackedStatementType) = 0;

    /*
        Retrieves statement entities of a specific type.
    */
    virtual vector<PQLEntity> retrieveStatementEntitiesByType(PKBTrackedStatementType pkbTrackedStatementType) = 0;
    /*
        Retrieves all statement entities.
    */
    virtual vector<PQLEntity> retrieveAllStatementEntities() = 0;

    // ===== Variables and constants =====
    /*
        Retrieves all stored variables.
    */
    virtual vector<PQLEntity> retrieveAllVariables() = 0;

    /*
        Retrieves variables by name.
    */
    virtual PQLEntity retrieveVariableByName(string name) = 0;

    /*
        Retrieves all constants.
    */
    virtual vector<PQLEntity> retrieveAllConstants() = 0;


    // ===== Relationships =====
    /*
        Retrieves all relationships of a specified supported type and a lhs and rhs. 
    */
    virtual vector<PQLRelationship> retrieveRelationshipByTypeAndLhsRhs(PKBTrackedRelationshipType relationshipType, ClauseArgument lhs, ClauseArgument rhs) = 0;
    
    /*
        Retrieves all relationships of a specified type.
    */
    virtual vector<PQLRelationship> retrieveRelationshipsByType(PKBTrackedRelationshipType relationshipType) = 0;
    
    // ===== Pattern =====

	/*
        Retrieves statements by lhs and rhs. If a lhs of type synonym is provided,
    */
	virtual vector<PQLPattern> retrievePatterns(PKBTrackedStatementType statementType, ClauseArgument lhs, ClauseArgument rhs) = 0;
    
};
