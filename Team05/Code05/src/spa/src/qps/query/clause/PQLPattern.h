#pragma once
#include <qps/query/clause/PQLEntity.h>
#include <qps/exceptions/PQLError.h>
#include <string>
using namespace std;

class PQLPattern {

private:
	PQLEntity statementEntity;
	PQLEntity variableEntity;

	PQLPattern(PQLEntity statementEntity, PQLEntity variableEntity) : statementEntity(statementEntity), variableEntity(variableEntity) {
		this->statementEntity = statementEntity;
		this->variableEntity = variableEntity;

		if (!statementEntity.isStatement() || !variableEntity.isVariable()) {
			throw PQLError("Trying to create PQLPattern, but LHS is not statement, or RHS is not variable!");
		}
	}

public:
	/*
	  Creates an assign pattern.
	*/
	static PQLPattern generateAssignPattern(int statementLineNumber, string variableName) {
		return PQLPattern(PQLEntity::generateStatement(statementLineNumber), PQLEntity::generateVariable(variableName));
	}

	PQLEntity getStatementEntity() {
		return this->statementEntity;
	}

	PQLEntity getVariableEntity() {
		return this->variableEntity;
	}


};