#pragma once

#include "PQLToken.h"
#include "PQLError.h"
#include <list>
#include <fstream>

using namespace std;

/**
 * This class handles the conversion of a given query string into PQLTokens for error checking and conversion to a
 * Query object.
 */
class QPSTokenizer {

public:

    /**
     * Tokenizes the given query string into a list of PQLTokens.
     *
     * @param queryText to be converted into a list of PQLTokens.
     * @return list of PQLTokens representing the given query string.
     */
    list<PQLToken> tokenize(istream& stream);

    /**
     * Checks if a given character is alphabetical.
     *
     * @param c
     * @return true if c is alphabetical, false otherwise.
     */
    bool isAlphabetical(char c);

    /**
     * Checks if a given character is an integer.
     *
     * @param c
     * @return true if c is an integer, false otherwise.
     */
    bool isDigit(char c);

    /**
     * Checks if a given character is a delimiter.
     *
     * @param c
     * @return true if c is a delimiter, false otherwise.
     */
    bool isDelimiter(char c);

    /**
     * Checks if a given character is an operator.
     *
     * @param c
     * @return true if c is an operator, false otherwise.
     */
    bool isOperator(char c);

    /**
     * Checks if a given character is a whitespace or a newline character.
     *
     * @param c
     * @return true if c is a whitespace or newline, false otherwise.
     */
    bool isWhitespaceOrNewline(char c);

    /**
     * Takes a stream and removes a string from the front to make a name token.
     *
     * @param stream to obtain name string from.
     * @return PQLToken object encapsulating the name token.
     */
    PQLToken extractNameFromStream(istream& stream);

    /**
     * Takes a stream and removes a string from the front to make an integer token.
     *
     * @param stream to obtain integer string from.
     * @return PQLToken object encapsulating the integer token.
     */
    PQLToken extractIntegerFromStream(istream& stream);

    /**
     * Takes a stream and removes a delimiter from the front to make an delimiter token.
     *
     * @param stream to obtain delimiter string from.
     * @return PQLToken object encapsulating the delimiter token.
     */
    PQLToken extractDelimiterFromStream(istream& stream);

    /**
     * Takes a stream and removes a string from the front to make an operator token.
     *
     * @param stream to obtain operator string from.
     * @return PQLToken object encapsulating the operator token.
     */
    PQLToken extractOperatorFromStream(istream& stream);

    /**
     * Takes a stream and removes a whitespace or newline from the front.
     *
     * @param stream to remove whitespace or newlines from.
     */
    void skipWhitespaceOrNewlineInStream(istream& stream);

};
