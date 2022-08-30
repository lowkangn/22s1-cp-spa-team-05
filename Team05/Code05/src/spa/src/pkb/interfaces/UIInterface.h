#pragma once
#include <string>
#include <vector>
#include <pkb/PKB.h>

class UIInterface {
private:
    ProgramKnowledgeBase KnowledgeBase;
public:
    UIInterface(ProgramKnowledgeBase pkb);

    void addKnowledge();

    void printKnowledge();
};