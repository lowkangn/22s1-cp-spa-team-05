#include "catch.hpp"

#include <qps/query/clause/CallsClause.h>
#include <qps/query/clause/CallsTClause.h>
#include <qps/query/clause/FollowsClause.h>
#include <qps/query/clause/FollowsTClause.h>
#include <qps/query/clause/ModifiesSClause.h>
#include <qps/query/clause/ModifiesPClause.h>
#include <qps/query/clause/ParentClause.h>
#include <qps/query/clause/ParentTClause.h>
#include <qps/query/clause/UsesSClause.h>
#include <qps/query/clause/UsesPClause.h>
#include <qps/query/clause/PatternAssignClause.h>
#include <qps/query_parser/QueryParser.h>

using namespace std;

/* QueryParserTestHelper is a helper class that tests private methods/fields of QueryParser. */
class QueryParserTestHelper {
private:
    QueryParser parserUnderTest;

    template <class T>
    void requireClausesEqual(list<shared_ptr<T>> expectedClauses, list<shared_ptr<T>> actualClauses) {
        bool isEqualLength = expectedClauses.size() == actualClauses.size();
        REQUIRE(isEqualLength);
        if (isEqualLength) {
            while (!actualClauses.empty()) {
                shared_ptr<T> actualPtr = actualClauses.front();
                shared_ptr<T> expectedPtr = expectedClauses.front();

                REQUIRE((*expectedPtr).equals(actualPtr));
                actualClauses.pop_front();
                expectedClauses.pop_front();
            }
        }
    }

public:
    QueryParserTestHelper(const QueryParser& parser) : parserUnderTest(parser) {}

    void parseConstraints(unordered_map<string, ArgumentType> declarations) {
        this->parserUnderTest.parseConstraints(declarations);
    }

    void requireAllEquals(list<shared_ptr<RelationshipClause>> expectedSuchThat,
        list<shared_ptr<PatternClause>> expectedPattern,
        list<shared_ptr<WithClause>> expectedWith) {

        list<shared_ptr<RelationshipClause>> actualSuchThat = this->parserUnderTest.suchThatClauses;
        requireClausesEqual<RelationshipClause>(expectedSuchThat, actualSuchThat);

        list<shared_ptr<PatternClause>> actualPattern = this->parserUnderTest.patternClauses;
        requireClausesEqual<PatternClause>(expectedPattern, actualPattern);

        list<shared_ptr<WithClause>> actualWith = this->parserUnderTest.withClauses;
        requireClausesEqual<WithClause>(expectedWith, actualWith);
    }

};

namespace QPSTestUtil {
    // design entity tokens
    PQLToken stmt = PQLToken::createNameToken("stmt");
    PQLToken procedure = PQLToken::createNameToken("procedure");
    PQLToken read = PQLToken::createNameToken("read");
    PQLToken assign = PQLToken::createNameToken("assign");
    PQLToken print = PQLToken::createNameToken("print");
    PQLToken while_ = PQLToken::createNameToken("while");
    PQLToken if_ = PQLToken::createNameToken("if");
    PQLToken variable = PQLToken::createNameToken("variable");
    PQLToken constant = PQLToken::createNameToken("constant");

    // clause delimiter tokens
    PQLToken openBracket = PQLToken::createDelimiterToken("(");
    PQLToken closeBracket = PQLToken::createDelimiterToken(")");
    PQLToken comma = PQLToken::createDelimiterToken(",");
    PQLToken semicolon = PQLToken::createDelimiterToken(";");

    // clause identifier tokens
    PQLToken select = PQLToken::createNameToken("Select");
    PQLToken such = PQLToken::createNameToken("such");
    PQLToken that = PQLToken::createNameToken("that");
    PQLToken pattern = PQLToken::createNameToken("pattern");
    PQLToken with = PQLToken::createNameToken("with");

    PQLToken calls = PQLToken::createNameToken("Calls");
    PQLToken modifies = PQLToken::createNameToken("Modifies");
    PQLToken uses = PQLToken::createNameToken("Uses");
    PQLToken follows = PQLToken::createNameToken("Follows");
    PQLToken parent = PQLToken::createNameToken("Parent");
    PQLToken star = PQLToken::createOperatorToken("*");

    // clause operator tokens
    PQLToken dot = PQLToken::createOperatorToken(".");
    PQLToken equals = PQLToken::createOperatorToken("=");

    // attribute tokens
    PQLToken procName = PQLToken::createNameToken("procName");
    PQLToken varName = PQLToken::createNameToken("varName");
    PQLToken value = PQLToken::createNameToken("value");
    PQLToken stmtNumStmt = PQLToken::createNameToken("stmt");
    PQLToken stmtNumHash = PQLToken::createDelimiterToken("#");

