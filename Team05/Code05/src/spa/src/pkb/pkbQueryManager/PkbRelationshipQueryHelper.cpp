#include <pkb/pkbQueryManager/PkbRelationshipQueryHelper.h>

// ==================== functions ====================

PkbEntityFilter getFilterFromClauseArgument(ClauseArgument arg, bool alwaysTrue) {
    // default filter is true
    PkbEntityFilter filter = [](shared_ptr<PkbEntity> entity, ClauseArgument arg) {
        return true;
    };

    // depending on clause argument, we return the filter
    if (arg.isWildcard() || alwaysTrue) {
        return filter;
    } else if (arg.isVariableSynonym()) {
        // return only true if entity is a variable
        filter = [](shared_ptr<PkbEntity> entity, ClauseArgument arg) {
            return entity->isVariable();
        };
    } else if (arg.isProcedureSynonym()) {
        // return only true if entity is a procedure
        filter = [](shared_ptr<PkbEntity> entity, ClauseArgument arg) {
            return entity->isProcedure();
        };
    } else if (arg.isStmtSynonym()) {
        // return only true if entity is a statement
        filter = [](shared_ptr<PkbEntity> entity, ClauseArgument arg) {
            return entity->isStatement();
        };
    } else if (arg.isLineNumber()) {
        // entity must be a statement matching line number
        // return only true if entity has that line number
        filter = [](shared_ptr<PkbEntity> entity, ClauseArgument arg) {
            if (!entity->isStatement()) {
                return false;
            }
            // else, cast and check if assign
            shared_ptr<PkbStatementEntity> cast = dynamic_pointer_cast<PkbStatementEntity>(entity);
            return cast->getLineNumber() == arg.getLineNumber();
        };
    } else if (arg.isStringLiteral()) {
        // can be procedure or variable name
        filter = [](shared_ptr<PkbEntity> entity, ClauseArgument arg) {
            if (entity->isVariable()) {
                // castand check
                shared_ptr<PkbVariableEntity> cast = dynamic_pointer_cast<PkbVariableEntity>(
                    entity);
                return cast->getIdentifier() == arg.getIdentifier();
            } else if (entity->isProcedure()) {
                // castand check
                shared_ptr<PkbProcedureEntity> cast = dynamic_pointer_cast<PkbProcedureEntity>(
                    entity);
                return cast->getIdentifier() == arg.getIdentifier();
            }
            return false;
        };
    } else if (arg.isAssignSynonym()) {
        // return only true if entity is a an assign statement
        filter = [](shared_ptr<PkbEntity> entity, ClauseArgument arg) {
            if (!entity->isStatement()) {
                return false;
            }
            // else, cast and check if assign
            shared_ptr<PkbStatementEntity> cast = dynamic_pointer_cast<PkbStatementEntity>(entity);
            return cast->isAssignStatement();
        };
    } else if (arg.isPrintSynonym()) {
        // return only true if entity is a print statement
        filter = [](shared_ptr<PkbEntity> entity, ClauseArgument arg) {
            if (!entity->isStatement()) {
                return false;
            }
            // else, cast and check if print
            shared_ptr<PkbStatementEntity> cast = dynamic_pointer_cast<PkbStatementEntity>(entity);
            return cast->isPrintStatement();
        };
    } else if (arg.isReadSynonym()) {
        // return only true if entity is a read statement
        filter = [](shared_ptr<PkbEntity> entity, ClauseArgument arg) {
            if (!entity->isStatement()) {
                return false;
            }
            // else, cast and check if read
            shared_ptr<PkbStatementEntity> cast = dynamic_pointer_cast<PkbStatementEntity>(entity);
            return cast->isReadStatement();
        };
    } else if (arg.isCallSynonym()) {
        // return only true if entity is a call statement
        filter = [](shared_ptr<PkbEntity> entity, ClauseArgument arg) {
            if (!entity->isStatement()) {
                return false;
            }
            // else, cast and check if call
            shared_ptr<PkbStatementEntity> cast = dynamic_pointer_cast<PkbStatementEntity>(entity);
            return cast->isCallStatement();
        };
    } else if (arg.isWhileSynonym()) {
        // return only true if entity is a while statement
        filter = [](shared_ptr<PkbEntity> entity, ClauseArgument arg) {
            if (!entity->isStatement()) {
                return false;
            }
            // else, cast and check if while
            shared_ptr<PkbStatementEntity> cast = dynamic_pointer_cast<PkbStatementEntity>(entity);
            return cast->isWhileStatement();
        };
    } else if (arg.isIfSynonym()) {
        // return only true if entity is if  statement
        filter = [](shared_ptr<PkbEntity> entity, ClauseArgument arg) {
            if (!entity->isStatement()) {
                return false;
            }
            // else, cast and check if if
            shared_ptr<PkbStatementEntity> cast = dynamic_pointer_cast<PkbStatementEntity>(entity);
            return cast->isIfStatement();
        };
    }

    return filter;
}

