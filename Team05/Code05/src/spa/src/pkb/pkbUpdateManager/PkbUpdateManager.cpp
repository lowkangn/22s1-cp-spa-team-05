#include <pkb/pkbUpdateManager/PkbUpdateManager.h>
#include <pkb/pkbRepository/design_objects/patterns/PkbAssignPattern.h>
#include <pkb/pkbRepository/design_objects/patterns/PkbIfPattern.h>
#include <pkb/pkbRepository/design_objects/patterns/PkbWhilePattern.h>

#include <pkb/pkbRepository/design_objects/graphs/PkbControlFlowGraphNode.h>

#include <pkb/pkbRepository/design_objects/entities/PkbProcedureEntity.h>
#include <pkb/pkbRepository/design_objects/entities/PkbStatementEntity.h>
#include <pkb/pkbRepository/design_objects/entities/PkbVariableEntity.h>
#include <pkb/pkbRepository/design_objects/entities/PkbConstantEntity.h>

#include <pkb/pkbRepository/design_objects/relationships/PkbCallsRelationship.h>
#include <pkb/pkbRepository/design_objects/relationships/PkbCallsStarRelationship.h>
#include <pkb/pkbRepository/design_objects/patterns/PkbWhilePattern.h>
#include <pkb/pkbRepository/design_objects/relationships/PkbFollowsRelationship.h>
#include <pkb/pkbRepository/design_objects/relationships/PkbFollowsStarRelationship.h>
#include <pkb/pkbRepository/design_objects/relationships/PkbParentRelationship.h>
#include <pkb/pkbRepository/design_objects/relationships/PkbParentStarRelationship.h>
#include <pkb/pkbRepository/design_objects/relationships/PkbUsesRelationship.h>
#include <pkb/pkbRepository/design_objects/relationships/PkbModifiesRelationship.h>
#include <pkb/pkbRepository/design_objects/relationships/PkbNextRelationship.h>
#include <pkb/pkbRepository/design_objects/relationships/PkbNextStarRelationship.h>
#include <pkb/pkbRepository/design_objects/relationships/PkbCallStmtAttributeRelationship.h>

#include <StringSplitter.h>

using namespace std;

// ==================== Private methods ====================

shared_ptr<PkbEntity> PkbUpdateManager::externalEntityToPkbEntity(Entity entity)
{
	if (entity.isVariableEntity()) {

		// create pkb entity object
		string identifier = entity.getString();
		shared_ptr<PkbEntity> object = shared_ptr<PkbEntity>(new PkbVariableEntity(identifier));
		return object;
	}
	else if (entity.isStmtEntity()) {
		// read line number, since that's the statement identifier
		int lineNumber = entity.getLine();

		// depending on type, we call factory method
		// and case to pointer
		if (entity.isPrintEntity()) {
			shared_ptr<PkbStatementEntity> statement = PkbStatementEntity::createPrintStatementEntity(lineNumber);
			shared_ptr<PkbEntity> object = shared_ptr<PkbEntity>(statement);
			return object;
		}
		else if (entity.isAssignEntity()) {
			shared_ptr<PkbStatementEntity> statement = PkbStatementEntity::createAssignStatementEntity(lineNumber);
			shared_ptr<PkbEntity> object = shared_ptr<PkbEntity>(statement);
			return object;
		}
		else if (entity.isIf()) {
			shared_ptr<PkbStatementEntity> statement = PkbStatementEntity::createIfStatementEntity(lineNumber);
			shared_ptr<PkbEntity> object = shared_ptr<PkbEntity>(statement);
			return object;
		}
		else if (entity.isReadEntity()) {
			shared_ptr<PkbStatementEntity> statement = PkbStatementEntity::createReadStatementEntity(lineNumber);
			shared_ptr<PkbEntity> object = shared_ptr<PkbEntity>(statement);
			return object;
		}
		else if (entity.isWhile()) {
			shared_ptr<PkbStatementEntity> statement = PkbStatementEntity::createWhileStatementEntity(lineNumber);
			shared_ptr<PkbEntity> object = shared_ptr<PkbEntity>(statement);
			return object;
		}
		else if (entity.isCallEntity()) {
			shared_ptr<PkbStatementEntity> statement = PkbStatementEntity::createCallStatementEntity(lineNumber);
			shared_ptr<PkbEntity> object = shared_ptr<PkbEntity>(statement);
			return object;
		}
		else {
			throw PkbException("Unknown statement type being passed to PKB!");
		}

	}
	else if (entity.isProcedureEntity()) {
		// create pkb entity object
		string identifier = entity.getString();
		shared_ptr<PkbEntity> object = shared_ptr<PkbEntity>(new PkbProcedureEntity(identifier));
		return object;
	}
	else if (entity.isConstantEntity()) {
		int value = entity.getValue();
		shared_ptr<PkbEntity> object = shared_ptr<PkbEntity>(new PkbConstantEntity(value));
		return object;
	}
	else {
		throw PkbException("Unknown entity type being passed to PKB!");
	}

}


