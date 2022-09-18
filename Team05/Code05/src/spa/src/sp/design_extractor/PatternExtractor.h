#pragma once
#include <vector>
#include <stack>
#include <sp/dataclasses/design_objects/Pattern.h>
#include <sp/dataclasses/ast/AST.h>
#include <sp/dataclasses/ast/AssignASTNode.h>
#include <sp/dataclasses/ast/BracketsASTNode.h>
#include <sp/dataclasses/ast/ExpressionASTNode.h>
#include <sp/dataclasses/ast/IfASTNode.h>
#include <sp/dataclasses/ast/WhileASTNode.h>
#include <sp/dataclasses/ast/exceptions/ASTException.h>
#include <sp/design_extractor/Extractor.h>

using namespace std;

class PatternExtractor : public Extractor<Pattern> {

public:
	PatternExtractor() {};

	/*
		This method is used to extract patterns from a provided abstract syntax tree.
	*/
	virtual vector<Pattern> extract(shared_ptr<ASTNode> ast) override;

	vector<Pattern> handleIf(shared_ptr<ASTNode> ast);
	vector<Pattern> handleAssign(shared_ptr<ASTNode> ast);
	vector<Pattern> handleWhile(shared_ptr<ASTNode> ast);
	vector<Pattern> handleContainer(shared_ptr<ASTNode> ast);
	string handleCondition(shared_ptr<ASTNode> ast);


	string recursiveExtractExpression(shared_ptr<ASTNode> ast);

	/*
		Helper function to determine precedence of operator
	*/
	// refer to https://www.tutorialspoint.com/Convert-Infix-to-Postfix-Expression#:~:text=To%20convert%20infix%20expression%20to,maintaining%20the%20precedence%20of%20them.
	int precedence(char c) {
		if (c == '/' || c == '*' || c == '%') {
			return 2;
		}
		else if (c == '+' || c == '-') {
			return 1;
		}
		else {
			return 0;
		}
	}

	string convertInfixToPostFix(string infixString) {
		stack<char> st; // to store the ordering of the operators

		string postFixString;

		string::iterator it;

		for (it = infixString.begin(); it != infixString.end(); it++) {
			char c = *it;
			if (isalnum(c)) {
				postFixString += c;
			}
			else if (c == '(') {
				st.push(c);
			}
			else if (c == ')') { // get all the operands inside the brackets
				while (!st.empty() && st.top() != '(') {
					postFixString += st.top();
					st.pop();
				}
				st.pop(); // remove the ')'
			}
			else {
				while (!st.empty() && this->precedence(c) <= this->precedence(st.top())) {
					postFixString += st.top();
					st.pop();
				}
				st.push(c);
			}
		}

		while (!st.empty()) {
			postFixString += st.top();
			st.pop();
		}
		return postFixString;
	};

};