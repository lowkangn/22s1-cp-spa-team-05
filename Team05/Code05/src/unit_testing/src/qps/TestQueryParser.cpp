#include "catch.hpp"
#include <qps/query/clause/FollowsClause.h>
#include <qps/query/clause/FollowsTClause.h>
#include <qps/query/clause/ModifiesSClause.h>
#include <qps/query/clause/ModifiesPClause.h>
#include <qps/query/clause/ParentClause.h>
#include <qps/query/clause/ParentTClause.h>
#include <qps/query/clause/UsesSClause.h>
#include <qps/query/clause/UsesPClause.h>
#include <qps/query_parser/QueryParser.h>


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
            PQLToken::createNameToken("procedure"),
            PQLToken::createNameToken("p"),
            PQLToken::createDelimiterToken(";"),
            PQLToken::createNameToken("Select"),
            PQLToken::createNameToken("p")
        };

        shared_ptr<SelectClause> selectClause = shared_ptr<SelectClause>(new SelectClause(
                ClauseArgument::createProcedureArg("p")));
        Query query = Query(selectClause, list<shared_ptr<RelationshipClause>>{}, list<shared_ptr<PatternClause>>{});

        testParseNoError(tokens, query);
    }

    SECTION("Select and such that clause") {
        list<PQLToken> tokens = list<PQLToken>{
            PQLToken::createNameToken("variable"),
            PQLToken::createNameToken("v"),
            PQLToken::createDelimiterToken(","),
            PQLToken::createNameToken("v1"),
            PQLToken::createDelimiterToken(";"),
            PQLToken::createNameToken("Select"),
            PQLToken::createNameToken("v1"),
            PQLToken::createNameToken("such"),
            PQLToken::createNameToken("that"),
            PQLToken::createNameToken("Modifies"),
            PQLToken::createDelimiterToken("("),
            PQLToken::createIntegerToken("1"),
            PQLToken::createDelimiterToken(","),
            PQLToken::createNameToken("v"),
            PQLToken::createDelimiterToken(")")
        };

        shared_ptr<SelectClause> selectClause = shared_ptr<SelectClause>(new SelectClause(
                ClauseArgument::createVariableArg("v1")));
        shared_ptr<RelationshipClause> modifiesClause = shared_ptr<RelationshipClause>(new ModifiesSClause(
                ClauseArgument::createLineNumberArg("1"),
                ClauseArgument::createVariableArg("v")));
        Query query = Query(selectClause, list<shared_ptr<RelationshipClause>>{modifiesClause}, list<shared_ptr<PatternClause>>{});

        testParseNoError(tokens, query);

        tokens = list<PQLToken>{
            PQLToken::createNameToken("constant"),
            PQLToken::createNameToken("c"),
            PQLToken::createDelimiterToken(";"),
            PQLToken::createNameToken("procedure"),
            PQLToken::createNameToken("p"),
            PQLToken::createDelimiterToken(";"),
            PQLToken::createNameToken("Select"),
            PQLToken::createNameToken("c"),
            PQLToken::createNameToken("such"),
            PQLToken::createNameToken("that"),
            PQLToken::createNameToken("Modifies"),
            PQLToken::createDelimiterToken("("),
            PQLToken::createNameToken("p"),
            PQLToken::createDelimiterToken(","),
            PQLToken::createDelimiterToken("\""),
            PQLToken::createNameToken("x"),
            PQLToken::createDelimiterToken("\""),
            PQLToken::createDelimiterToken(")")
        };
        selectClause = shared_ptr<SelectClause>(new SelectClause(
                ClauseArgument::createConstantArg("c")));
        modifiesClause = shared_ptr<RelationshipClause>(new ModifiesPClause(
                ClauseArgument::createProcedureArg("p"),
                ClauseArgument::createStringLiteralArg("x")));
        query = Query(selectClause, list<shared_ptr<RelationshipClause>>{modifiesClause}, list<shared_ptr<PatternClause>>{});

        testParseNoError(tokens, query);
    }
}

