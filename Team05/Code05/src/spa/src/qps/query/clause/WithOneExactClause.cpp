#include <qps/query/clause/WithOneExactClause.h>

shared_ptr<ClauseResult> WithOneExactClause::execute(shared_ptr<PKBQueryHandler> pkb) {
    ClauseArgument synonym = this->nonExactArgs.front();
    ClauseArgument attribute = this->nonExactArgs.back();
    assert(synonym.isSynonym() && attribute.isAttributeName() && exactArg.isExactReference());

    optional<PQLEntity> maybeEntity;
    if (synonym.isProcedureSynonym()) {
        maybeEntity = pkb->retrieveProcedureEntityByName(exactArg.getIdentifier());
    } else if (synonym.isVariableSynonym()) {
        maybeEntity = pkb->retrieveVariableByName(exactArg.getIdentifier());
    } else if (synonym.isConstantSynonym()) {
        maybeEntity = pkb->retrieveConstantByValue(exactArg.getLineNumber());
    } else if (synonym.isStmtRefNoWildcard() && attribute.isStmtNumAttribute()) {
        PKBTrackedStatementType stmtType = this->getPKBStmtType(synonym);
        maybeEntity = pkb->retrieveStatementByLineNumberAndType(
            exactArg.getLineNumber(),
            stmtType);
    } else if (synonym.isStmtRefNoWildcard() && (attribute.isProcNameAttribute() || attribute.
        isVarNameAttribute())) {
        return this->executeForStmtWithNameAttribute(pkb);
    } else {
        throw PQLLogicError("Unable to identify synonym type in with clause");
    }

    vector<PQLEntity> resultEntityVector = {};
    // return ClauseResult empty if no entity was retrieved
    if (!maybeEntity.has_value()) {
        return make_shared<EntityClauseResult>(synonym, resultEntityVector);
    }
    // else return the retrieved entity
    resultEntityVector.push_back(maybeEntity.value());
    return make_shared<EntityClauseResult>(synonym, resultEntityVector);
}

shared_ptr<ClauseResult> WithOneExactClause::executeForStmtWithNameAttribute(
    shared_ptr<PKBQueryHandler> pkb) {
    ClauseArgument synonym = this->nonExactArgs.front();
    ClauseArgument attribute = this->nonExactArgs.back();

    vector<PQLRelationship> relationships;
    if (synonym.isPrintSynonym()) {
        relationships = pkb->retrieveRelationshipByTypeAndLhsRhs(
            PKBTrackedRelationshipType::USES,
            synonym,
            this->exactArg);
    } else if (synonym.isReadSynonym()) {
        relationships = pkb->retrieveRelationshipByTypeAndLhsRhs(
            PKBTrackedRelationshipType::MODIFIES,
            synonym,
            this->exactArg);
    } else if (synonym.isCallSynonym()) {
        relationships = pkb->retrieveRelationshipByTypeAndLhsRhs(
            PKBTrackedRelationshipType::CALLSTMTATTRIBUTE,
            synonym,
            this->exactArg);
    } else {
        throw PQLLogicError(
            "For statements, only print, read and call synonyms can have a name attribute.");
    }
    return make_shared<RelationshipClauseResult>(synonym, attribute, relationships);
}

bool WithOneExactClause::equals(shared_ptr<WithClause> other) {
    if (dynamic_pointer_cast<WithOneExactClause>(other) == nullptr) {
        return false;
    }
    shared_ptr<WithOneExactClause> otherWithEntity = dynamic_pointer_cast<WithOneExactClause>(
        other);
    return (this->exactArg == otherWithEntity->exactArg) && (this->nonExactArgs == otherWithEntity
        ->nonExactArgs);
}
