#include "Clause.h"

class ParentTClause : public Clause {
public:
    shared_ptr<ClauseResult> execute() override;
};
