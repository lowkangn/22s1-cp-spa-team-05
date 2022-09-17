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
		ensureSynonymNotDeclared(synonym);
		this->declarations.insert({ synonym,  getDesignEntityArgumentType(designEntityToken.getTokenString())});
		isSynonymExpected = false;
	}

	if (isSynonymExpected) {
		throw PQLSyntaxError("Reached end of declaration with insufficient synonyms");
	}

	if (!token.isSemicolon()) {
		throw PQLSyntaxError("Declaration must end with semicolon, not " + token.getTokenString());
	}
};

void DeclarationParser::ensureSynonymNotDeclared(string synonym) {
	if (this->declarations.count(synonym) > 0) {
		throw PQLSemanticError("Synonym already declared: " + synonym);
	}
};