#include  <sp/parser/cfg_parser/ControlFlowGraphparser.h>

vector<shared_ptr<CFGNode>> ControlFlowParser::parse(shared_ptr<ASTNode> rootNode)
{
	vector<shared_ptr<CFGNode>> procedureCFGNodes;

	if (!rootNode->isProgramNode()) {
		throw CFGException("ControlFlowPraser should receive a program node");
	}

	for (shared_ptr<ASTNode> proc : rootNode->getChildren()) {
		shared_ptr<CFGNode> rootOfProc = this->handleProcedure(proc);
		procedureCFGNodes.push_back(rootOfProc);
	}

	return procedureCFGNodes;
}

shared_ptr<CFGNode> ControlFlowParser::handleProcedure(shared_ptr<ASTNode> rootNode)
{	
	if (!rootNode->isProcedureNode()) {
		throw CFGException("Error in node in handleProcedure is not a procedure");
	}

	shared_ptr<ProcedureASTNode> procNode = dynamic_pointer_cast<ProcedureASTNode>(rootNode);

	shared_ptr<ASTNode> childStatements = procNode->getStmtList();

	shared_ptr<CFGNode> rootOfCFG = this->handleStatementList(childStatements);

	return rootOfCFG;
}

shared_ptr<CFGNode> ControlFlowParser::handleIf(shared_ptr<ASTNode> ifNode)
{
	if (!ifNode->isIfNode()) {
		throw CFGException("Error node inside handleIf is not an IfASTNode");
	}

	shared_ptr<IfASTNode> rootNode = dynamic_pointer_cast<IfASTNode>(ifNode);

	shared_ptr<ASTNode> thenStmtLst = rootNode->getThenStatements();
	shared_ptr<ASTNode> elseStmtLst = rootNode->getElseStatements();

	shared_ptr<CFGNode> ifCFGNode = IfCFGNode::createIfCFGNode(rootNode->getLineNumber());

	// Get roots of the then and else statement lists
	shared_ptr<CFGNode> rootThenNode = this->handleStatementList(thenStmtLst);
	shared_ptr<CFGNode> rootElseNode = this->handleStatementList(elseStmtLst);

	// Add root of then and else statement to the ifCFGNode
	ifCFGNode->addChild(rootThenNode);
	ifCFGNode->addChild(rootElseNode);

	return ifCFGNode;
}

shared_ptr<CFGNode> ControlFlowParser::handleWhile(shared_ptr<ASTNode> whileNode)
{
	if (!whileNode->isWhileNode()) {
		throw CFGException("Error node inside handleWhile is not an WhileASTNode");
	}

	shared_ptr<WhileASTNode> rootNode = dynamic_pointer_cast<WhileASTNode>(whileNode);

	shared_ptr<ASTNode> stmtList = rootNode->getStmtList();

	// Get root of statement list within while
	shared_ptr<CFGNode> stmtListCFG = this->handleStatementList(stmtList);

	shared_ptr<WhileCFGNode> whileCFGNode = WhileCFGNode::createWhileCFGNode(whileNode->getLineNumber());

	// Append statement list first
	whileCFGNode->addChild(stmtListCFG);

	// Add whileCFGNode to the end of the statement list
	this->addChildToEndOfNode(stmtListCFG, whileCFGNode);
	
	return whileCFGNode;
}

shared_ptr<CFGNode> ControlFlowParser::handleStatementList(shared_ptr<ASTNode> rootNode)
{
	vector<shared_ptr<ASTNode>> children = rootNode->getChildren();

	// Initialize dummy node so that we can iterate through
	shared_ptr<CFGNode> dummyCFG = CFGNode::createCFGNode(-1);

	shared_ptr<CFGNode> currentCFG = dummyCFG;
	int counter = 0;

	while (counter < children.size()) {
		shared_ptr<ASTNode> currentAST = children[counter];
		// Initialize empty node
		shared_ptr<CFGNode> childCFGNode;

		// Handle next node for If and while
		if (currentAST->isIfNode()) {
			childCFGNode = this->handleIf(currentAST);
		}
		else if (currentAST->isWhileNode()) {
			childCFGNode = this->handleWhile(currentAST);
		}
		else {
			childCFGNode = CFGNode::createCFGNode(currentAST->getLineNumber());
		}
		
		this->addChildToEndOfNode(currentCFG, childCFGNode);
		currentCFG = childCFGNode;
		counter += 1;
	}

	// Return root
	return dummyCFG->getNext();
}

void ControlFlowParser::addChildToEndOfNode(shared_ptr<CFGNode> root, shared_ptr<CFGNode> child)
{
	if (root->isIfNode()) {
		// Check if the then and else have children
		shared_ptr<IfCFGNode> ifCFGNode = dynamic_pointer_cast<IfCFGNode>(root);

		shared_ptr<CFGNode> thenCFGNode = ifCFGNode->getThenNode();
		shared_ptr<CFGNode> elseCFGNode = ifCFGNode->getElseNode();

		// Recursively add child to the end of the then and else nodes
		this->addChildToEndOfNode(thenCFGNode, child);
		this->addChildToEndOfNode(elseCFGNode, child);
	}
	else if (root->isWhileNode()) {
		shared_ptr<WhileCFGNode> whileCFGNode = dynamic_pointer_cast<WhileCFGNode>(root);

		if (!whileCFGNode->hasNext()) {
			// Add child to whileCFGNode
			whileCFGNode->addChild(child);
		}
		else {
			// Recurse and get node after the while loop
			this->addChildToEndOfNode(whileCFGNode->getAfterWhile(), child);
		}
	}
	else {
		// Keep recursing untill we find the end
		if (root->hasNext()) {
			this->addChildToEndOfNode(root->getNext(), child);
		}
		else {
			root->addChild(child);
		}
	}
}