    // strings for tokens, declarations and ClauseArguments
    string s1String = "s1";
    string s2String = "s2";
    string a1String = "a1";
    string a2String = "a2";
    string w1String = "w1";
    string i1String = "i1";
    string r1String = "r1";
    string pri1String = "pri1";
    string call1String = "call1";
    string proc1String = "proc1";
    string proc2String = "proc2";
    string proc3String = "proc3";
    string const1String = "const1";
    string v1String = "v1";
    string v2String = "v2";

    // tokens for synonyms
    PQLToken s1 = PQLToken::createNameToken(s1String);
    PQLToken s2 = PQLToken::createNameToken(s2String);
    PQLToken a1 = PQLToken::createNameToken(a1String);
    PQLToken a2 = PQLToken::createNameToken(a2String);
    PQLToken w1 = PQLToken::createNameToken(w1String);
    PQLToken i1 = PQLToken::createNameToken(i1String);
    PQLToken r1 = PQLToken::createNameToken(r1String);
    PQLToken pri1 = PQLToken::createNameToken(pri1String);
    PQLToken call1 = PQLToken::createNameToken(call1String);
    PQLToken proc1 = PQLToken::createNameToken(proc1String);
    PQLToken proc2 = PQLToken::createNameToken(proc2String);
    PQLToken proc3 = PQLToken::createNameToken(proc3String);
    PQLToken const1 = PQLToken::createNameToken(const1String);
    PQLToken v1 = PQLToken::createNameToken(v1String);
    PQLToken v2 = PQLToken::createNameToken(v2String);

    // we use the same set of declarations for all cases
    unordered_map<string, ArgumentType> declarations = unordered_map<string, ArgumentType>{
        { s1String, ArgumentType::STMT },
        { a1String, ArgumentType::ASSIGN },
        { s2String, ArgumentType::STMT },
        { a2String, ArgumentType::ASSIGN },
        { w1String, ArgumentType::WHILE},
        { i1String, ArgumentType::IF },
        { r1String, ArgumentType::READ },
        { pri1String, ArgumentType::PRINT },
        { call1String, ArgumentType::CALL },
        { proc1String, ArgumentType::PROCEDURE },
        { proc2String, ArgumentType::PROCEDURE },
        { proc3String, ArgumentType::PROCEDURE },
        { const1String, ArgumentType::CONSTANT },
        { v1String, ArgumentType::VARIABLE },
        { v2String, ArgumentType::VARIABLE },
    };

    // synonym ClauseArguments
    ClauseArgument s1Arg = ClauseArgument::createStmtArg(s1String);
    ClauseArgument s2Arg = ClauseArgument::createStmtArg(s2String);
    ClauseArgument a1Arg = ClauseArgument::createAssignArg(a1String);
    ClauseArgument a2Arg = ClauseArgument::createAssignArg(a2String);
    ClauseArgument w1Arg = ClauseArgument::createWhileArg(w1String);
    ClauseArgument i1Arg = ClauseArgument::createIfArg(i1String);
    ClauseArgument r1Arg = ClauseArgument::createReadArg(r1String);
    ClauseArgument pri1Arg = ClauseArgument::createPrintArg(pri1String);
    ClauseArgument call1Arg = ClauseArgument::createCallArg(call1String);
    ClauseArgument proc1Arg = ClauseArgument::createProcedureArg(proc1String);
    ClauseArgument proc2Arg = ClauseArgument::createProcedureArg(proc2String);
    ClauseArgument proc3Arg = ClauseArgument::createProcedureArg(proc3String);
    ClauseArgument const1Arg = ClauseArgument::createConstantArg(const1String);
    ClauseArgument v1Arg = ClauseArgument::createVariableArg(v1String);
    ClauseArgument v2Arg = ClauseArgument::createVariableArg(v2String);

    // tokens and ClauseArguments for non-synonym clause arguments
    PQLToken multiplyToken = PQLToken::createOperatorToken("*");
    PQLToken divideToken = PQLToken::createOperatorToken("/");
    PQLToken minusToken = PQLToken::createOperatorToken("-");
    PQLToken plusToken = PQLToken::createOperatorToken("+");
    PQLToken two = PQLToken::createIntegerToken("2");
    PQLToken five = PQLToken::createIntegerToken("5");
    PQLToken wildcard = PQLToken::createDelimiterToken("_");
    PQLToken quotationMark = PQLToken::createDelimiterToken("\"");
    PQLToken name = PQLToken::createNameToken("name");

