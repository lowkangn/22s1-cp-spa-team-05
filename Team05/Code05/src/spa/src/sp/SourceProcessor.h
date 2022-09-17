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

class SourceProcessor {

private:
	bool isInitialized = false;
	shared_ptr<ASTNode> astRoot;
	DesignExtractorManager designManager;

public:
	SourceProcessor() {};

	void initialize(string program);
	vector<Relationship> extractRelations();
	vector<Pattern> extractPatterns();
	vector<Entity> extractEntities();
};