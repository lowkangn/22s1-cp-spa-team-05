#pragma once

#include <sp/dataclasses/ast/AST.h>
#include <sp/dataclasses/ast/ProcedureASTNode.h>
#include <sp/dataclasses/ast/WhileASTNode.h>
#include <sp/dataclasses/ast/IfASTNode.h>

#include <sp/dataclasses/cfg/CFGNode.h>
#include <sp/dataclasses/cfg/IfCFGNode.h>
#include <sp/dataclasses/cfg/WhileCFGNode.h>

#include <vector>
#include <memory>
#include <sp/dataclasses/cfg/exceptions/CFGException.h>

class ControlFlowParser {
public:
	ControlFlowParser() {};

	/*
		Takes in the root of the AST tree of the program and return a ControlFlowGraph for each method
	*/
	vector<shared_ptr<CFGNode>> parse(shared_ptr<ASTNode> rootNode);
	
	/*
		Processes a Procedure node in the AST
	*/
	shared_ptr<CFGNode> handleProcedure(shared_ptr<ASTNode> rootNode);

	/*
		Processes a If node in the AST
	*/
	shared_ptr<CFGNode> handleIf(shared_ptr<ASTNode> rootNode);

	/*
		Processes a While node in the AST
	*/
	shared_ptr<CFGNode> handleWhile(shared_ptr<ASTNode> rootNode);

	/*
		Processes a statement list node in the AST
	*/
	shared_ptr<CFGNode> handleStatementList(shared_ptr<ASTNode> rootNode);

	/*
		Recursively traverses a node and adds a child to the end of the CFGNode
	*/
	void addChildToEndOfNode(shared_ptr<CFGNode> root, shared_ptr<CFGNode> child);
};