#pragma once

#include <sp/dataclasses/Token.h>
#include <vector>
using namespace std;

class ASTNode {


private:
	// TODO: include node types
	int lineNumber;
	vector<Token> tokens;
	vector<ASTNode> children;

public: 
	// constructor
	ASTNode(vector<Token> tokens, int lineNumber) {
		this->tokens = tokens;
		this->lineNumber = lineNumber;
	}

	/*
		Adds a child to the node.
	*/
	void addChild(ASTNode child) {
		this->children.push_back(child);
	}
};