    ClauseArgument twoLineNumberArg = ClauseArgument::createLineNumberArg("2");
    ClauseArgument fiveLineNumberArg = ClauseArgument::createLineNumberArg("5");
    ClauseArgument wildcardArg = ClauseArgument::createWildcardArg();
    ClauseArgument quotedNameArg = ClauseArgument::createStringLiteralArg("name");
};

using namespace QPSTestUtil;

// =============== UNIT TESTS ====================
TEST_CASE("QueryParser: test parseNoError") {
    auto testParseNoError = [](list<PQLToken> tokens, Query expected) {
        // given
        QueryParser parser = QueryParser(tokens);

        // when
        Query actual = parser.parse();

        REQUIRE(actual == expected);
    };

    list<shared_ptr<RelationshipClause>> emptyRelationships{};
    list<shared_ptr<PatternClause>> emptyPatterns{};
    list<shared_ptr<WithClause>> emptyWiths{};

    SECTION("Select clause only") {
        list<PQLToken> tokens = list<PQLToken>{
            procedure, proc1, semicolon,
            select, proc1,
        };

        shared_ptr<SelectClause> selectClause = make_shared<SelectClause>(SelectClause::createSynonymSelectClause({proc1Arg}));
        Query query = Query(selectClause, emptyRelationships, emptyPatterns, emptyWiths);

        testParseNoError(tokens, query);
    }

    SECTION("Select and such that clause") {
        list<PQLToken> tokens = list<PQLToken>{
            variable, v1, comma, v2, semicolon,
            select, v2,
            such, that, modifies, openBracket, two, comma, v1, closeBracket,
        };

        shared_ptr<SelectClause> selectClause = make_shared<SelectClause>(SelectClause::createSynonymSelectClause({v2Arg}));
        shared_ptr<RelationshipClause> modifiesClause = shared_ptr<RelationshipClause>(
            new ModifiesSClause(twoLineNumberArg, v1Arg));
        Query query = Query(selectClause, list<shared_ptr<RelationshipClause>>{modifiesClause}, emptyPatterns, emptyWiths);

        testParseNoError(tokens, query);

        tokens = list<PQLToken>{
            constant, const1, semicolon, procedure, proc1, semicolon,
            select, const1,
            such, that, modifies, openBracket, proc1, comma, quotationMark, name, quotationMark, closeBracket,
        };

        selectClause = make_shared<SelectClause>(SelectClause::createSynonymSelectClause({const1Arg}));
        modifiesClause = shared_ptr<RelationshipClause>(new ModifiesPClause(proc1Arg, quotedNameArg));
        query = Query(selectClause, list<shared_ptr<RelationshipClause>>{modifiesClause}, emptyPatterns, emptyWiths);

        testParseNoError(tokens, query);
    }

    SECTION("Select, many such that and pattern clauses") {
        list<PQLToken> tokens = list<PQLToken>{
            variable, v1, comma, v2, semicolon, 
            assign, a1, comma, a2, semicolon,
            procedure, proc1, semicolon,
            procedure, proc2, semicolon,
            if_, i1, semicolon,
            select, v2,
            such, that, calls, openBracket, proc1, comma, proc2, closeBracket,
            such, that, modifies, openBracket, two, comma, v1, closeBracket,
            pattern, a1, openBracket, v1, comma, wildcard, quotationMark, name, quotationMark, wildcard, closeBracket,
            such, that, follows, star, openBracket, a1, comma, i1, closeBracket,
            pattern, a1, openBracket, wildcard, comma, wildcard, closeBracket,
            such, that, parent, openBracket, wildcard, comma, i1, closeBracket,
            such, that, uses, openBracket, proc1, comma, wildcard, closeBracket,
            pattern, a2, openBracket, v1, comma, quotationMark, name, multiplyToken, name, minusToken, five, quotationMark, closeBracket,
        };

        shared_ptr<SelectClause> selectClause = make_shared<SelectClause>(SelectClause::createSynonymSelectClause({v2Arg}));

        shared_ptr<RelationshipClause> callsClause =
            shared_ptr<RelationshipClause>(new CallsClause(proc1Arg, proc2Arg));
        shared_ptr<RelationshipClause> modifiesSClause = 
            shared_ptr<RelationshipClause>(new ModifiesSClause(twoLineNumberArg, v1Arg));
        shared_ptr<RelationshipClause> followsTClause =
            shared_ptr<RelationshipClause>(new FollowsTClause(a1Arg, i1Arg));
        shared_ptr<RelationshipClause> parentClause =
            shared_ptr<RelationshipClause>(new ParentClause(wildcardArg, i1Arg));
        shared_ptr<RelationshipClause> usesPClause =
            shared_ptr<RelationshipClause>(new UsesPClause(proc1Arg, wildcardArg));
        list<shared_ptr<RelationshipClause>> expectedSuchThat = {
            callsClause, modifiesSClause, followsTClause, parentClause, usesPClause,
        };
            
        shared_ptr<PatternClause> firstPatternAssign = shared_ptr<PatternClause>(
            new PatternAssignClause(a1Arg, v1Arg, ClauseArgument::createPatternStringWithWildcardsArg("_name_")));
        shared_ptr<PatternClause> secondPatternAssign = shared_ptr<PatternClause>(
            new PatternAssignClause(a1Arg, wildcardArg, wildcardArg));
        shared_ptr<PatternClause> thirdPatternAssign = shared_ptr<PatternClause>(
            new PatternAssignClause(a2Arg, v1Arg, ClauseArgument::createPatternStringArg("name name * 5 -")));
        list<shared_ptr<PatternClause>> expectedPattern = list<shared_ptr<PatternClause>>{
            firstPatternAssign, secondPatternAssign, thirdPatternAssign,
        };

       
        list<shared_ptr<WithClause>> expectedWiths{};

        Query query = Query(selectClause, expectedSuchThat, expectedPattern, expectedWiths);

        testParseNoError(tokens, query);
    }
}

