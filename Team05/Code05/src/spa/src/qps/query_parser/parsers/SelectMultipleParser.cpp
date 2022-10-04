#include <set>
#include <qps/query_parser/parsers/SelectMultipleParser.h>

list<ClauseArgument> SelectMultipleParser::extractArguments() {

	set<ClauseArgument> argumentSet;

	// check '<'
	consumeAngledOpenBracket();

	// get first synonym
	ClauseArgument firstArg = parseSynonym();
	argumentSet.insert(firstArg);

	while (!this->tokens.front().isAngledCloseBracket()) {
		// check ','
		consumeComma();

		// get next synonym
		ClauseArgument nextArg = parseSynonym();
		if (argumentSet.find(nextArg) != argumentSet.end()) {
			throw PQLSyntaxError("Cannot have duplicate synonyms in Select tuple");
		}
		argumentSet.insert(nextArg);
	}

	// check '>'
	consumeAngledCloseBracket();

	// convert to list
	list<ClauseArgument> argumentList;
	for (ClauseArgument argument : argumentSet) {
		argumentList.push_back(argument);
	}

	return argumentList;
}

shared_ptr<SelectClause> SelectMultipleParser::createClause(list<ClauseArgument>& args) {
	return make_shared<SelectClause>(SelectClause::createSynonymSelectClause(args));
}

bool SelectMultipleParser::isCorrectClauseType(PQLToken clauseTypeToken) {
	return clauseTypeToken.isAngledOpenBracket();
}
