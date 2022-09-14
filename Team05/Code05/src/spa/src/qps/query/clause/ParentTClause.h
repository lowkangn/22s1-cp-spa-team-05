#include "SuchThatClause.h"

class ParentTClause : public SuchThatClause {
public:
    shared_ptr<RelationshipClauseResult> execute(shared_ptr<PKB> pkb) override;
};
