#include <qps/QPS.h>

void QPS::evaluate(string queryString, shared_ptr<PKBQueryHandler> pkb) {

	try {
		// tokenize
		stringstream queryStream(queryString);
		istream& stream = queryStream;
		list<PQLToken> tokens = QPSTokenizer().tokenize(queryStream);

		// parse
		QueryParser parser(tokens);
		Query query = parser.parse();

		// evaluate
		this->evaluatorResults = QueryEvaluator().evaluate(query, pkb);
	}
	catch (const PQLSemanticError& e) {
		this->handleSemanticError(e);
	}
	catch (const PQLSyntaxError& e) {
		this->handleSyntaxError(e);
	}
	catch (const PQLLogicError& e) {
		this->handleLogicError(e);
	}
};

void QPS::projectResults(list<std::string>& results) {
	// project
	QueryResultsProjector projector(evaluatorResults);
	projector.populateResultsList(results);
};