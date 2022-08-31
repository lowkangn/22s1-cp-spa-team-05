#include <qps/query/clause/Clause.h>

class ModifiesPClause : public Clause {
public:
	ClauseResult execute() override;
};