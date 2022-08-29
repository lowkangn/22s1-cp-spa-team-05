#include "ErrorChecker.h"
#include "ParserManager.h"
#include "PQLToken.h"
#include "QueryParser.h"
#include "Tokenizer.h"
#include <vector>

Query ParserManager::parse(std::string queryText) {
    // tokenize the query string
    std::vector<PQLToken> tokens = Tokenizer::tokenize(queryText);

    // check for errors
    ErrorChecker::check(tokens);

    // need to break here if errors are found
    // if no errors, parse the tokens into a Query object
    return QueryParser::parse(tokens);
}
