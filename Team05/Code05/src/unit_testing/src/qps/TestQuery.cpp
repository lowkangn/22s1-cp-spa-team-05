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
        shared_ptr<Clause>(new SelectClause(ClauseArgument("v", ArgumentType::ENTREF_SYNONYM))),
        list<shared_ptr<Clause>>{
            shared_ptr<Clause>(new ModifiesSClause(
                ClauseArgument("1", ArgumentType::LINE_NUMBER),
                ClauseArgument("v", ArgumentType::ENTREF_SYNONYM)))
        });

    Query sameAsFirstQuery = Query(
        shared_ptr<Clause>(new SelectClause(ClauseArgument("v", ArgumentType::ENTREF_SYNONYM))),
        list<shared_ptr<Clause>>{
        shared_ptr<Clause>(new ModifiesSClause(
            ClauseArgument("1", ArgumentType::LINE_NUMBER),
            ClauseArgument("v", ArgumentType::ENTREF_SYNONYM)))
    });

    Query secondQuery = Query(
        shared_ptr<Clause>(new SelectClause(ClauseArgument("v", ArgumentType::ENTREF_SYNONYM))),
        list<shared_ptr<Clause>>{
        shared_ptr<Clause>(new ModifiesPClause(
            ClauseArgument("main", ArgumentType::ENTREF_SYNONYM),
            ClauseArgument("v", ArgumentType::ENTREF_SYNONYM)))
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

