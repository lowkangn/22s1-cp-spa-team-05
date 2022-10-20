#pragma once

#include <qps/query_tokenizer/PQLToken.h>
#include <qps/query/Query.h>
#include <qps/query_parser/parsers/SelectParser.h>
#include <qps/query_parser/parsers/DeclarationParser.h>

#include <qps/query_parser/query_parser_states/SuchThatState.h>
#include <qps/query_parser/query_parser_states/PatternState.h>
#include <qps/query_parser/query_parser_states/WithState.h>


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

    shared_ptr<QueryParserState> currentState;

    bool isSemanticallyValid = true;
    string semanticErrorMessage;

	shared_ptr<SelectClause> parseSelect(unordered_map<string, ArgumentType> declarations);

	void parseConstraints(unordered_map<string, ArgumentType> declarations);

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

    void addWithClause(shared_ptr<WithClause> clause) {
        this->withClauses.push_back(clause);
    }

    void addPatternClause(shared_ptr<PatternClause> clause) {
        this->patternClauses.push_back(clause);
    }

    void addRelationshipClause(shared_ptr<RelationshipClause> clause) {
        this->suchThatClauses.push_back(clause);
    }

    void setSemanticErrorFromParser(shared_ptr<SemanticChecker> parserPointer);

    /* A class that can access private fields to help with testing */
    friend class QueryParserTestHelper;
};
