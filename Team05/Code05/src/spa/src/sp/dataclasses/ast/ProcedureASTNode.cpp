#include <sp/dataclasses/ast/ProcedureASTNode.h>
#include <sp/dataclasses/design_objects/Entity.h>

Entity ProcedureASTNode::extractEntity() {
    Token procedureToken = this->getToken();
    return Entity::createProcedureEntity(procedureToken);
}
