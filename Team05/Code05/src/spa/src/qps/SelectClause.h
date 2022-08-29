#include <Clause.h>

class SelectClause : public Clause {
public:
	ClauseResult execute() override;
};