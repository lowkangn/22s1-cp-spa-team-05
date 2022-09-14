#include "catch.hpp"
#include <qps/query/clause/ModifiesSClause.h>
#include <qps/query_parser/QueryParser.h>
#include <qps/query_parser/QueryParser.cpp>


using namespace std;

// =============== UNIT TESTS ====================

TEST_CASE("QueryParser: test parseNoError") {
    auto testParseNoError = [](list<PQLToken> tokens, Query expected) {
        // given
        QueryParser parser = QueryParser(tokens);

        // when
        Query actual = parser.parse();

        REQUIRE(actual == expected);
    };

    SECTION("Select clause only") {
        list<PQLToken> tokens = list<PQLToken>{
            PQLToken("procedure", PQLTokenType::NAME),
            PQLToken("p", PQLTokenType::NAME),
            PQLToken(";", PQLTokenType::DELIMITER),
            PQLToken("Select", PQLTokenType::NAME),
            PQLToken("p", PQLTokenType::NAME)
        };

        shared_ptr<SelectClause> selectClause = shared_ptr<SelectClause>(new SelectClause(
                ClauseArgument("p", ArgumentType::PROCEDURE)));
        Query query = Query(selectClause, list<shared_ptr<RelationshipClause>>{});

        testParseNoError(tokens, query);
    }

    SECTION("Select and such that clause") {
        list<PQLToken> tokens = list<PQLToken>{
            PQLToken("variable", PQLTokenType::NAME),
            PQLToken("v", PQLTokenType::NAME),
            PQLToken(",", PQLTokenType::DELIMITER),
            PQLToken("v1", PQLTokenType::NAME),
            PQLToken(";", PQLTokenType::DELIMITER),
            PQLToken("Select", PQLTokenType::NAME),
            PQLToken("v1", PQLTokenType::NAME),
            PQLToken("such", PQLTokenType::NAME),
            PQLToken("that", PQLTokenType::NAME),
            PQLToken("Modifies", PQLTokenType::NAME),
            PQLToken("(", PQLTokenType::DELIMITER),
            PQLToken("1", PQLTokenType::INTEGER),
            PQLToken(",", PQLTokenType::DELIMITER),
            PQLToken("v", PQLTokenType::NAME),
            PQLToken(")", PQLTokenType::DELIMITER)
        };

        shared_ptr<SelectClause> selectClause = shared_ptr<SelectClause>(new SelectClause(
                ClauseArgument("v1", ArgumentType::VARIABLE)));
        shared_ptr<RelationshipClause> modifiesClause = shared_ptr<RelationshipClause>(new ModifiesSClause(
                ClauseArgument("1", ArgumentType::LINE_NUMBER),
                ClauseArgument("v", ArgumentType::VARIABLE)));
        Query query = Query(selectClause, list<shared_ptr<RelationshipClause>>{modifiesClause});

        testParseNoError(tokens, query);

        tokens = list<PQLToken>{
            PQLToken("constant", PQLTokenType::NAME),
            PQLToken("c", PQLTokenType::NAME),
            PQLToken(";", PQLTokenType::DELIMITER),
            PQLToken("procedure", PQLTokenType::NAME),
            PQLToken("p", PQLTokenType::NAME),
            PQLToken(";", PQLTokenType::DELIMITER),
            PQLToken("Select", PQLTokenType::NAME),
            PQLToken("c", PQLTokenType::NAME),
            PQLToken("such", PQLTokenType::NAME),
            PQLToken("that", PQLTokenType::NAME),
            PQLToken("Modifies", PQLTokenType::NAME),
            PQLToken("(", PQLTokenType::DELIMITER),
            PQLToken("p", PQLTokenType::NAME),
            PQLToken(",", PQLTokenType::DELIMITER),
            PQLToken("\"", PQLTokenType::DELIMITER),
            PQLToken("x", PQLTokenType::NAME),
            PQLToken("\"", PQLTokenType::DELIMITER),
            PQLToken(")", PQLTokenType::DELIMITER)
        };
        selectClause = shared_ptr<SelectClause>(new SelectClause(
                ClauseArgument("c", ArgumentType::CONSTANT)));
        modifiesClause = shared_ptr<RelationshipClause>(new ModifiesPClause(
                ClauseArgument("p", ArgumentType::PROCEDURE),
                ClauseArgument("x", ArgumentType::STRING_LITERAL)));
        query = Query(selectClause, list<shared_ptr<RelationshipClause>>{modifiesClause});

        testParseNoError(tokens, query);
    }
}

TEST_CASE("QueryParser: test parseConstraints") {
    auto testParseNoError = [](list<PQLToken> tokens, 
        unordered_map<string, ArgumentType> declarations,
        list<shared_ptr<RelationshipClause>> expected) {
            // given
            QueryParser parser = QueryParser(tokens);

            // when
            list<shared_ptr<RelationshipClause>> actual = parser.parseConstraints(declarations);
            bool isEqual = actual.size() == expected.size();
            if (isEqual) {
                while (!actual.empty()) {
                    //temporarily use casting to check equality for now
                    shared_ptr<RelationshipClause> actualPtr = actual.front();
                    shared_ptr<RelationshipClause> expectedPtr = expected.front();
                    shared_ptr<ModifiesSClause> expectedClause = dynamic_pointer_cast<ModifiesSClause>(actualPtr);

                    isEqual = isEqual && (*expectedClause.get()).equals(actualPtr.get());
                    actual.pop_front();
                    expected.pop_front();
                }
            }

            // then
            REQUIRE(isEqual);
    };

    shared_ptr<RelationshipClause> modifiesClause = shared_ptr<RelationshipClause>(new ModifiesPClause(
        ClauseArgument("s1", ArgumentType::STMT),
        ClauseArgument("v1", ArgumentType::VARIABLE)));

    list<PQLToken> tokens = list<PQLToken>{
        PQLToken("such", PQLTokenType::NAME),
        PQLToken("that", PQLTokenType::NAME),
        PQLToken("Modifies", PQLTokenType::NAME),
        PQLToken("(", PQLTokenType::DELIMITER),
        PQLToken("s1", PQLTokenType::NAME),
        PQLToken(",", PQLTokenType::DELIMITER),
        PQLToken("v1", PQLTokenType::NAME),
        PQLToken(")", PQLTokenType::DELIMITER)
    };

    unordered_map<string, ArgumentType> declarations = unordered_map<string, ArgumentType>{
        {"v1", ArgumentType::VARIABLE},
        { "s1", ArgumentType::STMT }
    };

    list<shared_ptr<RelationshipClause>> expected;
    expected.emplace_back(modifiesClause);
    testParseNoError(tokens, declarations, expected);
}
