#pragma once

// imported libraries
#include <string>
#include <list>
#include <fstream>

using namespace std;

// imported local files
#include <common/FileIOManager.h>
#include <sp/dataclasses/tokens/Token.h>

class Lexer {

public:
	Lexer() {};
	/* 
		This method converts a string from a text file into a list of internally recognized tokens.
	 */
	list<Token> tokenize(istream &stream);

	/*
		Checks whether a given character is alphabetical.
	*/
	bool charIsAlphabetical(char c);
	
	/*
		Checks whether a given character is a 0-9 digit.
	*/
	bool charIsDigit(char c);
	/*
		Checks whether a given character type is a delimiter type (e.g. ;, })
	*/
	bool charIsDelimiter(char c);

	/*
		Checks whether a given character type is an operator
	*/
	bool charIsOperator(char c);

	/*
		
	*/
	/*
		Checks whether a given character is whitespace.
	*/
	bool charIsWhiteSpace(char c);

	/*#include <FileIOManager.h>

		Traverses stream to remove whitespace characters.
		NOTE: has side effects - modifies the stream by reference.
	*/
	void traverseStreamUntilNoWhiteSpace(istream& stream);

	/*
		Traverses stream to remove newlines.
		NOTE: has side effects - modifies the stream by reference.
	*/
	void traverseStreamUntilNoComment(istream& stream);

	/*
		Traverses the stream and creates a token out of it. 
		NOTE: has side effects - modifies the stream by reference.
	*/
	Token createKeywordOrNameTokenFromTraversingStream(istream& stream);

	/*
		Traverses the stream and creates a token out of it.
		NOTE: has side effects - modifies the stream by reference.
	*/
	Token createIntegerTokenFromTraversingStream(istream& stream);

	/*
		Traverses the stream and creates a token out of it.
		NOTE: has side effects - modifies the stream by reference.
	*/
	Token createDelimiterTokenFromTraversingStream(istream& stream);

	/*
		Traverses the stream and creates a token out of it.
		NOTE: has side effects - modifies the stream by reference.
	*/
	Token createOperatorTokenFromTraversingStream(istream& stream);

};