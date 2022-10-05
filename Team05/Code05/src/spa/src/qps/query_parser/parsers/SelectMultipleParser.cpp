#include <set>
#include <qps/query_parser/parsers/SelectMultipleParser.h>

list<ClauseArgument> SelectMultipleParser::extractArguments() {

	list<ClauseArgument> argumentList;

	// check '<'
	consumeAngledOpenBracket();

	// get first synonym
	ClauseArgument firstArg = parseOneArgument();
	if (!firstArg.isSynonym()) {
		throw PQLSyntaxError("Select tuple should only have synonyms");
	}
	argumentList.push_back(firstArg);

	while (!this->tokens.empty() && !this->tokens.front().isAngledCloseBracket()) {
		// check ','
		consumeComma();

		// get next synonym
		ClauseArgument nextArg = parseOneArgument();
		if (!nextArg.isSynonym()) {
			throw PQLSyntaxError("Select tuple should only have synonyms");
		}
		argumentList.push_back(nextArg);
	}

	// check '>'
	consumeAngledCloseBracket();

	return argumentList;
}

shared_ptr<SelectClause> SelectMultipleParser::createClause(list<ClauseArgument>& args) {
	return make_shared<SelectClause>(SelectClause::createSynonymSelectClause(args));
}

bool SelectMultipleParser::isCorrectClauseType(PQLToken clauseTypeToken) {
	return clauseTypeToken.isAngledOpenBracket();
}
