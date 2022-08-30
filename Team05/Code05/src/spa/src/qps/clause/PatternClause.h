#include <Clause.h>

class PatternClause : public Clause {
public:
	ClauseResult execute() override;
};