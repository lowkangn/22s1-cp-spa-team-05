#pragma once
#include <string>
#include <vector>
#include "PKB.h"

class UIInterface {
private:
    ProgramKnowledgeBase KnowledgeBase;
public:
    UIInterface(ProgramKnowledgeBase pkb);

    void addKnowledge();

    void printKnowledge();
};