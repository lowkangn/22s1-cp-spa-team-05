// imported libraries
#include <vector>
#include <memory>


// imported locally
#include <sp/dataclasses/ast/AST.h>
#include <sp/dataclasses/ast/AssignASTNode.h>
#include <sp/dataclasses/ast/ReadASTNode.h>
#include <sp/dataclasses/ast/WhileASTNode.h>
#include <sp/dataclasses/ast/IfASTNode.h>
#include <sp/design_extractor/ModifiesExtractor.h>

vector<Relationship> ModifiesExtractor::extract(shared_ptr<ASTNode> ast) {

	vector<Relationship> modifies = vector<Relationship>();
	ASTNodeType type = ast->getType();

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
	default:
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

	// Cast to ReadASTNode
	shared_ptr<ReadASTNode> readNode = dynamic_pointer_cast<ReadASTNode>(ast);
	shared_ptr<ASTNode> child = readNode->getVariableToRead();

	Entity leftHandSide = readNode->extractEntity();
	Entity rightHandSide = child->extractEntity();

	modifiesRelationships.push_back(Relationship::createModifiesRelationship(leftHandSide, rightHandSide));

	return modifiesRelationships;
}

vector<Relationship> ModifiesExtractor::handleProcedure(shared_ptr<ASTNode> ast) {
	Entity leftHandSide = ast->extractEntity();

	vector<Relationship> extractedChildRelationships = recursiveContainerExtract(leftHandSide, ast);

	return extractedChildRelationships;
}

vector<Relationship> ModifiesExtractor::handleWhile(shared_ptr<ASTNode> ast) {
	// Get the name ASTNode
	shared_ptr<WhileASTNode> whileNode = dynamic_pointer_cast<WhileASTNode>(ast);
	shared_ptr<ASTNode> child = whileNode->getStmtList();

	Entity leftHandSide = whileNode->extractEntity();

	// Recursively extract relations from the while block
	vector<Relationship> extractedChildRelationships = recursiveContainerExtract(leftHandSide, child);

	return extractedChildRelationships;
}

vector<Relationship> ModifiesExtractor::handleIf(shared_ptr<ASTNode> ast) {
	// Get the name ASTNode
	shared_ptr<IfASTNode> ifASTNode = dynamic_pointer_cast<IfASTNode>(ast);
	shared_ptr<ASTNode> thenChild = ifASTNode->getThenStatements();
	shared_ptr<ASTNode> elseChild = ifASTNode->getElseStatements();


	Entity leftHandSide = ifASTNode->extractEntity();

	// Recursively extract relations from Then and else containers
	vector<Relationship> extractedThenChildRelationships = recursiveContainerExtract(leftHandSide, thenChild);
	vector<Relationship> extractedElseChildRelationships = recursiveContainerExtract(leftHandSide, elseChild);

	vector<Relationship> extractedChildRelationships = vector<Relationship>();

	extractedChildRelationships.insert(extractedChildRelationships.end(), extractedThenChildRelationships.begin(), extractedThenChildRelationships.end());
	extractedChildRelationships.insert(extractedChildRelationships.end(), extractedElseChildRelationships.begin(), extractedElseChildRelationships.end());

	return extractedChildRelationships;
}

// TODO in a future iteration
vector<Relationship> ModifiesExtractor::handleCall(shared_ptr<ASTNode> ast) {
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

		vector<Relationship> assignRelationship = handleAssign(ast);

		Entity childEntity = leftChild->extractEntity();
		Relationship toAdd = Relationship::createModifiesRelationship(leftHandSide, childEntity);

		modifiesRelationships.push_back(toAdd);
		modifiesRelationships.insert(modifiesRelationships.end(), assignRelationship.begin(), assignRelationship.end());
		break;
	}
	case ASTNodeType::READ:
	{
		// Cast to ReadASTNode
		shared_ptr<ReadASTNode> readNode = dynamic_pointer_cast<ReadASTNode>(ast);
		shared_ptr<ASTNode> child = readNode->getVariableToRead();

		vector<Relationship> readRelationship = handleRead(ast);
		
		Entity childEntity = child->extractEntity();
		Relationship toAdd = Relationship::createModifiesRelationship(leftHandSide, childEntity);

		modifiesRelationships.push_back(toAdd);
		modifiesRelationships.insert(modifiesRelationships.end(), readRelationship.begin(), readRelationship.end());
		break;
	}
	case ASTNodeType::WHILE:
	{
		// Cast to WhileASTNode
		shared_ptr<WhileASTNode> whileNode = dynamic_pointer_cast<WhileASTNode>(ast);
		shared_ptr<ASTNode> children = whileNode->getStmtList();

		vector<Relationship> toAdd = recursiveContainerExtract(leftHandSide, children);
		vector<Relationship> recursiveWhile = handleWhile(ast);
		modifiesRelationships.insert(modifiesRelationships.end(), toAdd.begin(), toAdd.end());
		break;
	}
	case ASTNodeType::IF:
	{
		// Cast IfASTNode
		shared_ptr<IfASTNode> ifNode = dynamic_pointer_cast<IfASTNode>(ast);
		shared_ptr<ASTNode> thenChildren = ifNode->getThenStatements();
		shared_ptr<ASTNode> elseChildren = ifNode->getElseStatements();


		vector<Relationship> toAddThen = recursiveContainerExtract(leftHandSide, thenChildren);
		vector<Relationship> toAddElse = recursiveContainerExtract(leftHandSide, elseChildren);
		vector<Relationship> recursiveIfRelations = handleIf(ast);

		modifiesRelationships.insert(modifiesRelationships.end(), toAddThen.begin(), toAddThen.end());
		modifiesRelationships.insert(modifiesRelationships.end(), toAddElse.begin(), toAddElse.end());
		modifiesRelationships.insert(modifiesRelationships.end(), recursiveIfRelations.begin(), recursiveIfRelations.end());
		break;
	}
	default:
	{
		// Iterate through child nodes calling this function recursively
		vector<shared_ptr<ASTNode>> children = ast->getChildren();
		for (int i = 0; i < children.size(); i++) {
			shared_ptr<ASTNode> child = children[i];
			vector<Relationship> extractedModifies = recursiveContainerExtract(leftHandSide, child);
			modifiesRelationships.insert(modifiesRelationships.end(), extractedModifies.begin(), extractedModifies.end());
		}
	}
	}
	return modifiesRelationships;
}
