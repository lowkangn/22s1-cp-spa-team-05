#include <sp/parser/Rule.h>
#include <sp/dataclasses/ASTNode.h>

using namespace std;

void Rule::validate() {

	// generate if needed
	if (!this->generated) {
		this->childRules = this->generateChildRules();
	} 

	// for each rule, recursively validate
	for (Rule rule : this->childRules) {
		rule.validate();
	}
}

ASTNode Rule::constructNode() {

	// generate if needed
	if (!this->generated) {
		this->childRules = this->generateChildRules();
	}

	// create current node
	ASTNode *node = new ASTNode(this->tokens);

	// for each rule, recursively create children and assign to children
	for (Rule rule : this->childRules) {
		ASTNode child = rule.constructNode();
		node->addChild(child);
	}

	return *node;
}