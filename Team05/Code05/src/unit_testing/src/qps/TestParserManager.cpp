#include "catch.hpp"

#include <sstream>

#include <qps/query/clause/SelectClause.h>
#include <qps/query/clause/ModifiesSClause.h>
#include <qps/query/clause/ModifiesPClause.h>
#include <qps/query_parser/ParserManager.h>
#include <qps/query_parser/ParserManager.cpp>

using namespace std;

// =============== UNIT TESTS ====================

TEST_CASE("ParserManager: test parseNoError") {
    auto testParseNoError = [](string queryString, Query expected) {
        // given
        ParserManager pm = ParserManager();
        stringstream ss(queryString);
        istream& stream = ss;

        // when
        Query actual = pm.parse(stream);

        REQUIRE(actual == expected);
    };

    SECTION("Select clause only") {
        string queryString = "procedure p; Select p";
        Query query = Query(
            shared_ptr<Clause>(new SelectClause(ClauseArgument("p", ArgumentType::ENTREF_SYNONYM))),
            list<shared_ptr<Clause>>{});
        testParseNoError(queryString, query);
    }

    SECTION("Select and such that clause") {
        string queryString = "variable v1, v; Select v1 such that Modifies(1,v)";
        Query query = Query(
            shared_ptr<Clause>(new SelectClause(ClauseArgument("v1", ArgumentType::ENTREF_SYNONYM))),
            list<shared_ptr<Clause>>{
            shared_ptr<Clause>(new ModifiesSClause(
                ClauseArgument("1", ArgumentType::LINE_NUMBER),
                ClauseArgument("v", ArgumentType::ENTREF_SYNONYM)))
        });
        testParseNoError(queryString, query);

        queryString = "constant c; Select c such that Modifies(\"main\",\"x\")";
        query = Query(
            shared_ptr<Clause>(new SelectClause(ClauseArgument("c", ArgumentType::ENTREF_SYNONYM))),
            list<shared_ptr<Clause>>{
            shared_ptr<Clause>(new ModifiesPClause(
                ClauseArgument("main", ArgumentType::STRING_LITERAL),
                ClauseArgument("x", ArgumentType::STRING_LITERAL)))
        });
        testParseNoError(queryString, query);
    }

    SECTION("Queries with spaces and newlines") {
        string queryString = "variable v1,v; Select            v1 such that Modifies(1\n,     \t\t\t\t v)";
        Query query = Query(
            shared_ptr<Clause>(new SelectClause(ClauseArgument("v1", ArgumentType::ENTREF_SYNONYM))),
            list<shared_ptr<Clause>>{
            shared_ptr<Clause>(new ModifiesSClause(
                ClauseArgument("1", ArgumentType::LINE_NUMBER),
                ClauseArgument("v", ArgumentType::ENTREF_SYNONYM)))
        });
        testParseNoError(queryString, query);

        queryString = "stmt c; variable \t\tt \t, v1; \n Select t such that Modifies(c, v1)";
        query = Query(
            shared_ptr<Clause>(new SelectClause(ClauseArgument("t", ArgumentType::ENTREF_SYNONYM))),
            list<shared_ptr<Clause>>{
            shared_ptr<Clause>(new ModifiesSClause(
                ClauseArgument("c", ArgumentType::STMTREF_SYNONYM),
                ClauseArgument("v1", ArgumentType::ENTREF_SYNONYM)))
        });
        testParseNoError(queryString, query);
    }
}