#pragma once

#include <sp/dataclasses/tokens/Token.h>
#include <vector>
using namespace std;

class ASTNode {


private:
	// Default line number is -1 (For validation purposes)
	int lineNumber = -1;
	vector<Token> tokens;
	vector<ASTNode> children;

public: 
	// constructor
	ASTNode(vector<Token> &tokens) {
		this->tokens = tokens;
		this->lineNumber = lineNumber;
	}

	/*
		Changes the line number of the node.
	*/
	void changeLineNumber(int lineNumber) {
		this->lineNumber = lineNumber;
	}

	/*
		Adds a child to the node.
	*/
	void addChild(ASTNode &child) {
		this->children.push_back(child);
	}

	int getLineNumber() {
		return this->lineNumber;
	}

	/*
		Returns the number of children this node contains
	*/
	int numChildren() {
		return children.size();
	}
};