shared_ptr<PkbRelationship> PkbUpdateManager::externalRelationshipToPkbRelationship(Relationship relationship)
{
	Entity lhs = relationship.getLhs();
	Entity rhs = relationship.getRhs();

	shared_ptr<PkbEntity> lhsToPkbEntity = this->externalEntityToPkbEntity(lhs);
	shared_ptr<PkbEntity> rhsToPkbEntity = this->externalEntityToPkbEntity(rhs);

	if (relationship.isFollows()) {
		shared_ptr<PkbRelationship> pkbRelationship = shared_ptr<PkbRelationship>(new PkbFollowsRelationship(lhsToPkbEntity, rhsToPkbEntity));
		return pkbRelationship;
	}
	else if (relationship.isFollowsStar()) {
		shared_ptr<PkbRelationship> pkbRelationship = shared_ptr<PkbRelationship>(new PkbFollowsStarRelationship(lhsToPkbEntity, rhsToPkbEntity));
		return pkbRelationship;
	}
	else if (relationship.isModifies()) {
		shared_ptr<PkbRelationship> pkbRelationship = shared_ptr<PkbRelationship>(new PkbModifiesRelationship(lhsToPkbEntity, rhsToPkbEntity));
		return pkbRelationship;
	}
	else if (relationship.isParent()) {
		shared_ptr<PkbRelationship> pkbRelationship = shared_ptr<PkbRelationship>(new PkbParentRelationship(lhsToPkbEntity, rhsToPkbEntity));
		return pkbRelationship;
	}
	else if (relationship.isParentStar()) {
		shared_ptr<PkbRelationship> pkbRelationship = shared_ptr<PkbRelationship>(new PkbParentStarRelationship(lhsToPkbEntity, rhsToPkbEntity));
		return pkbRelationship;
	}
	else if (relationship.isUses()) {
		shared_ptr<PkbRelationship> pkbRelationship = shared_ptr<PkbRelationship>(new PkbUsesRelationship(lhsToPkbEntity, rhsToPkbEntity));
		return pkbRelationship;
	}
	else if (relationship.isCallsStmtAttribute()) {
		shared_ptr<PkbRelationship> pkbRelationship = shared_ptr<PkbRelationship>(new PkbCallStmtAttributeRelationship(lhsToPkbEntity, rhsToPkbEntity));
		return pkbRelationship;
	}
	else if (relationship.isCalls()) {
		shared_ptr<PkbRelationship> pkbRelationship = shared_ptr<PkbRelationship>(new PkbCallsRelationship(lhsToPkbEntity, rhsToPkbEntity));
		return pkbRelationship;
	}
	else if (relationship.isCallsStar()) {
		shared_ptr<PkbRelationship> pkbRelationship = shared_ptr<PkbRelationship>(new PkbCallsStarRelationship(lhsToPkbEntity, rhsToPkbEntity));
		return pkbRelationship;

	}
	else if (relationship.isNext()) {
		shared_ptr<PkbRelationship> pkbRelationship = shared_ptr<PkbRelationship>(new PkbNextRelationship(lhsToPkbEntity, rhsToPkbEntity));
		return pkbRelationship;
	}
	else {
		throw PkbException("Unknown relationship being converted!");
	}
}

vector<shared_ptr<PkbPattern>> PkbUpdateManager::externalPatternToPkbPattern(Pattern pattern) {

	int currLineNum = pattern.getLine();
	vector<shared_ptr<PkbPattern>> out;

	if (pattern.isIfPattern()) {
		// get the pattern string from if pattern
		string conditions = pattern.getRhs();

		// split the pattern string and create a table (e.g. "x y z" -> [x, y, z])
		vector<string> variables = StringSplitter::splitStringByDelimiter(conditions, EXTERNAL_PATTERN_DELIMITER);

		// create if pattern for each variable and add it to table
		// we can do this now as we will only ever need to check if it's inside, not an exact pattern
		
		for (string v : variables) {
			vector<string> varToAdd = { v };
			shared_ptr<PkbPattern> p = PkbIfPattern::createIfPattern(currLineNum, varToAdd);
			out.push_back(p);
		}
	}
	else if (pattern.isWhilePattern()) {
		// get the pattern string from while pattern
		string conditions = pattern.getRhs();

		// split the pattern string and create a table (e.g. "x y z" -> [x, y, z])
		vector<string> variables = StringSplitter::splitStringByDelimiter(conditions, EXTERNAL_PATTERN_DELIMITER);

		// create while pattern for each variable and add it to table
		// just like if, we can do this because for now we will only need to check if its inside
		for (string v : variables) {
			vector<string> varToAdd = { v };
			shared_ptr<PkbPattern> p = PkbWhilePattern::createWhilePattern(currLineNum, varToAdd);
			out.push_back(p);
		}
	}
	else if (pattern.isAssignPattern()) {
		vector<string> strings = {
			pattern.getLhs(),
			pattern.getRhs()
		};

		// create required assign pattern
		shared_ptr<PkbPattern> p = PkbAssignPattern::createAssignPattern(currLineNum, strings);
		out.push_back(p);

	}
	else {
		throw PkbException("Unknown pattern type being added!");
	}
	return out;
}

