#include <string>
#include "Clause.h"
#include "ClauseResult.h"
#include "ClauseArgument.h"
#include "EntityClauseResult.h"

using namespace std;

class SelectClause : public Clause {
private:
    ClauseArgument arg;
public:
    shared_ptr<ClauseResult> execute() override;
};
