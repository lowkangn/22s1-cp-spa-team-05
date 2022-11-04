#include "catch.hpp"

#include <qps/query_evaluator/CfgClauseOptimiser.h>
#include <qps/query_evaluator/QueryResultsCombiner.h>

#include <qps/query/clause/SelectClause.h>
#include <qps/query/clause/FollowsClause.h>
#include <qps/query/clause/ParentClause.h>
#include <qps/query/clause/WithOneExactClause.h>
#include <qps/query/clause/PatternAssignClause.h>

/* Helper class to check order of clauses in query */
namespace {
    class TestQueryWrapper : public Query {
    public:
        TestQueryWrapper(Query query) : Query(query) {
        }

        void requireCorrectOrder(
            vector<shared_ptr<CfgRelationshipClause>> expectedClausesOrder) {
            REQUIRE(this->getCfgClauses() == expectedClausesOrder);
        }
    };

    //PQLEntities
    PQLEntity w1 = PQLEntity::generateStatement(1);
    PQLEntity a2 = PQLEntity::generateStatement(2);
    PQLEntity a3 = PQLEntity::generateStatement(3);
    PQLEntity r4 = PQLEntity::generateStatement(4);
    PQLEntity varX = PQLEntity::generateVariable("x");
    PQLEntity const5 = PQLEntity::generateConstant(5);

    //PQLRelationships
    PQLRelationship w1a2 = PQLRelationship(w1, a2);
    PQLRelationship w1a3 = PQLRelationship(w1, a3);
    PQLRelationship a3r4 = PQLRelationship(a3, r4);
    PQLRelationship a2x = PQLRelationship(a2, varX);

    //ClauseArguments
    ClauseArgument stmt1Arg = ClauseArgument::createStmtArg("s1");
    ClauseArgument stmt2Arg = ClauseArgument::createStmtArg("s2");
    ClauseArgument assign1Arg = ClauseArgument::createStmtArg("a1");
    ClauseArgument assign2Arg = ClauseArgument::createStmtArg("a2");
    ClauseArgument assign3Arg = ClauseArgument::createStmtArg("a3");
    ClauseArgument while1Arg = ClauseArgument::createStmtArg("w1");
    ClauseArgument variable1Arg = ClauseArgument::createVariableArg("v1");
    ClauseArgument wildcardArg = ClauseArgument::createWildcardArg();
    ClauseArgument fiveArg = ClauseArgument::createLineNumberArg("5");
    ClauseArgument xStringLiteralArg = ClauseArgument::createStringLiteralArg("x");

    ClauseArgument stmt2StmtNumAttributeArg = ClauseArgument::createStmtNumAttributeArg(stmt2Arg);
}

