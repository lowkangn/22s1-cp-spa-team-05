#pragma once
#include <string>
#include <vector>
#include <pkb/PKB.h>

/*
 * Interface used by QPS to retrieve data from PKB
 */
class PKBQueryHandler {
private:
    ProgramKnowledgeBase KnowledgeBase;
public:
    PKBQueryHandler(ProgramKnowledgeBase pkb);

    // TODO Change string to QueryClause
    void retrieveKnowledge(std::vector<std::string> queryClause);
};