TEST_CASE("QueryParser: test parseConstraints Modifies") {
    auto testParseNoError = [](list<PQLToken> tokens, 
        unordered_map<string, ArgumentType> declarations,
        list<shared_ptr<RelationshipClause>> expected) {
            // given
            QueryParser parser = QueryParser(tokens);
			list<shared_ptr<RelationshipClause>> suchThatClauses;
			shared_ptr<list<shared_ptr<RelationshipClause>>> suchThatClausesPointer = make_shared<list<shared_ptr<RelationshipClause>>>(suchThatClauses);
			list<shared_ptr<PatternClause>> patternClauses;
			shared_ptr<list<shared_ptr<PatternClause>>> patternClausesPointer =  make_shared<list<shared_ptr<PatternClause>>>(patternClauses);

            // when
            parser.parseConstraints(suchThatClausesPointer, patternClausesPointer, declarations);
			list<shared_ptr<RelationshipClause>> actual = *suchThatClausesPointer;
            bool isEqual = actual.size() == expected.size();
            if (isEqual) {
                while (!actual.empty()) {
                    //temporarily use casting to check equality for now
                    shared_ptr<RelationshipClause> actualPtr = actual.front();
                    shared_ptr<RelationshipClause> expectedPtr = expected.front();
                    shared_ptr<ModifiesSClause> expectedClause = dynamic_pointer_cast<ModifiesSClause>(actualPtr);

                    isEqual = isEqual && (*expectedClause.get()).equals(actualPtr);
                    actual.pop_front();
                    expected.pop_front();
                }
            }

            // then
            REQUIRE(isEqual);
    };

    shared_ptr<RelationshipClause> modifiesClause = shared_ptr<RelationshipClause>(new ModifiesPClause(
        ClauseArgument::createStmtArg("s1"),
        ClauseArgument::createVariableArg("v1")));

    list<PQLToken> tokens = list<PQLToken>{
        PQLToken::createNameToken("such"),
        PQLToken::createNameToken("that"),
        PQLToken::createNameToken("Modifies"),
        PQLToken::createDelimiterToken("("),
        PQLToken::createNameToken("s1"),
        PQLToken::createDelimiterToken(","),
        PQLToken::createNameToken("v1"),
        PQLToken::createDelimiterToken(")")
    };

    unordered_map<string, ArgumentType> declarations = unordered_map<string, ArgumentType>{
        { "v1", ArgumentType::VARIABLE },
        { "s1", ArgumentType::STMT }
    };

    list<shared_ptr<RelationshipClause>> expected;
    expected.emplace_back(modifiesClause);
    testParseNoError(tokens, declarations, expected);

}

TEST_CASE("QueryParser: test parseConstraints Parent") {
    auto testParseNoError = [](list<PQLToken> tokens,
        unordered_map<string, ArgumentType> declarations,
        list<shared_ptr<RelationshipClause>> expected) {
            // given
            QueryParser parser = QueryParser(tokens);
			list<shared_ptr<RelationshipClause>> suchThatClauses;
			shared_ptr<list<shared_ptr<RelationshipClause>>> suchThatClausesPointer = make_shared<list<shared_ptr<RelationshipClause>>>(suchThatClauses);
			list<shared_ptr<PatternClause>> patternClauses;
			shared_ptr<list<shared_ptr<PatternClause>>> patternClausesPointer =  make_shared<list<shared_ptr<PatternClause>>>(patternClauses);

			// when
			parser.parseConstraints(suchThatClausesPointer, patternClausesPointer, declarations);
			list<shared_ptr<RelationshipClause>> actual = *suchThatClausesPointer;
            bool isEqual = actual.size() == expected.size();
            if (isEqual) {
                while (!actual.empty()) {
                    //temporarily use casting to check equality for now
                    shared_ptr<RelationshipClause> actualPtr = actual.front();
                    shared_ptr<RelationshipClause> expectedPtr = expected.front();
                    shared_ptr<ParentTClause> expectedClause = dynamic_pointer_cast<ParentTClause>(actualPtr);

                    isEqual = isEqual && (*expectedClause.get()).equals(actualPtr);
                    actual.pop_front();
                    expected.pop_front();
                }
            }

            // then
            REQUIRE(isEqual);
    };

    shared_ptr<RelationshipClause> parentTClause = shared_ptr<RelationshipClause>(new ParentTClause(
        ClauseArgument::createStmtArg("s1"),
        ClauseArgument::createLineNumberArg("5")));

    list<PQLToken> tokens = list<PQLToken>{
        PQLToken::createNameToken("such"),
        PQLToken::createNameToken("that"),
        PQLToken::createNameToken("Parent"),
        PQLToken::createOperatorToken("*"),
        PQLToken::createDelimiterToken("("),
        PQLToken::createNameToken("s1"),
        PQLToken::createDelimiterToken(","),
        PQLToken::createIntegerToken("5"),
        PQLToken::createDelimiterToken(")")
    };

    unordered_map<string, ArgumentType> declarations = unordered_map<string, ArgumentType>{
        { "v1", ArgumentType::VARIABLE },
        { "s1", ArgumentType::STMT }
    };

    list<shared_ptr<RelationshipClause>> expected;
    expected.emplace_back(parentTClause);
    testParseNoError(tokens, declarations, expected);
}

