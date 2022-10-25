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

public:
	SourceProcessor(istream& stream) {
		// First tokenize using Lexer
		Lexer lexer = Lexer();

		list<Token> tokens = lexer.tokenize(stream);

		// Get a AST tree using ParserManager
		ParserManager parser = ParserManager(tokens);

		shared_ptr<ASTNode> root = parser.parse();

		// Get a control flow graph using ControlFlowGraphParser
		ControlFlowParser cfgParser = ControlFlowParser();

		//vector<shared_ptr<CFGNode>> controlFlowGraphs = cfgParser.parse(root);

		this->astRoot = root;
		//this->controlFlowGraphs = controlFlowGraphs;
		this->isInitialized = true;
	};

	vector<Relationship> extractRelations();
	vector<Relationship> extractCFGRelations();
	vector<Pattern> extractPatterns();
	vector<Entity> extractEntities();
};