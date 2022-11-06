#include <sp/dataclasses/ast/AST.h>

bool ASTNode::equals(const shared_ptr<ASTNode> other) const {
    // Check if type are the same
    bool isTypeSame = this->type == other->type;

    // Check if line number is the same
    bool isLineNumberSame = this->lineNumber == other->lineNumber;

    // Check if tokens are the same
    bool isTokenSame = this->token == other->token;

    // Check if children are the same
    bool isChildrenLengthSame = this->children.size() == other->children.size();

    if (!isChildrenLengthSame) {
        return false;
    }

    if (this->children.size() == 0) {
        return isTokenSame && isLineNumberSame && isTypeSame;
    }

    for (int i = 0; i < this->children.size(); i++) {
        shared_ptr<ASTNode> child = this->children[i];
        shared_ptr<ASTNode> otherChild = other->getChildren()[i];
        if (!(child->equals(otherChild))) {
            return false;
        }
    }

    return isTypeSame && isLineNumberSame && isTokenSame;
}