// ==================== private ====================
// ******************** relationship query handlers ********************
vector<shared_ptr<PkbRelationship>> PkbRelationshipQueryHelper::retrieveNextStarByTypeAndLhsRhs(
    ClauseArgument lhs, ClauseArgument rhs, shared_ptr<PkbRepository> repository, bool optimized) {
    // 1. validation - must be a statement
    if (!lhs.isStmtRefNoWildcard() && !lhs.isWildcard()) {
        throw PkbException("NEXTSTAR relationship expects lhs and rhs to both be statements!");
    }

    // 2. there are four cases.
    // if lhs and rhs are exact, we can do a direct check.
    // if exact and _, we need to do dfs from the specified node.
    // if _ and exact, we do dfs starting from the root node.
    // if _ and _, we do dfs from the root node and accumulate.
    vector<shared_ptr<PkbRelationship>> out;

    vector<shared_ptr<PkbGraphManager>> cfgs = repository->getCfgs();

    for (int i = 0; i < cfgs.size(); i++) {
        shared_ptr<PkbGraphManager> cfgManager = cfgs[i];

        // case 1: both exact
        if (lhs.isExactReference() && rhs.isExactReference()) {
            // convert to entity
            shared_ptr<PkbEntity> lhsEntity = this->convertClauseArgumentToPkbEntity(lhs);
            shared_ptr<PkbEntity> rhsEntity = this->convertClauseArgumentToPkbEntity(rhs);
            shared_ptr<PkbStatementEntity> left = dynamic_pointer_cast<PkbStatementEntity>(
                lhsEntity);
            shared_ptr<PkbStatementEntity> right = dynamic_pointer_cast<PkbStatementEntity>(
                rhsEntity);

            // check for impossible match
            if (!repository->statementsAreInTheSameProcedure(
                lhs.getLineNumber(),
                rhs.getLineNumber())) {
                return out;
            }

            // check cache
            shared_ptr<PkbNextStarRelationship> positiveNextStar =
                make_shared<PkbNextStarRelationship>(lhsEntity, rhsEntity);
            shared_ptr<PkbNotNextStarRelationship> negativeNextStar =
                make_shared<PkbNotNextStarRelationship>(lhsEntity, rhsEntity);
            if (this->findPkbRelationshipFromRepository(positiveNextStar, repository) != NULL) {
                // positive match
                out.push_back(positiveNextStar);
                continue;
            } else if (this->findPkbRelationshipFromRepository(negativeNextStar, repository) !=
                NULL) {
                // negative match
                continue;
            }

            // create graph nodes and get their keys
            string leftKey = PkbControlFlowGraphNode::createPkbControlFlowGraphNode(left)->
                getKey();
            string rightKey = PkbControlFlowGraphNode::createPkbControlFlowGraphNode(right)->
                getKey();

            // check they are both inside
            // if they are connected, return. else continue looking in other CFGs.
            if (cfgManager->isInside(leftKey) && cfgManager->isInside(rightKey)
                && cfgManager->canReachNodeBFromNodeA(leftKey, rightKey)) {
                // add to result
                out.push_back(make_shared<PkbNextStarRelationship>(lhsEntity, rhsEntity));

                // cache
                this->addPkbRelationship(positiveNextStar, repository);
                break;

            } else if (i == cfgs.size() - 1) {
                // path cannot be found between the nodes in any of the CFGs,
                // cache the negative result.
                this->addPkbRelationship(negativeNextStar, repository);
            }
            continue;
        }

        // otherwise, we will need to extract and filter
        PkbEntityFilter lhsFilter = getFilterFromClauseArgument(lhs);
        PkbEntityFilter rhsFilter = getFilterFromClauseArgument(rhs);

        // extractors
        PkbGraphNextStarRelationshipExtractor extractor;
        vector<shared_ptr<PkbRelationship>> extractedRelationships;

        // case 2: exact and wildcard
        if (lhs.isExactReference() && (rhs.isWildcard() || rhs.isSynonym())) {
            // convert lhs to entity, graph node, then get node
            shared_ptr<PkbStatementEntity> left = dynamic_pointer_cast<PkbStatementEntity>(
                this->convertClauseArgumentToPkbEntity(lhs));
            string leftKey = PkbControlFlowGraphNode::createPkbControlFlowGraphNode(left)->
                getKey();

            if (cfgManager->isInside(leftKey)) {
                shared_ptr<PkbGraphNode> leftAsNode =
                    PkbControlFlowGraphNode::createPkbControlFlowGraphNode(left);
                shared_ptr<PkbControlFlowGraphNode> startNode = static_pointer_cast<
                    PkbControlFlowGraphNode>(cfgManager->getNode(leftAsNode->getKey()));

                // starting from node, run dfs
                extractor.extractAllFromStart(startNode, true);
                extractedRelationships = extractor.getExtractedRelationships();
            }
        } else if ((lhs.isWildcard() || lhs.isSynonym()) && (rhs.isExactReference())) {
            // case 3: wildcard and exact
            // convert rhs to entity, graph node, then get target node
            shared_ptr<PkbStatementEntity> right = dynamic_pointer_cast<PkbStatementEntity>(
                this->convertClauseArgumentToPkbEntity(rhs));
            string rightKey = PkbControlFlowGraphNode::createPkbControlFlowGraphNode(right)->
                getKey();

            if (cfgManager->isInside(rightKey)) {
                shared_ptr<PkbGraphNode> rightAsNode =
                    PkbControlFlowGraphNode::createPkbControlFlowGraphNode(right);
                shared_ptr<PkbControlFlowGraphNode> endNode = static_pointer_cast<
                    PkbControlFlowGraphNode>(cfgManager->getNode(rightAsNode->getKey()));
                shared_ptr<PkbControlFlowGraphNode> startNode = static_pointer_cast<
                    PkbControlFlowGraphNode>(cfgManager->getRootNode());

                // starting from node, run dfs
                extractor.extractAllThatReachEnd(startNode, endNode, true);
                extractedRelationships = extractor.getExtractedRelationships();
            }
        } else {
            // case 4: all wild card
            // starting at root node, dfs all the way
            shared_ptr<PkbControlFlowGraphNode> startNode = static_pointer_cast<
                PkbControlFlowGraphNode>(cfgManager->getRootNode());
            extractor.extractAllFromStart(startNode);
            extractedRelationships = extractor.getExtractedRelationships();
        }

        // filter by lhs and rhs type
        for (shared_ptr<PkbRelationship> r : extractedRelationships) {
            shared_ptr<PkbEntity> lhsEntity = r->getLhs();
            shared_ptr<PkbEntity> rhsEntity = r->getRhs();

            bool hasMatch = lhsFilter(lhsEntity, lhs) && rhsFilter(rhsEntity, rhs);

            if (hasMatch && (lhs != rhs || (lhs == rhs && lhsEntity->equals(rhsEntity)))) {
                out.push_back(r);
            }
        }
    }
    return out;
}

