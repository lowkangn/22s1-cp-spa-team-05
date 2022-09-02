#include <qps/query/clause/Clause.h>

class UsesSClause : public Clause {
public:
	ClauseResult execute() override;
};