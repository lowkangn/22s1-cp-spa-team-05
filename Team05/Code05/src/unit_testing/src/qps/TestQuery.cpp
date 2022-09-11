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

    shared_ptr<Clause> selectClause = shared_ptr<Clause>(new SelectClause(ClauseArgument("v", ArgumentType::VARIABLE)));
    shared_ptr<Clause> modifiesSClause = shared_ptr<Clause>(new ModifiesSClause(ClauseArgument("1", ArgumentType::LINE_NUMBER),
                                                                               ClauseArgument("v", ArgumentType::VARIABLE)));
    shared_ptr<Clause> modifiesPClause = shared_ptr<Clause>(new ModifiesPClause(ClauseArgument("main", ArgumentType::PROCEDURE),
                                                                                ClauseArgument("v", ArgumentType::VARIABLE)));

    Query firstQuery = Query(selectClause, list<shared_ptr<Clause>>{modifiesSClause});
    Query sameAsFirstQuery = Query(selectClause, list<shared_ptr<Clause>>{modifiesSClause});
    Query secondQuery = Query(selectClause, list<shared_ptr<Clause>>{modifiesPClause});


    SECTION("Equal") {
        testOperatorEquals(firstQuery, sameAsFirstQuery, true);
        testOperatorEquals(firstQuery, firstQuery, true);
    }

    SECTION("Not equal") {
        testOperatorEquals(firstQuery, secondQuery, false);
        testOperatorEquals(sameAsFirstQuery, secondQuery, false);
    }

}

