#include "ParserManager.h"

Query ParserManager::parse(string queryText) {
    // tokenize the query string
    vector<PQLToken> tokens = tokenizer.tokenize(queryText);

    // check for errors
    checker.check(tokens);

    // need to break here if errors are found
    // if no errors, parse the tokens into a Query object
    return parser.parse(tokens);
}
