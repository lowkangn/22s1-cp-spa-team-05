#include "catch.hpp"
#include <common/PostFixConverter.h>
#include <iostream>

TEST_CASE("PostFixConverter: infix to postfix") {
	auto test = [](string infixString, string expectedString) {
		// Given
		PostFixConverter converter = PostFixConverter();

		// When
		string convertedString = converter.convertInfixToPostFix(infixString);

		// Then
		if (convertedString != expectedString) {
			cout << convertedString << endl;
			cout << expectedString << endl;
		}
		REQUIRE(convertedString == expectedString);
	};

	test("A + B * C + D", "A B C * + D +");
	test("x * y / ( 5 * z ) + 2", "x y * 5 z * / 2 +");
	test("x * y / ( 5 * z ) + 2 % 3", "x y * 5 z * / 2 3 % +");
	test("y + 5 % 3", "y 5 3 % +");
	test("v + x * y + z * t", "v x y * + z t * +");
}