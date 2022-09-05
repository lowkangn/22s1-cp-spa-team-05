#include "catch.hpp"
#include <qps/query/clause/ModifiesSClause.h>
#include <qps/query/clause/ModifiesSClause.cpp>
#include <qps/query/clause/ModifiesPClause.h>
#include <qps/query/clause/ModifiesPClause.cpp>
#include <qps/query_parser/parsers/SuchThatClauseParser.h>
#include <qps/query_parser/parsers/SuchThatClauseParser.cpp>
#include <qps/query_parser/parsers/ModifiesParser.h>
#include <qps/query_parser/parsers/ModifiesParser.cpp>


using namespace std;

// =============== UNIT TESTS ====================

TEST_CASE("ModifiesParser: test parseModifiesSNoError") {
    auto testParseNoError = [](list<PQLToken> tokens,
        unordered_map<string, DesignEntity> declarations,
        ModifiesSClause expected) {
            // given
            ModifiesParser parser = ModifiesParser(tokens, declarations);

            // when
            shared_ptr<Clause>  actualPtr = parser.parse();

            // then
            REQUIRE(expected.equals(actualPtr.get()));
    };

    ModifiesSClause expected = ModifiesSClause(
        ClauseArgument("s1", ArgumentType::STMTREF_SYNONYM),
        ClauseArgument("v1", ArgumentType::ENTREF_SYNONYM));
    testParseNoError(list<PQLToken>{
        PQLToken("Modifies", PQLTokenType::NAME),
            PQLToken("(", PQLTokenType::DELIMITER),
            PQLToken("s1", PQLTokenType::NAME),
            PQLToken(",", PQLTokenType::DELIMITER),
            PQLToken("v1", PQLTokenType::NAME),
            PQLToken(")", PQLTokenType::DELIMITER)},
        unordered_map<string, DesignEntity>{
            {"v1", DesignEntity::VARIABLE},
            { "s1", DesignEntity::STMT }},
            expected);

    expected = ModifiesSClause(
        ClauseArgument("1", ArgumentType::LINE_NUMBER),
        ClauseArgument("x", ArgumentType::STRING_LITERAL));
    testParseNoError(list<PQLToken>{
        PQLToken("Modifies", PQLTokenType::NAME),
            PQLToken("(", PQLTokenType::DELIMITER),
            PQLToken("1", PQLTokenType::INTEGER),
            PQLToken(",", PQLTokenType::DELIMITER),
            PQLToken("\"", PQLTokenType::DELIMITER),
            PQLToken("x", PQLTokenType::NAME),
            PQLToken("\"", PQLTokenType::DELIMITER),
            PQLToken(")", PQLTokenType::DELIMITER)},
        unordered_map<string, DesignEntity>{},
            expected);
}

TEST_CASE("ModifiesParser: test parseWithError") {
    auto testParseWithError = [](list<PQLToken> tokens,
        unordered_map<string, DesignEntity> declarations) {
            // given
            ModifiesParser parser = ModifiesParser(tokens, declarations);

            // then
            REQUIRE_THROWS_AS(parser.parse(), PQLError);
    };

    SECTION("Undeclared / mispelled synonym") {
        testParseWithError(list<PQLToken>{
            PQLToken("Modifies", PQLTokenType::NAME),
                PQLToken("(", PQLTokenType::DELIMITER)},
            unordered_map<string, DesignEntity>{ {"v2", DesignEntity::VARIABLE}});
        testParseWithError(list<PQLToken>{
            PQLToken("Modifies", PQLTokenType::NAME),
                PQLToken("(", PQLTokenType::DELIMITER),
                PQLToken("s1", PQLTokenType::NAME),
                PQLToken(",", PQLTokenType::DELIMITER),
                PQLToken("v1", PQLTokenType::NAME),
                PQLToken(")", PQLTokenType::DELIMITER)},
            unordered_map<string, DesignEntity>{
                {"v2", DesignEntity::VARIABLE},
                { "s1", DesignEntity::STMT }});
    }

    SECTION("Illegal arguments") {
        testParseWithError(list<PQLToken>{
            PQLToken("Modifies", PQLTokenType::NAME),
                PQLToken("(", PQLTokenType::DELIMITER),
                PQLToken("_", PQLTokenType::DELIMITER),
                PQLToken(",", PQLTokenType::DELIMITER),
                PQLToken("v1", PQLTokenType::NAME),
                PQLToken(")", PQLTokenType::DELIMITER)},
            unordered_map<string, DesignEntity>{
                {"v1", DesignEntity::VARIABLE},
            { "s1", DesignEntity::STMT }});
        testParseWithError(list<PQLToken>{
            PQLToken("Modifies", PQLTokenType::NAME),
                PQLToken("(", PQLTokenType::DELIMITER),
                PQLToken("s1", PQLTokenType::DELIMITER),
                PQLToken(",", PQLTokenType::DELIMITER),
                PQLToken("s2", PQLTokenType::NAME),
                PQLToken(")", PQLTokenType::DELIMITER)},
            unordered_map<string, DesignEntity>{
                {"s2", DesignEntity::STMT},
            { "s1", DesignEntity::STMT }});
    }

}
