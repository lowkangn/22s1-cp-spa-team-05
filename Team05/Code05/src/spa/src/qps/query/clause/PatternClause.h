#include "SuchThatClause.h"

class PatternClause : public SuchThatClause {
public:
    shared_ptr<RelationshipClauseResult> execute(shared_ptr<PKB> pkb) override;
};
