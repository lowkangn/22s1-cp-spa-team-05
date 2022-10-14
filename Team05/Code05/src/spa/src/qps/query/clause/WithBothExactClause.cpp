#include <qps/query/clause/WithBothExactClause.h>

shared_ptr<ClauseResult> WithBothExactClause::execute(shared_ptr<PKBQueryHandler> pkb) {
	assert(this->lhs.isExactReference() && this->rhs.isExactReference());

	assert((this->lhs.isLineNumber() && this->rhs.isLineNumber())
		|| (this->lhs.isStringLiteral() && this->rhs.isStringLiteral()));

	if (lhs != rhs) {
		return make_shared<EntityClauseResult>(this->lhs, vector<PQLEntity>{});
	}

	return make_shared<EntityClauseResult>(EntityClauseResult::createNonEmptyNoSynonymResult);
}

bool WithBothExactClause::equals(shared_ptr<WithClause> other) {
	if (dynamic_pointer_cast<WithBothExactClause>(other) == nullptr) {
		return false;
	}
	shared_ptr<WithBothExactClause> otherWithExact = dynamic_pointer_cast<WithBothExactClause>(other);
	return (this->lhs == otherWithExact->lhs) && (this->rhs == otherWithExact->rhs);
}