#pragma once

#include <unordered_map>
#include <list>

#include <qps/query_parser/ArgumentType.h>
#include <qps/query_tokenizer/PQLToken.h>

class DeclarationParser {
private:
	list<PQLToken> tokens;
	unordered_map<string, ArgumentType> declarations;
public:
	DeclarationParser(list<PQLToken> tokens) {
		this->tokens = tokens;
	};

	unordered_map<string, ArgumentType> getDeclarations() {
		return this->declarations;
	};

	list<PQLToken> getRemainingTokens() {
		return this->tokens;
	};

	/* Parses all declarations in the query. */
	unordered_map<string, ArgumentType> parse();

	/**
	 * Parses one declaration clause, defined
	 * as "design-entity synonym (',' synonym)* ';'"
	 *
	 * Preconditions: this->Tokens is non-empty
	 */
	void parseOneDeclaration();

	void ensureSynonymNotDeclared(string synonym);
};