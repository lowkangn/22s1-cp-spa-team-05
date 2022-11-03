#pragma once

#include <list>

#include <qps/query/clause/SelectClause.h>
#include <qps/query/clause/RelationshipClause.h>
#include <qps/query/clause/PatternClause.h>
#include <qps/query/clause/WithClause.h>
#include <qps/query/clause/CfgRelationshipClause.h>
#include <pkb/PKB.h>

using namespace std;

class CfgClauseOptimiser;
struct ClauseWeightComparator;

class Query {
private:
    shared_ptr<SelectClause> selectClause;
    list<shared_ptr<RelationshipClause>> earlySuchThatClauses;
	list<shared_ptr<PatternClause>> patternClauses;
	list<shared_ptr<WithClause>> withClauses;
    vector<shared_ptr<CfgRelationshipClause>> lateClauses;

    /* True iff a constraint clause has been executed */
    bool hasStartedConstraintExecution;

    /* True iff an empty result has been found at some point in execution */
    bool emptyResultFound;

    template <class ClauseType>
	bool areClausesAllEqual(list<shared_ptr<ClauseType>> firstClauseList, list<shared_ptr<ClauseType>> secondClauseList) {
		if (firstClauseList.size() != secondClauseList.size()) {
			return false;
		}

		typename list<shared_ptr<ClauseType>>::iterator firstIter = firstClauseList.begin();
		typename list<shared_ptr<ClauseType>>::iterator secondIter = secondClauseList.begin();

		bool isClauseEqual;
		while (firstIter != firstClauseList.end()) {
			isClauseEqual = (*firstIter)->equals(*secondIter);
			if (!isClauseEqual) {
				return false;
			}
			firstIter++;
			secondIter++;
		}
		return true;
	}

    /* Executes a list of clauses of type ClauseType that returns results of type ResultType */
    template <class ClauseType, class ResultType>
    void executeClauses(list<shared_ptr<ClauseType>>& clauses, list<shared_ptr<ResultType>>& results, shared_ptr<PKBQueryHandler> pkb);

protected:
    vector<shared_ptr<CfgRelationshipClause>> getCfgClauses() {
        return this->lateClauses;
    }
public:

    /* Instantiates a Query object containing the clauses. */
    Query(shared_ptr<SelectClause> select,
        list<shared_ptr<RelationshipClause>> relationships,
        list<shared_ptr<PatternClause>> patterns,
        list<shared_ptr<WithClause>> withs) {
        selectClause = select;
        patternClauses = patterns;
        withClauses = withs;
        hasStartedConstraintExecution = false;
        emptyResultFound = false;

        for (const shared_ptr<RelationshipClause>& suchThatClause : relationships) {
            if (!suchThatClause->requiresCfg()) {
                // does not require cfg
                this->earlySuchThatClauses.emplace_back(suchThatClause);
            } else if (suchThatClause->isAlwaysEmpty()) {
                this->hasStartedConstraintExecution = true;
                this->emptyResultFound = true;
                break;
            } else {
                //requires cfg and is possibly non-empty
                this->lateClauses.emplace_back(
                    static_pointer_cast<CfgRelationshipClause>(suchThatClause));
            }
        }
    }

    /* Returns the results obtained from the query's SelectClause. */
    list<shared_ptr<ClauseResult>> executeSelect(shared_ptr<PKBQueryHandler> pkb);

	/* Returns the results obtained from the query's SuchThat and Pattern clauses. */
	list<shared_ptr<RelationshipClauseResult>> executeEarlySuchThatAndPattern(shared_ptr<PKBQueryHandler> pkb);

    /* Returns the results obtained from the clauses that are executed late. */
    list<shared_ptr<RelationshipClauseResult>> executeLateClauses(shared_ptr<PKBQueryHandler> pkb,
        unordered_map<ClauseArgument, unordered_set<PQLEntity>>& restrictionMap);

	/* Returns the results obtained from the query's With clauses. */
	list<shared_ptr<ClauseResult>> executeWith(shared_ptr<PKBQueryHandler> pkb);

    void sortOptimisableClauses();

    void enableClauseOptimiserVisit(CfgClauseOptimiser* optimiser);

	/* Checks if the requested return type is BOOLEAN */
	bool checkIfBooleanReturnType() {
		return selectClause->checkIfBooleanReturnType();
	}

    bool hasLateExecutionClauses() {
        return !this->lateClauses.empty();
	}

    bool hasFoundEmptyResult() {
        return this->hasStartedConstraintExecution && this->emptyResultFound;
	}

	friend bool operator==(Query first, Query second);
};
