#include <qps/query_parser/query_parser_states/WithState.h>
#include <qps/query_parser/QueryParser.h>

void WithState::parseOneClause(list<PQLToken>& tokens, const unordered_map<string, ArgumentType>& declarations) {
	if (tokens.empty()) {
		throw PQLSyntaxError("Query ended after with");
	}
	shared_ptr<WithParser> withParser = make_shared<WithParser>(tokens, declarations);
	shared_ptr<WithClause> clause = withParser->parse();
	this->context->addWithClause(clause);
	this->context->setSemanticErrorFromParser(withParser);
}