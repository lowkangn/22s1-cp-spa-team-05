#include "catch.hpp"
#include <common/PostFixConverter.h>

TEST_CASE("PostFixConverter: infix to postfix") {
	auto test = [](string infixString, string expectedString) {
		// Given
		PostFixConverter converter = PostFixConverter();

		// When
		string convertedString = converter.convertInfixToPostFix(infixString);

		// Then
		REQUIRE(convertedString == expectedString);
	};

	test("A+B*C+D", "ABC*+D+");
	test("x*y/(5*z)+2", "xy*5z*/2+");
	test("x*y/(5*z)+2%3", "xy*5z*/23%+");
	test("y+5%3", "y53%+");
	test("v+x*y+z*t", "vxy*+zt*+");
}