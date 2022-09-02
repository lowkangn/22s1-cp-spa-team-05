#pragma once

#include <sp/dataclasses/tokens/Token.h>
#include <vector>
using namespace std;

class ASTNode {


private:
	vector<ASTNode> children;

public: 

	/*
		Adds a child to the node.
	*/
	void addChild(ASTNode child);
};