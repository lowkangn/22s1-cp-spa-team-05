#pragma once

#include <pkb/PkbException.h>
#include <pkb/pkbRepository/design_objects/entities/PkbStatementEntity.h>
#include <pkb/pkbRepository/design_objects/patterns/PkbPattern.h>

#include <iostream>
#include <memory>
#include <regex>
#include <string>
#include <vector>

/*
	Class to represent assign patterns
*/
class PkbAssignPattern : public PkbPattern {
protected:
    PkbAssignPattern(
        shared_ptr<PkbStatementEntity> statement, vector<string> strings) : PkbPattern(
        statement,
        strings,
        PkbPatternType::ASSIGN) {
    }

public :
    /*
        Takes in a vector of regex strings and returns true if all strings
        match on all strings within the pattern.

        NOTE: we expect the internal strings to be postfix
        e.g. x + z * y -> +x*zy. This allows us to model a tree with strings.
    */
    bool isRegexMatch(vector<string> regexStrings) override {
        if (regexStrings.size() != this->strings.size()) {
            throw PkbException(
                "Trying to regex match, expecting exactly 2 regex strings, one for lhs and rhs.");
        }
        // iterate through and match
        int n = regexStrings.size();
        for (int i = 0; i < n; i++) {
            string regexString = regexStrings[i];
            string s = this->strings[i];
            if (!regex_match(s, regex(regexString))) {
                return false;
            }
        }
        return true;
    }

    /*
        Factory method that creates an assign pattern.
    */
    static shared_ptr<PkbAssignPattern> createAssignPattern(
        int lineNumber, vector<string> strings) {
        if (strings.size() != 2) {
            throw PkbException(
                "Trying to create assign pattern, expecting only 2 strings, lhs, and rhs.");
        }
        shared_ptr<PkbStatementEntity> statement =
            PkbStatementEntity::createAssignStatementEntity(lineNumber);
        return shared_ptr<PkbAssignPattern>(new PkbAssignPattern(statement, strings));
    }

    /*
        Returns the LHS variable in assign statement
    */
    string getVariableIdentifier() override {
        assert(this->strings.size() > 0 && this->statement->isAssignStatement());
        return this->strings[0];
    }

    /*
        Gets a unique key for each pattern.
    */
    string getKey() override {
        string key;
        for (string pattern : this->strings) {
            key += pattern;
        }
        return key + this->statement->getKey();
    }
};
