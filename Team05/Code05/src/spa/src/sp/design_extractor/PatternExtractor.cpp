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
	ASTNodeType astType = ast->getType();
	vector<Pattern> patterns;

	vector<Pattern> extractedPatterns;
	switch (astType) {
		case ASTNodeType::ASSIGN: {
			extractedPatterns = this->handleAssign(ast);
			// Append
			patterns.insert(patterns.end(), extractedPatterns.begin(), extractedPatterns.end());
			break;
		}
		case ASTNodeType::WHILE: {
			extractedPatterns = this->handleWhile(ast);
			patterns.insert(patterns.end(), extractedPatterns.begin(), extractedPatterns.end());
			break;
		}
		case ASTNodeType::IF: {
			extractedPatterns = this->handleIf(ast);
			patterns.insert(patterns.end(), extractedPatterns.begin(), extractedPatterns.end());
			break;
		} 
		default: {
			if (!ast->isTerminal()) {
				vector<shared_ptr<ASTNode>> children = ast->getChildren();

				for (shared_ptr<ASTNode> child : children) {
					extractedPatterns = this->extract(child);
					patterns.insert(patterns.end(), extractedPatterns.begin(), extractedPatterns.end());
				}
			}
		}
	}

	return patterns;
}

vector<Pattern> PatternExtractor::handleWhile(shared_ptr<ASTNode> ast) {
	if (ast->getType() != ASTNodeType::WHILE) {
		throw ASTException("PatternExtractor: Node is not an while node");
	}

	shared_ptr<WhileASTNode> whileNode = dynamic_pointer_cast<WhileASTNode>(ast);

	// recursively extract from stmtlst
	vector<Pattern> extractedPatterns = this->extract(whileNode->getStmtList());

	// extract from condition
	shared_ptr<ASTNode> condNode = whileNode->getCondition();
	string condString = this->handleCondition(condNode);
	Pattern pattern = Pattern::createWhilePattern(whileNode->getLineNumber(), condString);

	extractedPatterns.push_back(pattern);

	return extractedPatterns;
}

vector<Pattern> PatternExtractor::handleIf(shared_ptr<ASTNode> ast) {
	if (ast->getType() != ASTNodeType::IF) {
		throw ASTException("PatternExtractor: Node is not an if node");
	}

	shared_ptr<IfASTNode> ifNode = dynamic_pointer_cast<IfASTNode>(ast);

	// recursively extract from then stmtlst
	vector<Pattern> extractedPatterns = this->extract(ifNode->getThenStatements());

	// recursively extract from else stmtlst
	vector<Pattern> elseExtractedPatterns = this->extract(ifNode->getElseStatements());

	// extract from condition
	shared_ptr<ASTNode> condNode = ifNode->getCondition();
	string condString = this->handleCondition(condNode);
	Pattern pattern = Pattern::createIfPattern(ifNode->getLineNumber(), condString);

	extractedPatterns.insert(extractedPatterns.end(), elseExtractedPatterns.begin(), elseExtractedPatterns.end());
	extractedPatterns.push_back(pattern);

	return extractedPatterns;
}



vector<Pattern> PatternExtractor::handleAssign(shared_ptr<ASTNode> ast) {
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

	rhsString = convertInfixToPostFix(rhsString);

	Pattern pattern = Pattern::createAssignPattern(ast->getLineNumber(), lhsString, rhsString);


	return vector<Pattern>{pattern};
}

string PatternExtractor::handleCondition(shared_ptr<ASTNode> ast) {
	string variablesInCondition = "";

	if (ast->isTerminal() && ast->getType() == ASTNodeType::VARIABLE) {
		return ast->getToken().getString();
	}
	else if (!ast->isTerminal()) {
		for (shared_ptr<ASTNode> child : ast->getChildren()) {
			string result = this->handleCondition(child);
			// Only insert if a non-empty string is returned
			if (result != "") {
				variablesInCondition += result + " ";
			}
		}
	}
	// Something was inserted
	if (variablesInCondition.size() > 1) {
		variablesInCondition.pop_back(); // removes the last space
	}
	return variablesInCondition;
}

string PatternExtractor::recursiveExtractExpression(shared_ptr<ASTNode> ast) {
	int numChildren = ast->getChildren().size();

	// node is terminal (Either VariableASTNode or ConstantValueASTNode) can return the token as a string
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
		// Returns infix initially, will converted to postfix in the handleAssign
		return recursiveExtractExpression(expressionNode->getLeftHandSide()) + operatorString + recursiveExtractExpression(expressionNode->getRightHandSide());
	}
	else {
		throw ASTException("Found an incorrect type of ASTNode in recursiveExtractExpression");
	}
}
