#include "catch.hpp"
#include <qps/results_projector/QueryResultsProjector.h>

TEST_CASE("QueryResultsProjector: test populateResultsList") {
    auto testListPopulatedCorrectly = [](unordered_set<string> evaluatorResults) {
        // given
        list<string> autotesterList;

        // when
        QueryResultsProjector(evaluatorResults).populateResultsList(autotesterList);

        // then
        unordered_set<string> autotesterSet {};
        for (string s : autotesterList) {
            autotesterSet.insert(s);
        }
        REQUIRE(autotesterSet == evaluatorResults);
    };

    SECTION("Nonempty result") {
        testListPopulatedCorrectly(unordered_set<string> {"1", "55", "3", "-10000"});
        testListPopulatedCorrectly(unordered_set<string> {"main", "methodA", "foo", "bar"});
        testListPopulatedCorrectly(unordered_set<string> {"x", "y", "z", "a", "st", "variableName"});
    };

    SECTION("Nonempty result") {
        testListPopulatedCorrectly(unordered_set<string> {});
    };

};