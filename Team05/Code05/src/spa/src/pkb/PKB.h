#pragma once
#include <string>
#include <vector>

class ProgramKnowledgeBase {

    // TODO Change from string to super class of Knowledge
    std::vector<std::string> readAll();

    // TODO Change return type from string to super class of Knowledge
    // TODO Change arg from string to QueryClause class
    std::string read(std::string queryClause);

    // TODO Change from string to super class of Knowledge
    void addKnowledge(std::string knowledge);

    // TODO Change from string to super class of Knowledge
    void deleteKnowledge(std::string knowledge);

};
