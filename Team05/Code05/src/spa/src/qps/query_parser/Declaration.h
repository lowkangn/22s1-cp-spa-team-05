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

	/**
	 * Tests if another declaration is equal to this declaration
	 * @param otherDeclaration to test.
	 * @return true if Declarations are equal.
	 */
	bool equals(Declaration otherDeclaration) {
		return designEntity == otherDeclaration.getDesignEntity() 
			&& synonym == otherDeclaration.getSynonym();
	}
};