vector<shared_ptr<PkbRelationship>> PkbRelationshipQueryHelper::retrieveAffectsByTypeAndLhsRhs(
    ClauseArgument lhs, ClauseArgument rhs, shared_ptr<PkbRepository> repository, bool optimized) {

    vector<shared_ptr<PkbRelationship>> out;

    // 0. validation - must be a statement
    if (!lhs.isStmtRefNoWildcard() && !lhs.isWildcard()) {
        throw PkbException("AFFECTS relationship expects lhs and rhs to both be statements!");
    }

    // case 1: exact, exact
    if (lhs.isExactReference() && rhs.isExactReference()) {
        for (shared_ptr<PkbGraphManager> cfgManager : repository->getCfgs()) {
            // 0. check if lhs and rhs are both assign and exist
            shared_ptr<PkbEntity> lhsEntity = this->convertClauseArgumentToPkbEntity(lhs);
            shared_ptr<PkbEntity> rhsEntity = this->convertClauseArgumentToPkbEntity(rhs);
            if (!this->entityExistAndIsAssign(lhsEntity, repository) || !this->
                entityExistAndIsAssign(rhsEntity, repository)) {
                return out;
            }

            // 1. check cache
            // 1.1 check for positive match
            shared_ptr<PkbAffectsRelationship> positiveMatch = make_shared<PkbAffectsRelationship>(
                lhsEntity,
                rhsEntity);
            if (this->findPkbRelationshipFromRepository(positiveMatch, repository) != NULL) {
                out.push_back(positiveMatch);
                return out;
            }
            // 1.2 check for negative match
            shared_ptr<PkbNotAffectsRelationship> negativeMatch = make_shared<
                PkbNotAffectsRelationship>(lhsEntity, rhsEntity);
            if (this->findPkbRelationshipFromRepository(negativeMatch, repository) != NULL) {
                return out;
            }

            // 2. short circuit 1: check that there are variables lhs modifies and rhs uses
            unordered_map<string, shared_ptr<PkbEntity>> intersectingVariableMap = this->
                getLhsModifiesRhsUsesIntersectingVariableMap(lhs, rhs, repository);
            if (intersectingVariableMap.size() == 0) {
                return out;
            }

            // 3.1 short circuit 1: check that they belong in the same procedure
            if (!repository->statementsAreInTheSameProcedure(
                lhs.getLineNumber(),
                rhs.getLineNumber())) {
                return out;
            }

            // 3.2 short circuit 2: check that lhs and rhs are reachable using next*
            if (this->retrieveNextStarByTypeAndLhsRhs(lhs, rhs, repository, optimized).size()
                == 0) {
                return out;
            }

            // 4. not in either, brute force check
            // 4.1 get start node (lhs)
            shared_ptr<PkbStatementEntity> left = dynamic_pointer_cast<PkbStatementEntity>(
                lhsEntity);
            string leftKey = PkbControlFlowGraphNode::createPkbControlFlowGraphNode(left)->
                getKey();

            // 4.2 get end node (rhs)
            shared_ptr<PkbStatementEntity> right = dynamic_pointer_cast<PkbStatementEntity>(
                rhsEntity);
            string rightKey = PkbControlFlowGraphNode::createPkbControlFlowGraphNode(right)->
                getKey();

            // 4.3 check that lhs and rhs are in the current CFG
            if (!cfgManager->isInside(leftKey) || !cfgManager->isInside(rightKey)) {
                continue;
            }

            shared_ptr<PkbControlFlowGraphNode> startNode = static_pointer_cast<
                PkbControlFlowGraphNode>(cfgManager->getNode(leftKey));
            shared_ptr<PkbControlFlowGraphNode> endNode = static_pointer_cast<
                PkbControlFlowGraphNode>(cfgManager->getNode(rightKey));

            // 4.4 use graph extractor to check
            // then cache and return
            PkbGraphAffectsRelationshipExtractor extractor;
            if (extractor.hasAffectsRelationship(
                startNode,
                endNode,
                repository,
                intersectingVariableMap)) {
                out.push_back(positiveMatch);
                this->addPkbRelationship(positiveMatch, repository);
                break;
            } else {
                this->addPkbRelationship(negativeMatch, repository);
            }
        }
    } else if (lhs.isExactReference() && (rhs.isWildcard() || rhs.isSynonym())) {
        // case 2: exact, non exact

        // 0.1 check if lhs and rhs is assign and exist
        shared_ptr<PkbEntity> lhsEntity = this->convertClauseArgumentToPkbEntity(lhs);
        if (!this->entityExistAndIsAssign(lhsEntity, repository)) {
            return out;
        }

        // 0.2 if synonym is not assign, empty
        if (!rhs.isWildcard() && !rhs.isAssignSynonym() && !rhs.isStmtSynonym()) {
            return out;
        }

        // 1. find all assign after lhs
        vector<shared_ptr<PkbEntity>> statements = repository->getEntityTableByType(
            PkbEntityType::STATEMENT)->getAll();

        // 2. for all these, check and append
        // O(n) enumeration
        for (shared_ptr<PkbEntity> statement : statements) {
            // cast
            shared_ptr<PkbStatementEntity> cast = dynamic_pointer_cast<PkbStatementEntity>(
                statement);
            assert(cast != nullptr);

            // check
            if (cast->isAssignStatement()) {
                // convert rhs to exact clause argument
                ClauseArgument castAsRhs = ClauseArgument::createLineNumberArg(
                    to_string(cast->getLineNumber()));

                // do self query for exact
                vector<shared_ptr<PkbRelationship>> found = this->retrieveAffectsByTypeAndLhsRhs(
                    lhs,
                    castAsRhs,
                    repository,
                    optimized);
                out.insert(out.end(), found.begin(), found.end());
                if (found.size() > 0 && optimized && rhs.isWildcard()) {
                    break; // if found 1 with lhs, done.
                }
            }
        }
    } else if (rhs.isExactReference() && (lhs.isWildcard() || lhs.isSynonym())) {
        // case 3: non exact, exact
        // 0.1 check if lhs and rhs is assign and exist
        shared_ptr<PkbEntity> rhsEntity = this->convertClauseArgumentToPkbEntity(rhs);
        if (!this->entityExistAndIsAssign(rhsEntity, repository)) {
            return out;
        }

        // 0.2 if synonym is not assign, empty
        if (!lhs.isWildcard() && !lhs.isAssignSynonym() && !lhs.isStmtSynonym()) {
            return out;
        }

        // 1. get all
        vector<shared_ptr<PkbEntity>> statements = repository->getEntityTableByType(
            PkbEntityType::STATEMENT)->getAll();

        // 2. for all these, check and append
        // O(n) enumeration
        for (shared_ptr<PkbEntity> statement : statements) {
            // cast
            shared_ptr<PkbStatementEntity> cast = dynamic_pointer_cast<PkbStatementEntity>(
                statement);
            assert(cast != nullptr);

            // check
            if (cast->isAssignStatement()) {
                // convert lhs to exact clause argument
                ClauseArgument castAsLhs = ClauseArgument::createLineNumberArg(
                    to_string(cast->getLineNumber()));

                // do self query for exact
                vector<shared_ptr<PkbRelationship>> found = this->retrieveAffectsByTypeAndLhsRhs(
                    castAsLhs,
                    rhs,
                    repository,
                    optimized);
                out.insert(out.end(), found.begin(), found.end());
                if (found.size() > 0 && optimized && lhs.isWildcard()) {
                    break; // if found 1 with rhs, done.
                }
            }
        }

    } else if ((lhs.isWildcard() || lhs.isSynonym()) && (rhs.isWildcard() || rhs.isSynonym())) {
        // case 4: non exact, non exact
        // 0.check if lhs and rhs are not wildcards, assigns or stmts
        if (!lhs.isWildcard() && !lhs.isAssignSynonym() && !lhs.isStmtSynonym() || !rhs.
            isWildcard() && !rhs.isAssignSynonym() && !rhs.isStmtSynonym()) {
            return out;
        }

        // 1. check if lhs and rhs should refer to the same
        bool lhsRhsSame = false;
        if (!lhs.isWildcard() && !rhs.isWildcard()) {
            lhsRhsSame = (lhs == rhs);
        }

        // 2. find all assign
        vector<shared_ptr<PkbEntity>> statements = repository->getEntityTableByType(
            PkbEntityType::STATEMENT)->getAll();

        // 3. for all pairs (diagonal matrix), check and append
        // 3.1 determine case. last case is synonym synonym (default)
        bool isSynonymWildcard = lhs.isSynonym() && rhs.isWildcard();
        bool isWildcardWildcard = lhs.isWildcard() && rhs.isWildcard();
        bool isWildcardSynonym = lhs.isWildcard() && rhs.isSynonym();

        // 3.2  O(n2) enumeration
        for (shared_ptr<PkbEntity> statement1 : statements) {
            bool foundOne = false;
            // cast
            shared_ptr<PkbStatementEntity> cast1 = dynamic_pointer_cast<PkbStatementEntity>(
                statement1);
            assert(cast1 != nullptr);

            // If first statement is not assign, then we skip over it.
            if (!cast1->isAssignStatement()) {
                continue;
            }

            for (shared_ptr<PkbEntity> statement2 : statements) {
                shared_ptr<PkbStatementEntity> cast2 = dynamic_pointer_cast<PkbStatementEntity>(
                    statement2);
                assert(cast2 != nullptr);

                // check
                if (cast2->isAssignStatement() // second statement must also be assign
                    && ((lhsRhsSame && cast1->equals(cast2)) || !lhsRhsSame)) {
                    // if both same, lhs and rhs must be same
                    // convert to exact clause argument
                    ClauseArgument castAsLhs = ClauseArgument::createLineNumberArg(
                        to_string(cast1->getLineNumber()));
                    ClauseArgument castAsRhs = ClauseArgument::createLineNumberArg(
                        to_string(cast2->getLineNumber()));

                    if (isWildcardSynonym) {
                        // flip the order so that rhs is the outer loop and lhs is the inner loop
                        castAsLhs = ClauseArgument::createLineNumberArg(
                            to_string(cast2->getLineNumber()));
                        castAsRhs = ClauseArgument::createLineNumberArg(
                            to_string(cast1->getLineNumber()));
                    }

                    // do self query for exact
                    vector<shared_ptr<PkbRelationship>> found = this->
                        retrieveAffectsByTypeAndLhsRhs(
                            castAsLhs,
                            castAsRhs,
                            repository,
                            optimized);
                    out.insert(out.end(), found.begin(), found.end());
                    if (found.size() > 0 && optimized) {
                        foundOne = true;

                        if (isSynonymWildcard || isWildcardWildcard || isWildcardSynonym) {
                            break; // found that satisfies rhs (or lhs), can break to outer loop
                        }
                    }
                }
            }
            if (foundOne && optimized && isWildcardWildcard) {
                break; // found one, can just stop
            }
        }
    } else {
        throw PkbException("Unknown case for affects!");
    }
    return out;
}

