#include <qps/query_parser/DeclarationParser.h>
#include <qps/query_parser/PQLError.h>


list<Declaration> DeclarationParser::parse() {
	list<Declaration> declarations;
	PQLToken token = tokens.front();
	while (!tokens.empty() && !token.isSelect()) {
		token = tokens.front();
		tokens.pop_front();
		if (tokens.empty() || token.isSelect()) {
			break;
		}
		declarations.splice(declarations.end(), parseOneDeclaration(token));
	}

	if (tokens.empty()) {
		throw PQLError("Reached end of query without a select clause.");
	}

	return declarations;
};

list<Declaration> DeclarationParser::parseOneDeclaration(PQLToken designEntityToken) {
	list<Declaration> result;
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
		result.emplace_back(Declaration(designEntityToken.getTokenString(), synonym));
		declaredSynonyms.insert(synonym);
		isSynonymExpected = false;
	}

	if (isSynonymExpected) {
		throw PQLError("Reached end of declaration with insufficient synonyms");
	}

	if (!token.isSemicolon()) {
		throw PQLError("Declaration must end with semicolon" + token.getTokenString());
	}

	return result;
};

void DeclarationParser::ensureSynonymNotDeclared(string synonym) {
	if (declaredSynonyms.find(synonym) != declaredSynonyms.end()) {
		throw PQLError("Synonym already declared: " + synonym);
	}
};