#include <vector>

#include <sp/dataclasses/ast/IfASTNode.h>
#include <sp/dataclasses/ast/ProcedureASTNode.h>
#include <sp/dataclasses/ast/ProgramASTNode.h>
#include <sp/dataclasses/ast/WhileASTNode.h>
#include <sp/design_extractor/FollowsExtractor.h>

using namespace std;

vector<Relationship> FollowsExtractor::extract(shared_ptr<ASTNode> ast) {
	vector<Relationship> extractedFollowsRelationships;

	if (ast->isIfNode()) {
		shared_ptr<IfASTNode> ifNode = dynamic_pointer_cast<IfASTNode>(ast);

		vector<Relationship> thenFollowsRelationships = this->handleStmtList(ifNode->getThenStatements());
		vector<Relationship> elseFollowsRelationships = this->handleStmtList(ifNode->getElseStatements());

		extractedFollowsRelationships.insert(extractedFollowsRelationships.end(), thenFollowsRelationships.begin(), thenFollowsRelationships.end());
		extractedFollowsRelationships.insert(extractedFollowsRelationships.end(), elseFollowsRelationships.begin(), elseFollowsRelationships.end());
	}
	else if (ast->isWhileNode()) {
		shared_ptr<WhileASTNode> whileNode = dynamic_pointer_cast<WhileASTNode>(ast);

		vector<Relationship> whileFollowsRelationships = this->handleStmtList(whileNode->getStmtList());

		extractedFollowsRelationships.insert(extractedFollowsRelationships.end(), whileFollowsRelationships.begin(), whileFollowsRelationships.end());
	}
	else if (ast->isProcedureNode() || ast->isProgramNode()) {
		// For procedure node, recursively extract follows relationships from its statement list.
		// For program node, recursively extract follows relationships from its procedures.
		for (shared_ptr<ASTNode> child : ast->getChildren()) {
			vector<Relationship> extractedRelations = this->extract(child);
			extractedFollowsRelationships.insert(extractedFollowsRelationships.begin(), extractedRelations.begin(), extractedRelations.end());
		}
	}
	else if (ast->isStmtLstNode()) {

		vector<Relationship> followsRelationships = this->handleStmtList(ast);

		extractedFollowsRelationships.insert(extractedFollowsRelationships.end(), followsRelationships.begin(), followsRelationships.end());
	}
	else {
		throw ASTException("Unknown AST node was passed to FollowsExtractor!");
	}

	return extractedFollowsRelationships;
}