vector<shared_ptr<PkbRelationship>> PkbRelationshipQueryHelper::retrieveAffectsStarByTypeAndLhsRhs(
    ClauseArgument lhs, ClauseArgument rhs, shared_ptr<PkbRepository> repository, bool optimized) {
    // 0. validity check - both assign/exact/wildcard
    if (!lhs.isStmtRefNoWildcard() && !lhs.isWildcard()) {
        throw PkbException("AFFECTS* relationship expects lhs and rhs to both be statements!");
    }

    vector<shared_ptr<PkbRelationship>> out;

    // case 1: exact, exact
    // check if affects(lhs, rhs)
    // if not, O(n) enumeration of affects(lhs, s), affects*(s, rhs)
    if (lhs.isExactReference() && rhs.isExactReference()) {
        // 0. check if lhs and rhs are both assign and exist
        shared_ptr<PkbEntity> lhsEntity = this->convertClauseArgumentToPkbEntity(lhs);
        shared_ptr<PkbEntity> rhsEntity = this->convertClauseArgumentToPkbEntity(rhs);
        if (!this->entityExistAndIsAssign(lhsEntity, repository) || !this->entityExistAndIsAssign(
            rhsEntity,
            repository)) {
            return out;
        }

        // 1. check cache
        // 1.1 check for positive match
        shared_ptr<PkbAffectsStarRelationship> positiveMatch = make_shared<
            PkbAffectsStarRelationship>(lhsEntity, rhsEntity);
        if (this->findPkbRelationshipFromRepository(positiveMatch, repository) != NULL) {
            out.push_back(positiveMatch);
            return out;
        }
        // 1.2 check for negative match
        shared_ptr<PkbNotAffectsStarRelationship> negativeMatch = make_shared<
            PkbNotAffectsStarRelationship>(lhsEntity, rhsEntity);
        if (this->findPkbRelationshipFromRepository(negativeMatch, repository) != NULL) {
            return out;
        }

        // 2. check if affects(lhs, rhs)
        vector<shared_ptr<PkbRelationship>> foundAffects = this->
            retrieveRelationshipsFromGraphsByTypeAndLhsRhs(
                PkbRelationshipType::AFFECTS,
                lhs,
                rhs,
                repository,
                optimized);
        if (foundAffects.size() == 1) {
            out.push_back(positiveMatch);

            // cache
            this->addPkbRelationship(positiveMatch, repository);
            return out;
        }

        // 3. else, for all assign, find a middle assign recursively
        // update cache as you go
        // O(n) enumeration
        vector<shared_ptr<PkbEntity>> statements = repository->getEntityTableByType(
            PkbEntityType::STATEMENT)->getAll();
        bool success = false;
        for (shared_ptr<PkbEntity> statement : statements) {
            // if is the same as either, skip
            if (statement->equals(lhsEntity) || statement->equals(rhsEntity)) {
                continue;
            }

            // cast
            shared_ptr<PkbStatementEntity> cast = dynamic_pointer_cast<PkbStatementEntity>(
                statement);
            assert(cast != nullptr);

            // check
            if (cast->isAssignStatement()) {
                // check if left->statement visited before, if visited, skip
                pair<string, string> affectsGraphEdge = pair<string, string>(
                    lhsEntity->getKey(),
                    statement->getKey());
                if (this->visitedAffectsEdges.count(affectsGraphEdge)) {
                    continue;
                }

                // convert lhs to exact clause argument
                ClauseArgument arg = ClauseArgument::createLineNumberArg(
                    to_string(cast->getLineNumber()));

                // do self query for exact affects(lhs->stmt)
                vector<shared_ptr<PkbRelationship>> foundWithLhs = this->
                    retrieveAffectsByTypeAndLhsRhs(lhs, arg, repository, optimized);

                if (foundWithLhs.size() == 0) {
                    // not found, continue
                    continue;
                } else {
                    // try to search recursively affects*(stmt->rhs)
                    vector<shared_ptr<PkbRelationship>> foundWithRhs = this->
                        retrieveAffectsStarByTypeAndLhsRhs(arg, rhs, repository, optimized);
                    if (foundWithRhs.size() >= 1) {
                        // success, cache it
                        this->addPkbRelationship(positiveMatch, repository);

                        // insert
                        out.push_back(positiveMatch);
                        success = true;
                        break;
                    } else {
                        continue;
                    }
                }
            }
        }
    } else if (lhs.isExactReference() && (rhs.isWildcard() || rhs.isSynonym())) {
        // case 2: exact, non
        // 0. check that exact exists and synonym is not invalid (e.g. while)
        if (!rhs.isWildcard() && !rhs.isStmtSynonym() && !rhs.isAssignSynonym()) {
            return out;
        }
        shared_ptr<PkbEntity> lhsEntity = this->convertClauseArgumentToPkbEntity(lhs);
        if (!this->entityExistAndIsAssign(lhsEntity, repository)) {
            return out;
        }

        // 1. get all statements
        vector<shared_ptr<PkbEntity>> statements = repository->getEntityTableByType(
            PkbEntityType::STATEMENT)->getAll();

        // 2. for all assign, check if affects*(lhs, s)
        for (shared_ptr<PkbEntity> statement : statements) {
            // cast
            shared_ptr<PkbStatementEntity> cast = dynamic_pointer_cast<PkbStatementEntity>(
                statement);
            assert(cast != nullptr);

            // check
            if (cast->isAssignStatement()) {
                ClauseArgument sArg = ClauseArgument::createLineNumberArg(
                    to_string(cast->getLineNumber()));
                vector<shared_ptr<PkbRelationship>> found = this->
                    retrieveAffectsStarByTypeAndLhsRhs(lhs, sArg, repository, optimized);
                out.insert(out.end(), found.begin(), found.end());
                if (found.size() > 0 && optimized && rhs.isWildcard()) {
                    break; // if found 1 with lhs, done.
                }
            }
        }
    } else if (rhs.isExactReference() && (lhs.isWildcard() || lhs.isSynonym())) {
        // case 3: non, exact
        // O(n) enumeration of affects*(s, rhs)
        // 0. check that exact exists and synonym is not invalid (e.g. while)
        if (!lhs.isWildcard() && !lhs.isStmtSynonym() && !lhs.isAssignSynonym()) {
            return out;
        }
        shared_ptr<PkbEntity> rhsEntity = this->convertClauseArgumentToPkbEntity(rhs);
        if (!this->entityExistAndIsAssign(rhsEntity, repository)) {
            return out;
        }


        // 1. get all statements
        vector<shared_ptr<PkbEntity>> statements = repository->getEntityTableByType(
            PkbEntityType::STATEMENT)->getAll();

        // 2. for all assign, check if affects*(lhs, s)
        for (shared_ptr<PkbEntity> statement : statements) {
            // cast
            shared_ptr<PkbStatementEntity> cast = dynamic_pointer_cast<PkbStatementEntity>(
                statement);
            assert(cast != nullptr);

            // check
            if (cast->isAssignStatement()) {
                ClauseArgument sArg = ClauseArgument::createLineNumberArg(
                    to_string(cast->getLineNumber()));
                vector<shared_ptr<PkbRelationship>> found = this->
                    retrieveAffectsStarByTypeAndLhsRhs(sArg, rhs, repository, optimized);
                out.insert(out.end(), found.begin(), found.end());
                if (found.size() > 0 && optimized && lhs.isWildcard()) {
                    break; // if found 1 with rhs, done.
                }
            }
        }
    } else if ((lhs.isWildcard() || lhs.isSynonym()) && (rhs.isWildcard() || rhs.isSynonym())) {
        // case 4: wildcard, wildcard
        // O(n2) enumeration of affects*(s1, s2)

        // 0.check if lhs and rhs are not wildcards, assigns or stmts
        if (!lhs.isWildcard() && !lhs.isAssignSynonym() && !lhs.isStmtSynonym() || !rhs.
            isWildcard() && !rhs.isAssignSynonym() && !rhs.isStmtSynonym()) {
            return out;
        }

        // 1. check if lhs and rhs should refer to the same
        bool lhsRhsSame = false;
        if (!lhs.isWildcard() && !rhs.isWildcard()) {
            lhsRhsSame = (lhs == rhs);
        }

        // 2. enumerate
        // 2.1 determine case. last case is synonym synonym (default)
        bool isSynonymWildcard = lhs.isSynonym() && rhs.isWildcard();
        bool isWildcardWildcard = lhs.isWildcard() && rhs.isWildcard();
        bool isWildcardSynonym = lhs.isWildcard() && rhs.isSynonym();

        // 2.2 O(n2) enumerate
        vector<shared_ptr<PkbEntity>> statements = repository->getEntityTableByType(
            PkbEntityType::STATEMENT)->getAll();
        for (shared_ptr<PkbEntity> statement1 : statements) {
            bool foundOne = false;

            // cast
            shared_ptr<PkbStatementEntity> cast1 = dynamic_pointer_cast<PkbStatementEntity>(
                statement1);
            assert(cast1 != nullptr);

            // check
            for (shared_ptr<PkbEntity> statement2 : statements) {
                shared_ptr<PkbStatementEntity> cast2 = dynamic_pointer_cast<PkbStatementEntity>(
                    statement2);
                assert(cast2 != nullptr);
                if (cast1->isAssignStatement() && cast2->isAssignStatement() && ((lhsRhsSame &&
                    cast1->equals(cast2)) || !lhsRhsSame)) {
                    ClauseArgument leftArg = ClauseArgument::createLineNumberArg(
                        to_string(cast1->getLineNumber()));
                    ClauseArgument rightArg = ClauseArgument::createLineNumberArg(
                        to_string(cast2->getLineNumber()));
                    if (isWildcardSynonym) {
                        // flip the order so that rhs is the outer loop and lhs is the inner loop
                        leftArg = ClauseArgument::createLineNumberArg(
                            to_string(cast2->getLineNumber()));
                        rightArg = ClauseArgument::createLineNumberArg(
                            to_string(cast1->getLineNumber()));
                    }

                    // do self query for exact
                    vector<shared_ptr<PkbRelationship>> found = this->
                        retrieveAffectsStarByTypeAndLhsRhs(
                            leftArg,
                            rightArg,
                            repository,
                            optimized);
                    out.insert(out.end(), found.begin(), found.end());
                    if (found.size() > 0 && optimized) {
                        foundOne = true;
                        if (isSynonymWildcard || isWildcardWildcard || isWildcardSynonym) {
                            break; // found that satisfies rhs (or lhs), can break to outer loop
                        }
                    }
                }
            }
            if (foundOne && optimized && isWildcardWildcard) {
                break;
            }
        }
    } else {
        throw PkbException("Unknown case for affects!");
    }

    return out;
}


