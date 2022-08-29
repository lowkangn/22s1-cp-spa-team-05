#pragma once
#include <string>
#include <vector>
#include "PKB.h"

class SPInterface {
private:
    ProgramKnowledgeBase KnowledgeBase;
public:
    SPInterface(ProgramKnowledgeBase pkb);

    // TODO Change string to super class of relationships
    void addKnowledge(std::vector<std::string> relationships);
};