TEST_CASE("QueryParser: test parseConstraints single such that clause") {
    auto testParseNoError = [](list<PQLToken> tokens, 
        unordered_map<string, ArgumentType> declarations,
        list<shared_ptr<RelationshipClause>> expected) {
            // given
            QueryParser parser = QueryParser(tokens);
            QueryParserTestHelper helper = QueryParserTestHelper(parser);

            // when
            helper.parseConstraints(declarations);

            // then
            helper.requireAllEquals(expected, list<shared_ptr<PatternClause>>{}, list<shared_ptr<WithClause>>{});
    };


    SECTION("Modifies clause") {
        shared_ptr<RelationshipClause> modifiesClause = shared_ptr<RelationshipClause>(
            new ModifiesSClause(s1Arg, v1Arg));

        list<PQLToken> tokens = list<PQLToken>{
            such, that, modifies, openBracket, s1, comma, v1, closeBracket,
        };

        list<shared_ptr<RelationshipClause>> expected;
        expected.emplace_back(modifiesClause);
        testParseNoError(tokens, declarations, expected);
    }

    SECTION("Parent clause") {
        shared_ptr<RelationshipClause> parentTClause = shared_ptr<RelationshipClause>(
            new ParentTClause(s1Arg, fiveLineNumberArg));

        list<PQLToken> tokens = list<PQLToken>{
            such, that, parent, star, openBracket, s1, comma, five, closeBracket,
        };

        list<shared_ptr<RelationshipClause>> expected;
        expected.emplace_back(parentTClause);
        testParseNoError(tokens, declarations, expected);
    }

    SECTION("Uses clause") {
        shared_ptr<RelationshipClause> usesSClause = shared_ptr<RelationshipClause>(
            new UsesSClause(s1Arg, v1Arg));

        list<PQLToken> tokens = list<PQLToken>{
            such, that, uses, openBracket, s1, comma, v1, closeBracket,
        };

        list<shared_ptr<RelationshipClause>> expected;
        expected.emplace_back(usesSClause);
        testParseNoError(tokens, declarations, expected);
    }

    SECTION("Follows clause") {
        shared_ptr<RelationshipClause> followsTClause = shared_ptr<RelationshipClause>(
            new FollowsTClause(s1Arg, fiveLineNumberArg));

        list<PQLToken> tokens = list<PQLToken>{
            such, that, follows, star, openBracket, s1, comma, five, closeBracket,
        };

        list<shared_ptr<RelationshipClause>> expected;
        expected.emplace_back(followsTClause);
        testParseNoError(tokens, declarations, expected);
    }

    SECTION("Calls clause") {
        shared_ptr<RelationshipClause> callsTClause = shared_ptr<RelationshipClause>(
            new CallsTClause(proc1Arg, proc2Arg));

        list<PQLToken> tokens = list<PQLToken>{
            such, that, calls, star, openBracket, proc1, comma, proc2, closeBracket,
        };

        list<shared_ptr<RelationshipClause>> expected;
        expected.emplace_back(callsTClause);
        testParseNoError(tokens, declarations, expected);
    }
}

