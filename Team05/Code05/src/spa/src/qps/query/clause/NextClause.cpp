#include <qps/query/clause/NextClause.h>

PKBTrackedRelationshipType NextClause::getPkbTrackedRelationshipType() {
    return PKBTrackedRelationshipType::NEXT;
}

bool NextClause::equals(shared_ptr<RelationshipClause> other) {
	if (dynamic_pointer_cast<NextClause>(other) == nullptr) {
		return false;
	}
	shared_ptr<NextClause> otherNext = dynamic_pointer_cast<NextClause>(other);
	return (this->lhs == otherNext->lhs) && (this->rhs == otherNext->rhs);
}
