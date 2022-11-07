#include <sp/dataclasses/ast/CallASTNode.h>

Entity CallASTNode::extractEntity() {
    int lineNumber = this->getLineNumber();
    return Entity::createCallEntity(lineNumber);
}
