#include "catch.hpp"

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
public:
    QueryParserTestHelper(const QueryParser& parser) : parserUnderTest(parser) {}

    void parseConstraints(unordered_map<string, ArgumentType> declarations) {
        this->parserUnderTest.parseConstraints(declarations);
    }

    void requireSuchThatClausesEquals(list<shared_ptr<RelationshipClause>> expectedSuchThat) {
        list<shared_ptr<RelationshipClause>> actualSuchThat = this->parserUnderTest.suchThatClauses;
        bool isEqual = actualSuchThat.size() == expectedSuchThat.size();
        REQUIRE(isEqual);
        if (isEqual) {
            while (!actualSuchThat.empty()) {
                shared_ptr<RelationshipClause> actualPtr = actualSuchThat.front();
                shared_ptr<RelationshipClause> expectedPtr = expectedSuchThat.front();

                REQUIRE((*expectedPtr).equals(actualPtr));
                actualSuchThat.pop_front();
                expectedSuchThat.pop_front();
            }
        }
    }

    void requirePatternClausesEquals(list<shared_ptr<PatternClause>> expectedPattern) {
        list<shared_ptr<PatternClause>> actualPattern = this->parserUnderTest.patternClauses;
        bool isEqual = actualPattern.size() == expectedPattern.size();
        REQUIRE(isEqual);
        if (isEqual) {
            while (!actualPattern.empty()) {
                shared_ptr<PatternClause> actualPtr = actualPattern.front();
                shared_ptr<PatternClause> expectedPtr = expectedPattern.front();
                bool equals = (*expectedPtr).equals(actualPtr);
                REQUIRE(equals);
                actualPattern.pop_front();
                expectedPattern.pop_front();
            }
        }
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

    PQLToken modifies = PQLToken::createNameToken("Modifies");
    PQLToken uses = PQLToken::createNameToken("Uses");
    PQLToken follows = PQLToken::createNameToken("Follows");
    PQLToken parent = PQLToken::createNameToken("Parent");
    PQLToken star = PQLToken::createOperatorToken("*");

    // strings for tokens, declarations and ClauseArguments
    string s1String = "s1";
    string s2String = "s2";
    string a1String = "a1";
    string a2String = "a2";
    string w1String = "w1";
    string i1String = "i1";
    string r1String = "r1";
    string pri1String = "pri1";
    string proc1String = "proc1";
    string c1String = "c1";
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
    PQLToken proc1 = PQLToken::createNameToken(proc1String);
    PQLToken c1 = PQLToken::createNameToken(c1String);
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
        { proc1String, ArgumentType::PROCEDURE },
        { c1String, ArgumentType::CONSTANT },
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
    ClauseArgument proc1Arg = ClauseArgument::createProcedureArg(proc1String);
    ClauseArgument c1Arg = ClauseArgument::createConstantArg(c1String);
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

    SECTION("Select clause only") {
        list<PQLToken> tokens = list<PQLToken>{
            procedure, proc1, semicolon,
            select, proc1,
        };

        shared_ptr<SelectClause> selectClause = shared_ptr<SelectClause>(new SelectClause(proc1Arg));
        Query query = Query(selectClause, list<shared_ptr<RelationshipClause>>{}, list<shared_ptr<PatternClause>>{});

        testParseNoError(tokens, query);
    }

    SECTION("Select and such that clause") {
        list<PQLToken> tokens = list<PQLToken>{
            variable, v1, comma, v2, semicolon,
            select, v2,
            such, that, modifies, openBracket, two, comma, v1, closeBracket,
        };

        shared_ptr<SelectClause> selectClause = shared_ptr<SelectClause>(new SelectClause(v2Arg));
        shared_ptr<RelationshipClause> modifiesClause = shared_ptr<RelationshipClause>(
            new ModifiesSClause(twoLineNumberArg, v1Arg));
        Query query = Query(selectClause, list<shared_ptr<RelationshipClause>>{modifiesClause}, list<shared_ptr<PatternClause>>{});

        testParseNoError(tokens, query);

        tokens = list<PQLToken>{
            constant, c1, semicolon, procedure, proc1, semicolon,
            select, c1,
            such, that, modifies, openBracket, proc1, comma, quotationMark, name, quotationMark, closeBracket,
        };

        selectClause = shared_ptr<SelectClause>(new SelectClause(c1Arg));
        modifiesClause = shared_ptr<RelationshipClause>(new ModifiesPClause(proc1Arg, quotedNameArg));
        query = Query(selectClause, list<shared_ptr<RelationshipClause>>{modifiesClause}, list<shared_ptr<PatternClause>>{});

        testParseNoError(tokens, query);
    }

    SECTION("Select, many such that and pattern clauses") {
        list<PQLToken> tokens = list<PQLToken>{
            variable, v1, comma, v2, semicolon, 
            assign, a1, comma, a2, semicolon,
            procedure, proc1, semicolon,
            if_, i1, semicolon,
            select, v2,
            such, that, modifies, openBracket, two, comma, v1, closeBracket,
            pattern, a1, openBracket, v1, comma, wildcard, quotationMark, name, quotationMark, wildcard, closeBracket,
            such, that, follows, star, openBracket, a1, comma, i1, closeBracket,
            pattern, a1, openBracket, wildcard, comma, wildcard, closeBracket,
            such, that, parent, openBracket, wildcard, comma, i1, closeBracket,
            such, that, uses, openBracket, proc1, comma, wildcard, closeBracket,
            pattern, a2, openBracket, v1, comma, quotationMark, name, multiplyToken, name, minusToken, five, quotationMark, closeBracket,
        };

        shared_ptr<SelectClause> selectClause = shared_ptr<SelectClause>(new SelectClause(v2Arg));

        shared_ptr<RelationshipClause> modifiesSClause = 
            shared_ptr<RelationshipClause>(new ModifiesSClause(twoLineNumberArg, v1Arg));
        shared_ptr<RelationshipClause> followsTClause =
            shared_ptr<RelationshipClause>(new FollowsTClause(a1Arg, i1Arg));
        shared_ptr<RelationshipClause> parentClause =
            shared_ptr<RelationshipClause>(new ParentClause(wildcardArg, i1Arg));
        shared_ptr<RelationshipClause> usesPClause =
            shared_ptr<RelationshipClause>(new UsesPClause(proc1Arg, wildcardArg));
        list<shared_ptr<RelationshipClause>> expectedSuchThat = {
            modifiesSClause, followsTClause, parentClause, usesPClause,
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

        Query query = Query(selectClause, expectedSuchThat, expectedPattern);

        testParseNoError(tokens, query);
    }
}

TEST_CASE("QueryParser: test parseConstraints single clause") {
    auto testParseNoError = [](list<PQLToken> tokens, 
        unordered_map<string, ArgumentType> declarations,
        list<shared_ptr<RelationshipClause>> expected) {
            // given
            QueryParser parser = QueryParser(tokens);
            QueryParserTestHelper helper = QueryParserTestHelper(parser);

            // when
            helper.parseConstraints(declarations);

            // then
            helper.requireSuchThatClausesEquals(expected);
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
}

TEST_CASE("QueryParser: test parseConstraints Multiple clauses") {
    auto testParseNoError = [](list<PQLToken> tokens,
        unordered_map<string, ArgumentType> declarations,
        list<shared_ptr<RelationshipClause>> expectedSuchThat,
        list<shared_ptr<PatternClause>> expectedPattern) {
            // given
            QueryParser parser = QueryParser(tokens);
            QueryParserTestHelper helper = QueryParserTestHelper(parser);

            // when
            helper.parseConstraints(declarations);

            // then
            helper.requireSuchThatClausesEquals(expectedSuchThat);
            helper.requirePatternClausesEquals(expectedPattern);
    };

    // Empty clause lists used for testing
    list<shared_ptr<RelationshipClause>> emptySuchThat = list<shared_ptr<RelationshipClause>>{};
    list<shared_ptr<PatternClause>> emptyPattern = list<shared_ptr<PatternClause>>{};


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

        testParseNoError(tokens, declarations, expectedSuchThat, emptyPattern);
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

        testParseNoError(tokens, declarations, expectedSuchThat, emptyPattern);
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

        testParseNoError(tokens, declarations, expectedSuchThat, emptyPattern);
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

        testParseNoError(tokens, declarations, expectedSuchThat, emptyPattern);
    }

    SECTION("All of the above such that clauses") {
        list<PQLToken> tokens = list<PQLToken>{
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

        testParseNoError(tokens, declarations, expectedSuchThat, emptyPattern);
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

        testParseNoError(tokens, declarations, emptySuchThat, expectedPattern);
    }

    SECTION("Everything everywhere all at once") {
        list<PQLToken> tokens = list<PQLToken>{
            such, that, uses, openBracket, proc1, comma, v1, closeBracket,
            such, that, parent, star, openBracket, i1, comma, pri1, closeBracket,
            pattern, a1, openBracket, v1, comma, wildcard, quotationMark, name, quotationMark, wildcard, closeBracket,
            such, that, follows, star, openBracket, a1, comma, w1, closeBracket,
            pattern, a1, openBracket, wildcard, comma, wildcard, closeBracket,
            such, that, parent, openBracket, wildcard, comma, i1, closeBracket,
            pattern, a2, openBracket, v1, comma, quotationMark, name, multiplyToken, name, minusToken, five, quotationMark, closeBracket,
            pattern, a2, openBracket, v1, comma,
                quotationMark, name, plusToken, openBracket, openBracket, five, closeBracket, closeBracket, quotationMark, closeBracket,
            such, that, modifies, openBracket, proc1, comma, v1, closeBracket,
            such, that, uses, openBracket, i1, comma, wildcard, closeBracket,
        };

        list<shared_ptr<RelationshipClause>> expectedSuchThat = list<shared_ptr<RelationshipClause>>{
           usesPClauseProcedure1andVariable1,
           parentTClauseIf1andPrint1,
           followsTClauseA1andW1,
           parentClauseWildcardandIf1,
           modifiesPClauseProcedure1andVariable1,
           usesSClauseIf1andWildcard,
        };

        list<shared_ptr<PatternClause>> expectedPattern = list<shared_ptr<PatternClause>>{
            patternAssign1Variable1AndWildcardSurroundedName,
            patternAssign1WildcardAndWildcard,
            patternAssign2Variable1AndMultMinus,
            patternAssign2Variable1AndPlusBracket,
        };

        testParseNoError(tokens, declarations, expectedSuchThat, expectedPattern);
    }

}