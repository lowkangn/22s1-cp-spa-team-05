#pragma once

#include <pkb/PkbException.h>
#include <pkb/pkbRepository/design_objects/entities/PkbEntity.h>

#include <memory>
#include <string>

using namespace std;
/*
	This class encapsulates a variable stored in the pkb. A variable
	can be identified primarily by its string.
*/
class PkbVariableEntity : public PkbEntity {
private:
    string identifier;

public:
    explicit PkbVariableEntity(string identifier) : PkbEntity(PkbEntityType::VARIABLE) {
        this->identifier = identifier;
    }

    /*
        Gets the identifier.
    */
    string getIdentifier() override {
        return this->identifier;
    }

    /*
        Gets the line number.
    */
    int getLineNumber() override {
        throw PkbException("Variable does not have a line number!");
    }


    int getValue() override {
        throw PkbException("Variable does not have a value!");
    }


    /*
        Overriding the equals operator.
    */
    const bool operator==(const PkbVariableEntity& other) const {
        return other.identifier == this->identifier;
    }

    /*
        Returns a unique key.
    */
    string getKey() override {
        return this->identifier;
    }

    bool equals(shared_ptr<PkbEntity> other) override {
        if (!other->isVariable()) {
            return false;
        }
        return this->getIdentifier() == other->getIdentifier();
    }
};
