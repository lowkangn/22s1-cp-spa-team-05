#include <sp/SourceProcessor.h>

using namespace std;

void SourceProcessor::initialize(istream& sourceProgram) {
    if (this->isInitialized) {
        throw SPException("SourceProcessor has already been initialized.");
    }

    this->isInitialized = true;

    list<Token> tokens = this->lexer.tokenize(sourceProgram);
    this->astRoot = this->parser.parse(tokens);
    this->controlFlowGraphs = this->cfgParser.parse(this->astRoot);
}

vector<Relationship> SourceProcessor::extractRelations() {
    if (!this->isInitialized) {
        throw SPException("SP has not been initialized with the source program");
    }
    vector<Relationship> relations = this->designManager.extractRelationships(this->astRoot);
    vector<Relationship> cfgRelations = this->designManager.extractCFGRelationships(
        this->controlFlowGraphs);

    return relations;
}

vector<Relationship> SourceProcessor::extractCFGRelations() {
    if (!this->isInitialized) {
        throw SPException("SP has not been initialized with the source program");
    }
    vector<Relationship> cfgRelations = this->designManager.extractCFGRelationships(
        this->controlFlowGraphs);

    return cfgRelations;
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

void SourceProcessor::extractAllAndAddToPkb(shared_ptr<PKBUpdateHandler> pkb) {
    //extract
    vector<Entity> entities = this->extractEntities();
    vector<Relationship> relationships = this->extractRelations();
    vector<Pattern> patterns = this->extractPatterns();
    vector<Relationship> cfgRelationships = this->extractCFGRelations();

    //add to pkb
    pkb->addEntities(entities);
    pkb->addRelationships(relationships);
    pkb->addPatterns(patterns);
    pkb->addRelationships(cfgRelationships);
    pkb->addCfgs(this->controlFlowGraphs);
}
