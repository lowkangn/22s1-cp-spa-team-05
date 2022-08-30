#include <Clause.h>

class ParentClause : public Clause {
public:
	ClauseResult execute() override;
};