#include "Clause.h"
#include "ClauseResult.h"
#include "ClauseArgument.h"

class SelectClause : public Clause {
private:
    ClauseArgument arg;
public:
	ClauseResult execute() override;
};
