#include <qps/query/clause/Clause.h>

class FollowsClause : public Clause {
public:
	ClauseResult execute() override;
};