TEST_CASE("QueryParser: test parseConstraints Multiple clauses") {
    auto testParseNoError = [](list<PQLToken> tokens,
        unordered_map<string, ArgumentType> declarations,
        list<shared_ptr<RelationshipClause>> expectedSuchThat,
        list<shared_ptr<PatternClause>> expectedPattern,
        list<shared_ptr<WithClause>> expectedWith) {
            // given
            QueryParser parser = QueryParser(tokens);
            QueryParserTestHelper helper = QueryParserTestHelper(parser);

            // when
            helper.parseConstraints(declarations);

            // then
            helper.requireAllEquals(expectedSuchThat, expectedPattern, expectedWith);
    };

    // Empty clause lists used for testing
    list<shared_ptr<RelationshipClause>> emptySuchThat = list<shared_ptr<RelationshipClause>>{};
    list<shared_ptr<PatternClause>> emptyPattern = list<shared_ptr<PatternClause>>{};
    list<shared_ptr<WithClause>> emptyWith = list<shared_ptr<WithClause>>{};


    // Follows clauses
    shared_ptr<RelationshipClause> followsClauseS2andA1 = shared_ptr<RelationshipClause>(
        new FollowsClause(s2Arg, a1Arg));
    shared_ptr<RelationshipClause> followsTClauseA1andW1 = shared_ptr<RelationshipClause>(
        new FollowsTClause(a1Arg, w1Arg));
    shared_ptr<RelationshipClause> followsTClauseS1and5 = shared_ptr<RelationshipClause>(
        new FollowsTClause(s1Arg, fiveLineNumberArg));

    SECTION("Multiple Follows/Follows*") {
        list<PQLToken> tokens = list<PQLToken>{
            such, that, follows, openBracket, s2, comma, a1, closeBracket,
            such, that, follows, star, openBracket, a1, comma, w1, closeBracket,
            such, that, follows, star, openBracket, s1, comma, five, closeBracket,
        };

        list<shared_ptr<RelationshipClause>> expectedSuchThat = list<shared_ptr<RelationshipClause>>{
            followsClauseS2andA1, 
            followsTClauseA1andW1,
            followsTClauseS1and5,
        };
        testParseNoError(tokens, declarations, expectedSuchThat, emptyPattern, emptyWith);
    }

    // Parent clauses
    shared_ptr<RelationshipClause> parentTClauseIf1andPrint1 = shared_ptr<RelationshipClause>(
        new ParentTClause(i1Arg, pri1Arg));
    shared_ptr<RelationshipClause> parentClauseWildcardandIf1 = shared_ptr<RelationshipClause>(
        new ParentClause(wildcardArg, i1Arg));
    shared_ptr<RelationshipClause> parentClausePrint1andIf1 = shared_ptr<RelationshipClause>(
        new ParentClause(pri1Arg, i1Arg));

    SECTION("Multiple Parent/Parent*") {
        list<PQLToken> tokens = list<PQLToken>{
            such, that, parent, star, openBracket, i1, comma, pri1, closeBracket,
            such, that, parent, openBracket, wildcard, comma, i1, closeBracket,
            such, that, parent, openBracket, pri1, comma, i1, closeBracket,
        };

        list<shared_ptr<RelationshipClause>> expectedSuchThat = list<shared_ptr<RelationshipClause>>{
            parentTClauseIf1andPrint1,
            parentClauseWildcardandIf1,
            parentClausePrint1andIf1,
        };

        testParseNoError(tokens, declarations, expectedSuchThat, emptyPattern, emptyWith);
    }

    // Modifies clauses
    shared_ptr<RelationshipClause> modifiesSClause5andVariable1 = shared_ptr<RelationshipClause>(
        new ModifiesSClause(fiveLineNumberArg, v1Arg));
    shared_ptr<RelationshipClause> modifiesSClauseRead1andQuotedName = shared_ptr<RelationshipClause>(
        new ModifiesSClause(r1Arg, quotedNameArg));
    shared_ptr<RelationshipClause> modifiesPClauseQuotedNameandWildcard = shared_ptr<RelationshipClause>(
        new ModifiesPClause(quotedNameArg, wildcardArg));
    shared_ptr<RelationshipClause> modifiesPClauseProcedure1andVariable1 = shared_ptr<RelationshipClause>(
        new ModifiesPClause(proc1Arg, v1Arg));

    SECTION("Multiple ModifiesS/ModifiesP") {
        list<PQLToken> tokens = list<PQLToken>{
            such, that, modifies, openBracket, five, comma, v1, closeBracket,
            such, that, modifies, openBracket, r1, comma, quotationMark, name, quotationMark, closeBracket,
            such, that, modifies, openBracket, quotationMark, name, quotationMark, comma, wildcard, closeBracket,
            such, that, modifies, openBracket, proc1, comma, v1, closeBracket,
        };

        list<shared_ptr<RelationshipClause>> expectedSuchThat = list<shared_ptr<RelationshipClause>>{
            modifiesSClause5andVariable1,
            modifiesSClauseRead1andQuotedName,
            modifiesPClauseQuotedNameandWildcard,
            modifiesPClauseProcedure1andVariable1,
        };

        testParseNoError(tokens, declarations, expectedSuchThat, emptyPattern, emptyWith);
    }

    // Uses clauses
    shared_ptr<RelationshipClause> usesPClauseQuotedNameandQuotedName = shared_ptr<RelationshipClause>(
        new UsesPClause(quotedNameArg, quotedNameArg));
    shared_ptr<RelationshipClause> usesPClauseProcedure1andVariable1 = shared_ptr<RelationshipClause>(
        new UsesPClause(proc1Arg, v1Arg));
    shared_ptr<RelationshipClause> usesSClauseIf1andWildcard = shared_ptr<RelationshipClause>(
        new UsesSClause(i1Arg, wildcardArg));
    shared_ptr<RelationshipClause> usesSClausePrint1andVariable1 = shared_ptr<RelationshipClause>(
        new UsesSClause(pri1Arg, v1Arg));

    SECTION("Multiple UsesS/UsesP") {
        list<PQLToken> tokens = list<PQLToken>{
            such, that, uses, openBracket, quotationMark, name, quotationMark, comma, quotationMark, name, quotationMark, closeBracket,
            such, that, uses, openBracket, proc1, comma, v1, closeBracket,
            such, that, uses, openBracket, i1, comma, wildcard, closeBracket,
            such, that, uses, openBracket, pri1, comma, v1, closeBracket,
        };

        list<shared_ptr<RelationshipClause>> expectedSuchThat = list<shared_ptr<RelationshipClause>>{
            usesPClauseQuotedNameandQuotedName,
            usesPClauseProcedure1andVariable1,
            usesSClauseIf1andWildcard,
            usesSClausePrint1andVariable1,
        };

        testParseNoError(tokens, declarations, expectedSuchThat, emptyPattern, emptyWith);
    }

    // Calls clauses
    shared_ptr<RelationshipClause> callsTClauseProc1andProc3 = shared_ptr<RelationshipClause>(
        new CallsTClause(proc1Arg, proc3Arg));
    shared_ptr<RelationshipClause> callsClauseWildcardandProc2 = shared_ptr<RelationshipClause>(
        new CallsClause(wildcardArg, proc2Arg));
    shared_ptr<RelationshipClause> callsClauseProc2andProc3 = shared_ptr<RelationshipClause>(
        new CallsClause(proc2Arg, proc3Arg));

    SECTION("Multiple Calls/Calls*") {
        list<PQLToken> tokens = list<PQLToken>{
            such, that, calls, star, openBracket, proc1, comma, proc3, closeBracket,
            such, that, calls, openBracket, wildcard, comma, proc2, closeBracket,
            such, that, calls, openBracket, proc2, comma, proc3, closeBracket,
        };

        list<shared_ptr<RelationshipClause>> expectedSuchThat = list<shared_ptr<RelationshipClause>>{
            callsTClauseProc1andProc3,
            callsClauseWildcardandProc2,
            callsClauseProc2andProc3,
        };

        testParseNoError(tokens, declarations, expectedSuchThat, emptyPattern, emptyWith);
    }

    SECTION("All of the above such that clauses") {
        list<PQLToken> tokens = list<PQLToken>{
            such, that, calls, star, openBracket, proc1, comma, proc3, closeBracket,
            such, that, calls, openBracket, wildcard, comma, proc2, closeBracket,
            such, that, calls, openBracket, proc2, comma, proc3, closeBracket,
            such, that, follows, openBracket, s2, comma, a1, closeBracket,
            such, that, follows, star, openBracket, a1, comma, w1, closeBracket,
            such, that, follows, star, openBracket, s1, comma, five, closeBracket,
            such, that, parent, star, openBracket, i1, comma, pri1, closeBracket,
            such, that, parent, openBracket, wildcard, comma, i1, closeBracket,
            such, that, parent, openBracket, pri1, comma, i1, closeBracket,
            such, that, modifies, openBracket, five, comma, v1, closeBracket,
            such, that, modifies, openBracket, r1, comma, quotationMark, name, quotationMark, closeBracket,
            such, that, modifies, openBracket, quotationMark, name, quotationMark, comma, wildcard, closeBracket,
            such, that, modifies, openBracket, proc1, comma, v1, closeBracket,
            such, that, uses, openBracket, quotationMark, name, quotationMark, comma, quotationMark, name, quotationMark, closeBracket,
            such, that, uses, openBracket, proc1, comma, v1, closeBracket,
            such, that, uses, openBracket, i1, comma, wildcard, closeBracket,
            such, that, uses, openBracket, pri1, comma, v1, closeBracket,
        };

        list<shared_ptr<RelationshipClause>> expectedSuchThat = list<shared_ptr<RelationshipClause>>{
            callsTClauseProc1andProc3,
            callsClauseWildcardandProc2,
            callsClauseProc2andProc3,
            followsClauseS2andA1,
            followsTClauseA1andW1,
            followsTClauseS1and5,
            parentTClauseIf1andPrint1,
            parentClauseWildcardandIf1,
            parentClausePrint1andIf1,
            modifiesSClause5andVariable1,
            modifiesSClauseRead1andQuotedName,
            modifiesPClauseQuotedNameandWildcard,
            modifiesPClauseProcedure1andVariable1,
            usesPClauseQuotedNameandQuotedName,
            usesPClauseProcedure1andVariable1,
            usesSClauseIf1andWildcard,
            usesSClausePrint1andVariable1,
        };

        testParseNoError(tokens, declarations, expectedSuchThat, emptyPattern, emptyWith);
    }

    // Pattern Assign Clauses

    shared_ptr<PatternClause> patternAssign1WildcardAndWildcard = shared_ptr<PatternClause>(
        new PatternAssignClause(a1Arg, wildcardArg, wildcardArg));

    //pattern a1(v1,_"name"_)
    shared_ptr<PatternClause> patternAssign1Variable1AndWildcardSurroundedName = shared_ptr<PatternClause>(
        new PatternAssignClause(a1Arg, v1Arg, ClauseArgument::createPatternStringWithWildcardsArg("_name_")));

    //pattern a1(v1,"5")
    shared_ptr<PatternClause> patternAssign1Variable1AndFive = shared_ptr<PatternClause>(
        new PatternAssignClause(a1Arg, v1Arg, ClauseArgument::createPatternStringArg("5")));

    //pattern a2(_,_"5 / name"_)
    shared_ptr<PatternClause> patternAssign2WildcardAndWildcardSurroundedDiv = shared_ptr<PatternClause>(
        new PatternAssignClause(a2Arg, wildcardArg, ClauseArgument::createPatternStringWithWildcardsArg("_5 name /_")));

    //pattern a2(_,"name * name - 5")
    shared_ptr<PatternClause> patternAssign2Variable1AndMultMinus = shared_ptr<PatternClause>(
        new PatternAssignClause(a2Arg, v1Arg, ClauseArgument::createPatternStringArg("name name * 5 -")));

    //pattern a2(_,"name + ((5))")
    shared_ptr<PatternClause> patternAssign2Variable1AndPlusBracket = shared_ptr<PatternClause>(
        new PatternAssignClause(a2Arg, v1Arg, ClauseArgument::createPatternStringArg("name 5 +")));

    SECTION("Many many pattern assigns") {
        list<PQLToken> tokens = list<PQLToken>{
            pattern, a1, openBracket, wildcard, comma, wildcard, closeBracket,
            pattern, a1, openBracket, v1, comma, wildcard, quotationMark, name, quotationMark, wildcard, closeBracket,
            pattern, a1, openBracket, v1, comma, quotationMark, five, quotationMark, closeBracket,
            pattern, a2, openBracket, wildcard, comma, wildcard, quotationMark, five, divideToken, name, quotationMark, wildcard, closeBracket,
            pattern, a2, openBracket, v1, comma, quotationMark, name, multiplyToken, name, minusToken, five, quotationMark, closeBracket,
            pattern, a2, openBracket, v1, comma, 
                quotationMark, name, plusToken, openBracket, openBracket, five, closeBracket, closeBracket, quotationMark, closeBracket,
        };

        list<shared_ptr<PatternClause>> expectedPattern = list<shared_ptr<PatternClause>>{
            patternAssign1WildcardAndWildcard,
            patternAssign1Variable1AndWildcardSurroundedName,
            patternAssign1Variable1AndFive,
            patternAssign2WildcardAndWildcardSurroundedDiv,
            patternAssign2Variable1AndMultMinus,
            patternAssign2Variable1AndPlusBracket,
        };

        testParseNoError(tokens, declarations, emptySuchThat, expectedPattern, emptyWith);
    }

    // With Clauses
    shared_ptr<WithClause> with25 = make_shared<WithBothExactClause>(twoLineNumberArg, fiveLineNumberArg);
    shared_ptr<WithClause> withNameName = make_shared<WithBothExactClause>(quotedNameArg, quotedNameArg);
    
    shared_ptr<WithClause> withNameProc1ProcName = make_shared<WithOneExactClause>(quotedNameArg,
        vector<ClauseArgument>{proc1Arg, ClauseArgument::createProcNameAttributeArg(proc1Arg)});
    shared_ptr<WithClause> withPri1StmtNum5 = make_shared<WithOneExactClause>(fiveLineNumberArg,
        vector<ClauseArgument>{pri1Arg, ClauseArgument::createStmtNumAttributeArg(pri1Arg)});

    shared_ptr<WithClause> withR1VarNameCall1ProcName = make_shared<WithNoExactClause>(
        vector<ClauseArgument>{r1Arg, ClauseArgument::createVarNameAttributeArg(r1Arg)},
        vector<ClauseArgument>{call1Arg, ClauseArgument::createProcNameAttributeArg(call1Arg)});
    shared_ptr<WithClause> withConst1ValueS1StmtNum = make_shared<WithNoExactClause>(
        vector<ClauseArgument>{const1Arg, ClauseArgument::createValueAttributeArg(const1Arg)},
        vector<ClauseArgument>{s1Arg, ClauseArgument::createStmtNumAttributeArg(s1Arg)});

    SECTION("Multiple withs") {
        list<PQLToken> tokens = list<PQLToken>{
            with, two, equals, five,
            with, quotationMark, name, quotationMark, equals, quotationMark, name, quotationMark,
            with, quotationMark, name, quotationMark, equals, proc1, dot, procName,
            with, pri1, dot, stmtNumStmt, stmtNumHash, equals, five,
            with, r1, dot, varName, equals, call1, dot, procName,
            with, const1, dot, value, equals, s1, dot, stmtNumStmt, stmtNumHash,
        };

        list<shared_ptr<WithClause>> expectedWith = list<shared_ptr<WithClause>>{
            with25, withNameName, withNameProc1ProcName, withPri1StmtNum5, 
            withR1VarNameCall1ProcName, withConst1ValueS1StmtNum
        };

        testParseNoError(tokens, declarations, emptySuchThat, emptyPattern, expectedWith);
    }

    SECTION("Everything everywhere all at once") {
        list<PQLToken> tokens = list<PQLToken>{
            such, that, uses, openBracket, proc1, comma, v1, closeBracket,
            with, two, equals, five,
            with, quotationMark, name, quotationMark, equals, quotationMark, name, quotationMark,
            such, that, parent, star, openBracket, i1, comma, pri1, closeBracket,
            pattern, a1, openBracket, v1, comma, wildcard, quotationMark, name, quotationMark, wildcard, closeBracket,
            with, const1, dot, value, equals, s1, dot, stmtNumStmt, stmtNumHash,
            such, that, follows, star, openBracket, a1, comma, w1, closeBracket,
            pattern, a1, openBracket, wildcard, comma, wildcard, closeBracket,
            with, r1, dot, varName, equals, call1, dot, procName,
            such, that, parent, openBracket, wildcard, comma, i1, closeBracket,
            pattern, a2, openBracket, v1, comma, quotationMark, name, multiplyToken, name, minusToken, five, quotationMark, closeBracket,
            pattern, a2, openBracket, v1, comma,
                quotationMark, name, plusToken, openBracket, openBracket, five, closeBracket, closeBracket, quotationMark, closeBracket,
            such, that, modifies, openBracket, proc1, comma, v1, closeBracket,
            with, pri1, dot, stmtNumStmt, stmtNumHash, equals, five,
            such, that, uses, openBracket, i1, comma, wildcard, closeBracket,
            such, that, calls, star, openBracket, proc1, comma, proc3, closeBracket,
            such, that, calls, openBracket, proc2, comma, proc3, closeBracket,
            with, quotationMark, name, quotationMark, equals, proc1, dot, procName,
           
        };

        list<shared_ptr<RelationshipClause>> expectedSuchThat = list<shared_ptr<RelationshipClause>>{
           usesPClauseProcedure1andVariable1,
           parentTClauseIf1andPrint1,
           followsTClauseA1andW1,
           parentClauseWildcardandIf1,
           modifiesPClauseProcedure1andVariable1,
           usesSClauseIf1andWildcard,
           callsTClauseProc1andProc3,
           callsClauseProc2andProc3,
        };

        list<shared_ptr<PatternClause>> expectedPattern = list<shared_ptr<PatternClause>>{
            patternAssign1Variable1AndWildcardSurroundedName,
            patternAssign1WildcardAndWildcard,
            patternAssign2Variable1AndMultMinus,
            patternAssign2Variable1AndPlusBracket,
        };

        list<shared_ptr<WithClause>> expectedWith = list<shared_ptr<WithClause>>{
            with25, 
            withNameName, 
            withConst1ValueS1StmtNum, 
            withR1VarNameCall1ProcName, 
            withPri1StmtNum5,
            withNameProc1ProcName,
        };

        testParseNoError(tokens, declarations, expectedSuchThat, expectedPattern, expectedWith);
    }
}