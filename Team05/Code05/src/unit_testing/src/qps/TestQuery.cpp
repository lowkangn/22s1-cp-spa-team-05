#include "catch.hpp"
#include <qps/query/Query.h>
#include <qps/query/clause/SelectClause.h>
#include <qps/query/clause/SelectClause.cpp>
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

	ClauseArgument varArg = ClauseArgument::createVariableArg("v");
    shared_ptr<SelectClause> selectClause = make_shared<SelectClause>(SelectClause::createSynonymSelectClause({varArg}));
    shared_ptr<RelationshipClause> modifiesSClause = shared_ptr<RelationshipClause>(new ModifiesSClause(ClauseArgument::createLineNumberArg("1"),
																										ClauseArgument::createVariableArg("v")));
    shared_ptr<RelationshipClause> modifiesPClause = shared_ptr<RelationshipClause>(new ModifiesPClause(ClauseArgument::createProcedureArg("main"),
																										ClauseArgument::createVariableArg("v")));
    list<shared_ptr<PatternClause>> emptyPatterns{};
    list<shared_ptr<WithClause>> emptyWiths{};
    Query firstQuery = Query(selectClause, list<shared_ptr<RelationshipClause>>{modifiesSClause}, emptyPatterns, emptyWiths);
    Query sameAsFirstQuery = Query(selectClause, list<shared_ptr<RelationshipClause>>{modifiesSClause}, emptyPatterns, emptyWiths);
    Query secondQuery = Query(selectClause, list<shared_ptr<RelationshipClause>>{modifiesPClause}, emptyPatterns, emptyWiths);

	// TODO: Different select clauses, relationship clauses, pattern clauses

    SECTION("Equal") {
        testOperatorEquals(firstQuery, sameAsFirstQuery, true);
        testOperatorEquals(firstQuery, firstQuery, true);
    }

    SECTION("Not equal") {
        testOperatorEquals(firstQuery, secondQuery, false);
        testOperatorEquals(sameAsFirstQuery, secondQuery, false);
    }

}

// =============== INTEGRATION TESTS ====================

// TODO: Waiting for PKB interaction implementation
TEST_CASE("Query: test evaluateSelect") {}

// TODO: Waiting for PKB interaction implementation
TEST_CASE("Query: test evaluateSuchThatAndPattern") {}
