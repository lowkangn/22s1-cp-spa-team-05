#include "catch.hpp"
#include <qps/query/clause/ModifiesSClause.h>
#include <qps/query_parser/QueryParser.h>
#include <qps/query_parser/QueryParser.cpp>


using namespace std;

// =============== UNIT TESTS ====================

TEST_CASE("QueryParser: test parseConstraints") {
    auto testParseNoError = [](list<PQLToken> tokens, 
        unordered_map<string, DesignEntity> declarations,
        list<shared_ptr<Clause>> expected) {
            // given
            QueryParser parser = QueryParser(tokens);

            // when
            list<shared_ptr<Clause>> actual = parser.parseConstraints(declarations);
            bool isEqual = actual.size() == expected.size();
            if (isEqual) {
                while (!actual.empty()) {
                    //temporarily use casting to check equality for now
                    shared_ptr<Clause>  actualPtr = actual.front();
                    shared_ptr<ModifiesSClause> actualClause = dynamic_pointer_cast<ModifiesSClause>(actualPtr);
                    shared_ptr<Clause>  expectedPtr = expected.front();
                    shared_ptr<ModifiesSClause> expectedClause = dynamic_pointer_cast<ModifiesSClause>(actualPtr);

                    isEqual = isEqual && (*actualClause.get() == *expectedClause.get());
                    actual.pop_front();
                    expected.pop_front();
                }
            }

            // then
            REQUIRE(isEqual);
    };

    list<shared_ptr<Clause>> expected;
    expected.emplace_back(shared_ptr<Clause>(new ModifiesPClause(
        ClauseArgument("s1", ArgumentType::STMTREF_SYNONYM),
        ClauseArgument("v1", ArgumentType::ENTREF_SYNONYM))));
    testParseNoError(list<PQLToken>{
        PQLToken("such", PQLTokenType::NAME),
            PQLToken("that", PQLTokenType::NAME),
            PQLToken("Modifies", PQLTokenType::NAME),
            PQLToken("(", PQLTokenType::DELIMITER),
            PQLToken("s1", PQLTokenType::NAME),
            PQLToken(",", PQLTokenType::DELIMITER),
            PQLToken("v1", PQLTokenType::NAME),
            PQLToken(")", PQLTokenType::DELIMITER)},
        unordered_map<string, DesignEntity>{ {"v1", DesignEntity::VARIABLE},
            { "s1", DesignEntity::STMT }},
            expected);
}