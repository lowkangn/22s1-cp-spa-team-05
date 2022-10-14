#include <qps/query/clause/WithClause.h>

PKBTrackedStatementType WithClause::getPKBStmtType(ClauseArgument& stmtSynonymArg) {
	if (stmtSynonymArg.isReadSynonym()) {
		return PKBTrackedStatementType::READ;
	}
	else if (stmtSynonymArg.isPrintSynonym()) {
		return PKBTrackedStatementType::PRINT;
	}
	else if (stmtSynonymArg.isAssignSynonym()) {
		return PKBTrackedStatementType::ASSIGN;
	}
	else if (stmtSynonymArg.isCallSynonym()) {
		return PKBTrackedStatementType::CALL;
	}
	else if (stmtSynonymArg.isWhileSynonym()) {
		return PKBTrackedStatementType::WHILE;
	}
	else if (stmtSynonymArg.isIfSynonym()) {
		return PKBTrackedStatementType::IF;
	}
	else if (stmtSynonymArg.isStmtSynonym()) {
		return PKBTrackedStatementType::ALL;
	}
	else {
		throw PQLLogicError("Cannot identify stmt type in with clause synonym");
	}
}