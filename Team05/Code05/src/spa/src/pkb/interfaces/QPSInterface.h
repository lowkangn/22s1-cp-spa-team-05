#pragma once
#include <string>
#include <vector>
#include <pkb/PKB.h>

class QPSInterface {
private:
    ProgramKnowledgeBase KnowledgeBase;
public:
    QPSInterface(ProgramKnowledgeBase pkb);

    // TODO Change string to QueryClause
    void retrieveKnowledge(std::vector<std::string> queryClause);
};