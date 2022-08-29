#include <Clause.h>

class UsesClause : public Clause {
public:
	ClauseResult execute() override;
};