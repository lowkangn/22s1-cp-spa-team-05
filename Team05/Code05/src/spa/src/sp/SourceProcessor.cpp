#include <sp/SourceProcessor.h>
#include <sp/lexer/Lexer.h>
#include <sp/parser/SimpleSyntaxParserManager.h>
#include <sp/design_extractor/DesignExtractorManager.h>
#include <sp/design_extractor/EntityExtractor.h>
#include <sp/design_extractor/PatternExtractor.h>
#include <sp/design_extractor/ModifiesExtractor.h>
#include <sp/design_extractor/UsesExtractor.h>
#include <sp/design_extractor/ParentExtractor.h>
#include <sp/design_extractor/ParentTExtractor.h>
#include <sp/design_extractor/FollowsExtractor.h>
#include <sp/design_extractor/FollowsTExtractor.h>

#include <sp/dataclasses/tokens/Token.h>
#include <sp/dataclasses/ast/AST.h>
#include <sp/SPException.h>

#include <string>
#include <list>
#include <vector>
#include <sstream>
#include <memory>

using namespace std;

vector<Relationship> SourceProcessor::extractRelations() {
	if (!this->isInitialized) {
		throw SPException("SP has not been initialized with the source program");
	}

	vector<Relationship> relations = this->designManager.extractRelationships(this->astRoot);
	return relations;
}

vector<Pattern> SourceProcessor::extractPatterns() {
	if (!this->isInitialized) {
		throw SPException("SP has not been initialized with the source program");
	}

	vector<Pattern> patterns = this->designManager.extractPatterns(this->astRoot);
	return patterns;
}

vector<Entity> SourceProcessor::extractEntities() {
	if (!this->isInitialized) {
		throw SPException("SP has not been initialized with the source program");
	}

	vector<Entity> entities = this->designManager.extractEntities(this->astRoot);
	return entities;
}