// ==================== public ====================
vector<shared_ptr<PkbRelationship>>
PkbRelationshipQueryHelper::retrieveRelationshipsFromTablesByTypeAndLhsRhs(
    PkbRelationshipType relationshipType, ClauseArgument lhs, ClauseArgument rhs,
    shared_ptr<PkbRepository> repository) {
    // 0. get table based on type
    shared_ptr<PkbRelationshipTable> table = repository->getRelationshipTableByRelationshipType(
        relationshipType);

    // 1. short circuiting
    if (this->canShortCircuitRetrieveRelationshipByTypeAndLhsRhs(relationshipType, lhs, rhs)) {
        return vector<shared_ptr<PkbRelationship>>();
    }

    // 2a. if both side are exact, we can search by hash
    // we create the key we are looking for based on lhs and rhs
    if (lhs.isExactReference() && rhs.isExactReference()) {
        // 2.1 create relationship
        shared_ptr<PkbEntity> left = this->convertClauseArgumentToPkbEntity(lhs);
        shared_ptr<PkbEntity> right = this->convertClauseArgumentToPkbEntity(rhs);
        shared_ptr<PkbRelationship> toFind;
        switch (relationshipType) {
        case PkbRelationshipType::FOLLOWS:
            toFind = make_shared<PkbFollowsRelationship>(left, right);
            break;
        case PkbRelationshipType::FOLLOWSSTAR:
            toFind = make_shared<PkbFollowsStarRelationship>(left, right);
            break;
        case PkbRelationshipType::PARENT:
            toFind = make_shared<PkbParentRelationship>(left, right);
            break;
        case PkbRelationshipType::PARENTSTAR:
            toFind = make_shared<PkbParentStarRelationship>(left, right);
            break;
        case PkbRelationshipType::USES:
            toFind = make_shared<PkbUsesRelationship>(left, right);
            break;
        case PkbRelationshipType::MODIFIES:
            toFind = make_shared<PkbModifiesRelationship>(left, right);
            break;
        case PkbRelationshipType::NEXT:
            toFind = make_shared<PkbNextRelationship>(left, right);
            break;
        case PkbRelationshipType::CALLSTMTATTRIBUTE:
            toFind = make_shared<PkbCallStmtAttributeRelationship>(left, right);
            break;
        case PkbRelationshipType::CALLS:
            toFind = make_shared<PkbCallsRelationship>(left, right);
            break;
        case PkbRelationshipType::CALLSSTAR:
            toFind = make_shared<PkbCallsStarRelationship>(left, right);
            break;
        default:
            throw PkbException("Unknown relationship type to be retrieved!");
        }

        // 2.2 get by key
        string key = toFind->getKey();
        shared_ptr<PkbRelationship> found = table->get(key);

        // 2.3 if null, return empty. else, return
        vector<shared_ptr<PkbRelationship>> retrievedRelationships;
        if (found == NULL) {
            return retrievedRelationships;
        }
        retrievedRelationships.push_back(found);
        return retrievedRelationships;
    }

    // 2b. if not, we have to manually filter
    PkbEntityFilter lhsFilter = getFilterFromClauseArgument(lhs);
    PkbEntityFilter rhsFilter = getFilterFromClauseArgument(rhs);
    vector<shared_ptr<PkbRelationship>> out;
    for (shared_ptr<PkbRelationship> relationship : table->getAll()) {
        shared_ptr<PkbEntity> lhsEntity = relationship->getLhs();
        shared_ptr<PkbEntity> rhsEntity = relationship->getRhs();

        if (lhsFilter(lhsEntity, lhs) && rhsFilter(rhsEntity, rhs)) {
            out.push_back(relationship);
        }
    }
    return out;
}

vector<shared_ptr<PkbRelationship>>
PkbRelationshipQueryHelper::retrieveRelationshipsFromGraphsByTypeAndLhsRhs(
    PkbRelationshipType relationshipType, ClauseArgument lhs, ClauseArgument rhs,
    shared_ptr<PkbRepository> repository, bool optimized) {
    if (relationshipType == PkbRelationshipType::NEXTSTAR) {
        return this->retrieveNextStarByTypeAndLhsRhs(lhs, rhs, repository, optimized);
    } else if (relationshipType == PkbRelationshipType::AFFECTS) {
        return this->retrieveAffectsByTypeAndLhsRhs(lhs, rhs, repository, optimized);
    } else if (relationshipType == PkbRelationshipType::AFFECTSSTAR) {
        return this->retrieveAffectsStarByTypeAndLhsRhs(lhs, rhs, repository, optimized);
    } else {
        throw PkbException("Unknown graph type relationship trying to be retrieved.");
    }
}
