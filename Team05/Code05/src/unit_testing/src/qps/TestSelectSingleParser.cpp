#include "catch.hpp"
#include <qps/query_parser/parsers/SelectSingleParser.h>

using namespace std;

// =============== UNIT TESTS ====================

TEST_CASE("SelectSingleParser: test parseNoError") {
    auto testParseNoError = [](list<PQLToken> tokens,
        unordered_map<string, ArgumentType> declarations,
        SelectClause expected) {
            // given
			SelectSingleParser parser = SelectSingleParser(tokens, declarations);

            // when
            shared_ptr<SelectClause> actualPtr = parser.parse();

            // then
            REQUIRE(expected.equals(actualPtr));
    };

    list<PQLToken> tokens = list<PQLToken>{
            PQLToken::createNameToken("v1")
    };

    unordered_map<string, ArgumentType> declarations = unordered_map<string, ArgumentType>{
        {"v1", ArgumentType::VARIABLE}
    };

	ClauseArgument varArg = ClauseArgument::createVariableArg("v1");
    SelectClause expected = SelectClause::createSynonymSelectClause({varArg});

    testParseNoError(tokens, declarations, expected);

    tokens = list<PQLToken>{
        PQLToken::createNameToken("abcdefgLongName")
    };

    declarations = unordered_map<string, ArgumentType>{{"abcdefgLongName", ArgumentType::CONSTANT}};
    ClauseArgument constArg = ClauseArgument::createConstantArg("abcdefgLongName");
	expected = SelectClause::createSynonymSelectClause({constArg});

    testParseNoError(tokens, declarations, expected);

    SECTION("BOOLEAN declared as a synonym") {
        // Select BOOLEAN as declared synonym should be parsed by SelectSingleParser
        tokens = list<PQLToken>{
                PQLToken::createNameToken("BOOLEAN")
        };

        declarations = unordered_map<string, ArgumentType>{ {"BOOLEAN", ArgumentType::VARIABLE} };
        varArg = ClauseArgument::createVariableArg("BOOLEAN");
        expected = SelectClause::createSynonymSelectClause({ varArg });

        testParseNoError(tokens, declarations, expected);
    }

    SECTION("Select an attribute") {
        tokens = list<PQLToken>{
            PQLToken::createNameToken("someProcedureName"),
            PQLToken::createOperatorToken("."),
            PQLToken::createNameToken("procName"),
        };
        declarations = unordered_map<string, ArgumentType>{ {"someProcedureName", ArgumentType::PROCEDURE}, };
        ClauseArgument procedureArg = ClauseArgument::createProcedureArg("someProcedureName");
        ClauseArgument procNameArg = ClauseArgument::createProcNameAttributeArg(procedureArg);
        expected = SelectClause::createSynonymSelectClause({ procedureArg, procNameArg });

        testParseNoError(tokens, declarations, expected);

        tokens = list<PQLToken>{
            PQLToken::createNameToken("rrr"),
            PQLToken::createOperatorToken("."),
            PQLToken::createNameToken("varName"),
        };
        declarations = unordered_map<string, ArgumentType>{ {"rrr", ArgumentType::READ}, };
        ClauseArgument readArg = ClauseArgument::createReadArg("rrr");
        ClauseArgument varNameArg = ClauseArgument::createVarNameAttributeArg(readArg);
        expected = SelectClause::createSynonymSelectClause({ readArg, varNameArg });

        testParseNoError(tokens, declarations, expected);

        tokens = list<PQLToken>{
            PQLToken::createNameToken("ppp"),
            PQLToken::createOperatorToken("."),
            PQLToken::createNameToken("stmt"),
            PQLToken::createDelimiterToken("#"),
        };
        declarations = unordered_map<string, ArgumentType>{ {"ppp", ArgumentType::PRINT}, };
        ClauseArgument printArg = ClauseArgument::createPrintArg("ppp");
        ClauseArgument stmtNumArg = ClauseArgument::createStmtNumAttributeArg(printArg);
        expected = SelectClause::createSynonymSelectClause({ printArg, stmtNumArg });

        testParseNoError(tokens, declarations, expected);
    }
}

TEST_CASE("SelectSingleParser: test parseWithSemanticError") {
    auto testParseWithSemanticError = [](list<PQLToken> tokens,
        unordered_map<string, ArgumentType> declarations) {
            // given
			SelectSingleParser parser = SelectSingleParser(tokens, declarations);

            // when
            parser.parse();

            // then
            REQUIRE(!parser.isSemanticallyValid());
    };

    SECTION("Undeclared / misspelled synonym") {
        list<PQLToken> tokens = list<PQLToken>{
            PQLToken::createNameToken("v1")
        };

        unordered_map<string, ArgumentType> declarations = unordered_map<string, ArgumentType>{
            {"v2", ArgumentType::VARIABLE}
        };

        testParseWithSemanticError(tokens, declarations);

        tokens = list<PQLToken>{
            PQLToken::createNameToken("abcdefgLongName")
        };

        declarations = unordered_map<string, ArgumentType>{
            {"v1", ArgumentType::VARIABLE},
            {"v2", ArgumentType::CONSTANT}
        };

        testParseWithSemanticError(tokens, declarations);
    }

    SECTION("Mistmatched attrRef") {
        list<PQLToken> tokens = list<PQLToken>{
            PQLToken::createNameToken("someProcedureName"),
            PQLToken::createOperatorToken("."),
            PQLToken::createNameToken("varName"),
        };
        unordered_map<string, ArgumentType> declarations = unordered_map<string, ArgumentType>{ {"someProcedureName", ArgumentType::PROCEDURE}, };
        testParseWithSemanticError(tokens, declarations);

        tokens = list<PQLToken>{
            PQLToken::createNameToken("rrr"),
            PQLToken::createOperatorToken("."),
            PQLToken::createNameToken("procName"),
        };
        declarations = unordered_map<string, ArgumentType>{ {"rrr", ArgumentType::READ}, };
        testParseWithSemanticError(tokens, declarations);

        tokens = list<PQLToken>{
            PQLToken::createNameToken("ppp"),
            PQLToken::createOperatorToken("."),
            PQLToken::createNameToken("value")
        };
        declarations = unordered_map<string, ArgumentType>{ {"ppp", ArgumentType::PRINT}, };
        testParseWithSemanticError(tokens, declarations);
    }
}
