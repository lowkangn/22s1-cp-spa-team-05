#include <qps/query/clause/Clause.h>

class PatternClause : public Clause {
public:
	ClauseResult execute() override;
};