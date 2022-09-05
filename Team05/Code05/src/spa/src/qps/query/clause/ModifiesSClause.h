#include <string>
#include "Clause.h"
#include "ClauseResult.h"
#include "RelationshipClauseResult.h"
#include "ClauseArgument.h"
#include "../../pkb/interfaces/PKBQueryHandler.h"

class ModifiesSClause : public Clause {
private:
    ClauseArgument lhs;
    ClauseArgument rhs;
public:
    shared_ptr<ClauseResult> execute() override;
};
