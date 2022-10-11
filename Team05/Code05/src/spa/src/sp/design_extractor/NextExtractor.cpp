#include <sp/design_extractor/NextExtractor.h>

vector<Relationship> NextExtractor::extract(shared_ptr<CFGNode> cfg)
{
	vector<Relationship> nextRelationships;

	if (cfg->hasNext()) {
		vector<shared_ptr<CFGNode>> nextChildren = cfg->getChildren();
		Entity lhs = Entity::createStatementEntity(cfg->hash());

		// Extract this cfg nodes relationships
		for (int i = 0; i < nextChildren.size(); i++) {

			shared_ptr<CFGNode> child = nextChildren[i];
			Entity rhs = Entity::createStatementEntity(child->hash());
			Relationship nextRelation = Relationship::createNextRelationship(lhs, rhs);

			// Append
			nextRelationships.push_back(nextRelation);

			// If it is not a cycle recursively extract
			if (child->hash() < cfg->hash()) {
				vector<Relationship> recursiveNextRelationships = this->extract(child);
				nextRelationships.insert(nextRelationships.begin(), recursiveNextRelationships.begin(), recursiveNextRelationships.end());
			}
		}
	}

	return nextRelationships;
}
