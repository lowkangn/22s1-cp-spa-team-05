#include "Clause.h"

class PatternClause : public Clause {
public:
    shared_ptr<ClauseResult> execute() override;
};
