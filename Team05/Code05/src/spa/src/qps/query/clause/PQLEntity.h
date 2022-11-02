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

    /* Copy constructor */
    PQLEntity(const PQLEntity& other) = default;

    /* Move constructor */
    PQLEntity(PQLEntity&& other) noexcept : type(other.type), str(move(other.str)),
        lineNum(other.lineNum), value(other.value) {
        // other will be destroyed
        other.type = PQLEntityType::PROCEDURE;
        other.lineNum = 0;
        other.value = 0;
    }

    PQLEntity& operator=(const PQLEntity&) = default;

    PQLEntity& operator=(PQLEntity&&) = default;

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

    bool isProcedure() const {
        return this->type == PQLEntityType::PROCEDURE;
    }

    bool isStatement() const {
        return this->type == PQLEntityType::STATEMENT;
    }

    bool isVariable() const {
        return this->type == PQLEntityType::VARIABLE;
    }

    bool isConstant() const {
        return this->type == PQLEntityType::CONSTANT;
    }

    string toString() const {
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
		return hash<string>{}(entity.toString());
	}
};
