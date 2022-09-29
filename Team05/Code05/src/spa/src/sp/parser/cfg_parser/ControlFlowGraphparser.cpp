#include  <sp/parser/cfg_parser/ControlFlowGraphparser.h>

vector<shared_ptr<CFGNode>> ControlFlowParser::parse(shared_ptr<ASTNode> rootNode)
{
	vector<shared_ptr<CFGNode>> procedureCFGNodes;

	if (rootNode->getType() != ASTNodeType::PROGRAM) {
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
	shared_ptr<ProcedureASTNode> procNode = dynamic_pointer_cast<ProcedureASTNode>(rootNode);
	// Ensure that the node is a procedure node
	if (procNode == nullptr) {
		throw CFGException("Error in casting node in handleProcedure to procedure node");
	}

	shared_ptr<ASTNode> childStatements = procNode->getStmtList();

	shared_ptr<CFGNode> rootOfCFG = this->handleStatementList(childStatements);

	return rootOfCFG;
}

shared_ptr<CFGNode> ControlFlowParser::handleIf(shared_ptr<ASTNode> ifNode)
{
	shared_ptr<IfASTNode> rootNode = dynamic_pointer_cast<IfASTNode>(ifNode);
	// Ensure that the node is a procedure node
	if (rootNode == nullptr) {
		throw CFGException("Error in casting node in handleIf to If node");
	}

	shared_ptr<ASTNode> thenStmtLst = rootNode->getThenStatements();
	shared_ptr<ASTNode> elseStmtLst = rootNode->getElseStatements();

	shared_ptr<CFGNode> ifCFGNode(new IfCFGNode(rootNode->getLineNumber()));

	// Get roots of the then and else statement lists
	shared_ptr<CFGNode> rootThenNode = this->handleStatementList(thenStmtLst);
	shared_ptr<CFGNode> rootElseNode = this->handleStatementList(elseStmtLst);

	// Add root of then and else statement to the ifCFGNode
	ifCFGNode->addNext(rootThenNode);
	ifCFGNode->addNext(rootElseNode);

	return ifCFGNode;
}

shared_ptr<CFGNode> ControlFlowParser::handleWhile(shared_ptr<ASTNode> whileNode)
{
	shared_ptr<WhileASTNode> rootNode = dynamic_pointer_cast<WhileASTNode>(whileNode);
	// Ensure that the node is a procedure node
	if (rootNode == nullptr) {
		throw CFGException("Error in casting node in handleWhile to While node");
	}

	shared_ptr<ASTNode> stmtList = rootNode->getStmtList();

	// Get root of statement list within while
	shared_ptr<CFGNode> stmtListCFG = this->handleStatementList(stmtList);

	shared_ptr<CFGNode> whileCFGNode(new WhileCFGNode(whileNode->getLineNumber()));

	// Append statement list first
	whileCFGNode->addNext(stmtListCFG);

	// Add whileCFGNode to the end of the statement list
	this->addChildToTheEndOfRoot(stmtListCFG, whileCFGNode);
	
	return whileCFGNode;
}

shared_ptr<CFGNode> ControlFlowParser::handleStatementList(shared_ptr<ASTNode> rootNode)
{
	vector<shared_ptr<ASTNode>> children = rootNode->getChildren();

	// Initialize dummy node so that we can iterate through
	shared_ptr<CFGNode> dummyCFG(new CFGNode(-1));

	shared_ptr<CFGNode> currentCFG = dummyCFG;
	int counter = 0;

	while (counter < children.size()) {
		shared_ptr<ASTNode> currentAST = children[counter];
		ASTNodeType childType = currentAST->getType();

		// Initialize empty node
		shared_ptr<CFGNode> childCFGNode;

		// Handle next node for If and while
		switch (childType) {
		// We need to have diamond shape
		case ASTNodeType::IF: {
			childCFGNode = this->handleIf(currentAST);
			break;
		}
		// Need to add loop to the last statement in while
		case ASTNodeType::WHILE: {
			childCFGNode = this->handleWhile(currentAST);
			break;
		}
		// Add to last node and update current node
		default: {
			childCFGNode = shared_ptr<CFGNode>(new CFGNode(currentAST->getLineNumber()));
		}
		}
		this->addChildToTheEndOfRoot(currentCFG, childCFGNode);
		currentCFG = childCFGNode;
		counter += 1;
	}

	// Return root
	return dummyCFG->getNext();
}

void ControlFlowParser::addChildToTheEndOfRoot(shared_ptr<CFGNode> root, shared_ptr<CFGNode> child)
{
	bool isIfNode = root->isIfNode();

	if (root->isIfNode()) {
		// Check if the then and else have children
		shared_ptr<IfCFGNode> ifCFGNode = dynamic_pointer_cast<IfCFGNode>(root);

		shared_ptr<CFGNode> thenCFGNode = ifCFGNode->getThenNode();
		shared_ptr<CFGNode> elseCFGNode = ifCFGNode->getElseNode();

		// Recursively add child to the end of the then and else nodes
		this->addChildToTheEndOfRoot(thenCFGNode, child);
		this->addChildToTheEndOfRoot(elseCFGNode, child);
	}
	else if (root->isWhileNode()) {
		shared_ptr<WhileCFGNode> whileCFGNode = dynamic_pointer_cast<WhileCFGNode>(root);

		if (!whileCFGNode->hasNext()) {
			// Add child to whileCFGNode
			whileCFGNode->addNext(child);
		}
		else {
			// Recurse and get node after the while loop
			this->addChildToTheEndOfRoot(whileCFGNode->getAfterWhile(), child);
		}
	}
	else {
		// Keep recursing untill we find the end
		if (root->hasNext()) {
			this->addChildToTheEndOfRoot(root->getNext(), child);
		}
		else {
			root->addNext(child);
		}
	}
}
