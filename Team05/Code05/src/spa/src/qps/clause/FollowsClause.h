#include <Clause.h>

class FollowsClause : public Clause {
public:
	ClauseResult execute() override;
};