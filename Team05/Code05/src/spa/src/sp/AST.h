#pragma once

#include "Token.h"
#include "vector"
using namespace std;

class ASTNode {


private:
	// TODO: include node types
	vector<Token> tokens;
	vector<ASTNode> children;

public: 
	// constructor
	ASTNode(vector<Token> tokens) {
		this->tokens = tokens;
	}


	/*
		Adds a child to the node.
	*/
	void addChild(ASTNode child) {
		this->children.push_back(child);
	}
};