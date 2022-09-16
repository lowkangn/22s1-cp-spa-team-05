// imported libraries
#include <vector>
#include <memory>
#include <assert.h>


// imported locally
#include <sp/dataclasses/ast/AST.h>
#include <sp/dataclasses/ast/AssignASTNode.h>
#include <sp/dataclasses/ast/ReadASTNode.h>
#include <sp/dataclasses/ast/WhileASTNode.h>
#include <sp/dataclasses/ast/ProcedureASTNode.h>
#include <sp/dataclasses/ast/IfASTNode.h>
#include <sp/design_extractor/ModifiesExtractor.h>

vector<Relationship> ModifiesExtractor::extract(shared_ptr<ASTNode> ast) {

	vector<Relationship> modifies = vector<Relationship>();
	ASTNodeType type = ast->getType();

	// Stmt types
	switch (type) {
	case ASTNodeType::ASSIGN:
	{
		vector<Relationship> extractedModifies = this->handleAssign(ast);
		modifies.insert(modifies.end(), extractedModifies.begin(), extractedModifies.end());
		break;
	}
	case ASTNodeType::READ:
	{
		vector<Relationship> extractedModifies = this->handleRead(ast);
		modifies.insert(modifies.end(), extractedModifies.begin(), extractedModifies.end());
		break;
	}
	case ASTNodeType::PROCEDURE:
	{
		vector<Relationship> extractedModifies = this->handleProcedure(ast);
		modifies.insert(modifies.end(), extractedModifies.begin(), extractedModifies.end());
		break;
	}
	case ASTNodeType::WHILE:
	{	
		vector<Relationship> extractedModifies = this->handleWhile(ast);
		modifies.insert(modifies.end(), extractedModifies.begin(), extractedModifies.end());
		break;
	}
	case ASTNodeType::IF:
	{
		vector<Relationship> extractedModifies = this->handleIf(ast);
		modifies.insert(modifies.end(), extractedModifies.begin(), extractedModifies.end());
		break;
	}
	// TODO (Not in milestone 1)
	case ASTNodeType::CALL:
		break;
	}

	if (ast->hasContainer()) {
		vector<shared_ptr<ASTNode>> children = ast->getChildren();
		for (int i = 0; i < children.size(); i++) {
			shared_ptr<ASTNode> child = children[i];
			vector<Relationship> extractedRelationships = this->extract(child);
			modifies.insert(modifies.end(), extractedRelationships.begin(), extractedRelationships.end());
		}
	}

	return modifies;
}

vector<Relationship> ModifiesExtractor::handleAssign(shared_ptr<ASTNode> ast) {
	vector<Relationship> modifiesRelationships = vector<Relationship>();

	// Sanity check to prevent erroneous calls outside of extract() since handleAssign is public 
	ASTNodeType currNodeType = ast->getType();
	if (currNodeType != ASTNodeType::ASSIGN) {
		throw ASTException("handleAssign can only accept assign AST nodes");
	}

	// This is the Left hand side of the assign relation
	shared_ptr<AssignASTNode> assignNode = dynamic_pointer_cast<AssignASTNode>(ast);
	shared_ptr<ASTNode> leftChild = assignNode->getLeftHandSide();

	// Extracting tokens of the entity which is modified
	Entity modifiedEntity = leftChild->extractEntity();
	Entity leftHandSide = assignNode->extractEntity();

	modifiesRelationships.push_back(Relationship::createModifiesRelationship(leftHandSide, modifiedEntity));

	return modifiesRelationships;
}

vector<Relationship> ModifiesExtractor::handleRead(shared_ptr<ASTNode> ast) {
	vector<Relationship> modifiesRelationships = vector<Relationship>();

	ASTNodeType currNodeType = ast->getType();
	if (currNodeType != ASTNodeType::READ) {
		throw ASTException("handleRead can only accept read AST nodes");
	}

	// Cast to ReadASTNode
	shared_ptr<ReadASTNode> readNode = dynamic_pointer_cast<ReadASTNode>(ast);
	shared_ptr<ASTNode> child = readNode->getVariableToRead();

	Entity leftHandSide = readNode->extractEntity();
	Entity rightHandSide = child->extractEntity();

	modifiesRelationships.push_back(Relationship::createModifiesRelationship(leftHandSide, rightHandSide));

	return modifiesRelationships;
}

vector<Relationship> ModifiesExtractor::handleProcedure(shared_ptr<ASTNode> ast) {
	ASTNodeType currNodeType = ast->getType();
	if (currNodeType != ASTNodeType::PROCEDURE) {
		throw ASTException("handleProcedure can only accept procedure AST nodes");
	}

	Entity leftHandSide = ast->extractEntity();

	vector<Relationship> extractedChildRelationships;
	shared_ptr<ProcedureASTNode> procedureNode = dynamic_pointer_cast<ProcedureASTNode>(ast);
	shared_ptr<ASTNode> childContainer = procedureNode->getStmtLst();

	// Iterate through children and extract Procedure Stmt relationships
	for (shared_ptr<ASTNode> child : childContainer->getChildren()) {
		vector <Relationship> extractedRelationships = recursiveContainerExtract(leftHandSide, child);
		extractedChildRelationships.insert(extractedChildRelationships.end(), extractedRelationships.begin(), extractedRelationships.end());
	}

	return extractedChildRelationships;
}