// ==================== Public methods ====================

void PkbUpdateManager::addEntities(vector<Entity> entities, shared_ptr<PkbRepository> repository)
{
	for (Entity e : entities) {
		// 0. skip opeartor entities
		if (e.isOperator()) {
			continue;
		}

		// 1. convert
		shared_ptr<PkbEntity> converted = this->externalEntityToPkbEntity(e);

		// 2. add, letting repository handle sorting logic
		repository->addPkbEntity(converted);
	}
}

void PkbUpdateManager::addRelationships(vector<Relationship> relationships, shared_ptr<PkbRepository> repository)
{
	for (Relationship r : relationships) {
		// 1. convert
		shared_ptr<PkbRelationship> converted = this->externalRelationshipToPkbRelationship(r);

		// 2. add, letting repository handle sorting logic
		repository->addPkbRelationship(converted);
	}
}

void PkbUpdateManager::addPatterns(vector<Pattern> patterns, shared_ptr<PkbRepository> repository)
{
	for (Pattern p : patterns) {
		// 1. convert
		vector<shared_ptr<PkbPattern>> converted = this->externalPatternToPkbPattern(p);

		// 2. add, letting repository handle sorting logic
		for (shared_ptr<PkbPattern> c : converted) {
			repository->addPkbPattern(c);
		}
		
	}
}

void PkbUpdateManager::addCfgs(vector<shared_ptr<CFGNode>> rootNodes, shared_ptr<PkbRepository> repository)
{
	/*
		Hash function for an edge, which we represent as a pair of strings.
		We define it locally within the function as it shouldn't be used anywhere else.
	*/
	struct EdgeKeyHash {
		size_t operator()(const pair<string, string>& p) const {
			// we choose 31 as it's a prime number typically used for hashing strings
			return hash<string>()(p.first) * 31 + hash<string>()(p.second);

		}
	};

	for (shared_ptr<CFGNode> rootNode : rootNodes) {
		// 1. traverse cfg to convert to pkb graph
	// 1.1 pointer to new root node
		shared_ptr<PkbGraphNode> node = NULL;

		// 1.2 initialize edge visited list and queue
		unordered_set<pair<string, string>, EdgeKeyHash> visitedEdges;
		queue<shared_ptr<CFGNode>> q;
		q.push(rootNode);

		// 1.3 initialize node set
		unordered_map<string, shared_ptr<PkbGraphNode>> keyToNodeMap;

	// 2 bfs with visited edges
	while (!q.empty()) {

		// 1. pop
		shared_ptr<CFGNode> n = q.front();
		q.pop();

			// 2. convert to pkb graph node. if already inside, use that.
			shared_ptr<PkbStatementEntity> castedParent = static_pointer_cast<PkbStatementEntity>(this->externalEntityToPkbEntity(n->getEntity()));
			if (castedParent == NULL) { // conversion failed
				throw PkbException("Tried to convert cfg node to statement entity, but couldn't!");
			}
			shared_ptr<PkbGraphNode> parentNode = PkbControlFlowGraphNode::createPkbControlFlowGraphNode(castedParent);
			string parentKey = parentNode->getKey();
			if (keyToNodeMap.count(parentKey)) {
				parentNode = keyToNodeMap.at(parentKey);
			}
			else {
				keyToNodeMap.insert({ parentKey, parentNode });
			}


		// 2.1 if root node not initialized, put it
		if (node == NULL) {
			node = parentNode;
		}

			// 3. traverse neighbours
			for (shared_ptr<CFGNode> child : n->getChildren()) {

				// 3.1 convert to pkbgraph node
				shared_ptr<PkbStatementEntity> castedChild = static_pointer_cast<PkbStatementEntity>(this->externalEntityToPkbEntity(child->getEntity()));
				if (castedChild == NULL) { // conversion failed
					throw PkbException("Tried to convert cfg node to statement entity, but couldn't!");
				}
				shared_ptr<PkbGraphNode> childNode = PkbControlFlowGraphNode::createPkbControlFlowGraphNode(castedChild);

				// 3.2 if already seen before, use that node instead. else, insert
				string childKey = childNode->getKey();
				if (keyToNodeMap.count(childKey)) {
					childNode = keyToNodeMap.at(childKey);
				}
				else {
					keyToNodeMap.insert({ childKey, childNode });
				}

				// 3.3 if edge visited, continue
				pair<string, string> edgeKey = pair<string, string>(parentNode->getKey(), childNode->getKey());
				if (visitedEdges.count(edgeKey)) {
					continue;
				}

				// 3.4 else, add as neighbour of parent and to visited set
				parentNode->addNeighbour(childNode); // add as neighbour of parent
				visitedEdges.insert(edgeKey); // add edge to visited set 

				// 3.4 add to queue and key map
				q.push(child);
			}
		}

	// 2. pass the pkb graph into the repository for storage
	repository->addPkbGraph(node);

}
