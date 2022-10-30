#include <qps/results_projector/QueryResultsProjector.h>

void QueryResultsProjector::populateResultsList(list<string>& autotesterResults,
    bool isReturningBoolean, string errorString) {
    // Populate list with error if there is an error
    if (!errorString.empty()) {
        autotesterResults.emplace_back(errorString);
        return;
    }
    // Otherwise, populate with results
    set<string> stringResults = this->evaluatorResult.convertTableToString(isReturningBoolean);
    for (string result : stringResults) {
        autotesterResults.emplace_back(result);
    }
}