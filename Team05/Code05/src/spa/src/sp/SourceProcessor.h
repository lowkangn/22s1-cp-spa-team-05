#include <sp/lexer/Lexer.h>
#include <sp/parser/SimpleSyntaxParserManager.h>
#include <sp/parser/cfg_parser/ControlFlowGraphparser.h>
#include <sp/design_extractor/DesignExtractorManager.h>
#include <sp/design_extractor/EntityExtractor.h>
#include <sp/design_extractor/PatternExtractor.h>
#include <sp/design_extractor/NextExtractor.h>
#include <sp/design_extractor/ModifiesExtractor.h>
#include <sp/design_extractor/UsesExtractor.h>
#include <sp/design_extractor/ParentExtractor.h>
#include <sp/design_extractor/ParentTExtractor.h>
#include <sp/design_extractor/FollowsAndFollowsTExtractor.h>
#include <sp/design_extractor/CallsAndCallsTExtractor.h>

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
	vector<shared_ptr<CFGNode>> controlFlowGraphs;
	Lexer lexer = Lexer();
	ParserManager parser = ParserManager();
	DesignExtractorManager designManager{
		EntityExtractor(),
		PatternExtractor(),
		NextExtractor(),
       
		{
			shared_ptr<Extractor<Relationship>>(new FollowsAndFollowsTExtractor()),
			shared_ptr<Extractor<Relationship>>(new ParentExtractor()),
			shared_ptr<Extractor<Relationship>>(new ParentTExtractor()),
			shared_ptr<Extractor<Relationship>>(new ModifiesExtractor()),
			shared_ptr<Extractor<Relationship>>(new UsesExtractor()),
			shared_ptr<Extractor<Relationship>>(new CallsAndCallsTExtractor())
		}
	};
    ControlFlowParser cfgParser = ControlFlowParser();

public:
	SourceProcessor() {};

	void initialize(istream& sourceProgram);
	void extractAllAndAddToPkb(shared_ptr<PKBUpdateHandler> pkb);

	vector<Relationship> extractRelations();
	vector<Relationship> extractCFGRelations();
	vector<Pattern> extractPatterns();
	vector<Entity> extractEntities();

};