#include <qps/query_parser/parsers/AttributeClauseParser.h>

// ==================== SYNTAX RELATED METHODS ==================== //

ClauseArgument AttributeClauseParser::parseAttribute(ClauseArgument synonym) {
	PQLToken attrNameToken = this->tokens.front();
	this->tokens.pop_front();
	if (attrNameToken.isProcName()) {
		return ClauseArgument::createProcNameAttributeArg(synonym);
	}
	else if (attrNameToken.isVarName()) {
		return ClauseArgument::createVarNameAttributeArg(synonym);
	}
	else if (attrNameToken.isValue()) {
		return ClauseArgument::createValueAttributeArg(synonym);
	}
	else if (attrNameToken.isStmtNumStmt()) {
		if (this->tokens.empty()) {
			throw PQLSyntaxError("Query ended after 'stmt' in with clause");
		}
		attrNameToken = this->tokens.front();
		this->tokens.pop_front();
		if (!attrNameToken.isStmtNumHash()) {
			throw PQLSyntaxError("Expected '#' after 'stmt' in with clause, got: " + attrNameToken.getTokenString());
		}
		return ClauseArgument::createStmtNumAttributeArg(synonym);
	}
	else {
		throw PQLSyntaxError("Unknown attribute name: " + attrNameToken.getTokenString());
	}
}

void AttributeClauseParser::consumeDot() {
	if (this->tokens.empty() || !this->tokens.front().isDot()) {
		throw PQLSyntaxError("Expected '.'");
	}
	this->tokens.pop_front();
	if (this->tokens.empty()) {
		throw PQLSyntaxError("Expected argument after '.'");
	}
}

// ==================== SEMANTICS RELATED METHODS ==================== //

void AttributeClauseParser::checkSynonymAttributeCompatible(ClauseArgument& synonym, ClauseArgument& attribute) {
	assert(synonym.isSynonym() && attribute.isAttributeName());

	bool acceptsProcName = synonym.isProcedureSynonym() || synonym.isCallSynonym();
	bool acceptsVarName = synonym.isVariableSynonym() || synonym.isPrintSynonym() || synonym.isReadSynonym();
	bool acceptsValue = synonym.isConstantSynonym();
	bool acceptsStmtNum = synonym.isStmtRefNoWildcard();

	bool isSemanticallyCorrect = (acceptsProcName && attribute.isProcNameAttribute())
		|| (acceptsVarName && attribute.isVarNameAttribute())
		|| (acceptsValue && attribute.isValueAttribute())
		|| (acceptsStmtNum && attribute.isStmtNumAttribute());
	if (!isSemanticallyCorrect) {
		this->semanticErrorMessage = "synonym and attribute name in attrRef do not match";
	}
}