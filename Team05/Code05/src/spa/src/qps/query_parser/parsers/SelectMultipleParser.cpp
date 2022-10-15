#include <set>
#include <qps/query_parser/parsers/SelectMultipleParser.h>

list<ClauseArgument> SelectMultipleParser::extractArguments() {

	list<ClauseArgument> argumentList;

	// check '<'
	consumeAngledOpenBracket();

	// add first entry (synonym or synonym.attrName)
	this->parseOneEntry(argumentList);

	while (!this->tokens.empty() && !this->tokens.front().isAngledCloseBracket()) {
		// check ','
		consumeComma();

		// add next entry (synonym or synonym.attrName)
		this->parseOneEntry(argumentList);
	}

	// check '>'
	consumeAngledCloseBracket();
	
	return argumentList;
}

void SelectMultipleParser::parseOneEntry(list<ClauseArgument>& argumentListToAddTo) {
	ClauseArgument synonym = this->parseSynonym();
	argumentListToAddTo.push_back(synonym);
	if (!this->tokens.empty() && this->tokens.front().isDot()) {
		this->consumeDot();
		ClauseArgument attribute = this->parseAttribute(synonym);
		argumentListToAddTo.push_back(attribute);
		this->checkSynonymAttributeCompatible(synonym, attribute);
	}
}

shared_ptr<SelectClause> SelectMultipleParser::createClause(list<ClauseArgument>& args) {
	return make_shared<SelectClause>(SelectClause::createSynonymSelectClause(args));
}

bool SelectMultipleParser::isCorrectClauseType(PQLToken clauseTypeToken) {
	return clauseTypeToken.isAngledOpenBracket();
}
