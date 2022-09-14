// imported libraries
#include <vector>


// imported locally
#include <sp/design_extractor/PatternExtractor.h>
#include <sp/dataclasses/ast/AST.h>
#include <sp/dataclasses/ast/BracketsASTNode.h>
#include <sp/dataclasses/ast/ExpressionASTNode.h>
#include <sp/dataclasses/ast/exceptions/ASTException.h>
#include <sp/dataclasses/design_objects/Pattern.h>

using namespace std;

vector<Pattern> PatternExtractor::extract(shared_ptr<ASTNode> ast) {
	return vector<Pattern>();
}

Pattern PatternExtractor::handleAssign(shared_ptr<ASTNode> ast) {
	if (ast->getType() != ASTNodeType::ASSIGN) {
		throw ASTException("PatternExtractor: Node is not an assign node");
	}
	
	shared_ptr<AssignASTNode> assignNode = dynamic_pointer_cast<AssignASTNode>(ast);

	shared_ptr<ASTNode> lhsNode = assignNode->getLeftHandSide();
	shared_ptr<ASTNode> rhsNode = assignNode->getRightHandSide();

	// Check that lhs is variable
	if (lhsNode->getType() != ASTNodeType::VARIABLE) {
		throw ASTException("AssignASTNode LHS has to be a variable");
	}

	string lhsString = lhsNode->getToken().getString();


	string rhsString = this->recursiveExtractExpression(rhsNode);

	Pattern pattern = Pattern::createAssignPattern(ast->getLineNumber(), lhsString, rhsString);

	return pattern;
}

string PatternExtractor::recursiveExtractExpression(shared_ptr<ASTNode> ast) {
	int numChildren = ast->getChildren().size();

	if (ast->isTerminal()) {
		return ast->getToken().getString();
	}
	// If ASTNode is not terminal this node must be an expression node
	else if (numChildren == 1) {
		assert(ast->getType() == ASTNodeType::BRACKETS);
		shared_ptr<BracketsASTNode> bracketsNode = dynamic_pointer_cast<BracketsASTNode> (ast);
		return "(" + recursiveExtractExpression(bracketsNode->getInBrackets()) + ")";
	} else if (numChildren == 2) {
		// Must be an expression node
		assert(ast->getType() == ASTNodeType::EXPRESSION);
		shared_ptr<ExpressionASTNode> expressionNode = dynamic_pointer_cast<ExpressionASTNode> (ast);
		string operatorString = expressionNode->getToken().getString();
		return recursiveExtractExpression(expressionNode->getLeftHandSide()) + " " + operatorString + recursiveExtractExpression(expressionNode->getRightHandSide());
	}
	else {
		throw ASTException("Found an incorrect type of ASTNode in recurstiveExtractExpression");
	}
}

