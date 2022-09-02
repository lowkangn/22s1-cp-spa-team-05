#pragma once

#include <sp/dataclasses/tokens/Token.h>
#include <vector>
using namespace std;

enum class ASTNodeType { PROCEDURE, STMTLIST, READ, PRINT, ASSIGN, CALL, WHILE, IF, VARIABLE, CONSTANT, UNDEFINED };

class ASTNode {

private:
	// Default line number is -1 (For validation purposes)
	int lineNumber = -1;
	// Default is UNDEFINED
	ASTNodeType type = ASTNodeType::UNDEFINED;
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

	vector<ASTNode> getChildren() {
		return this->children;
	}

	/*
		Set the type of the ASTNode from the enum class ASTNodeType
	*/
	void setType(ASTNodeType type) {
		this->type = type;
	}

	/*
		Get the type of the ASTNode from the enum class ASTNodeType
	*/
	ASTNodeType getType(ASTNodeType type) {
		return this->type;
	}

	/*
		Get the token present in the ASTNode
	*/
	vector<Token> getTokens(ASTNodeType type) {
		return this->tokens;
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