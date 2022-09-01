#pragma once
#include <string>
#include <vector>
#include <pkb/PKB.h>

/*
 * Utility class to add and retrieve knowledge directly to PKB
 */
class UItoPKBFacade {
private:
    ProgramKnowledgeBase KnowledgeBase;
public:
    UItoPKBFacade(ProgramKnowledgeBase pkb);

    void addKnowledge();

    void printKnowledge();
};