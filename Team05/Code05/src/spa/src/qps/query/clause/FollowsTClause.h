#include <qps/query/clause/Clause.h>

class FollowsTClause : public Clause {
public:
	ClauseResult execute() override;
};