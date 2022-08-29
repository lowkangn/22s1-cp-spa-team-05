#include <Clause.h>

class UsesPClause : public Clause {
public:
	ClauseResult execute() override;
};