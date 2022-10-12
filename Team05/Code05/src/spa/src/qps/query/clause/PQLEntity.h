#pragma once
#include <string>

using namespace std;

enum class PQLEntityType { PROCEDURE, STATEMENT, VARIABLE, CONSTANT };

class PQLEntity {

private:

    PQLEntityType type;
    string str;
    int lineNum;
    int value;

    PQLEntity(PQLEntityType type, string str, int lineNum, int value) {
        this->type = type;
        this->str = str;
        this->lineNum = lineNum;
        this->value = value;
    }

public:

    static PQLEntity generateProcedure(string name) {
        return PQLEntity(PQLEntityType::PROCEDURE, name, 0, 0);
    }

    static PQLEntity generateStatement(int lineNumber) {
        return PQLEntity(PQLEntityType::STATEMENT, "", lineNumber, 0);
    }

    static PQLEntity generateVariable(string name) {
        return PQLEntity(PQLEntityType::VARIABLE, name, 0, 0);
    }

    static PQLEntity generateConstant(int value) {
        return PQLEntity(PQLEntityType::CONSTANT, "", 0, value);
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
        if (type == PQLEntityType::VARIABLE || type == PQLEntityType::PROCEDURE) {
            return this->str;
        } else if (type == PQLEntityType::STATEMENT) {
            return to_string(this->lineNum);
        } else {
            return to_string(this->value);
        }
    }

    friend bool operator==(PQLEntity first, PQLEntity second);

    friend bool operator<(PQLEntity first, PQLEntity second);
};

template<> struct hash<PQLEntity> {
	size_t operator()(PQLEntity entity) const noexcept {
		// Can just use string since two different entities won't have the same string
		return hash<string>{}(entity.toString());
	}
};
