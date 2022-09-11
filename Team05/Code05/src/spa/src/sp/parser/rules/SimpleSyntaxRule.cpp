#include <sp/parser/rules/SimpleSyntaxRule.h>
#include <sp/parser/exceptions/SimpleSyntaxParserException.h>
#include <sp/dataclasses/ast/AST.h>

using namespace std;

void SimpleSyntaxRule::validate() {

	// generate if needed
	if (!this->generated) {
		this->childRules = this->generateChildRules();
	}

	// for each rule, recursively validate
	// note that if no childrules, is terminal, then we are done
	for (auto rulePointer = this->childRules.begin(); rulePointer != this->childRules.end(); rulePointer++) {
		(*rulePointer)->validate();
	}
}

bool SimpleSyntaxRule::equals(SimpleSyntaxRule &other) {
	// status should be equal
	bool attributesEqual = (this->generated == other.generated) && (this->initialized == other.initialized);
	if (!attributesEqual) {
		return false;
	}
	
	// rules should be recursively identical
	bool sameNumberOfChildRules = (this->childRules.size() == other.childRules.size());
	if (!sameNumberOfChildRules) {
		return false;
	}
	else {
		// iterate and check
		for (int i = 0; i < this->childRules.size(); i++) {
			auto rule = this->childRules.at(i);
			auto otherRule = other.childRules.at(i);
			if ((*rule).equals(*otherRule)) {
				return false;
			}
		}
		return true;

	}

}

