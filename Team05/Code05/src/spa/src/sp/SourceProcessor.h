#include <string>
#include <list>
#include <sstream>
#include <memory>

using namespace std;

#include <sp/dataclasses/tokens/Token.h>
#include <sp/lexer/Lexer.h>
#include <sp/parser/SimpleSyntaxParserManager.h>
#include <sp/design_extractor/DesignExtractorManager.h>
#include <sp/dataclasses/design_objects/Entity.h>
#include <sp/dataclasses/design_objects/Pattern.h>
#include <sp/dataclasses/design_objects/Relationship.h>

class SourceProcessor {
private:
	string program;
	bool isInitialized = false;
	shared_ptr<ASTNode> astRoot;
	DesignExtractorManager designManager;
public: 
	SourceProcessor() {

	void initialize(string program);
	vector<Relationship> extractRelations();
	vector<Pattern> extractPatterns();
	vector<Entity> extractEntities();
};