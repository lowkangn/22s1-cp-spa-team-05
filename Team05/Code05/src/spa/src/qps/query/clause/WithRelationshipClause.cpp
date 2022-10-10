#include <qps/query/clause/WithRelationshipClause.h>

shared_ptr<ClauseResult> WithRelationshipClause::execute(shared_ptr<PKBQueryHandler> pkb) {
	
	return make_shared<RelationshipClauseResult>(lhsArgs.front(), rhsArgs.front(), vector<PQLRelationship>{});
}

bool WithRelationshipClause::equals(shared_ptr<WithClause> other) {
	if (dynamic_pointer_cast<WithRelationshipClause>(other) == nullptr) {
		return false;
	}
	shared_ptr<WithRelationshipClause> otherWithRelationship = dynamic_pointer_cast<WithRelationshipClause>(other);
	return (this->lhsArgs == otherWithRelationship->lhsArgs) && (this->rhsArgs == otherWithRelationship->rhsArgs);
}