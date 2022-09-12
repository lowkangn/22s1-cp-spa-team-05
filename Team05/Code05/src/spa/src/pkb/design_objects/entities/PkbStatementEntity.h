#pragma once
#include <pkb/design_objects/entities/PkbEntity.h>

#include <string>

using namespace std;

enum class PkbStatementType {
    READ,
    PRINT,
    WHILE,
    IF,
    ASSIGN,
};

class PkbStatementEntity : PkbEntity  {
private: 
    
    PkbStatementType statementType;
    int lineNumber;
    PkbStatementEntity(PkbStatementType statementType, int lineNumber) : PkbEntity(PkbEntityType::STATEMENT)  {
        this->statementType = statementType;
        this->lineNumber = lineNumber;
    }

public: 
    // ===== Factory methods to restrict the type =====
    PkbStatementEntity createReadStatementEntity(int lineNumber) {
        return PkbStatementEntity(PkbStatementType::READ, lineNumber);
    }

    PkbStatementEntity createPrintStatementEntity(int lineNumber) {
        return PkbStatementEntity(PkbStatementType::PRINT, lineNumber);
    }

    PkbStatementEntity createWhileStatementEntity(int lineNumber) {
        return PkbStatementEntity(PkbStatementType::WHILE, lineNumber);
    }

    PkbStatementEntity createAssignStatementEntity(int lineNumber) {
        return PkbStatementEntity(PkbStatementType::ASSIGN, lineNumber);
    }

    PkbStatementEntity createIfStatementEntity(int lineNumber) {
        return PkbStatementEntity(PkbStatementType::IF, lineNumber);
    }

    // ===== identifier methods ======
    bool isReadStatement() {
        return this->statementType == PkbStatementType::READ;
    }
    bool isPrintStatement() {
        return this->statementType == PkbStatementType::PRINT;
    }
    bool isWhileStatement() {
        return this->statementType == PkbStatementType::WHILE;
    }
    bool isAssignStatement() {
        return this->statementType == PkbStatementType::ASSIGN;
    }
    bool isIfStatement() {
        return this->statementType == PkbStatementType::IF;
    }

    /*
        Returns a unique key.
    */
    string getKey() override {
        return this->identifier;
    }

    /*
        Overriding the equals operator.
    */
    const bool operator==(const PkbStatementEntity& other) const {
        return (other.lineNumber == this->lineNumber) && (other.statementType == this->statementType);
    }

};

template <>
struct hash<PkbStatementEntity>
{
    size_t operator()(const PkbStatementEntity& k) const
    {
        // no two statements should have the same line number. so we can 
        // hash by line numbers.
        return hash<int>(k.getKey());
    }
};