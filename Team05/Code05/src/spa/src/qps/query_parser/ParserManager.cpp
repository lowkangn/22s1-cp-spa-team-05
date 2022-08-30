#include <qps/query_parser/ErrorChecker.h>
#include <qps/query_parser/ParserManager.h>
#include <qps/query_parser/PQLToken.h>
#include <qps/query_parser/Query.h>
#include <qps/query_parser/QueryParser.h>
#include <qps/query_parser/Tokenizer.h>

Query ParserManager::parse(string queryText) {
    // tokenize the query string
    vector<PQLToken> tokens = Tokenizer().tokenize(queryText);

    // check for errors
    ErrorChecker().check(tokens);

    // need to break here if errors are found
    // if no errors, parse the tokens into a Query object
    return QueryParser().parse(tokens);
}
