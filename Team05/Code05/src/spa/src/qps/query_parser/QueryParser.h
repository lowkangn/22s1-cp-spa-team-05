#pragma once

#include <qps/exceptions/PQLSyntaxError.h>
#include <qps/exceptions/PQLSemanticError.h>
#include <qps/query_tokenizer/PQLToken.h>
#include <qps/query_parser/ArgumentType.h>
#include <qps/query/Query.h>
#include <qps/query/clause/PatternClause.h>
#include <qps/query_parser/parsers/WithParser.h>
#include <qps/query_parser/parsers/SelectParser.h>
#include <qps/query_parser/parsers/DeclarationParser.h>
#include <qps/query_parser/parsers/FollowsParser.h>
#include <qps/query_parser/parsers/ModifiesParser.h>
#include <qps/query_parser/parsers/ParentParser.h>
#include <qps/query_parser/parsers/PatternParser.h>
#include <qps/query_parser/parsers/UsesParser.h>

#include <list>
#include <unordered_map>

using namespace std;

/**
 * This class handles the creation of a Query object from a list of PQLTokens.
 */
class QueryParser {
private:
    list<PQLToken> tokens;
    list<shared_ptr<RelationshipClause>> suchThatClauses;
    list<shared_ptr<PatternClause>> patternClauses;
    list<shared_ptr<WithClause>> withClauses;

    bool isSemanticallyValid = true;
    string semanticErrorMessage;

    void setSemanticErrorFromParser(shared_ptr<SemanticChecker> parserPointer);

	shared_ptr<SelectClause> parseSelect(unordered_map<string, ArgumentType> declarations);

	void parseConstraints(unordered_map<string, ArgumentType> declarations);

    shared_ptr<RelationshipClause> parseSuchThat(unordered_map<string, ArgumentType>& declarations);

    shared_ptr<PatternClause> parsePattern(unordered_map<string, ArgumentType>& declarations);

    shared_ptr<WithClause> parseWith(unordered_map<string, ArgumentType>& declarations);

public:

    /**
     * Instantiates a QueryParser that will parse the given tokens.
     *
     * @param tokens to be parsed into a Query object.
     */
    QueryParser(list<PQLToken> tokens) {
        this->tokens = tokens;
        this->suchThatClauses = list<shared_ptr<RelationshipClause>>{};
        this->patternClauses = list<shared_ptr<PatternClause>>{};
        this->withClauses = list<shared_ptr<WithClause>>{};
        this->isSemanticallyValid = true;
        this->semanticErrorMessage = "";
    };

    /**
     * Parses list of PQLTokens into a Query object.
     * 
     * @return Query object.
     */
    Query parse();

    /* A class that can access private fields to help with testing */
    friend class QueryParserTestHelper;
};
