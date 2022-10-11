#include <qps/query/clause/WithNoExactClause.h>

shared_ptr<ClauseResult> WithNoExactClause::execute(shared_ptr<PKBQueryHandler> pkb) {
	
	return make_shared<RelationshipClauseResult>(lhsArgs.front(), rhsArgs.front(), vector<PQLRelationship>{});
}

bool WithNoExactClause::equals(shared_ptr<WithClause> other) {
	if (dynamic_pointer_cast<WithNoExactClause>(other) == nullptr) {
		return false;
	}
	shared_ptr<WithNoExactClause> otherWithRelationship = dynamic_pointer_cast<WithNoExactClause>(other);
	return (this->lhsArgs == otherWithRelationship->lhsArgs) && (this->rhsArgs == otherWithRelationship->rhsArgs);
}