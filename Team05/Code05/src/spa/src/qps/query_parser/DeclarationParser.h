#pragma once

#include <unordered_set>

#include <qps/query_parser/Declaration.h>
#include <qps/query_parser/PQLToken.h>

class DeclarationParser {
private:
	list<PQLToken> tokens;
	unordered_set<string> declaredSynonyms;
public:
	DeclarationParser(list<PQLToken> tokens) {
		this->tokens = tokens;
	};

	list<PQLToken> getRemainingTokens() {
		return tokens; 
	};

	unordered_set<string> getDeclaredSynonyms() {
		return declaredSynonyms;
	};

	/* Parses all declarations in the query. */
	list<Declaration> parse();

	/**
	 * Parses one declaration clause, defined
	 * as "design-entity synonym (',' synonym)* ';'"
	 * 
	 * Precondition: this->Tokens is non-empty
	 */
	list<Declaration> parseOneDeclaration(PQLToken designEntityToken);

	void ensureSynonymNotDeclared(string synonym);
};