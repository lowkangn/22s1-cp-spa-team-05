#include <qps/query/clause/Clause.h>

class ModifiesSClause : public Clause {
public:
	ClauseResult execute() override;
};