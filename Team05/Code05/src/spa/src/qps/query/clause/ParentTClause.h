#include <qps/query/clause/RelationshipClause.h>

class ParentTClause : public RelationshipClause {
public:
    shared_ptr<RelationshipClauseResult> execute(shared_ptr<PKB> pkb) override;
};
