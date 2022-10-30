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
	}
	else if (arg.isVariableSynonym()) {
		// return only true if entity is a variable
		filter = [](shared_ptr<PkbEntity> entity, ClauseArgument arg) {
			return entity->isVariable();
		};
	}
	else if (arg.isProcedureSynonym()) {
		// return only true if entity is a procedure
		filter = [](shared_ptr<PkbEntity> entity, ClauseArgument arg) {
			return entity->isProcedure();
		};
	}
	else if (arg.isStmtSynonym()) {
		// return only true if entity is a statement
		filter = [](shared_ptr<PkbEntity> entity, ClauseArgument arg) {
			return entity->isStatement();
		};
	}
	else if (arg.isLineNumber()) {
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
	}
	else if (arg.isStringLiteral()) {
		// can be procedure or variable name
		filter = [](shared_ptr<PkbEntity> entity, ClauseArgument arg) {
			if (entity->isVariable()) {
				// castand check 
				shared_ptr<PkbVariableEntity> cast = dynamic_pointer_cast<PkbVariableEntity>(entity);
				return cast->getIdentifier() == arg.getIdentifier();
			}
			else if (entity->isProcedure()) {
				// castand check 
				shared_ptr<PkbProcedureEntity> cast = dynamic_pointer_cast<PkbProcedureEntity>(entity);
				return cast->getIdentifier() == arg.getIdentifier();
			}
			return false;
		};

	}
	else if (arg.isAssignSynonym()) {
		// return only true if entity is a an assign statement
		filter = [](shared_ptr<PkbEntity> entity, ClauseArgument arg) {
			if (!entity->isStatement()) {
				return false;
			}
			// else, cast and check if assign
			shared_ptr<PkbStatementEntity> cast = dynamic_pointer_cast<PkbStatementEntity>(entity);
			return cast->isAssignStatement();
		};
	}
	else if (arg.isPrintSynonym()) {
		// return only true if entity is a print statement
		filter = [](shared_ptr<PkbEntity> entity, ClauseArgument arg) {
			if (!entity->isStatement()) {
				return false;
			}
			// else, cast and check if print
			shared_ptr<PkbStatementEntity> cast = dynamic_pointer_cast<PkbStatementEntity>(entity);
			return cast->isPrintStatement();
		};
	}
	else if (arg.isReadSynonym()) {
		// return only true if entity is a read statement
		filter = [](shared_ptr<PkbEntity> entity, ClauseArgument arg) {
			if (!entity->isStatement()) {
				return false;
			}
			// else, cast and check if read
			shared_ptr<PkbStatementEntity> cast = dynamic_pointer_cast<PkbStatementEntity>(entity);
			return cast->isReadStatement();
		};
	}
	else if (arg.isCallSynonym()) {
		// return only true if entity is a call statement
		filter = [](shared_ptr<PkbEntity> entity, ClauseArgument arg) {
			if (!entity->isStatement()) {
				return false;
			}
			// else, cast and check if call
			shared_ptr<PkbStatementEntity> cast = dynamic_pointer_cast<PkbStatementEntity>(entity);
			return cast->isCallStatement();
		};
	}
	else if (arg.isWhileSynonym()) {
		// return only true if entity is a while statement
		filter = [](shared_ptr<PkbEntity> entity, ClauseArgument arg) {
			if (!entity->isStatement()) {
				return false;
			}
			// else, cast and check if while
			shared_ptr<PkbStatementEntity> cast = dynamic_pointer_cast<PkbStatementEntity>(entity);
			return cast->isWhileStatement();
		};
	}
	else if (arg.isIfSynonym()) {
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
vector<shared_ptr<PkbRelationship>> PkbRelationshipQueryHelper::retrieveNextStarByTypeAndLhsRhs(ClauseArgument lhs, ClauseArgument rhs, shared_ptr<PkbRepository> repository) {
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
		for (shared_ptr<PkbGraphManager> cfgManager : repository->getCfgs()) {
			
			// case 1: both exact
			if (lhs.isExactReference() && rhs.isExactReference()) {
				// construct key from lhs and rhs
				shared_ptr<PkbEntity> lhsEntity = this->convertClauseArgumentToPkbEntity(lhs);
				shared_ptr<PkbEntity> rhsEntity = this->convertClauseArgumentToPkbEntity(rhs);
				shared_ptr<PkbStatementEntity> left = dynamic_pointer_cast<PkbStatementEntity>(lhsEntity);
				shared_ptr<PkbStatementEntity> right = dynamic_pointer_cast<PkbStatementEntity>(rhsEntity);

				// create graph nodes and get their keys 
				string leftKey = PkbControlFlowGraphNode::createPkbControlFlowGraphNode(left)->getKey();
				string rightKey = PkbControlFlowGraphNode::createPkbControlFlowGraphNode(right)->getKey();

				// check they are both inside
				// if they are connected, return. else, return empty list
				if (cfgManager->isInside(leftKey)
					&& cfgManager->isInside(rightKey)
					&& cfgManager->canReachNodeBFromNodeA(leftKey, rightKey)) {
					out.push_back(shared_ptr<PkbRelationship>(new PkbNextStarRelationship(lhsEntity, rhsEntity)));
				}
				return out;
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
				shared_ptr<PkbStatementEntity> left = dynamic_pointer_cast<PkbStatementEntity>(this->convertClauseArgumentToPkbEntity(lhs));
				string leftKey = PkbControlFlowGraphNode::createPkbControlFlowGraphNode(left)->getKey();

				if (cfgManager->isInside(leftKey)) {
					shared_ptr<PkbGraphNode> leftAsNode = PkbControlFlowGraphNode::createPkbControlFlowGraphNode(left);
					shared_ptr<PkbControlFlowGraphNode> startNode = static_pointer_cast<PkbControlFlowGraphNode>(cfgManager->getNode(leftAsNode->getKey()));

					// starting from node, run dfs 
					extractor.extractAllFromStart(startNode, true);
					extractedRelationships = extractor.getExtractedRelationships();
				}
			}
			// case 3: wildcard and exact
			else if ((lhs.isWildcard() || lhs.isSynonym()) && (rhs.isExactReference())) {
				// convert rhs to entity, graph node, then get target node 
				shared_ptr<PkbStatementEntity> right = dynamic_pointer_cast<PkbStatementEntity>(this->convertClauseArgumentToPkbEntity(rhs));
				string rightKey = PkbControlFlowGraphNode::createPkbControlFlowGraphNode(right)->getKey();

				if (cfgManager->isInside(rightKey)) {
					shared_ptr<PkbGraphNode> rightAsNode = PkbControlFlowGraphNode::createPkbControlFlowGraphNode(right);
					shared_ptr<PkbControlFlowGraphNode> endNode = static_pointer_cast<PkbControlFlowGraphNode>(cfgManager->getNode(rightAsNode->getKey()));
					shared_ptr<PkbControlFlowGraphNode> startNode = static_pointer_cast<PkbControlFlowGraphNode>(cfgManager->getRootNode());

					// starting from node, run dfs 
					extractor.extractAllThatReachEnd(startNode, endNode, true);
					extractedRelationships = extractor.getExtractedRelationships();
				}
			}
			else { // case 4: all wild card
				// starting at root node, dfs all the way
				shared_ptr<PkbControlFlowGraphNode> startNode = static_pointer_cast<PkbControlFlowGraphNode>(cfgManager->getRootNode());
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

vector<shared_ptr<PkbRelationship>> PkbRelationshipQueryHelper::retrieveAffectsByTypeAndLhsRhs(ClauseArgument lhs, ClauseArgument rhs, shared_ptr<PkbRepository> repository) {

	vector<shared_ptr<PkbRelationship>> out;
	for (shared_ptr<PkbGraphManager> cfgManager : repository->getCfgs()) {
		// 0. validation - must be a statement
		if (!lhs.isStmtRefNoWildcard() && !lhs.isWildcard()) {
			throw PkbException("AFFECTS relationship expects lhs and rhs to both be statements!");
		}

		// case 1: exact, exact
		if (lhs.isExactReference() && rhs.isExactReference()) {

			// 0. check if lhs and rhs are both assign and exist
			// convert 
			shared_ptr<PkbEntity> lhsEntity = this->convertClauseArgumentToPkbEntity(lhs);
			shared_ptr<PkbEntity> rhsEntity = this->convertClauseArgumentToPkbEntity(rhs);

			// check entity tables
			shared_ptr<PkbEntity> lhsFound = repository->getEntityTableByType(PkbEntityType::STATEMENT)->get(lhsEntity->getKey());
			shared_ptr<PkbEntity> rhsFound = repository->getEntityTableByType(PkbEntityType::STATEMENT)->get(rhsEntity->getKey());
			if (rhsFound == NULL || lhsFound == NULL) {
				continue;
			}
			if (lhsFound != NULL) {
				// cast and check 
				shared_ptr<PkbStatementEntity> cast = dynamic_pointer_cast<PkbStatementEntity>(lhsFound);
				assert(cast != nullptr);
				if (!cast->isAssignStatement()) {
					continue;
				}
			}
			if (rhsFound != NULL) {
				// cast and check 
				shared_ptr<PkbStatementEntity> cast = dynamic_pointer_cast<PkbStatementEntity>(rhsFound);
				assert(cast != nullptr);
				if (!cast->isAssignStatement()) {
					continue;
				}
			}
			

			// 1. check cache
			// 1.1 check for positive match
			shared_ptr<PkbAffectsRelationship> positiveMatch = shared_ptr<PkbAffectsRelationship>(new PkbAffectsRelationship(lhsEntity, rhsEntity));
			if (repository->getRelationshipTableByRelationshipType(PkbRelationshipType::AFFECTS)->get(positiveMatch->getKey()) != NULL) {
				out.push_back(positiveMatch);
				continue;
			}
			// 1.2 check for negative match
			shared_ptr<PkbNotAffectsRelationship> negativeMatch = shared_ptr<PkbNotAffectsRelationship>(new PkbNotAffectsRelationship(lhsEntity, rhsEntity));
			if (repository->getRelationshipTableByRelationshipType(PkbRelationshipType::NOT_AFFECTS)->get(negativeMatch->getKey()) != NULL) {
				continue;
			}

			// 2. short circuit 1
			// 2.1 find all v lhs modifies and convert to set
			ClauseArgument variable = ClauseArgument::createVariableArg("v");
			vector<shared_ptr<PkbRelationship>> lhsModifies = this->retrieveRelationshipsFromTablesByTypeAndLhsRhs(PkbRelationshipType::MODIFIES, lhs, variable, repository);
			unordered_map<string, shared_ptr<PkbEntity>> lhsModifiesMap;
			for (shared_ptr<PkbRelationship> relationship : lhsModifies) {
				lhsModifiesMap.insert({ relationship->getRhs()->getKey(), relationship->getRhs() });
			}

			// 2.2 find all v rhs uses and convert to set
			vector<shared_ptr<PkbRelationship>> rhsUses = this->retrieveRelationshipsFromTablesByTypeAndLhsRhs(PkbRelationshipType::USES, rhs, variable, repository);
			unordered_map<string, shared_ptr<PkbEntity>> rhsUsesMap;
			for (shared_ptr<PkbRelationship> relationship : rhsUses) {
				rhsUsesMap.insert({ relationship->getRhs()->getKey(), relationship->getRhs() });
			}

			// 2.3 get intersection and see if size is 0
			unordered_map<string, shared_ptr<PkbEntity>> intersectingVariableMap;
			for (auto keyValue = rhsUsesMap.begin(); keyValue != rhsUsesMap.end(); keyValue++) {
				string key = keyValue->first;
				shared_ptr<PkbEntity> variable = keyValue->second;
				if (lhsModifiesMap.find(key) != lhsModifiesMap.end()) {
					intersectingVariableMap.insert({ variable->getKey(), variable });
				}
			}
			if (intersectingVariableMap.size() == 0) {
				continue;
			}

			// 3. short circuit 2: check that lhs and rhs are reachable using next*
			// 3.1 check not cache
			if (repository->getRelationshipTableByRelationshipType(PkbRelationshipType::NEXTSTAR)->get(positiveMatch->getKey()) != NULL) {
				// do nothing
			}
			else {
				// 3.2 check for negative match
				shared_ptr<PkbNotAffectsRelationship> negativeMatch = shared_ptr<PkbNotAffectsRelationship>(new PkbNotAffectsRelationship(lhsEntity, rhsEntity));
				if (repository->getRelationshipTableByRelationshipType(PkbRelationshipType::NOT_NEXTSTAR)->get(negativeMatch->getKey()) != NULL) {
					continue;
				}
			}

			// 4. not in either, brute force check
			// 4.1 get start node (lhs)
			shared_ptr<PkbStatementEntity> left = dynamic_pointer_cast<PkbStatementEntity>(lhsEntity);
			shared_ptr<PkbGraphNode> leftAsNode = PkbControlFlowGraphNode::createPkbControlFlowGraphNode(left);
			shared_ptr<PkbControlFlowGraphNode> startNode = static_pointer_cast<PkbControlFlowGraphNode>(cfgManager->getNode(leftAsNode->getKey()));

			// 4.2 get end node (rhs)
			shared_ptr<PkbStatementEntity> right = dynamic_pointer_cast<PkbStatementEntity>(rhsEntity);
			shared_ptr<PkbGraphNode> rightAsNode = PkbControlFlowGraphNode::createPkbControlFlowGraphNode(right);
			shared_ptr<PkbControlFlowGraphNode> endNode = static_pointer_cast<PkbControlFlowGraphNode>(cfgManager->getNode(rightAsNode->getKey()));

			// 4.3 use graph extractor to check 
			// then cache and return
			PkbGraphAffectsRelationshipExtractor extractor;
			if (extractor.hasAffectsRelationship(startNode, endNode, repository, intersectingVariableMap)) {
				out.push_back(positiveMatch);
				repository->getRelationshipTableByRelationshipType(PkbRelationshipType::AFFECTS)->add(positiveMatch);
			}
			else {
				repository->getRelationshipTableByRelationshipType(PkbRelationshipType::NOT_AFFECTS)->add(negativeMatch);
			}
			continue;

		}
		else if (lhs.isExactReference() && (rhs.isWildcard() || rhs.isSynonym())) { // case 2: exact, non exact
			
			// 0.1 check if lhs and rhs is assign and exist
			// convert 
			shared_ptr<PkbEntity> lhsEntity = this->convertClauseArgumentToPkbEntity(lhs);

			// check entity tables
			shared_ptr<PkbEntity> lhsFound = repository->getEntityTableByType(PkbEntityType::STATEMENT)->get(lhsEntity->getKey());
			if (lhsFound == NULL) {
				continue;
			}
			else if (lhsFound != NULL) {
				// cast and check 
				shared_ptr<PkbStatementEntity> cast = dynamic_pointer_cast<PkbStatementEntity>(lhsFound);
				assert(cast != nullptr);
				if (!cast->isAssignStatement()) {
					continue;
				}
			}
			
			// 0.2 if synonym is not assign, empty	
			if (!rhs.isWildcard() && !rhs.isAssignSynonym() &&!rhs.isStmtSynonym()) {
				continue;
			}
																		
			// 1. find all assign after lhs
			vector<shared_ptr<PkbEntity>> statements = repository->getEntityTableByType(PkbEntityType::STATEMENT)->getAll();

			// 2. for all these, check and append
			// O(n) enumeration
			for (shared_ptr<PkbEntity> statement : statements) {
				// cast
				shared_ptr<PkbStatementEntity> cast = dynamic_pointer_cast<PkbStatementEntity>(statement);
				assert(cast != nullptr);

				// check
				if (cast->isAssignStatement()) {
					// convert rhs to exact clause argument 
					ClauseArgument castAsRhs = ClauseArgument::createLineNumberArg(to_string(cast->getLineNumber()));

					// do self query for exact
					vector<shared_ptr<PkbRelationship>> found = this->retrieveAffectsByTypeAndLhsRhs(lhs, castAsRhs, repository);
					out.insert(out.end(), found.begin(), found.end());
				}
			}

		}
		else if (rhs.isExactReference() && (lhs.isWildcard() || lhs.isSynonym())) { // case 3: non exact, exact 
			// 0.1 check if lhs and rhs is assign and exist
			// convert 
			shared_ptr<PkbEntity> rhsEntity = this->convertClauseArgumentToPkbEntity(rhs);

			// check entity tables
			shared_ptr<PkbEntity> rhsFound = repository->getEntityTableByType(PkbEntityType::STATEMENT)->get(rhsEntity->getKey());
			if (rhsFound == NULL) {
				continue;
			}
			else if (rhsFound != NULL) {
				// cast and check 
				shared_ptr<PkbStatementEntity> cast = dynamic_pointer_cast<PkbStatementEntity>(rhsFound);
				assert(cast != nullptr);
				if (!cast->isAssignStatement()) {
					continue;
				}
			}

			// 0.2 if synonym is not assign, empty	
			if (!lhs.isWildcard() && !lhs.isAssignSynonym() && !lhs.isStmtSynonym()) {
				continue;
			}
			
			// 1. get all
			vector<shared_ptr<PkbEntity>> statements = repository->getEntityTableByType(PkbEntityType::STATEMENT)->getAll();

			// 2. for all these, check and append
			// O(n) enumeration
			for (shared_ptr<PkbEntity> statement : statements) {
				// cast
				shared_ptr<PkbStatementEntity> cast = dynamic_pointer_cast<PkbStatementEntity>(statement);
				assert(cast != nullptr);

				// check
				if (cast->isAssignStatement()) {
					// convert lhs to exact clause argument 
					ClauseArgument castAsLhs = ClauseArgument::createLineNumberArg(to_string(cast->getLineNumber()));

					// do self query for exact
					vector<shared_ptr<PkbRelationship>> found = this->retrieveAffectsByTypeAndLhsRhs(castAsLhs, rhs, repository);
					out.insert(out.end(), found.begin(), found.end());
				}
			}

		}
		else if ((lhs.isWildcard() || lhs.isSynonym()) && (rhs.isWildcard() || rhs.isSynonym())) { // case 4: non exact, non exact
			// 0. check that they don't refer to the same thing
			if (lhs.isStmtRefNoWildcard() && rhs.isSynonym() && lhs == rhs) {
				continue;
			}

			// 1. find all assign
			vector<shared_ptr<PkbEntity>> statements = repository->getEntityTableByType(PkbEntityType::STATEMENT)->getAll();

			// 2. for all pairs (diagonal matrix), check and append
			// O(n2) enumeration
			for (shared_ptr<PkbEntity> statement1 : statements) {
				// cast
				shared_ptr<PkbStatementEntity> cast1 = dynamic_pointer_cast<PkbStatementEntity>(statement1);
				assert(cast1 != nullptr);
				for (shared_ptr<PkbEntity> statement2 : statements) {
					shared_ptr<PkbStatementEntity> cast2 = dynamic_pointer_cast<PkbStatementEntity>(statement2);
					assert(cast2 != nullptr);

					// check
					if (cast1->isAssignStatement() && cast2->isAssignStatement()) {
						// convert to exact clause argument 
						ClauseArgument castAsLhs = ClauseArgument::createLineNumberArg(to_string(cast1->getLineNumber()));
						ClauseArgument castAsRhs = ClauseArgument::createLineNumberArg(to_string(cast2->getLineNumber()));

						// do self query for exact
						vector<shared_ptr<PkbRelationship>> found = this->retrieveAffectsByTypeAndLhsRhs(castAsLhs, castAsRhs, repository);
						out.insert(out.end(), found.begin(), found.end());
					}
				}
			}

		}
		else {
			throw PkbException("Unknown case for affects!");
		}
		
	
	}
	return out;
	
	
	
}

vector<shared_ptr<PkbRelationship>> PkbRelationshipQueryHelper::retrieveAffectsStarByTypeAndLhsRhs(ClauseArgument lhs, ClauseArgument rhs, shared_ptr<PkbRepository> repository) {
	// 0. validity check - both assign/exact/wildcard

	vector<shared_ptr<PkbRelationship>> out;
	for (shared_ptr<PkbGraphManager> cfgManager : repository->getCfgs()) {
		// 0. validation - must be a statement
		if (!lhs.isStmtRefNoWildcard() && !lhs.isWildcard()) {
			throw PkbException("AFFECTS* relationship expects lhs and rhs to both be statements!");
		}
		// case 1: exact, exact
		// check if affects(lhs, rhs)
		// if not, O(n) enumeration of affects(lhs, s), affects*(s, rhs)
		if (lhs.isExactReference() && rhs.isExactReference()) {
			// 0. check if lhs and rhs are both assign and exist
			// convert 
			shared_ptr<PkbEntity> lhsEntity = this->convertClauseArgumentToPkbEntity(lhs);
			shared_ptr<PkbEntity> rhsEntity = this->convertClauseArgumentToPkbEntity(rhs);

			// check entity tables
			shared_ptr<PkbEntity> lhsFound = repository->getEntityTableByType(PkbEntityType::STATEMENT)->get(lhsEntity->getKey());
			shared_ptr<PkbEntity> rhsFound = repository->getEntityTableByType(PkbEntityType::STATEMENT)->get(rhsEntity->getKey());
			if (rhsFound == NULL || lhsFound == NULL) {
				continue;
			}
			if (lhsFound != NULL) {
				// cast and check 
				shared_ptr<PkbStatementEntity> cast = dynamic_pointer_cast<PkbStatementEntity>(lhsFound);
				assert(cast != nullptr);
				if (!cast->isAssignStatement()) {
					continue;
				}
			}
			if (rhsFound != NULL) {
				// cast and check 
				shared_ptr<PkbStatementEntity> cast = dynamic_pointer_cast<PkbStatementEntity>(rhsFound);
				assert(cast != nullptr);
				if (!cast->isAssignStatement()) {
					continue;
				}
			}

			// 1. check cache
			// 1.1 check for positive match
			shared_ptr<PkbAffectsStarRelationship> positiveMatch = shared_ptr<PkbAffectsStarRelationship>(new PkbAffectsStarRelationship(lhsEntity, rhsEntity));
			if (repository->getRelationshipTableByRelationshipType(PkbRelationshipType::AFFECTSSTAR)->get(positiveMatch->getKey()) != NULL) {
				out.push_back(positiveMatch);
				continue;
			}
			// 1.2 check for negative match
			shared_ptr<PkbAffectsStarRelationship> negativeMatch = shared_ptr<PkbAffectsStarRelationship>(new PkbAffectsStarRelationship(lhsEntity, rhsEntity));
			if (repository->getRelationshipTableByRelationshipType(PkbRelationshipType::NOT_AFFECTSSTAR)->get(negativeMatch->getKey()) != NULL) {
				continue;
			}
			// 1.3 check if visited before, if visited, skip
			pair<string, string> affectsGraphEdge = pair<string, string>(lhsEntity->getKey(), rhsEntity->getKey());
			if (this->visitedAffectsEdges.count(affectsGraphEdge)) {
				continue;
			}
			

			// 2. check if affects(lhs, rhs)
			vector<shared_ptr<PkbRelationship>> foundAffects = this->retrieveRelationshipsFromGraphsByTypeAndLhsRhs(PkbRelationshipType::AFFECTS, lhs, rhs, repository);
			if (foundAffects.size() == 1) {
				out.push_back(positiveMatch);

				// cache
				repository->getRelationshipTableByRelationshipType(PkbRelationshipType::AFFECTSSTAR)->add(positiveMatch);
				continue;
			}

			// 3. else, for all assign, find a middle assign recursively
			// update cache as you go
			// O(n) enumeration
			vector<shared_ptr<PkbEntity>> statements = repository->getEntityTableByType(PkbEntityType::STATEMENT)->getAll();
			for (shared_ptr<PkbEntity> statement : statements) {
				// if is the same as either, skip
				if (statement->equals(lhsEntity) || statement->equals(rhsEntity)) {
					continue;
				}
				
				// cast
				shared_ptr<PkbStatementEntity> cast = dynamic_pointer_cast<PkbStatementEntity>(statement);
				assert(cast != nullptr);

				// check
				if (cast->isAssignStatement()) {

					// convert lhs to exact clause argument 
					ClauseArgument arg = ClauseArgument::createLineNumberArg(to_string(cast->getLineNumber()));

					// do self query for exact
					vector<shared_ptr<PkbRelationship>> foundWithLhs = this->retrieveAffectsByTypeAndLhsRhs(lhs, arg, repository);
					if (foundWithLhs.size() != 1) {
						// not found, continue
						// failure, cache it
						repository->getRelationshipTableByRelationshipType(PkbRelationshipType::NOT_AFFECTSSTAR)->add(negativeMatch);
						continue;
					}
					else {
						// mark as visited
						pair<string, string> exploringEdge = pair<string, string>(lhsEntity->getKey(), statement->getKey());
						this->visitedAffectsEdges.insert(exploringEdge);

						// try to search recursively 
						vector<shared_ptr<PkbRelationship>> foundWithRhs = this->retrieveAffectsStarByTypeAndLhsRhs(arg, rhs, repository);
						if (foundWithRhs.size() == 1) {
							// success, cache it
							repository->getRelationshipTableByRelationshipType(PkbRelationshipType::AFFECTSSTAR)->add(positiveMatch);

							// insert
							out.push_back(positiveMatch);
							continue;
						}
						else {
							// failure, also cache it
							repository->getRelationshipTableByRelationshipType(PkbRelationshipType::NOT_AFFECTSSTAR)->add(negativeMatch);
							continue;
						}
						
					}					
				}
			}
		}
		else if (lhs.isExactReference() && (rhs.isWildcard() || rhs.isSynonym())) {
			// case 2: exact, non
			// O(n2) enumeration of affects(lhs, s1), affects*(s1, s2)
		}
		else if (rhs.isExactReference() && (lhs.isWildcard() || lhs.isSynonym())) {
			// case 3: non, exact
			// O(n2) enumeration of affects(s1, s2), affects(s2, rhs)
		}
		else if ((lhs.isWildcard() || lhs.isSynonym()) && (rhs.isWildcard() || rhs.isSynonym())) {
			// case 4: wildcard, wildcard
			// O(n3) enumeration of affects(s1, s2), affects(s2, s3)

		}
		else {
			throw PkbException("Unknown case for affects!");
		}
	}

	return out;
	
}



// ==================== public ====================
vector<shared_ptr<PkbRelationship>> PkbRelationshipQueryHelper::retrieveRelationshipsFromTablesByTypeAndLhsRhs(PkbRelationshipType relationshipType, ClauseArgument lhs, ClauseArgument rhs, shared_ptr<PkbRepository> repository)
{
	// 0. get table based on type
	shared_ptr<PkbRelationshipTable> table = repository->getRelationshipTableByRelationshipType(relationshipType);

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
		shared_ptr<PkbRelationship> toFind; // TODO: refactor to use factory methods
		switch (relationshipType) {
		case PkbRelationshipType::FOLLOWS:
			toFind = shared_ptr<PkbRelationship>(new PkbFollowsRelationship(left, right));
			break;
		case PkbRelationshipType::FOLLOWSSTAR:
			toFind = shared_ptr<PkbRelationship>(new PkbFollowsStarRelationship(left, right));
			break;
		case PkbRelationshipType::PARENT:
			toFind = shared_ptr<PkbRelationship>(new PkbParentRelationship(left, right));
			break;
		case PkbRelationshipType::PARENTSTAR:
			toFind = shared_ptr<PkbRelationship>(new PkbParentStarRelationship(left, right));
			break;
		case PkbRelationshipType::USES:
			toFind = shared_ptr<PkbRelationship>(new PkbUsesRelationship(left, right));
			break;
		case PkbRelationshipType::MODIFIES:
			toFind = shared_ptr<PkbRelationship>(new PkbModifiesRelationship(left, right));
			break;
		case  PkbRelationshipType::NEXT:
			toFind = shared_ptr<PkbRelationship>(new PkbNextRelationship(left, right));
			break;
		case PkbRelationshipType::CALLSTMTATTRIBUTE:
			toFind = shared_ptr<PkbRelationship>(new PkbCallStmtAttributeRelationship(left, right));
			break;
		case PkbRelationshipType::CALLS:
			toFind = shared_ptr<PkbRelationship>(new PkbCallsRelationship(left, right));
			break;
		case PkbRelationshipType::CALLSSTAR:
			toFind = shared_ptr<PkbRelationship>(new PkbCallsStarRelationship(left, right));
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

vector<shared_ptr<PkbRelationship>> PkbRelationshipQueryHelper::retrieveRelationshipsFromGraphsByTypeAndLhsRhs(PkbRelationshipType relationshipType, ClauseArgument lhs, ClauseArgument rhs, shared_ptr<PkbRepository> repository)
{
	if (relationshipType == PkbRelationshipType::NEXTSTAR) {
		return this->retrieveNextStarByTypeAndLhsRhs(lhs, rhs, repository);
	}
	else if (relationshipType == PkbRelationshipType::AFFECTS) {
		return this->retrieveAffectsByTypeAndLhsRhs(lhs, rhs, repository);
	}
	else if (relationshipType == PkbRelationshipType::AFFECTSSTAR) {
		return this->retrieveAffectsStarByTypeAndLhsRhs(lhs, rhs, repository);
	}
	else {
		throw PkbException("Unknown graph type relationship trying to be retrieved.");
	}
}
