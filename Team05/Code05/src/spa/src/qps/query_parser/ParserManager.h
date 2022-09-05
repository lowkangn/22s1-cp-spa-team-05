#pragma once

#include <fstream>

#include <qps/query/Query.h>
#include <qps/query_parser/QueryParser.h>
#include <qps/query_parser/QPSTokenizer.h>

using namespace std;

/**
 * This class handles the conversion of a query string to a Query object.
 */
class ParserManager {
public:

    /**
     * Takes in a PQL query in the form of a stream and returns a Query object.
     *
     * @param queryText The input stream used to create the Query.
     * @return Query object generated, provided the query stream has no errors.
     */
    Query parse(istream& queryStream);

};
