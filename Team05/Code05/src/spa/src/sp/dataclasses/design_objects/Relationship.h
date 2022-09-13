#pragma once
#include <vector>
#include <sp/dataclasses/design_objects/Entity.h>
#include <sp/dataclasses/tokens/Token.h>

enum class RelationshipType {
    MODIFIES,
    USES,
    FOLLOWS,
    FOLLOWST,
    PARENT,
    PARENTT
};

class Relationship {
private:
    Entity entity1;
    Entity entity2;
    RelationshipType type;
public:
    Relationship(Entity entity1, Entity entity2, RelationshipType type) : entity1(entity1), entity2(entity2), type(type) {
        this->entity1 = entity1;
        this->entity2 = entity2;
        this->type = type;
    }

    Entity getLhs() {
        return this->entity1;
    }
    Entity getRhs() {
        return this->entity2;
    }

    RelationshipType getType() {
        return this->type;
    }

    bool equals(Relationship other) {
        return (this->type == other.type) && (this->entity1.equals(other.entity1)) && (this->entity2.equals(other.entity2));
    }

    bool isModifies() {
        return this->type == RelationshipType::MODIFIES;
    }
    bool isFollowsStar() {
        return this->type == RelationshipType::FOLLOWST;
    }
    bool isFollows() {
        return this->type == RelationshipType::FOLLOWS;
    }
    bool isParent() {
        return this->type == RelationshipType::PARENT;
    }
    bool isParentStar() {
        return this->type == RelationshipType::PARENTT;
    }
    bool isUses() {
        return this->type == RelationshipType::USES;
    }
};