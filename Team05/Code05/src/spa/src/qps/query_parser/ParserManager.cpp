#include "ParserManager.h"

Query ParserManager::parse(istream& queryStream) {
    // tokenize the query string
    list<PQLToken> tokens = tokenizer.tokenize(queryStream);

    // parse the tokens
    QueryParser parser = QueryParser(tokens);
    return parser.parse();
}
