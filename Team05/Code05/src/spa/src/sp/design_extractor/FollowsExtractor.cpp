#pragma once

#include <vector>

#include <sp/design_extractor/FollowsExtractor.h>
#include <sp/dataclasses/ast/IfASTNode.h>
#include <sp/dataclasses/ast/WhileASTNode.h>

using namespace std;

vector<Relationship> FollowsExtractor::extract(shared_ptr<ASTNode> ast) {
	return vector<Relationship>();
}

vector<Relationship> FollowsExtractor::extractFromContainer(Entity leftHandSide, shared_ptr<ASTNode> containerASTNode) {
	return vector<Relationship>();
}
