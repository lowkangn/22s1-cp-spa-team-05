#include <qps/query_parser/parsers/DeclarationParser.h>
#include <qps/query_parser/PQLError.h>

unordered_map<string, DesignEntity> DeclarationParser::parse() {
	PQLToken token = this->tokens.front();
	while (!this->tokens.empty() && !token.isSelect()) {
		token = this->tokens.front();
		if (this->tokens.empty() || token.isSelect()) {
			break;
		}
		parseOneDeclaration();
	}

	if (this->tokens.empty()) {
		throw PQLError("Reached end of query without a select clause.");
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
			throw PQLError("Expected synonym name, got: " + token.getTokenString());
		} else if (token.isComma()) {
			isSynonymExpected = true;
			continue;
		} else if (token.isSemicolon()) {
			break;
		} 
		string synonym = token.getTokenString();
		ensureSynonymNotDeclared(synonym);
		this->declarations.insert({ synonym,  getDesignEntity(designEntityToken.getTokenString())});
		isSynonymExpected = false;
	}

	if (isSynonymExpected) {
		throw PQLError("Reached end of declaration with insufficient synonyms");
	}

	if (!token.isSemicolon()) {
		throw PQLError("Declaration must end with semicolon, not " + token.getTokenString());
	}
};

void DeclarationParser::ensureSynonymNotDeclared(string synonym) {
	if (this->declarations.count(synonym) > 0) {
		throw PQLError("Synonym already declared: " + synonym);
	}
};