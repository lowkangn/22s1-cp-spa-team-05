#include <sp/parser/SimpleSyntaxParserManager.h>
#include <sp/dataclasses/ASTNode.h>
#include <sp/dataclasses/tokens/Token.h>
#include <sp/parser/rules/ProgramSimpleSyntaxRule.h>

#include <list>
#include <string>
#include <memory>
using namespace std;


shared_ptr<ASTNode> ParserManager::parse() {
	/*
		Main logic of this method: 
		1. We wrap all tokens into the program rule.
		2. Then we call validation and construction on the program rule.

		The main idea on why it works is that the parser simply organizes the tokens into the program rule, 
		and then lets the recursive definitions of the rules themselves to handle what to do.
	*/

	// create program rule
	ProgramSimpleSyntaxRule programRule = ProgramSimpleSyntaxRule();
	programRule.consumeTokens(this->tokens);

	// recursively call validation
	programRule.validate();

	// recursively call construct
	shared_ptr<ASTNode> programNode = programRule.constructNode();

	return programNode;
}
