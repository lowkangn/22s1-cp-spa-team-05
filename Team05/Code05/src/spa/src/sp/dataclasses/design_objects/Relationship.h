#pragma once
#include <vector>
#include <sp/dataclasses/design_objects/Entity.h>
#include <sp/dataclasses/tokens/Token.h>



class Relationship {
private:
    enum class RelationshipType {
        MODIFIES = 0,
        USES = 1,
        FOLLOWS = 2,
        FOLLOWST = 3,
        PARENT = 4,
        PARENTT = 5,
        CALLS = 6,
        CALLST = 7,
        NEXT = 8,
        CALLSTMTATTRIBUTE = 9,
    };

    Entity entity1;
    Entity entity2;
    RelationshipType type;

    Relationship(Entity entity1, Entity entity2, RelationshipType type) : entity1(entity1), entity2(entity2), type(type) {
        this->entity1 = entity1;
        this->entity2 = entity2;
        this->type = type;
    }
public:
    static Relationship createModifiesRelationship(Entity& lhs, Entity& rhs) {
        return Relationship(lhs, rhs, RelationshipType::MODIFIES);
    }

    static Relationship createUsesRelationship(Entity& lhs, Entity& rhs) {
        return Relationship(lhs, rhs, RelationshipType::USES);
    }

    static Relationship createFollowsRelationship(Entity& lhs, Entity& rhs) {
        return Relationship(lhs, rhs, RelationshipType::FOLLOWS);
    }

    static Relationship createFollowsTRelationship(Entity& lhs, Entity& rhs) {
        return Relationship(lhs, rhs, RelationshipType::FOLLOWST);
    }

    static Relationship createParentRelationship(Entity& lhs, Entity& rhs) {
        return Relationship(lhs, rhs, RelationshipType::PARENT);
    }

    static Relationship createParentTRelationship(Entity& lhs, Entity& rhs) {
        return Relationship(lhs, rhs, RelationshipType::PARENTT);
    }

    static Relationship createCallsRelationship(Entity& lhs, Entity& rhs) {
        return Relationship(lhs, rhs, RelationshipType::CALLS);
    }

    static Relationship createCallsTRelationship(Entity& lhs, Entity& rhs) {
        return Relationship(lhs, rhs, RelationshipType::CALLST);
    }

    static Relationship createCallStmtAttributeRelationship(Entity& lhs, Entity& rhs) {
        return Relationship(lhs, rhs, RelationshipType::CALLSTMTATTRIBUTE);
    }

    static Relationship createNextRelationship(Entity& lhs, Entity& rhs) {
        return Relationship(lhs, rhs, RelationshipType::NEXT);
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

    static bool compareRelationship(Relationship relationshipOne, Relationship relationshipTwo) {
        if (relationshipOne.getType() == relationshipTwo.getType()) {
            return relationshipOne.getRhs().getLine() > relationshipTwo.getRhs().getLine();
        }
        return relationshipOne.getType() > relationshipTwo.getType();
    }

    bool equals(Relationship other) {
        return (this->type == other.type) && (this->entity1.equals(other.entity1)) && (this->entity2.equals(other.entity2));
    }

    bool operator==(Relationship other) {
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

    bool isCalls() {
        return this->type == RelationshipType::CALLS;
    }

    bool isCallsStar() {
        return this->type == RelationshipType::CALLST;
    }

    bool isCallsStmtAttribute() {
        return this->type == RelationshipType::CALLSTMTATTRIBUTE;
    }

    bool isNext() {
        return this->type == RelationshipType::NEXT;
    }
};