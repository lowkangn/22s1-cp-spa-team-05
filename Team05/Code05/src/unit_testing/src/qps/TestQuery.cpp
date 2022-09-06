#include "catch.hpp"
#include <qps/query/Query.h>
#include <qps/query/Query.cpp>
#include <qps/query/clause/SelectClause.h>
#include <qps/query/clause/ModifiesSClause.h>
#include <qps/query/clause/ModifiesPClause.h>

using namespace std;

// =============== UNIT TESTS ====================

TEST_CASE("Query: test operator==") {
    auto testOperatorEquals = [](Query first, Query second, bool expected) {
        // when
        bool actual = first == second;

        // then
        REQUIRE(actual == expected);
    };

    Query firstQuery = Query(
        shared_ptr<Clause>(new SelectClause(ClauseArgument("v", ArgumentType::VARIABLE))),
        list<shared_ptr<Clause>>{
            shared_ptr<Clause>(new ModifiesSClause(
                ClauseArgument("1", ArgumentType::LINE_NUMBER),
                ClauseArgument("v", ArgumentType::VARIABLE)))
        });

    Query sameAsFirstQuery = Query(
        shared_ptr<Clause>(new SelectClause(ClauseArgument("v", ArgumentType::VARIABLE))),
        list<shared_ptr<Clause>>{
        shared_ptr<Clause>(new ModifiesSClause(
            ClauseArgument("1", ArgumentType::LINE_NUMBER),
            ClauseArgument("v", ArgumentType::VARIABLE)))
    });

    Query secondQuery = Query(
        shared_ptr<Clause>(new SelectClause(ClauseArgument("v", ArgumentType::VARIABLE))),
        list<shared_ptr<Clause>>{
        shared_ptr<Clause>(new ModifiesPClause(
            ClauseArgument("main", ArgumentType::PROCEDURE),
            ClauseArgument("v", ArgumentType::VARIABLE)))
    });

    SECTION("Equal") {
        testOperatorEquals(firstQuery, sameAsFirstQuery, true);
        testOperatorEquals(firstQuery, firstQuery, true);
    }

    SECTION("Not equal") {
        testOperatorEquals(firstQuery, secondQuery, false);
        testOperatorEquals(sameAsFirstQuery, secondQuery, false);
    }

}

