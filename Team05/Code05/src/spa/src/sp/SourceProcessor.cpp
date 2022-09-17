#include <sp/SourceProcessor.h>
#include <sp/lexer/Lexer.h>
#include <sp/parser/SimpleSyntaxParserManager.h>
#include <sp/design_extractor/DesignExtractorManager.h>
#include <sp/design_extractor/EntityExtractor.h>
#include <sp/design_extractor/PatternExtractor.h>
#include <sp/design_extractor/ModifiesExtractor.h>

#include <sp/design_extractor/DesignExtractorManager.h>

#include <sp/dataclasses/tokens/Token.h>
#include <sp/dataclasses/ast/AST.h>

#include <list>
#include <memory>

using namespace std;


void SourceProcessor::tokenizeParseExtractAndUpdatePkb(istream& filestream, shared_ptr<PKBUpdateHandler> pkb) {
	// initialize tokenizer and tokenize
	Lexer lexer = Lexer();
	list<Token> tokens = lexer.tokenize(filestream);
	
	// initialize parser and parse
	ParserManager parser = ParserManager(tokens);
	shared_ptr<ASTNode> programTree = parser.parse();
	
	// initialize extractor and extract
	// extractors
	shared_ptr<EntityExtractor> entityExtractor(new EntityExtractor());
	shared_ptr<PatternExtractor> patternExtractor(new PatternExtractor());
	shared_ptr<Extractor<Relationship>> modifiesExtractor = shared_ptr<Extractor<Relationship>>(new ModifiesExtractor());
	
	// put relationship extractors into a vector
	vector<shared_ptr<Extractor<Relationship>>> relationExtractors = vector<shared_ptr<Extractor<Relationship>>>{ modifiesExtractor };
	
	// create manager
	DesignExtractorManager extractor = DesignExtractorManager(*entityExtractor, *patternExtractor, relationExtractors);

}
