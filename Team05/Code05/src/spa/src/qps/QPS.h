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
    ClauseResult evaluatorResult;
    string errorString;
    bool isReturningBoolean;

	void handleSemanticError(const PQLSemanticError& e) {
		cout << e.what();
		cout << "\n";
		errorString = "SemanticError";
	}

	void handleSyntaxError(const PQLSyntaxError& e) {
		cout << e.what();
		cout << "\n";
        errorString = "SyntaxError";
	}

	void handleLogicError(const PQLLogicError& e) {
		cout << e.what();
		cout << "\n";
	}

public:
    QPS() : errorString(""), isReturningBoolean(false) {}

    void evaluate(string queryStream, shared_ptr<PKBQueryHandler> pkb);

	void projectResults(list<std::string>& results);
};