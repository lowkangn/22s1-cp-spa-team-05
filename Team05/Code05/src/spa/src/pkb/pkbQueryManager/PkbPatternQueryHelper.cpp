#include "PkbPatternQueryHelper.h"

#include <pkb/pkbRepository/table_managers/PkbPatternTable.h>

vector<shared_ptr<PkbPattern>> PkbPatternQueryHelper::retrieveAssignPatterns(
    ClauseArgument lhs, ClauseArgument rhs, shared_ptr<PkbRepository> repository) {
    // 1. parse lhs
    string lhsStringPattern;

    bool lhsIsSynonym = false;
    if (lhs.isVariableSynonym() || lhs.isWildcard()) {
        // if lhs is a synonym, we treat as a wildcard, but flag it - we will deal with it later
        lhsIsSynonym = lhs.isVariableSynonym();
        // in either case, we match lhs with a wildcard
        lhsStringPattern = WILDCARD_CHAR;
    } else if (lhs.isStringLiteral() || lhs.isPatternString() || lhs.
        isPatternStringWithWildcards()) {
        // string literal, we match exactly
        lhsStringPattern = lhs.getIdentifier();
    } else {
        // should never get here
        throw PkbException("Unknown assign pattern being retrieved!");
    }

    // 2. parse rhs - it should only be wildcard or string literal
    string rhsStringPattern;
    if (rhs.isWildcard()) {
        rhsStringPattern = WILDCARD_CHAR;
    } else if (rhs.isPatternString() || rhs.isPatternStringWithWildcards()) {
        rhsStringPattern = rhs.getIdentifier();
    } else {
        throw PkbException(
            "RHS of assign pattern should be wildcard, sandwiched wildcard or string, but is neither."); //NOLINT(whitespace/line_length)
    }

    // 3. find all patterns that match in the assign table
    vector<string> postFixStringsToMatch = {lhsStringPattern, rhsStringPattern};
    shared_ptr<PkbPatternTable> table = repository->getPatternTableByType(PkbPatternType::ASSIGN);
    vector<shared_ptr<PkbPattern>> matchingPatterns = table->getAllThatMatchPostFixStrings(
        postFixStringsToMatch);

    return matchingPatterns;
}

vector<shared_ptr<PkbPattern>> PkbPatternQueryHelper::retrieveIfPatterns(
    ClauseArgument lhs, ClauseArgument rhs, shared_ptr<PkbRepository> repository) {
    // parse lhs
    // to be specific, we store only lhs patterns for if
    // e.g. if has x, y, z -> we store x, y, z as individual patterns with
    //      lhs as their strings, rhs doesn't matter
    string lhsStringPattern;

    bool lhsIsSynonym = false;
    if (lhs.isVariableSynonym() || lhs.isWildcard()) {
        // if lhs is synonym, flag as wildcard
        lhsIsSynonym = lhs.isVariableSynonym();
        lhsStringPattern = WILDCARD_CHAR;
    } else if (lhs.isStringLiteral()) {
        // string literal, we match exactly
        lhsStringPattern = lhs.getIdentifier();
    } else {
        throw PkbException("Unknown if pattern being retrieved!");
    }

    shared_ptr<PkbPatternTable> table = repository->getPatternTableByType(PkbPatternType::IF);
    vector<shared_ptr<PkbPattern>> matchingPatterns = table->getVariableMatch(lhsStringPattern);
    return matchingPatterns;
}

vector<shared_ptr<PkbPattern>> PkbPatternQueryHelper::retrieveWhilePatterns(
    ClauseArgument lhs, ClauseArgument rhs, shared_ptr<PkbRepository> repository) {
    // parse lhs
    // to be specific, we store only lhs patterns for while
    // e.g. while has x, y, z -> we store x, y, z as individual patterns with
    //      lhs as their strings, rhs doesn't matter
    string lhsStringPattern;

    bool lhsIsSynonym = false;
    if (lhs.isVariableSynonym() || lhs.isWildcard()) {
        // if lhs is synonym, flag as wildcard
        lhsIsSynonym = lhs.isVariableSynonym();
        lhsStringPattern = WILDCARD_CHAR;
    } else if (lhs.isStringLiteral()) {
        // string literal, we match exactly
        lhsStringPattern = lhs.getIdentifier();
    } else {
        throw PkbException("Unknown while pattern being retrieved!");
    }

    shared_ptr<PkbPatternTable> table = repository->getPatternTableByType(PkbPatternType::WHILE);
    vector<shared_ptr<PkbPattern>> matchingPatterns = table->getVariableMatch(lhsStringPattern);
    return matchingPatterns;
}
