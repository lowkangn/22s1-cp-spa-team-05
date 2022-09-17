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
		evaluatorResults = QueryEvaluator().evaluate(query, pkb);
	}
	catch (PQLSemanticError e) {
		cout << e.what();
		cout << "\n";
		evaluatorResults.clear();
		evaluatorResults.insert("SemanticError");
	}
	catch (PQLSyntaxError e) {
		cout << e.what();
		cout << "\n";
		evaluatorResults.clear();
		evaluatorResults.insert("SyntaxError");
	}
};

void QPS::projectResults(list<std::string>& results) {
	// project
	QueryResultsProjector projector(evaluatorResults);
	projector.populateResultsList(results);
};