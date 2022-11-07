#pragma once

#include <sp/dataclasses/ast/AST.h>
#include <sp/dataclasses/design_objects/Entity.h>
#include <sp/design_extractor/Extractor.h>

#include <memory>
#include <vector>

using namespace std;

class EntityExtractor : Extractor<Entity> {
public:
    EntityExtractor() {
    }

    /*
    This method is used to extract entities from a provided abstract syntax tree. It is meant to be
    overriden for each specific entity type.
    */
    vector<Entity> extract(shared_ptr<ASTNode> ast) override;
};
