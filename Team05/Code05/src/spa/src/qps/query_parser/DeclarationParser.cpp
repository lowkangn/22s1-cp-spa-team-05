#include <qps/query_parser/DeclarationParser.h>
#include <qps/query_parser/PQLError.h>

unordered_map<string, DesignEntity> DeclarationParser::parse() {
	PQLToken token = tokens.front();
	while (!tokens.empty() && !token.isSelect()) {
		token = tokens.front();
		if (tokens.empty() || token.isSelect()) {
			break;
		}
		parseOneDeclaration();
	}

	if (tokens.empty()) {
		throw PQLError("Reached end of query without a select clause.");
	}

	return declarations;
};

void DeclarationParser::parseOneDeclaration() {
	PQLToken designEntityToken = tokens.front();
	tokens.pop_front();
	PQLToken token = tokens.front();
	bool isSynonymExpected = true;

	while (!tokens.empty()) {
		token = tokens.front();
		tokens.pop_front();
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
		declarations.insert({ synonym,  getDesignEntity(designEntityToken.getTokenString())});
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
	if (declarations.count(synonym) > 0) {
		throw PQLError("Synonym already declared: " + synonym);
	}
};