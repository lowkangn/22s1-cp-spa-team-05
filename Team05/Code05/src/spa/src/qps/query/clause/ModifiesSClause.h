#include <string>
#include "Clause.h"
#include "ClauseResult.h"
#include "ClauseArgument.h"
#include "../../pkb/interfaces/PKBQueryHandler.h"

class ModifiesSClause : public Clause {
private:
    ClauseType clauseType;
    ClauseArgument lhs;
    ClauseArgument rhs;
public:
    ClauseResult execute() override;
};
