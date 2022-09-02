#include <vector>
#include "PKB.h"
#include <sp/dataclasses/design_objects/Entity.h>
#include <qps/query/clause/Clause.h>

ProgramKnowledgeBase::ProgramKnowledgeBase() {

}

ProgramKnowledgeBase *ProgramKnowledgeBase::getInstance() {
    if (!PKBInstance) {
        PKBInstance = new ProgramKnowledgeBase;
    }
    return PKBInstance;
}

std::vector<Entity> ProgramKnowledgeBase::retrieveAllKnowledge() {
    return std::vector<Entity>();
}

Entity ProgramKnowledgeBase::retrieveKnowledge(Clause queryClause) {
    return Entity();
}

// TODO Refactor to dataclass coming from QPS
void ProgramKnowledgeBase::addKnowledge(std::string knowledge) {

}

// TODO Refactor to dataclass coming from QPS
void ProgramKnowledgeBase::deleteKnowledge(std::string knowledge) {

}


