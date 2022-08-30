#include <sp/parser/Rule.h>
#include <sp/dataclasses/ASTNode.h>

using namespace std;

void Rule::validate() {

	// generate if needed
	if (!this->generated) {
		this->rules = this->generateRules();
	} 

	// for each rule, recursively validate
	for (Rule rule : this->rules) {
		rule.validate();
	}
}

ASTNode Rule::constructNode() {

	// generate if needed
	if (!this->generated) {
		this->rules = this->generateRules();
	}

	// create current node
	ASTNode *node = new ASTNode(this->tokens);

	// for each rule, recursively create children and assign to children
	for (Rule rule : this->rules) {
		ASTNode child = rule.constructNode();
		node->addChild(child);
	}

	return *node;
}