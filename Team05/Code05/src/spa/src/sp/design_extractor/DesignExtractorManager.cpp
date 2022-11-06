#include <sp/dataclasses/design_objects/Entity.h>
#include <sp/dataclasses/design_objects/Pattern.h>
#include <sp/dataclasses/design_objects/Relationship.h>
#include <sp/design_extractor/DesignExtractorManager.h>

#include <memory>
#include <vector>

using namespace std;

vector<Relationship> DesignExtractorManager::extractRelationships(shared_ptr<ASTNode> ast) {
    vector<Relationship> relationships = vector<Relationship>();

    // Loops through all the different relationshipExtractor (Modifies, Uses, Parent)
    // and extracts their respective relationships
    for (auto& relationshipExtractor : this->relationshipExtractorList) {
        vector<Relationship> extractedRelations = relationshipExtractor->extract(ast);
        relationships.insert(
            relationships.end(),
            extractedRelations.begin(),
            extractedRelations.end());
    }

    return relationships;
}

vector<Entity> DesignExtractorManager::extractEntities(shared_ptr<ASTNode> ast) {
    vector<Entity> entities = this->entityExtractor.extract(ast);
    return entities;
}

vector<Pattern> DesignExtractorManager::extractPatterns(shared_ptr<ASTNode> ast) {
    vector<Pattern> patterns = this->patternExtractor.extract(ast);
    return patterns;
}

vector<Relationship> DesignExtractorManager::extractCFGRelationships(
    vector<shared_ptr<CFGNode>> cfgNodes) {
    vector<Relationship> extractedCFGRelations;

    for (shared_ptr<CFGNode> nodeToExtractFrom : cfgNodes) {
        vector<Relationship> extractedRelations = this->nextExtractor.extract(nodeToExtractFrom);
        extractedCFGRelations.insert(
            extractedCFGRelations.end(),
            extractedRelations.begin(),
            extractedRelations.end());
    }

    return extractedCFGRelations;
}
