#pragma once
#include <qps/query/clause/PQLEntity.h>
#include <qps/exceptions/PQLLogicError.h>
#include <string>
using namespace std;

class PQLPattern {

private:
	PQLEntity statementEntity;
	PQLEntity variableEntity;

	PQLPattern(PQLEntity& statementEntity, PQLEntity& variableEntity) : statementEntity(statementEntity), variableEntity(variableEntity) {
		this->statementEntity = statementEntity;
		this->variableEntity = variableEntity;

		if (!statementEntity.isStatement() || !variableEntity.isVariable()) {
			throw PQLLogicError("Trying to create PQLPattern, but LHS is not statement, or RHS is not variable!");
		}
	}

public: 
	/*
		Creates an assign pattern.
	*/
	static PQLPattern generateAssignPattern(int statementLineNumber, string variableName) {
		PQLEntity stmtEntity = PQLEntity::generateStatement(statementLineNumber);
		PQLEntity varEntity = PQLEntity::generateVariable(variableName);
		return PQLPattern(stmtEntity, varEntity);
	}
	
	friend bool operator==(PQLPattern first, PQLPattern second) {
		return first.statementEntity == second.statementEntity && first.variableEntity == second.variableEntity;
	}

	string toString() {
		return this->statementEntity.toString() + this->variableEntity.toString();
	}

	
	

	PQLEntity getStatementEntity() {
		return this->statementEntity;
	}

	PQLEntity getVariableEntity() {
		return this->variableEntity;
	}

};