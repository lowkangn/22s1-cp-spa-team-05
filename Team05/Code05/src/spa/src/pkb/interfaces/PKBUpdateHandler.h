#pragma once
#include <string>
#include <vector>
#include <pkb/PKB.h>

class PKBUpdateHandler {
private:
    ProgramKnowledgeBase KnowledgeBase;
public:
    PKBUpdateHandler(ProgramKnowledgeBase pkb);

    // TODO Change string to super class of relationships
    void addKnowledge(std::vector<std::string> relationships);
};