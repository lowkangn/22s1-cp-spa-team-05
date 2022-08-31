#pragma once
#include <string>
#include <vector>
#include <pkb/PKB.h>

class QPStoPKBFacade {
private:
    ProgramKnowledgeBase KnowledgeBase;
public:
    QPStoPKBFacade(ProgramKnowledgeBase pkb);

    // TODO Change string to QueryClause
    void retrieveKnowledge(std::vector<std::string> queryClause);
};