#include <Clause.h>

class ModifiesSClause : public Clause {
public:
	ClauseResult execute() override;
};