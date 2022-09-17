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

#include <sp/dataclasses/tokens/Token.h>
#include <sp/dataclasses/ast/AST.h>
#include <sp/SPException.h>

#include <string>
#include <list>
#include <vector>
#include <sstream>
#include <memory>


using namespace std;

void SourceProcessor::initialize(string program) {
	this->program = program;

	// First tokenize using Lexer
	Lexer lexer = Lexer();
	
	// Create stream for lexer
	stringstream ss(this->program);
	istream& stream = ss;

	list<Token> tokens = lexer.tokenize(ss);

	// Get a AST tree using ParserManager
	ParserManager parser = ParserManager(tokens);

	shared_ptr<ASTNode> root = parser.parse();

	// Set up design extractor manager
	EntityExtractor entityExtractor = EntityExtractor();
	PatternExtractor patternExtractor = PatternExtractor();

	// The several Relationships extractors
	vector<shared_ptr<Extractor<Relationship>>> relationshipExtractors;

	// IP, when done uncomment
	/*
		shared_ptr<Extractor<Relationship>> followsExtractor(new FollowsExtractor());
		shared_ptr<Extractor<Relationship>> followsTExtractor(new FollowsTExtractor());

	*/
	shared_ptr<Extractor<Relationship>> parentExtractor(new ParentExtractor());
	shared_ptr<Extractor<Relationship>> parentTExtractor(new ParentTExtractor());
	shared_ptr<Extractor<Relationship>> modifiesExtractor(new ModifiesExtractor());
	shared_ptr<Extractor<Relationship>> usesExtractor(new UsesExtractor());

	// relationshipExtractors.push_back(followsExtractor);
	// relationshipExtractors.push_back(followsTExtractor);
	relationshipExtractors.push_back(parentExtractor);
	relationshipExtractors.push_back(parentTExtractor);
	relationshipExtractors.push_back(modifiesExtractor);
	relationshipExtractors.push_back(usesExtractor);


	DesignExtractorManager designManager = DesignExtractorManager(entityExtractor, patternExtractor, relationshipExtractors);

	this->astRoot = root;
	this->designManager = designManager;
	this->isInitialized = true;
}

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

