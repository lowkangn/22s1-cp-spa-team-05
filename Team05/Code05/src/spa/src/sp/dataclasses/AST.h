#pragma once

#include <sp/dataclasses/tokens/Token.h>
#include <vector>
#include <assert.h>
#include <memory>
using namespace std;



enum class ASTNodeType { PROGRAM, PROCEDURE, NAME, STMTLIST, READ, PRINT, ASSIGN, CALL, WHILE, IF, EXPRESSION, CONSTANT, OPERATOR, UNDEFINED };

class ASTNode {



private:
	// Default line number is -1 (For validation purposes)
	int lineNumber = -1;
	// Default is UNDEFINED
	ASTNodeType type = ASTNodeType::UNDEFINED;
	vector<Token> tokens;
	vector<shared_ptr<ASTNode>> children;

public:
	const static int LEFT_CHILD = 0;
	const static int RIGHT_CHILD = 1;
	// constructor
	ASTNode(vector<Token> &tokens) : tokens(tokens) {
		this->tokens = tokens;
		this->lineNumber = lineNumber;
	}

	/*
		Changes the line number of the node.
	*/
	void setLineNumber(int lineNumber) {
		this->lineNumber = lineNumber;
	}
public: 

	/*
		Adds a child to the node.
	*/
	void addChild(shared_ptr<ASTNode> child) {
		this->children.push_back(child);
	}

	vector<shared_ptr<ASTNode>> getChildren() {
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
	ASTNodeType getType() {
		return this->type;
	}

	/*
		Get the token present in the ASTNode
	*/
	vector<Token> getTokens() {
		return this->tokens;
	}

	/*
		Returns the line number of this ASTNode
	*/
	int getLineNumber() {
		return this->lineNumber;
	}

	/*
		Returns the number of children this node contains
	*/
	int numChildren() {
		return children.size();
	}

	/*
		Returns true if the ASTNode is terminal else false
	*/
	bool isTerminal() {
		return this->children.size() == 0;
	}

	/*
		Returns the name of the ASTNode if the ASTNode is of the Name type
	*/
	Token getNameToken() {
		assert(this->getTokens().size() == 1);

		return this->getTokens()[0];
	}

	bool equals(ASTNode other) {
		bool typeCheck = this->type == other.type;
		bool lineNumberCheck = this->lineNumber == other.lineNumber;
		

		// Check if tokens are the same
		bool tokenLengthCheck = this->tokens.size() == other.tokens.size();
		for (int i = 0; i < this->tokens.size(); i++) {
			auto token = this->tokens.at(i);
			auto otherToken = other.tokens.at(i);
			if (!token.equals(otherToken)) {
				return false;
			}
		}

		// Check if children are the same
		bool childrenLengthCheck = this->children.size() == other.children.size();
		for (int i = 0; i < this->children.size(); i++) {
			auto child = *this->children.at(i);
			auto otherChild = *other.children.at(i);
			if (!child.equals(otherChild)) {
				return false;
			}
		}

		return typeCheck && lineNumberCheck;
	}
};
