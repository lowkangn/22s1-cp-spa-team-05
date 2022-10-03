#include <qps/query_parser/parsers/DeclarationParser.h>

unordered_map<string, ArgumentType> DeclarationParser::parse() {
	PQLToken token = this->tokens.front();
	while (!this->tokens.empty() && !token.isSelect()) {
		token = this->tokens.front();
		if (this->tokens.empty() || token.isSelect()) {
			break;
		}
		parseOneDeclaration();
	}

	if (this->tokens.empty()) {
		throw PQLSyntaxError("Reached end of query without a select clause.");
	}

	return this->declarations;
};

void DeclarationParser::parseOneDeclaration() {
	PQLToken designEntityToken = this->tokens.front();
	this->tokens.pop_front();
	ArgumentType argType = getDesignEntityArgumentType(designEntityToken.getTokenString());

	PQLToken token = this->tokens.front();
	bool isSynonymExpected = true;

	while (!this->tokens.empty()) {
		token = this->tokens.front();
		this->tokens.pop_front();
		if (isSynonymExpected && !token.isName()) {
			throw PQLSyntaxError("Expected synonym name, got: " + token.getTokenString());
		} else if (token.isComma()) {
			isSynonymExpected = true;
			continue;
		} else if (token.isSemicolon()) {
			break;
		} else if (!isSynonymExpected) {
			throw PQLSyntaxError("Expected comma or semicolon, got: " + token.getTokenString());
		}
		string synonym = token.getTokenString();
		isSynonymExpected = false;
		if (isSynonymAlreadyDeclared(synonym)) {
			continue;
		}
		this->declarations.insert({ synonym, argType});
	}

	if (isSynonymExpected) {
		throw PQLSyntaxError("Reached end of declaration with insufficient synonyms");
	}

	if (!token.isSemicolon()) {
		throw PQLSyntaxError("Declaration must end with semicolon, not " + token.getTokenString());
	}
};

bool DeclarationParser::isSynonymAlreadyDeclared(string synonym) {
	if (this->declarations.count(synonym) > 0) {
		this->semanticErrorMessage = "Synonym already declared: " + synonym;
		return true;
	}
	return false;
};