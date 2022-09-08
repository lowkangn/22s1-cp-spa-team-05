#include <sp/dataclasses/ast/AST.h>

bool ASTNode::equals(shared_ptr<ASTNode> other) {
	// Check if type are the same
	bool typeCheck = this->type == other->type;

	// Check if line number is the same
	bool lineNumberCheck = this->lineNumber == other->lineNumber;


	// Check if tokens are the same
	bool tokenCheck = this->token.equals(other->token);

	// Check if children are the same
	bool childrenLengthCheck = this->children.size() == other->children.size();

	if (this->numChildren() == 0) {
		return tokenCheck && lineNumberCheck && typeCheck;
	}

	for (int i = 0; i < this->children.size(); i++) {
		shared_ptr<ASTNode> child = this->getChildren()[i];
		shared_ptr<ASTNode> otherChild = other->getChildren()[i];
		if (!(child->equals(otherChild))) {
			return false;
		}
	}

	return typeCheck && lineNumberCheck && tokenCheck && childrenLengthCheck;
}