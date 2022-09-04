#pragma once

#include <unordered_set>

#include <qps/query_parser/Declaration.h>
#include <qps/query_parser/PQLToken.h>

class DeclarationParser {
private:
	list<PQLToken> tokens;
	unordered_map<string, Declaration> declarations;
public:
	DeclarationParser(list<PQLToken> tokens) {
		this->tokens = tokens;
	};

	unordered_map<string, Declaration> getDeclarations() {
		return declarations;
	};

	list<PQLToken> getRemainingTokens() {
		return tokens;
	};

	/* Parses all declarations in the query. */
	unordered_map<string, Declaration> parse();

	/**
	 * Parses one declaration clause, defined
	 * as "design-entity synonym (',' synonym)* ';'"
	 *
	 * Preconditions: this->Tokens is non-empty
	 */
	void parseOneDeclaration();

	void ensureSynonymNotDeclared(string synonym);
};