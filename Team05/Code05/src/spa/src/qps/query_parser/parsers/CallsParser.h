#pragma once

#include <qps/query/clause/CallsClause.h>
#include <qps/query/clause/CallsTClause.h>
#include <qps/query_parser/ArgumentType.h>
#include <qps/query_parser/parsers/SuchThatClauseParser.h>
#include <qps/query_tokenizer/PQLToken.h>

#include <list>
#include <memory>
#include <string>
#include <unordered_map>

class CallsParser : public SuchThatClauseParser {
private:
    bool isStar;
public:
    CallsParser(
        list<PQLToken>& tokens,
        const unordered_map<string, ArgumentType>& declarations) : SuchThatClauseParser(
        tokens,
        declarations) {
        this->isStar = false;
    }

    bool isCorrectClauseType(PQLToken clauseTypeToken) override;

    void checkArguments(list<ClauseArgument>& args) override;

    shared_ptr<RelationshipClause> createClause(
        PQLToken clauseTypeToken, list<ClauseArgument>& args) override;
};
