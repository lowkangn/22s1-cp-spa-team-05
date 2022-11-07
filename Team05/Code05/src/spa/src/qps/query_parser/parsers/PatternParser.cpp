#include <qps/query_parser/parsers/PatternParser.h>

shared_ptr<PatternClause> PatternParser::parse() {
    PQLToken clauseTypeToken = this->tokens.front();

    vector<ClauseArgument> args = extractArguments();
    this->checkArguments(args);
    this->isParseCompleted = true;
    return createClause(clauseTypeToken, args);
}

void PatternParser::checkArguments(vector<ClauseArgument>& args) {
    //Pattern clause should have 3 arguments for assign and if, 4 arguments for while
    assert(args.size() == 3 || args.size() == 4);

    // CASE: IF pattern
    if (args.size() == 4) {
        // first arg must be if
        ClauseArgument arg = args[0];
        if (!arg.isIfSynonym()) {
            this->semanticErrorMessage = "First arg for pattern with 3 inputs must be if";
        }

        // second arg must be wildcard or string literal or variable
        arg = args[1];
        if (!arg.isWildcard() && !arg.isVariableSynonym() && !arg.isStringLiteral()) {
            this->semanticErrorMessage =
                "Second arg for pattern must be a variable synonym, string name, or wildcard";
        }

        arg = args[2];
        if (!arg.isWildcard()) {
            throw PQLSyntaxError("Third argument to If Pattern must only be wildcard");
        }

        arg = args[3];
        if (!arg.isWildcard()) {
            throw PQLSyntaxError("Fourth argument to If Pattern must only be wildcard");
        }
    } else if (args.size() == 3) {
        // CASE: while or assign patterns

        // first arg must be assignment or while
        ClauseArgument arg = args[0];
        if (!(arg.isAssignSynonym() || arg.isWhileSynonym())) {
            this->semanticErrorMessage =
                "First arg for pattern with 2 inputs must be assign or while";
        }

        // second arg must be wildcard or string literal or variable
        arg = args[1];
        if (!arg.isWildcard() && !arg.isVariableSynonym() && !arg.isStringLiteral()) {
            this->semanticErrorMessage =
                "Second arg for pattern must be a variable synonym, string name, or wildcard";
        }

        // if not wildcard, the first synonym must be assign, cannot be while
        arg = args[2];
        if (!arg.isWildcard() && !args[0].isAssignSynonym()) {
            this->semanticErrorMessage =
                "First arg for pattern with pattern string must be an assign synonym";
        }
    }
}

shared_ptr<PatternClause> PatternParser::createClause(
    PQLToken clauseTypeToken, vector<ClauseArgument>& args) {
    //Pattern clause should have exactly 3 arguments
    assert(args.size() == 3 || args.size() == 4);

    /*
        Note: createClause() will still attempt to create a pattern clause for non-if/while/assign
        synonyms. However, these will come with semantic errors and QueryParser will not allow
        these illegal pattern clauses to be parsed anyway
    */
    if (args.size() == 4) {
        // Note: Functionally, args[3], the third input in if pattern, does not affect output
        // and is omitted from the PatternIfClause constructor
        return make_shared<PatternIfClause>(args[0], args[1], args[2]);
    }
    if (args[0].isAssignSynonym()) {
        return make_shared<PatternAssignClause>(args[0], args[1], args[2]);
    }
    return make_shared<PatternWhileClause>(args[0], args[1], args[2]);
}

vector<ClauseArgument> PatternParser::extractArguments() {
    vector<ClauseArgument> output;

    // get first arg
    ClauseArgument firstArg = this->parseSynonym();
    output.push_back(firstArg);

    // check '('
    consumeOpenBracket();

    // get second arg
    ClauseArgument secondArg = parseOneStmtRefOrEntRef();
    if (secondArg.isLineNumber()) {
        throw PQLSyntaxError("First arg inside brackets for patterns cannot be an integer");
    }
    output.push_back(secondArg);

    // check ','
    consumeComma();

    // in this case, third arg can still be wildcard or pattern string
    if (this->tokens.front().isUnderscore()) {
        PQLToken wildcardToken = this->tokens.front();
        this->tokens.pop_front();

        if (this->tokens.empty() || !this->tokens.front().isQuote()) {
            ClauseArgument thirdArg = ClauseArgument::createWildcardArg();
            output.push_back(thirdArg);
        } else {
            ClauseArgument thirdArg = parsePatternStringWithWildcards();
            output.push_back(thirdArg);
        }
    } else if (this->tokens.front().isQuote()) {
        ClauseArgument thirdArg = parsePatternString();
        output.push_back(thirdArg);
    } else {
        throw PQLSyntaxError("Third argument to Pattern should start with quote or underscore");
    }

    if (this->tokens.empty()) {
        throw PQLSyntaxError("Must either end with fourth argument or closed bracket");
    }

    // If a ')' is not next, there must be a fourth arg which must be a wildcard
    if (!this->tokens.front().isCloseBracket()) {
        consumeComma();
        output.push_back(this->parseWildcard());
    }
    consumeCloseBracket();

    return output;
}
