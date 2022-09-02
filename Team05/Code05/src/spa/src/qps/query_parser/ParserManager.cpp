#include "ParserManager.h"

Query ParserManager::parse(istream& queryStream) {
    // tokenize the query string
    list<PQLToken> tokens = tokenizer.tokenize(queryStream);

    // check for errors
    checker.check(tokens);

    // need to break here if errors are found
    // if no errors, parse the tokens into a Query object
    return parser.parse(tokens);
}
