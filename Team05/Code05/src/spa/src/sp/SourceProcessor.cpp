#include <sp/SourceProcessor.h>
#include <sp/lexer/Lexer.h>
#include <sp/parser/SimpleSyntaxParserManager.h>
#include <sp/dataclasses/tokens/Token.h>
#include <sp/dataclasses/AST.h>

#include <list>
#include <memory>

using namespace std;


void SourceProcessor::tokenizeParseExtractAndUpdatePkb(istream& filestream, PKBUpdateHandler pkb) {
	// initialize tokenizer and tokenize
	Lexer lexer = Lexer();
	list<Token> tokens = lexer.tokenize(filestream);

	// initialize parser and parse
	ParserManager parser = ParserManager(tokens);
	shared_ptr<ASTNode> programTree = parser.parse();

	// initialize extractor and extract



}
