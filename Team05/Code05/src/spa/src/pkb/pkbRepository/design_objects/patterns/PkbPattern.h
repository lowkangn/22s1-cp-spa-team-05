#pragma once
#include <assert.h>

#include <pkb/PkbException.h>
#include <pkb/pkbRepository/design_objects/entities/PkbStatementEntity.h>

#include <iostream>
#include <memory>
#include <regex>
#include <string>
#include <vector>

using namespace std;

enum class PkbPatternType {
    ASSIGN,
    IF,
    WHILE
};

/*
	Parent class for all PKB patterns
*/
class PkbPattern {
protected:
    shared_ptr<PkbStatementEntity> statement;
    /*
        Assign pattern: expects 2 strings (lhs, rhs)
        If and while pattern: expects one string (lhs only)
    */
    vector<string> strings;

    PkbPatternType type;

    PkbPattern(
        shared_ptr<PkbStatementEntity> statement, vector<string> strings, PkbPatternType type) {
        this->strings = strings;
        this->statement = statement;
        this->type = type;
    }

public:
    virtual bool isRegexMatch(vector<string> regexStrings) = 0;

    virtual string getKey() = 0;

    virtual string getVariableIdentifier() = 0;

    bool isWhilePattern() {
        return this->statement->isWhileStatement();
    }

    bool isIfPattern() {
        return this->statement->isIfStatement();
    }

    bool isAssignPattern() {
        return this->statement->isAssignStatement();
    }

    bool equals(shared_ptr<PkbPattern> other) {
        return this->statement->equals(other->statement) && this->strings == other->strings;
    }

    int getStatementLineNumber() {
        return this->statement->getLineNumber();
    }
};
