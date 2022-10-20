#include <qps/query_parser/query_parser_states/PatternState.h>
#include <qps/query_parser/QueryParser.h>

void PatternState::parseOneClause(list<PQLToken>& tokens, unordered_map<string, ArgumentType>& declarations) {
	if (tokens.empty() || !tokens.front().isName()) {
		throw PQLSyntaxError("Missing synonym after pattern");
	}
	PQLToken token = tokens.front();
	shared_ptr<PatternParser> parserPointer = make_shared<PatternParser>(tokens, declarations);
	shared_ptr<PatternClause> clause = parserPointer->parse();
	this->context->addPatternClause(clause);
	this->context->setSemanticErrorFromParser(parserPointer);
}