TEST_CASE("QueryParser: test parseConstraints Uses") {
    auto testParseNoError = [](list<PQLToken> tokens,
        unordered_map<string, ArgumentType> declarations,
        list<shared_ptr<RelationshipClause>> expected) {
            // given
            QueryParser parser = QueryParser(tokens);
			list<shared_ptr<RelationshipClause>> suchThatClauses;
			shared_ptr<list<shared_ptr<RelationshipClause>>> suchThatClausesPointer = make_shared<list<shared_ptr<RelationshipClause>>>(suchThatClauses);
			list<shared_ptr<PatternClause>> patternClauses;
			shared_ptr<list<shared_ptr<PatternClause>>> patternClausesPointer =  make_shared<list<shared_ptr<PatternClause>>>(patternClauses);

			// when
			parser.parseConstraints(suchThatClausesPointer, patternClausesPointer, declarations);
			list<shared_ptr<RelationshipClause>> actual = *suchThatClausesPointer;
            bool isEqual = actual.size() == expected.size();
            if (isEqual) {
                while (!actual.empty()) {
                    //temporarily use casting to check equality for now
                    shared_ptr<RelationshipClause> actualPtr = actual.front();
                    shared_ptr<RelationshipClause> expectedPtr = expected.front();
                    shared_ptr<UsesSClause> expectedClause = dynamic_pointer_cast<UsesSClause>(actualPtr);

                    isEqual = isEqual && (*expectedClause.get()).equals(actualPtr);
                    actual.pop_front();
                    expected.pop_front();
                }
            }

            // then
            REQUIRE(isEqual);
    };

    shared_ptr<RelationshipClause> usesSClause = shared_ptr<RelationshipClause>(new UsesSClause(
        ClauseArgument::createStmtArg("s1"),
        ClauseArgument::createVariableArg("v1")));

    list<PQLToken> tokens = list<PQLToken>{
        PQLToken::createNameToken("such"),
        PQLToken::createNameToken("that"),
        PQLToken::createNameToken("Uses"),
        PQLToken::createDelimiterToken("("),
        PQLToken::createNameToken("s1"),
        PQLToken::createDelimiterToken(","),
        PQLToken::createNameToken("v1"),
        PQLToken::createDelimiterToken(")")
    };

    unordered_map<string, ArgumentType> declarations = unordered_map<string, ArgumentType>{
        { "v1", ArgumentType::VARIABLE },
        { "s1", ArgumentType::STMT }
    };

    list<shared_ptr<RelationshipClause>> expected;
    expected.emplace_back(usesSClause);
    testParseNoError(tokens, declarations, expected);

}

TEST_CASE("QueryParser: test parseConstraints Follows") {
    auto testParseNoError = [](list<PQLToken> tokens,
        unordered_map<string, ArgumentType> declarations,
        list<shared_ptr<RelationshipClause>> expected) {
            // given
            QueryParser parser = QueryParser(tokens);
			list<shared_ptr<RelationshipClause>> suchThatClauses;
			shared_ptr<list<shared_ptr<RelationshipClause>>> suchThatClausesPointer = make_shared<list<shared_ptr<RelationshipClause>>>(suchThatClauses);
			list<shared_ptr<PatternClause>> patternClauses;
			shared_ptr<list<shared_ptr<PatternClause>>> patternClausesPointer =  make_shared<list<shared_ptr<PatternClause>>>(patternClauses);

			// when
			parser.parseConstraints(suchThatClausesPointer, patternClausesPointer, declarations);
			list<shared_ptr<RelationshipClause>> actual = *suchThatClausesPointer;
            bool isEqual = actual.size() == expected.size();
            if (isEqual) {
                while (!actual.empty()) {
                    //temporarily use casting to check equality for now
                    shared_ptr<RelationshipClause> actualPtr = actual.front();
                    shared_ptr<RelationshipClause> expectedPtr = expected.front();
                    shared_ptr<FollowsTClause> expectedClause = dynamic_pointer_cast<FollowsTClause>(actualPtr);

                    isEqual = isEqual && (*expectedClause.get()).equals(actualPtr);
                    actual.pop_front();
                    expected.pop_front();
                }
            }

            // then
            REQUIRE(isEqual);
    };

    shared_ptr<RelationshipClause> followsTClause = shared_ptr<RelationshipClause>(new FollowsTClause(
        ClauseArgument::createStmtArg("s1"),
        ClauseArgument::createLineNumberArg("5")));

    list<PQLToken> tokens = list<PQLToken>{
        PQLToken::createNameToken("such"),
        PQLToken::createNameToken("that"),
        PQLToken::createNameToken("Follows"),
        PQLToken::createOperatorToken("*"),
        PQLToken::createDelimiterToken("("),
        PQLToken::createNameToken("s1"),
        PQLToken::createDelimiterToken(","),
        PQLToken::createIntegerToken("5"),
        PQLToken::createDelimiterToken(")")
    };

    unordered_map<string, ArgumentType> declarations = unordered_map<string, ArgumentType>{
        { "v1", ArgumentType::VARIABLE },
        { "s1", ArgumentType::STMT }
    };

    list<shared_ptr<RelationshipClause>> expected;
    expected.emplace_back(followsTClause);
    testParseNoError(tokens, declarations, expected);
}
