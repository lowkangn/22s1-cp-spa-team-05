#include <sp/dataclasses/ast/BracketsASTNode.h>
#include <sp/dataclasses/design_objects/Entity.h>

Entity BracketsASTNode::extractEntity() {
    return Entity::createBracketEntity();
}
