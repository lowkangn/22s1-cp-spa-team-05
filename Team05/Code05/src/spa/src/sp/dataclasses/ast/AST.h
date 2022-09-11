#pragma once

#include <sp/dataclasses/tokens/Token.h>
#include <vector>
#include <assert.h>
#include <memory>
#include <sp/dataclasses/design_objects/Entity.h>
using namespace std;

// Default line number is -1 (For validation purposes)
const int DEAFULT_LINE_NUMBER = -1;

enum class ASTNodeType { PROGRAM, PROCEDURE, STMTLIST, READ, PRINT, ASSIGN, CALL, WHILE, IF, VARIABLE, CONSTANT, EXPRESSION };

class ASTNode {

private:
	int lineNumber = DEAFULT_LINE_NUMBER;
	// Default is UNDEFINED
protected:
	ASTNodeType type;
	Token token;
	vector<shared_ptr<ASTNode>> children;
public:

	ASTNode(Token token) : token(token) {
		this->token = token;
	}
	/*
		Get the type of the ASTNode from the enum class ASTNodeType
	*/
	ASTNodeType getType() {
		return this->type;
	}

	/*
		Returns the line number of this ASTNode
	*/
	int getLineNumber() {
		return this->lineNumber;
	}

	/*
		Changes the line number of the node.
	*/
	void setLineNumber(int lineNumber) {
		this->lineNumber = lineNumber;
	}

	/*
		Adds a child to the node.
	*/
	virtual void addChild(shared_ptr<ASTNode> child) {
		this->children.push_back(child);
	}

	/*
		Returns the children of this node
	*/
	vector<shared_ptr<ASTNode>> getChildren() {
		return this->children;
	}

	int numChildren() {
		return this->children.size();
	}

	/*
		Return the token in the ast
	*/
	Token getToken() {
		return this->token;
	}

	/*
		Checks if one node is equal to the other
	*/
	bool equals(const shared_ptr<ASTNode> other) const;

	/*
		Returns true if the ASTNode is terminal else false
	*/
	virtual bool isTerminal()  = 0;

	/*
		Returns an extracted entity from the ASTNode
	*/
	virtual Entity extractEntity() = 0;

};
