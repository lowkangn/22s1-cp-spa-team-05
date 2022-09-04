#pragma once

#include <string>
#include <unordered_map>

#include <qps/query_parser/PQLToken.h>

using namespace std;

enum class DesignEntity {
	PROCEDURE,
	STMT,
	READ,
	PRINT,
	ASSIGN,
	CALL,
	WHILE,
	IF,
	VARIABLE,
	CONSTANT
};

class Declaration {
private:
	/* The design entity being declared. */
	DesignEntity designEntity;

	/* The synonym being declared. */
	string synonym;
public:
	Declaration(string designEntity, string synonym);

	/**
	 * Returns the design entity of the declaration.
	 *
	 * @return design entity of the declaration.
	 */
	DesignEntity getDesignEntity() {
		return designEntity;
	}

	/**
	 * Returns the synonym of the declaration.
	 *
	 * @return synonym of the declaration.
	 */
	string getSynonym() {
		return synonym;
	}


	/* Returns true if the declaration is a statement synonym */
	bool isStatement() {
		return  designEntity == DesignEntity::STMT || designEntity == DesignEntity::READ
			|| designEntity == DesignEntity::PRINT || designEntity == DesignEntity::ASSIGN
			|| designEntity == DesignEntity::CALL || designEntity == DesignEntity::WHILE
			|| designEntity == DesignEntity::IF;
	}

	/* Returns true if the declaration is an assign synonym */
	bool isAssign() {
		return  designEntity == DesignEntity::ASSIGN;
	}

	/* Returns true if the declaration is a procedure synonym */
	bool isProcedure() {
		return  designEntity == DesignEntity::PROCEDURE;
	}

	/* Returns true if the declaration is a variable synonym */
	bool isVariable() {
		return  designEntity == DesignEntity::VARIABLE;
	}

	/* Returns true if the declaration is a constant synonym */
	bool isConstant() {
		return  designEntity == DesignEntity::CONSTANT;
	}
};

bool operator== (Declaration firstDeclaration, Declaration otherDeclaration);