#pragma once
#include <string>
#include <vector>
#include <pkb/PKB.h>

class UItoPKBFacade {
private:
    ProgramKnowledgeBase KnowledgeBase;
public:
    UItoPKBFacade(ProgramKnowledgeBase pkb);

    void addKnowledge();

    void printKnowledge();
};