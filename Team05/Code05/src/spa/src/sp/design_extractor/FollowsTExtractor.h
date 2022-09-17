#pragma once
#include <vector>

#include <sp/dataclasses/ast/AST.h>
#include <sp/dataclasses/ast/StatementListASTNode.h>
#include <sp/dataclasses/design_objects/Relationship.h>
#include <sp/design_extractor/Extractor.h>

using namespace std;

class FollowsTExtractor : Extractor<Relationship> {

private:
	/*
		This method is used to extract relationships from a Statement list node.
	*/
	vector<Relationship> handleStmtList(shared_ptr<ASTNode> ast) {
		vector<Relationship> extractedFollowsTRelationships;

		shared_ptr<StatementListASTNode> stmtListNode = dynamic_pointer_cast<StatementListASTNode>(ast);
		vector<shared_ptr<ASTNode>> children = stmtListNode->getChildren();

		for (int i = 0; i < children.size(); i++) {

			shared_ptr<ASTNode> child = children[i];

			// Skip last child as there are no statements that follow it.
			if (i < children.size() - 1) {

				Entity followee = children[i]->extractEntity();

				// Form followsT relationships with every other statement in the statement list.
				for (int j = i + 1; j < children.size(); j++) {
					
					Entity follower = children[j]->extractEntity();

					Relationship toAdd = Relationship::createFollowsTRelationship(followee, follower);
					extractedFollowsTRelationships.push_back(toAdd);
				}
			}
			// If any statement is a if or while statement, extract the followsT relationships within it.
			if (child->isIfNode() || child->isWhileNode()) {
				vector<Relationship> nestedFollowsTRelationships = this->extract(child);
				extractedFollowsTRelationships.insert(extractedFollowsTRelationships.end(), 
					nestedFollowsTRelationships.begin(), nestedFollowsTRelationships.end());
			}
		}

		return extractedFollowsTRelationships;
	}
public:
	/*
		This method is used to extract relationships from a provided abstract syntax tree. It is meant to be
		overriden for each specific relationship type.
	*/
	virtual vector<Relationship> extract(shared_ptr<ASTNode> ast) override;
};

