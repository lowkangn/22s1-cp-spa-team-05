#pragma once
#include <string>
#include <vector>
#include <pkb/PKB.h>

class SPtoPKBFacade {
private:
    ProgramKnowledgeBase KnowledgeBase;
public:
    SPtoPKBFacade(ProgramKnowledgeBase pkb);

    // TODO Change string to super class of relationships
    void addKnowledge(std::vector<std::string> relationships);
};