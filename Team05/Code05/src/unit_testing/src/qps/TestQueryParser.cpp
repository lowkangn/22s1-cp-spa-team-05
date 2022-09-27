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
    string stmt1String = "s1";
    string stmt2String = "s2";
    string assign1String = "a1";
    string assign2String = "a2";
    string while1String = "w1";
    string if1String = "i1";
    string read1String = "r1";
    string print1String = "pr1";
    string procedure1String = "proc1";
    string constant1String = "c1";
    string variable1String = "v1";
    string variable2String = "v2";

    // tokens for synonyms
    PQLToken stmt1 = PQLToken::createNameToken(stmt1String);
    PQLToken stmt2 = PQLToken::createNameToken(stmt2String);
    PQLToken assign1 = PQLToken::createNameToken(assign1String);
    PQLToken assign2 = PQLToken::createNameToken(assign2String);
    PQLToken while1 = PQLToken::createNameToken(while1String);
    PQLToken if1 = PQLToken::createNameToken(if1String);
    PQLToken read1 = PQLToken::createNameToken(read1String);
    PQLToken print1 = PQLToken::createNameToken(print1String);
    PQLToken procedure1 = PQLToken::createNameToken(procedure1String);
    PQLToken constant1 = PQLToken::createNameToken(constant1String);
    PQLToken variable1 = PQLToken::createNameToken(variable1String);
    PQLToken variable2 = PQLToken::createNameToken(variable2String);

    // we use the same set of declarations for all cases
    unordered_map<string, ArgumentType> declarations = unordered_map<string, ArgumentType>{
        { stmt1String, ArgumentType::STMT },
        { assign1String, ArgumentType::ASSIGN },
        { stmt2String, ArgumentType::STMT },
        { assign2String, ArgumentType::ASSIGN },
        { while1String, ArgumentType::WHILE},
        { if1String, ArgumentType::IF },
        { read1String, ArgumentType::READ },
        { print1String, ArgumentType::PRINT },
        { procedure1String, ArgumentType::PROCEDURE },
        { constant1String, ArgumentType::CONSTANT },
        { variable1String, ArgumentType::VARIABLE },
        { variable2String, ArgumentType::VARIABLE },
    };

    // synonym ClauseArguments
    ClauseArgument stmt1Arg = ClauseArgument::createStmtArg(stmt1String);
    ClauseArgument stmt2Arg = ClauseArgument::createStmtArg(stmt2String);
    ClauseArgument assign1Arg = ClauseArgument::createAssignArg(assign1String);
    ClauseArgument assign2Arg = ClauseArgument::createAssignArg(assign2String);
    ClauseArgument while1Arg = ClauseArgument::createWhileArg(while1String);
    ClauseArgument if1Arg = ClauseArgument::createIfArg(if1String);
    ClauseArgument read1Arg = ClauseArgument::createReadArg(read1String);
    ClauseArgument print1Arg = ClauseArgument::createPrintArg(print1String);
    ClauseArgument procedure1Arg = ClauseArgument::createProcedureArg(procedure1String);
    ClauseArgument constant1Arg = ClauseArgument::createConstantArg(constant1String);
    ClauseArgument variable1Arg = ClauseArgument::createVariableArg(variable1String);
    ClauseArgument variable2Arg = ClauseArgument::createVariableArg(variable2String);

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
            procedure, procedure1, semicolon,
            select, procedure1,
        };

        shared_ptr<SelectClause> selectClause = shared_ptr<SelectClause>(new SelectClause(procedure1Arg));
        Query query = Query(selectClause, list<shared_ptr<RelationshipClause>>{}, list<shared_ptr<PatternClause>>{});

        testParseNoError(tokens, query);
    }

    SECTION("Select and such that clause") {
        list<PQLToken> tokens = list<PQLToken>{
            variable, variable1, comma, variable2, semicolon,
            select, variable2,
            such, that, modifies, openBracket, two, comma, variable1, closeBracket,
        };

        shared_ptr<SelectClause> selectClause = shared_ptr<SelectClause>(new SelectClause(variable2Arg));
        shared_ptr<RelationshipClause> modifiesClause = shared_ptr<RelationshipClause>(
            new ModifiesSClause(twoLineNumberArg, variable1Arg));
        Query query = Query(selectClause, list<shared_ptr<RelationshipClause>>{modifiesClause}, list<shared_ptr<PatternClause>>{});

        testParseNoError(tokens, query);

        tokens = list<PQLToken>{
            constant, constant1, semicolon, procedure, procedure1, semicolon,
            select, constant1,
            such, that, modifies, openBracket, procedure1, comma, quotationMark, name, quotationMark, closeBracket,
        };

        selectClause = shared_ptr<SelectClause>(new SelectClause(constant1Arg));
        modifiesClause = shared_ptr<RelationshipClause>(new ModifiesPClause(procedure1Arg, quotedNameArg));
        query = Query(selectClause, list<shared_ptr<RelationshipClause>>{modifiesClause}, list<shared_ptr<PatternClause>>{});

        testParseNoError(tokens, query);
    }

    SECTION("Select, many such that and pattern clauses") {
        list<PQLToken> tokens = list<PQLToken>{
            variable, variable1, comma, variable2, semicolon, 
            assign, assign1, comma, assign2, semicolon,
            procedure, procedure1, semicolon,
            if_, if1, semicolon,
            select, variable2,
            such, that, modifies, openBracket, two, comma, variable1, closeBracket,
            pattern, assign1, openBracket, variable1, comma, wildcard, quotationMark, name, quotationMark, wildcard, closeBracket,
            such, that, follows, star, openBracket, assign1, comma, if1, closeBracket,
            pattern, assign1, openBracket, wildcard, comma, wildcard, closeBracket,
            such, that, parent, openBracket, wildcard, comma, if1, closeBracket,
            such, that, uses, openBracket, procedure1, comma, wildcard, closeBracket,
            pattern, assign2, openBracket, variable1, comma, quotationMark, name, multiplyToken, name, minusToken, five, quotationMark, closeBracket,
        };

        shared_ptr<SelectClause> selectClause = shared_ptr<SelectClause>(new SelectClause(variable2Arg));

        shared_ptr<RelationshipClause> modifiesSClause = 
            shared_ptr<RelationshipClause>(new ModifiesSClause(twoLineNumberArg, variable1Arg));
        shared_ptr<RelationshipClause> followsTClause =
            shared_ptr<RelationshipClause>(new FollowsTClause(assign1Arg, if1Arg));
        shared_ptr<RelationshipClause> parentClause =
            shared_ptr<RelationshipClause>(new ParentClause(wildcardArg, if1Arg));
        shared_ptr<RelationshipClause> usesPClause =
            shared_ptr<RelationshipClause>(new UsesPClause(procedure1Arg, wildcardArg));
        list<shared_ptr<RelationshipClause>> expectedSuchThat = {
            modifiesSClause, followsTClause, parentClause, usesPClause,
        };
            
        shared_ptr<PatternClause> firstPatternAssign = shared_ptr<PatternClause>(
            new PatternAssignClause(assign1Arg, variable1Arg, ClauseArgument::createPatternStringWithWildcardsArg("_name_")));
        shared_ptr<PatternClause> secondPatternAssign = shared_ptr<PatternClause>(
            new PatternAssignClause(assign1Arg, wildcardArg, wildcardArg));
        shared_ptr<PatternClause> thirdPatternAssign = shared_ptr<PatternClause>(
            new PatternAssignClause(assign2Arg, variable1Arg, ClauseArgument::createPatternStringArg("name name * 5 -")));
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
            new ModifiesSClause(stmt1Arg, variable1Arg));

        list<PQLToken> tokens = list<PQLToken>{
            such, that, modifies, openBracket, stmt1, comma, variable1, closeBracket,
        };

        list<shared_ptr<RelationshipClause>> expected;
        expected.emplace_back(modifiesClause);
        testParseNoError(tokens, declarations, expected);
    }

    SECTION("Parent clause") {
        shared_ptr<RelationshipClause> parentTClause = shared_ptr<RelationshipClause>(
            new ParentTClause(stmt1Arg, fiveLineNumberArg));

        list<PQLToken> tokens = list<PQLToken>{
            such, that, parent, star, openBracket, stmt1, comma, five, closeBracket,
        };

        list<shared_ptr<RelationshipClause>> expected;
        expected.emplace_back(parentTClause);
        testParseNoError(tokens, declarations, expected);
    }

    SECTION("Uses clause") {
        shared_ptr<RelationshipClause> usesSClause = shared_ptr<RelationshipClause>(
            new UsesSClause(stmt1Arg, variable1Arg));

        list<PQLToken> tokens = list<PQLToken>{
            such, that, uses, openBracket, stmt1, comma, variable1, closeBracket,
        };

        list<shared_ptr<RelationshipClause>> expected;
        expected.emplace_back(usesSClause);
        testParseNoError(tokens, declarations, expected);
    }

    SECTION("Follows clause") {
        shared_ptr<RelationshipClause> followsTClause = shared_ptr<RelationshipClause>(
            new FollowsTClause(stmt1Arg, fiveLineNumberArg));

        list<PQLToken> tokens = list<PQLToken>{
            such, that, follows, star, openBracket, stmt1, comma, five, closeBracket,
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
        new FollowsClause(stmt2Arg, assign1Arg));
    shared_ptr<RelationshipClause> followsTClauseA1andW1 = shared_ptr<RelationshipClause>(
        new FollowsTClause(assign1Arg, while1Arg));
    shared_ptr<RelationshipClause> followsTClauseS1and5 = shared_ptr<RelationshipClause>(
        new FollowsTClause(stmt1Arg, fiveLineNumberArg));

    SECTION("Multiple Follows/Follows*") {
        list<PQLToken> tokens = list<PQLToken>{
            such, that, follows, openBracket, stmt2, comma, assign1, closeBracket,
            such, that, follows, star, openBracket, assign1, comma, while1, closeBracket,
            such, that, follows, star, openBracket, stmt1, comma, five, closeBracket,
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
        new ParentTClause(if1Arg, print1Arg));
    shared_ptr<RelationshipClause> parentClauseWildcardandIf1 = shared_ptr<RelationshipClause>(
        new ParentClause(wildcardArg, if1Arg));
    shared_ptr<RelationshipClause> parentClausePrint1andIf1 = shared_ptr<RelationshipClause>(
        new ParentClause(print1Arg, if1Arg));

    SECTION("Multiple Parent/Parent*") {
        list<PQLToken> tokens = list<PQLToken>{
            such, that, parent, star, openBracket, if1, comma, print1, closeBracket,
            such, that, parent, openBracket, wildcard, comma, if1, closeBracket,
            such, that, parent, openBracket, print1, comma, if1, closeBracket,
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
        new ModifiesSClause(fiveLineNumberArg, variable1Arg));
    shared_ptr<RelationshipClause> modifiesSClauseRead1andQuotedName = shared_ptr<RelationshipClause>(
        new ModifiesSClause(read1Arg, quotedNameArg));
    shared_ptr<RelationshipClause> modifiesPClauseQuotedNameandWildcard = shared_ptr<RelationshipClause>(
        new ModifiesPClause(quotedNameArg, wildcardArg));
    shared_ptr<RelationshipClause> modifiesPClauseProcedure1andVariable1 = shared_ptr<RelationshipClause>(
        new ModifiesPClause(procedure1Arg, variable1Arg));

    SECTION("Multiple ModifiesS/ModifiesP") {
        list<PQLToken> tokens = list<PQLToken>{
            such, that, modifies, openBracket, five, comma, variable1, closeBracket,
            such, that, modifies, openBracket, read1, comma, quotationMark, name, quotationMark, closeBracket,
            such, that, modifies, openBracket, quotationMark, name, quotationMark, comma, wildcard, closeBracket,
            such, that, modifies, openBracket, procedure1, comma, variable1, closeBracket,
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
        new UsesPClause(procedure1Arg, variable1Arg));
    shared_ptr<RelationshipClause> usesSClauseIf1andWildcard = shared_ptr<RelationshipClause>(
        new UsesSClause(if1Arg, wildcardArg));
    shared_ptr<RelationshipClause> usesSClausePrint1andVariable1 = shared_ptr<RelationshipClause>(
        new UsesSClause(print1Arg, variable1Arg));

    SECTION("Multiple UsesS/UsesP") {
        list<PQLToken> tokens = list<PQLToken>{
            such, that, uses, openBracket, quotationMark, name, quotationMark, comma, quotationMark, name, quotationMark, closeBracket,
            such, that, uses, openBracket, procedure1, comma, variable1, closeBracket,
            such, that, uses, openBracket, if1, comma, wildcard, closeBracket,
            such, that, uses, openBracket, print1, comma, variable1, closeBracket,
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
            such, that, follows, openBracket, stmt2, comma, assign1, closeBracket,
            such, that, follows, star, openBracket, assign1, comma, while1, closeBracket,
            such, that, follows, star, openBracket, stmt1, comma, five, closeBracket,
            such, that, parent, star, openBracket, if1, comma, print1, closeBracket,
            such, that, parent, openBracket, wildcard, comma, if1, closeBracket,
            such, that, parent, openBracket, print1, comma, if1, closeBracket,
            such, that, modifies, openBracket, five, comma, variable1, closeBracket,
            such, that, modifies, openBracket, read1, comma, quotationMark, name, quotationMark, closeBracket,
            such, that, modifies, openBracket, quotationMark, name, quotationMark, comma, wildcard, closeBracket,
            such, that, modifies, openBracket, procedure1, comma, variable1, closeBracket,
            such, that, uses, openBracket, quotationMark, name, quotationMark, comma, quotationMark, name, quotationMark, closeBracket,
            such, that, uses, openBracket, procedure1, comma, variable1, closeBracket,
            such, that, uses, openBracket, if1, comma, wildcard, closeBracket,
            such, that, uses, openBracket, print1, comma, variable1, closeBracket,
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
        new PatternAssignClause(assign1Arg, wildcardArg, wildcardArg));

    //pattern a1(v1,_"name"_)
    shared_ptr<PatternClause> patternAssign1Variable1AndWildcardSurroundedName = shared_ptr<PatternClause>(
        new PatternAssignClause(assign1Arg, variable1Arg, ClauseArgument::createPatternStringWithWildcardsArg("_name_")));

    //pattern a1(v1,"5")
    shared_ptr<PatternClause> patternAssign1Variable1AndFive = shared_ptr<PatternClause>(
        new PatternAssignClause(assign1Arg, variable1Arg, ClauseArgument::createPatternStringArg("5")));

    //pattern a2(_,_"5 / name"_)
    shared_ptr<PatternClause> patternAssign2WildcardAndWildcardSurroundedDiv = shared_ptr<PatternClause>(
        new PatternAssignClause(assign2Arg, wildcardArg, ClauseArgument::createPatternStringWithWildcardsArg("_5 name /_")));

    //pattern a2(_,"name * name - 5")
    shared_ptr<PatternClause> patternAssign2Variable1AndMultMinus = shared_ptr<PatternClause>(
        new PatternAssignClause(assign2Arg, variable1Arg, ClauseArgument::createPatternStringArg("name name * 5 -")));

    //pattern a2(_,"name + ((5))")
    shared_ptr<PatternClause> patternAssign2Variable1AndPlusBracket = shared_ptr<PatternClause>(
        new PatternAssignClause(assign2Arg, variable1Arg, ClauseArgument::createPatternStringArg("name 5 +")));

    SECTION("Many many pattern assigns") {
        list<PQLToken> tokens = list<PQLToken>{
            pattern, assign1, openBracket, wildcard, comma, wildcard, closeBracket,
            pattern, assign1, openBracket, variable1, comma, wildcard, quotationMark, name, quotationMark, wildcard, closeBracket,
            pattern, assign1, openBracket, variable1, comma, quotationMark, five, quotationMark, closeBracket,
            pattern, assign2, openBracket, wildcard, comma, wildcard, quotationMark, five, divideToken, name, quotationMark, wildcard, closeBracket,
            pattern, assign2, openBracket, variable1, comma, quotationMark, name, multiplyToken, name, minusToken, five, quotationMark, closeBracket,
            pattern, assign2, openBracket, variable1, comma, 
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
            such, that, uses, openBracket, procedure1, comma, variable1, closeBracket,
            such, that, parent, star, openBracket, if1, comma, print1, closeBracket,
            pattern, assign1, openBracket, variable1, comma, wildcard, quotationMark, name, quotationMark, wildcard, closeBracket,
            such, that, follows, star, openBracket, assign1, comma, while1, closeBracket,
            pattern, assign1, openBracket, wildcard, comma, wildcard, closeBracket,
            such, that, parent, openBracket, wildcard, comma, if1, closeBracket,
            pattern, assign2, openBracket, variable1, comma, quotationMark, name, multiplyToken, name, minusToken, five, quotationMark, closeBracket,
            pattern, assign2, openBracket, variable1, comma,
                quotationMark, name, plusToken, openBracket, openBracket, five, closeBracket, closeBracket, quotationMark, closeBracket,
            such, that, modifies, openBracket, procedure1, comma, variable1, closeBracket,
            such, that, uses, openBracket, if1, comma, wildcard, closeBracket,
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