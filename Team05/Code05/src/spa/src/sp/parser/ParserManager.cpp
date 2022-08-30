#include <sp/parser/ParserManager.h>
#include <sp/dataclasses/ASTNode.h>

using namespace std;

ASTNode ParserManager::parse() {
	// create dummy root node

	// while token pointer not at end
	// iterate through all tokens, with a switch case
		// case hit -> use relevant rule.create to wrap into object and shift token pointer
		// add to child of dummy root node
	return ASTNode(this->tokens);
}
