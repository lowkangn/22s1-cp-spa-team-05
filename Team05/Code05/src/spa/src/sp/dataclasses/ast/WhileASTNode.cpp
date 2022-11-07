#include <sp/dataclasses/ast/WhileASTNode.h>

Entity WhileASTNode::extractEntity() {
    int lineNumber = this->getLineNumber();
    return Entity::createWhileEntity(lineNumber);
}
