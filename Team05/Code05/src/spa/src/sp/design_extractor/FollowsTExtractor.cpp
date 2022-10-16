#include <vector>

#include <sp/design_extractor/FollowsTExtractor.h>
#include <sp/dataclasses/ast/IfASTNode.h>
#include <sp/dataclasses/ast/WhileASTNode.h>

using namespace std;

vector<Relationship> FollowsTExtractor::extract(shared_ptr<ASTNode> ast) {
	vector<Relationship> extractedFollowsTRelationships;

	if (ast->isIfNode()) {
		shared_ptr<IfASTNode> ifNode = dynamic_pointer_cast<IfASTNode>(ast);

		vector<Relationship> thenFollowsTRelationships = this->handleStmtList(ifNode->getThenStatements());
		vector<Relationship> elseFollowsTRelationships = this->handleStmtList(ifNode->getElseStatements());

		extractedFollowsTRelationships.insert(extractedFollowsTRelationships.end(), thenFollowsTRelationships.begin(), thenFollowsTRelationships.end());
		extractedFollowsTRelationships.insert(extractedFollowsTRelationships.end(), elseFollowsTRelationships.begin(), elseFollowsTRelationships.end());
	}
	else if (ast->isWhileNode()) {
		shared_ptr<WhileASTNode> whileNode = dynamic_pointer_cast<WhileASTNode>(ast);

		vector<Relationship> whileFollowsTRelationships = this->handleStmtList(whileNode->getStmtList());

		extractedFollowsTRelationships.insert(extractedFollowsTRelationships.end(), whileFollowsTRelationships.begin(), whileFollowsTRelationships.end());
	}
	else if (ast->isProcedureNode() || ast->isProgramNode()) {
		// For procedure node, recursively extract followsT relationships from its statement list.
		// For program node, recursively extract followsT relationships from its procedures.
		for (shared_ptr<ASTNode> child : ast->getChildren()) {
			vector<Relationship> extractedRelations = this->extract(child);
			extractedFollowsTRelationships.insert(extractedFollowsTRelationships.begin(), extractedRelations.begin(), extractedRelations.end());
		}
	}
	else if (ast->isStmtLstNode()) {

		vector<Relationship> followsRelationships = this->handleStmtList(ast);

		extractedFollowsTRelationships.insert(extractedFollowsTRelationships.end(), followsRelationships.begin(), followsRelationships.end());
	}
	else {
		throw ASTException("Unknown AST node was passed to FollowsTExtractor!");
	}

	return extractedFollowsTRelationships;
}
