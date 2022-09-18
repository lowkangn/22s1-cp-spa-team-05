#pragma once

#include <iostream>
#include <sstream>
#include <list>

#include <pkb/interfaces/PKBQueryHandler.h>
#include <qps/query_tokenizer/QPSTokenizer.h>
#include <qps/query_parser/QueryParser.h>
#include <qps/query_evaluator/QueryEvaluator.h>
#include <qps/results_projector/QueryResultsProjector.h>

class QPS {
private:
	set<string> evaluatorResults;

	void handleSemanticError(const PQLSemanticError& e) {
		cout << e.what();
		cout << "\n";
		evaluatorResults.clear();
		evaluatorResults.insert("SemanticError");
	}

	void handleSyntaxError(const PQLSyntaxError& e) {
		cout << e.what();
		cout << "\n";
		evaluatorResults.clear();
		evaluatorResults.insert("SyntaxError");
	}

	void handleLogicError(const PQLLogicError& e) {
		cout << e.what();
		cout << "\n";
		evaluatorResults.clear();
	}

public:
	void evaluate(string queryStream, shared_ptr<PKBQueryHandler> pkb);
	
	set<string> getResults() {
		return this->evaluatorResults;
	}

	void projectResults(list<std::string>& results);
};