TEST_CASE("CfgClauseOptimiser: test working correctly") {
    auto test = [](Query query,
        list<ClauseResult> selectResults,
        list<ClauseResult> earlyRelationshipResults,
        list<ClauseResult> withResults,
        vector<shared_ptr<CfgRelationshipClause>> expectedClausesOrder,
        unordered_map<ClauseArgument, unordered_set<PQLEntity>> expectedRestrictions
    ) {
        // given
        TestQueryWrapper wrappedQuery(query);
        QueryResultsOptimiser resultsOptimiser(selectResults, earlyRelationshipResults, withResults);
        bool isEmpty = false;
        QueryResultsCombiner combiner(selectResults, resultsOptimiser.optimise(isEmpty));
        vector<vector<ClauseResult>> combined = combiner.combineWithinGroupsOnly();

        // when
        CfgClauseOptimiser clauseOptimiser(wrappedQuery, combined, resultsOptimiser);
        unordered_map<ClauseArgument, unordered_set<PQLEntity>> actualRestrictions = 
            clauseOptimiser.optimise();

        // then
        wrappedQuery.requireCorrectOrder(expectedClausesOrder);
        REQUIRE(actualRestrictions == expectedRestrictions);
    };

    ClauseArgument varArg = ClauseArgument::createVariableArg("v");
    shared_ptr<SelectClause> selectClause = make_shared<SelectClause>(SelectClause::createSynonymSelectClause({ assign1Arg }));

    /* assign a1, a2 a3; stmt s1, s2; variable v; while w; Select <a1, v>
       such that Affects*(a1,a2) and Affects*(a1, a3) and Affects*(a1, 5) and Next(s2, _) and Affects(s1, s2)
       and Next*(_,5) and Affects(_,_) and Affects(_,5)
       and Follows(s1, _) and Parent(w1,a1) with s2.stmt# = 5 pattern a2("x",_);
       
       --> among the cfg clauses' arguments, only a3 is unrestricted
    */ 

    //Clauses
    shared_ptr<SelectClause> select = make_shared<SelectClause>(
        SelectClause::createSynonymSelectClause({ assign1Arg, variable1Arg }));

    shared_ptr<AffectsTClause> affectsStarA1A2 = make_shared<AffectsTClause>(assign1Arg, assign2Arg);
    shared_ptr<AffectsTClause> affectsStarA1A3 = make_shared<AffectsTClause>(assign1Arg, assign3Arg);
    shared_ptr<AffectsTClause> affectsStarA15 = make_shared<AffectsTClause>(assign1Arg, fiveArg);
    shared_ptr<AffectsClause> affectsS1S2 = make_shared<AffectsClause>(stmt1Arg, stmt2Arg);
    shared_ptr<AffectsClause> affectsWildcard5 = make_shared<AffectsClause>(wildcardArg, fiveArg);
    shared_ptr<AffectsClause> affectsWildcards = make_shared<AffectsClause>(wildcardArg, wildcardArg);
    shared_ptr<NextTClause> nextStarWildcard5 = make_shared<NextTClause>(wildcardArg, fiveArg);
    shared_ptr<NextClause> nextStmt2Wildcard = make_shared<NextClause>(stmt2Arg, wildcardArg);

    shared_ptr<FollowsClause> followsS1Wildcard = make_shared<FollowsClause>(stmt1Arg, wildcardArg);
    shared_ptr<ParentClause> parentW1A1 = make_shared<ParentClause>(while1Arg, assign1Arg);
    shared_ptr<PatternAssignClause> patternA2X = make_shared<PatternAssignClause>(assign2Arg, 
        xStringLiteralArg, wildcardArg);

    shared_ptr<WithOneExactClause> withS25 = make_shared<WithOneExactClause>(fiveArg,
        vector{ stmt2Arg, stmt2StmtNumAttributeArg });

    //Executed earlier
    ClauseResult assigns({ assign1Arg }, vector<PQLEntity>{ a2, a3 });
    ClauseResult variables({ variable1Arg }, vector<PQLEntity>{ varX});

    ClauseResult follows({ stmt1Arg, wildcardArg }, { w1a3, a3r4 });
    ClauseResult parent({ while1Arg, assign1Arg }, { w1a2 });
    ClauseResult with({ stmt2Arg }, { const5 });
    ClauseResult pattern({ assign2Arg, xStringLiteralArg }, { a2x });

    list<ClauseResult> selectResults = { assigns, variables };
    list<ClauseResult> earlyRelationshipResults = {follows, parent, pattern};
    list<ClauseResult> withResults = { with };

    //Expected restrictions
    unordered_map<ClauseArgument, unordered_set<PQLEntity>> expectedRestrictions({
        {stmt1Arg, {w1, a3}},
        {stmt2Arg, {const5}},
        {assign1Arg, {a2}},
        {assign2Arg, {a2}},
    });

    //Expected order (see CfgClauseOptimiser)
    vector<shared_ptr<CfgRelationshipClause>> expectedClauseOrder({
        nextStmt2Wildcard, nextStarWildcard5, affectsWildcard5, affectsWildcards, affectsS1S2,
        affectsStarA15, affectsStarA1A2, affectsStarA1A3
    });

    Query query(
        { select }, 
        { affectsStarA1A2, affectsStarA1A3, followsS1Wildcard, affectsStarA15,
            nextStmt2Wildcard, affectsS1S2, nextStarWildcard5, affectsWildcard5, parentW1A1,
            affectsWildcards},
        { patternA2X },
        {withS25});

    test(query, selectResults, earlyRelationshipResults, withResults, expectedClauseOrder, expectedRestrictions);
}