#include <sp/parser/rules/ProgramSimpleSyntaxRule.h>
#include <sp/parser/rules/ProcedureSimpleSyntaxRule.h>
#include <sp/parser/rules/SimpleSyntaxRule.h>
#include <sp/parser/exceptions/SimpleSyntaxParserException.h>

vector<SimpleSyntaxRule> ProgramSimpleSyntaxRule::generateChildRules() {
	
	/*
		By definition, a program must be made up of multiple procedures. 
		So with the tokens at hand, we need to look out for procedure keywords/symbols 
		and group them into corresponding child procedure rules.
	*/

	vector<SimpleSyntaxRule> childRules; // create arraylist
	list<Token> tokens = this->tokens;
	list<Token> remainingTokens;
	ProcedureSimpleSyntaxRule childProcedure;
	while (!tokens.empty()) {

		// create child rule and let it consume the tokens it needs
		childProcedure = ProcedureSimpleSyntaxRule();
		remainingTokens = childProcedure.consumeTokens(tokens);

		// check that it actually consumed some tokens
		if ((remainingTokens.size() == tokens.size()) && (tokens.size()!=0)) {
			// none consumed
			// this should not happen, since the a program must be made up of an exact number of procedures
			throw SimpleSyntaxParserException(
				"Unable to parse another procedure when there are still tokens left. This should not happen!"
			);
		}

		// all is ok, add this rule to list of rules
		childRules.push_back(childProcedure);

		// set remaining to tokens
		tokens = remainingTokens;

	}

	return childRules;
}

list<Token> ProgramSimpleSyntaxRule::consumeTokens(list<Token> tokens) {
	// a program is the root, and should consume all tokens
	// assign and do state management
	this->tokens = tokens;
	this->initialized = true;

	// then return an empty list of tokens
	return list<Token>();
}
