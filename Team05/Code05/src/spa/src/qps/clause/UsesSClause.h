#include <Clause.h>

class UsesSClause : public Clause {
public:
	ClauseResult execute() override;
};