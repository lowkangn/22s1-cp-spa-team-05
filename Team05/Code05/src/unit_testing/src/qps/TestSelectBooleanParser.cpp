#include "catch.hpp"
#include <qps/query_parser/parsers/SelectBooleanParser.h>

using namespace std;

// =============== UNIT TESTS ====================

TEST_CASE("SelectBooleanParser: test parseNoError") {
	auto testParseNoError = [](list<PQLToken> tokens,
							   unordered_map<string, ArgumentType> declarations,
							   SelectClause expected) {
		// given
		SelectBooleanParser parser = SelectBooleanParser(tokens, declarations);

		// when
		shared_ptr<SelectClause> actualPtr = parser.parse();

		// then
		REQUIRE(expected.equals(actualPtr.get()));
	};

	list<PQLToken> tokens = list<PQLToken>{
			PQLToken::createNameToken("BOOLEAN")
	};

	SelectClause expected = SelectClause::createBooleanSelectClause();

	testParseNoError(tokens, {}, expected);
}
