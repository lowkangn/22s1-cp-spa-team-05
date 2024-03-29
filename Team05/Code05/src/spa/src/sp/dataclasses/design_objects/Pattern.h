#pragma once
#include <sp/dataclasses/design_objects/Entity.h>
#include <sp/dataclasses/tokens/Token.h>

#include <string>
#include <vector>

using namespace std;

class Pattern {
private:
    Pattern(Entity entity, string lhs, string rhs)
        : entity(entity), lhs(lhs), rhs(rhs) {
        this->entity = entity;
        this->lhs = lhs;
        this->rhs = rhs;
    }

    Entity entity;
    string lhs;
    string rhs;

public:
    static Pattern createAssignPattern(int lineNumber, string lhs, string rhs) {
        Entity entity = Entity::createAssignEntity(lineNumber);
        return Pattern(entity, lhs, rhs);
    }

    static Pattern createIfPattern(int lineNumber, string rhs) {
        Entity entity = Entity::createIfEntity(lineNumber);
        return Pattern(entity, IF_KEYWORD, rhs);
    }

    static Pattern createWhilePattern(int lineNumber, string rhs) {
        Entity entity = Entity::createWhileEntity(lineNumber);
        return Pattern(entity, WHILE_KEYWORD, rhs);
    }

    bool equals(Pattern other) {
        return this->entity.equals(other.entity)
            && this->lhs == other.lhs
            && this->rhs == other.rhs;
    }

    bool operator==(Pattern other) {
        return this->entity.equals(other.entity)
            && this->lhs == other.lhs
            && this->rhs == other.rhs;
    }

    bool isAssignPattern() {
        return this->entity.isAssignEntity();
    }

    bool isIfPattern() {
        return this->entity.isIf();
    }

    bool isWhilePattern() {
        return this->entity.isWhile();
    }

    int getLine() {
        return this->entity.getLine();
    }

    Entity getEntity() {
        return this->entity;
    }

    string getLhs() {
        return this->lhs;
    }

    string getRhs() {
        return this->rhs;
    }
};
