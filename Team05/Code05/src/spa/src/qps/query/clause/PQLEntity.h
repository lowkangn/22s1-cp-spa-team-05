#pragma once
#include <string>

using namespace std;

enum class PQLEntityType { PROCEDURE, STATEMENT, VARIABLE, CONSTANT };

class PQLEntity {

private:

    PQLEntityType type;
    string str;
    int value;

    PQLEntity(PQLEntityType type, string str, int value) {
        this->type = type;
        this->str = str;
        this->value = value;
    }

public:

    static PQLEntity generateProcedure(string name) {
        return PQLEntity(PQLEntityType::PROCEDURE, name, 0);
    }

    static PQLEntity generateStatement(int lineNumber) {
        return PQLEntity(PQLEntityType::STATEMENT, "", lineNumber);
    }

    static PQLEntity generateVariable(string name) {
        return PQLEntity(PQLEntityType::VARIABLE, name, 0);
    }

    static PQLEntity generateConstant(int value) {
        return PQLEntity(PQLEntityType::VARIABLE, "", value);
    }

    bool isProcedure() {
        return this->type == PQLEntityType::PROCEDURE;
    }

    bool isStatement() {
        return this->type == PQLEntityType::STATEMENT;
    }

    bool isVariable() {
        return this->type == PQLEntityType::VARIABLE;
    }

    bool isConstant() {
        return this->type == PQLEntityType::CONSTANT;
    }

    string toString() {
        return this->str;
    }

    int getValue() {
        return this->value;
    }

};
