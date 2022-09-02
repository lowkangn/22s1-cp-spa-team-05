#include <qps/query/clause/Clause.h>

class ParentClause : public Clause {
public:
	ClauseResult execute() override;
};