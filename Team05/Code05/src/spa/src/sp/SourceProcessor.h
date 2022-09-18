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

class SourceProcessor {

private:
	bool isInitialized = false;
	shared_ptr<ASTNode> astRoot;
	DesignExtractorManager designManager {
		EntityExtractor(),
		PatternExtractor(),
		{
			// Un comment once Follows is done
			shared_ptr<Extractor<Relationship>> (new FollowsExtractor()),
			shared_ptr<Extractor<Relationship>> (new FollowsTExtractor()),
			shared_ptr<Extractor<Relationship>> (new ParentExtractor()),
			shared_ptr<Extractor<Relationship>> (new ParentTExtractor()),
			shared_ptr<Extractor<Relationship>> (new ModifiesExtractor()),
			shared_ptr<Extractor<Relationship>> (new UsesExtractor()),
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

		this->astRoot = root;
		this->designManager = designManager;
		this->isInitialized = true;
	};

	vector<Relationship> extractRelations();
	vector<Pattern> extractPatterns();
	vector<Entity> extractEntities();
};