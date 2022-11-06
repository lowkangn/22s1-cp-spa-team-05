#pragma once
#include <sp/dataclasses/ast/AST.h>
#include <sp/dataclasses/ast/StatementListASTNode.h>
#include <sp/dataclasses/design_objects/Relationship.h>
#include <sp/design_extractor/Extractor.h>

#include <memory>
#include <vector>

using namespace std;

class FollowsAndFollowsTExtractor : public Extractor<Relationship> {
private:
    /*
        This method is used to extract relationships from a Statement list node.
    */
    vector<Relationship> handleStmtList(shared_ptr<ASTNode> ast) {
        vector<Relationship> extractedRelationships;

        shared_ptr<StatementListASTNode> stmtListNode = dynamic_pointer_cast<StatementListASTNode>(
            ast);
        vector<shared_ptr<ASTNode>> children = stmtListNode->getChildren();

        for (int i = 0; i < children.size(); i++) {
            shared_ptr<ASTNode> child = children[i];

            // Skip last child as there are no statements that follow it.
            if (i < children.size() - 1) {
                Entity followee = children[i]->extractEntity();
                Entity follower = children[i + 1]->extractEntity();

                Relationship follows = Relationship::createFollowsRelationship(followee, follower);
                extractedRelationships.push_back(follows);

                // Form followsT relationships with every other statement in the statement list.
                for (int j = i + 1; j < children.size(); j++) {
                    Entity follower = children[j]->extractEntity();

                    Relationship toAdd = Relationship::createFollowsTRelationship(
                        followee,
                        follower);
                    extractedRelationships.push_back(toAdd);
                }
            }

            // If a statement is an if or while, extract the follows relationships within it.
            if (child->isIfNode() || child->isWhileNode()) {
                vector<Relationship> recursivelyExtracted = this->extract(child);
                extractedRelationships.insert(
                    extractedRelationships.end(),
                    recursivelyExtracted.begin(),
                    recursivelyExtracted.end());
            }
        }

        return extractedRelationships;
    }

public:
    /*
        This method is used to extract relationships from a provided abstract syntax tree. It is meant to be
        overriden for each specific relationship type.
    */
    vector<Relationship> extract(shared_ptr<ASTNode> ast) override;
};
