#include <sp/parser/rules/SimpleSyntaxRule.h>
#include <sp/dataclasses/ASTNode.h>

using namespace std;

void SimpleSyntaxRule::validate() {

	// generate if needed
	if (!this->generated) {
		this->childRules = this->generateChildRules();
	} 

	// for each rule, recursively validate
	for (SimpleSyntaxRule rule : this->childRules) {
		rule.validate();
	}
}

ASTNode SimpleSyntaxRule::constructNode() {

	// check that initialized correctly 
	if (!)

	// generate if needed
	if (!this->generated) {
		this->childRules = this->generateChildRules();
	}

	// create current node
	ASTNode *node = new ASTNode(this->tokens);

	// for each rule, recursively create children and assign to children
	for (SimpleSyntaxRule rule : this->childRules) {
		ASTNode child = rule.constructNode();
		node->addChild(child);
	}

	return *node;
}