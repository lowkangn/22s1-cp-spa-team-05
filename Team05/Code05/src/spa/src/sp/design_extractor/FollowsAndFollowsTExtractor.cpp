#include <vector>

#include <sp/dataclasses/ast/IfASTNode.h>
#include <sp/dataclasses/ast/ProcedureASTNode.h>
#include <sp/dataclasses/ast/ProgramASTNode.h>
#include <sp/dataclasses/ast/WhileASTNode.h>
#include <sp/design_extractor/FollowsAndFollowsTExtractor.h>

using namespace std;

vector<Relationship> FollowsAndFollowsTExtractor::extract(shared_ptr<ASTNode> ast) {
	vector<Relationship> extractedRelationships;

	if (ast->isIfNode()) {
		shared_ptr<IfASTNode> ifNode = dynamic_pointer_cast<IfASTNode>(ast);

		vector<Relationship> extractedFromThen = this->handleStmtList(ifNode->getThenStatements());
		vector<Relationship> extractedFromElse = this->handleStmtList(ifNode->getElseStatements());

		extractedRelationships.insert(extractedRelationships.end(), extractedFromThen.begin(), extractedFromThen.end());
		extractedRelationships.insert(extractedRelationships.end(), extractedFromElse.begin(), extractedFromElse.end());
	}
	else if (ast->isWhileNode()) {
		shared_ptr<WhileASTNode> whileNode = dynamic_pointer_cast<WhileASTNode>(ast);

		vector<Relationship> extractedFromWhile = this->handleStmtList(whileNode->getStmtList());

		extractedRelationships.insert(extractedRelationships.end(), extractedFromWhile.begin(), extractedFromWhile.end());
	}
	else if (ast->isProcedureNode() || ast->isProgramNode()) {
		// For procedure node, recursively extract follows relationships from its statement list.
		// For program node, recursively extract follows relationships from its procedures.
		for (shared_ptr<ASTNode> child : ast->getChildren()) {
			vector<Relationship> extractedRelations = this->extract(child);
			extractedRelationships.insert(extractedRelationships.begin(), extractedRelations.begin(), extractedRelations.end());
		}
	}
	else if (ast->isStmtLstNode()) {

		vector<Relationship> followsRelationships = this->handleStmtList(ast);

		extractedRelationships.insert(extractedRelationships.end(), followsRelationships.begin(), followsRelationships.end());
	}
	else {
		throw ASTException("Unknown AST node was passed to FollowsAndFollowsTExtractor!");
	}

	return extractedRelationships;
}
