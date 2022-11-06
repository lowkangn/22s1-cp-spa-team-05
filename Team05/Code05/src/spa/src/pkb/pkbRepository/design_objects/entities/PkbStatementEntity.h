#pragma once
#include <pkb/PkbException.h>
#include <pkb/pkbRepository/design_objects/entities/PkbEntity.h>

#include <memory>
#include <string>

using namespace std;

enum class PkbStatementType {
    READ,
    PRINT,
    WHILE,
    IF,
    ASSIGN,
    CALL,
};

class PkbStatementEntity : public PkbEntity {
private:
    PkbStatementType statementType;
    int lineNumber;

    PkbStatementEntity(PkbStatementType statementType, int lineNumber)
        : PkbEntity(PkbEntityType::STATEMENT) {
        this->statementType = statementType;
        this->lineNumber = lineNumber;
    }

public:
    // ===== Factory methods to restrict the type =====

    static shared_ptr<PkbStatementEntity> createReadStatementEntity(int lineNumber) {
        return shared_ptr<PkbStatementEntity>(
            new PkbStatementEntity(PkbStatementType::READ, lineNumber));
    }

    static shared_ptr<PkbStatementEntity> createCallStatementEntity(int lineNumber) {
        return shared_ptr<PkbStatementEntity>(
            new PkbStatementEntity(PkbStatementType::CALL, lineNumber));
    }

    static shared_ptr<PkbStatementEntity> createWhileStatementEntity(int lineNumber) {
        return shared_ptr<PkbStatementEntity>(
            new PkbStatementEntity(PkbStatementType::WHILE, lineNumber));
    }

    static shared_ptr<PkbStatementEntity> createAssignStatementEntity(int lineNumber) {
        return shared_ptr<PkbStatementEntity>(
            new PkbStatementEntity(PkbStatementType::ASSIGN, lineNumber));
    }

    static shared_ptr<PkbStatementEntity> createIfStatementEntity(int lineNumber) {
        return shared_ptr<PkbStatementEntity>(
            new PkbStatementEntity(PkbStatementType::IF, lineNumber));
    }

    static shared_ptr<PkbStatementEntity> createPrintStatementEntity(int lineNumber) {
        return shared_ptr<PkbStatementEntity>(
            new PkbStatementEntity(PkbStatementType::PRINT, lineNumber));
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

    bool isCallStatement() {
        return this->statementType == PkbStatementType::CALL;
    }

    /*
        Returns a unique key.
    */
    string getKey() override {
        return to_string(this->lineNumber);
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
        return this->lineNumber;
    }

    int getValue() override {
        throw PkbException("Statement does not have a value!");
    }

    /*
        Overriding the equals operator.
    */
    const bool operator==(const PkbStatementEntity& other) const {
        return (other.lineNumber == this->lineNumber) && (other.statementType == this->
            statementType);
    }

    bool equals(shared_ptr<PkbEntity> other) override {
        if (!other->isStatement()) {
            return false;
        }
        return this->getLineNumber() == other->getLineNumber();
    }
};
