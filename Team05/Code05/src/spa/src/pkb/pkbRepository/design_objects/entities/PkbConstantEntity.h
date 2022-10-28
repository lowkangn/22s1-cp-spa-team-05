#pragma once
#include <pkb/pkbRepository/design_objects/entities/PkbEntity.h>
#include <pkb/PkbException.h>
#include <string>
#include <memory>

using namespace std;

class PkbConstantEntity : public PkbEntity {
private:

    int value;
    

public:

    PkbConstantEntity(int value) : PkbEntity(PkbEntityType::CONSTANT) {
        this->value = value;
    }
    
    /*
        Returns a unique key.
    */
    string getKey() override {
        return string("constant") + to_string(this->value) ;
    }

    /*
        Gets the identifier.
    */
    string getIdentifier() override {
        throw PkbException("Statement does not have a identifier!");
    }

    /*
        Gets the line number.
    */
    int getLineNumber() override {
        throw PkbException("Constant does not have a line number!");
    }

    int getValue() override {
        return this->value;
    }

    /*
        Overriding the equals operator.
    */
    const bool operator==(const PkbConstantEntity& other) const {
        return (other.value == this->value);
    }

    bool equals(shared_ptr<PkbEntity> other) override {
        if (!other->isConstant()) {
            return false;
        }
        return this->getValue() == other->getValue();
    }

};
