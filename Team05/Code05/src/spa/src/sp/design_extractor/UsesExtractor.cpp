#include <vector>
#include <memory>
#include <sp/dataclasses/ast/AST.h>
#include <sp/dataclasses/ast/AssignASTNode.h>
#include <sp/dataclasses/ast/ExpressionASTNode.h>
#include <sp/dataclasses/ast/WhileASTNode.h>
#include <sp/dataclasses/ast/IfASTNode.h>
#include <sp/design_extractor/UsesExtractor.h>

vector<Relationship> UsesExtractor::extract(shared_ptr<ASTNode> ast) {

	vector<Relationship> uses = vector<Relationship>();
	ASTNodeType type = ast->getType();

	switch (type) {
	case ASTNodeType::PROCEDURE:
	{
		vector<Relationship> extractedUses = this->handleProcedure(ast);
		uses.insert(uses.end(), extractedUses.begin(), extractedUses.end());
		break;
	}
	case ASTNodeType::ASSIGN:
	{
		vector<Relationship> extractedUses = this->handleAssign(ast);
		uses.insert(uses.end(), extractedUses.begin(), extractedUses.end());
		break;
	}
	case ASTNodeType::PRINT:
	{
		vector<Relationship> extractedUses = this->handlePrint(ast);
		uses.insert(uses.end(), extractedUses.begin(), extractedUses.end());
		break;
	}
	case ASTNodeType::WHILE:
	{
		vector<Relationship> extractedUses = this->handleWhile(ast);
		uses.insert(uses.end(), extractedUses.begin(), extractedUses.end());
		break;
	}
	case ASTNodeType::IF:
	{
		vector<Relationship> extractedUses = this->handleIf(ast);
		uses.insert(uses.end(), extractedUses.begin(), extractedUses.end());
		break;
	}
	case ASTNodeType::CALL:
	{
		vector<Relationship> extractedUses = this->handleCall(ast);
		uses.insert(uses.end(), extractedUses.begin(), extractedUses.end());
		break;
	}
	default:
		vector<shared_ptr<ASTNode>> children = ast->getChildren();
		for (int i = 0; i < children.size(); i++) {
			shared_ptr<ASTNode> child = children[i];
			vector<Relationship> extractedRelationships = this->extract(child);
			uses.insert(uses.end(), extractedRelationships.begin(), extractedRelationships.end());
		}
	}
	return uses;
}

// TODO in a future iteration
vector<Relationship> UsesExtractor::handleProcedure(shared_ptr<ASTNode> ast) {
	Entity leftHandSide = ast->extractEntity();

	vector<Relationship> extractedChildRelationships = recursiveContainerExtract(leftHandSide, ast);

	return extractedChildRelationships;
}

vector<Relationship> UsesExtractor::handleAssign(shared_ptr<ASTNode> ast) {
	vector<Relationship> usesRelationships = vector<Relationship>();

	// This is the Right hand side of the assign relation
	shared_ptr<AssignASTNode> assignNode = dynamic_pointer_cast<AssignASTNode>(ast);
	shared_ptr<ASTNode> rightChild = assignNode->getRightHandSide();

	vector<Entity> extractedVariables = extractVariables(rightChild);
	Entity leftHandSide = assignNode->extractEntity();

	for (int i = 0; i < extractedVariables.size(); i++) {

		Entity usedVariable = extractedVariables[i];
		usesRelationships.push_back(Relationship{ leftHandSide, usedVariable, RelationshipType::USES });
	}

	return usesRelationships;
}

vector<Relationship> UsesExtractor::handlePrint(shared_ptr<ASTNode> ast) {
	return vector<Relationship>();
}

vector<Relationship> UsesExtractor::handleWhile(shared_ptr<ASTNode> ast) {
	return vector<Relationship>();
}

vector<Relationship> UsesExtractor::handleIf(shared_ptr<ASTNode> ast) {
	return vector<Relationship>();
}

// TODO in a future iteration
vector<Relationship> UsesExtractor::handleCall(shared_ptr<ASTNode> ast) {
	return vector<Relationship>();
}

vector<Entity> UsesExtractor::extractVariables(shared_ptr<ASTNode> ast) {
	vector<Entity> variables = vector<Entity>();

	if (ast->getType() == ASTNodeType::VARIABLE) {
		// Extract the variable and add it to the result.
		Entity variable = ast->extractEntity();
		variables.push_back(variable);
	}
	else if (ast->getType() == ASTNodeType::EXPRESSION) {
		// Recursively extract the variables in the sub-expression.
		shared_ptr<ExpressionASTNode> expressionNode = dynamic_pointer_cast<ExpressionASTNode>(ast);
		shared_ptr<ASTNode> leftChild = expressionNode->getLeftHandSide();
		shared_ptr<ASTNode> rightChild = expressionNode->getRightHandSide();

		vector<Entity> variablesInLhs = extractVariables(leftChild);
		variables.insert(variables.end(), variablesInLhs.begin(), variablesInLhs.end());

		vector<Entity> variablesInRhs = extractVariables(rightChild);
		variables.insert(variables.end(), variablesInRhs.begin(), variablesInRhs.end());
	}
	return variables;
}

vector<Relationship> UsesExtractor::recursiveContainerExtract(Entity& leftHandSide, shared_ptr<ASTNode> ast) {
	return vector<Relationship>();
}