vector<Relationship> ModifiesExtractor::handleWhile(shared_ptr<ASTNode> ast) {
	ASTNodeType currNodeType = ast->getType();
	if (currNodeType != ASTNodeType::WHILE) {
		throw ASTException("handleWhile can only accept while AST nodes");
	}

	vector<Relationship> extractedChildRelationships;
	// Get the name ASTNode
	shared_ptr<WhileASTNode> whileNode = dynamic_pointer_cast<WhileASTNode>(ast);
	shared_ptr<ASTNode> childStmtLst = whileNode->getStmtList();

	Entity leftHandSide = whileNode->extractEntity();

	// Iterate through children and extract While Stmt relationships
	for (shared_ptr<ASTNode> child : childStmtLst->getChildren()) {
		vector <Relationship> extractedRelationships = recursiveContainerExtract(leftHandSide, child);
		extractedChildRelationships.insert(extractedChildRelationships.end(), extractedRelationships.begin(), extractedRelationships.end());
	}

	return extractedChildRelationships;
}

vector<Relationship> ModifiesExtractor::handleIf(shared_ptr<ASTNode> ast) {
	ASTNodeType currNodeType = ast->getType();
	if (currNodeType != ASTNodeType::IF) {
		throw ASTException("handleIf can only accept if AST nodes");
	}
	vector<Relationship> extractedChildRelationships = vector<Relationship>();

	// Get the name ASTNode
	shared_ptr<IfASTNode> ifASTNode = dynamic_pointer_cast<IfASTNode>(ast);
	shared_ptr<ASTNode> thenChild = ifASTNode->getThenStatements();
	shared_ptr<ASTNode> elseChild = ifASTNode->getElseStatements();


	Entity leftHandSide = ifASTNode->extractEntity();

	// Iterate through children and extract If Stmt relationships
	for (shared_ptr<ASTNode> child : thenChild->getChildren()) {
		vector <Relationship> extractedThenRelationships = recursiveContainerExtract(leftHandSide, child);
		extractedChildRelationships.insert(extractedChildRelationships.end(), extractedThenRelationships.begin(), extractedThenRelationships.end());
	}

	for (shared_ptr<ASTNode> child : elseChild->getChildren()) {
		vector <Relationship> extractedElseRelationships = recursiveContainerExtract(leftHandSide, child);
		extractedChildRelationships.insert(extractedChildRelationships.end(), extractedElseRelationships.begin(), extractedElseRelationships.end());
	}

	return extractedChildRelationships;
}

// TODO in a future iteration
vector<Relationship> ModifiesExtractor::handleCall(shared_ptr<ASTNode> ast) {
	ASTNodeType currNodeType = ast->getType();
	if (currNodeType != ASTNodeType::CALL) {
		throw ASTException("handleCall can only accept call AST nodes");
	}
	return vector<Relationship>();
}

vector<Relationship> ModifiesExtractor::recursiveContainerExtract(Entity& leftHandSide, shared_ptr<ASTNode> ast) {
	vector<Relationship> modifiesRelationships = vector<Relationship>();
	ASTNodeType type = ast->getType();

	switch (type) {
	case ASTNodeType::ASSIGN:
	{
		// Cast to AssignASTNode
		shared_ptr<AssignASTNode> assignNode = dynamic_pointer_cast<AssignASTNode>(ast);
		shared_ptr<ASTNode> leftChild = assignNode->getLeftHandSide();

		// Create relationship for parent of this container
		Entity childEntity = leftChild->extractEntity();
		Relationship toAdd = Relationship::createModifiesRelationship(leftHandSide, childEntity);

		modifiesRelationships.push_back(toAdd);
		break;
	}
	case ASTNodeType::READ:
	{
		// Cast to ReadASTNode
		shared_ptr<ReadASTNode> readNode = dynamic_pointer_cast<ReadASTNode>(ast);
		shared_ptr<ASTNode> child = readNode->getVariableToRead();
		
		// Create relationship for parent of this container
		Entity childEntity = child->extractEntity();
		Relationship toAdd = Relationship::createModifiesRelationship(leftHandSide, childEntity);

		modifiesRelationships.push_back(toAdd);
		break;
	}
	case ASTNodeType::WHILE:
	{
		// Cast to WhileASTNode
		shared_ptr<WhileASTNode> whileNode = dynamic_pointer_cast<WhileASTNode>(ast);
		shared_ptr<ASTNode> childrenStmtLst = whileNode->getStmtList();

		// Create relationship for parent of this container and the stmtlst inside this while node
		for (shared_ptr<ASTNode> child : childrenStmtLst->getChildren()) {
			vector<Relationship> toAdd = recursiveContainerExtract(leftHandSide, child);
			modifiesRelationships.insert(modifiesRelationships.end(), toAdd.begin(), toAdd.end());
		}
		break;
	}
	case ASTNodeType::IF:
	{
		// Cast IfASTNode
		shared_ptr<IfASTNode> ifNode = dynamic_pointer_cast<IfASTNode>(ast);
		shared_ptr<ASTNode> thenChildren = ifNode->getThenStatements();
		shared_ptr<ASTNode> elseChildren = ifNode->getElseStatements();


		// Create relationship for parent of this container and the then and else stmtlst inside this if node
		for (shared_ptr<ASTNode> child : thenChildren->getChildren()) {
			vector<Relationship> toAdd = recursiveContainerExtract(leftHandSide, child);
			modifiesRelationships.insert(modifiesRelationships.end(), toAdd.begin(), toAdd.end());
		}

		for (shared_ptr<ASTNode> child : elseChildren->getChildren()) {
			vector<Relationship> toAdd = recursiveContainerExtract(leftHandSide, child);
			modifiesRelationships.insert(modifiesRelationships.end(), toAdd.begin(), toAdd.end());
		}
		break;
	} 
	}
	return modifiesRelationships;
}
