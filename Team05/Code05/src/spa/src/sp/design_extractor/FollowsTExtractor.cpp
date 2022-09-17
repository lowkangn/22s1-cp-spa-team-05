#pragma once

#include <vector>

#include <sp/design_extractor/FollowsTExtractor.h>
#include <sp/dataclasses/ast/IfASTNode.h>
#include <sp/dataclasses/ast/WhileASTNode.h>

using namespace std;

vector<Relationship> FollowsTExtractor::extract(shared_ptr<ASTNode> ast) {
	return vector<Relationship>();
}

vector<Relationship> FollowsTExtractor::recursiveExtractFromContainer(Entity leftHandSide, shared_ptr<ASTNode> containerASTNode) {
	return vector<Relationship>();
}
