#include "ParserManager.h"

Query ParserManager::parse(istream& queryStream) {
    //tokenize the queryStream
    list<PQLToken> tokens = QPSTokenizer().tokenize(queryStream);
    
    //parse the tokens
    QueryParser parser = QueryParser(tokens);
    return parser.parse();
}
