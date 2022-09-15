#include <vector>
#include <memory>
#include <sp/dataclasses/ast/AST.h>
#include <sp/dataclasses/ast/AssignASTNode.h>
#include <sp/dataclasses/ast/ExpressionASTNode.h>
#include <sp/dataclasses/ast/IfASTNode.h>
#include <sp/dataclasses/ast/PrintASTNode.h>
#include <sp/dataclasses/ast/WhileASTNode.h>
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

	addRelationshipsWithVariables(usesRelationships, leftHandSide, extractedVariables);

	return usesRelationships;
}

vector<Relationship> UsesExtractor::handlePrint(shared_ptr<ASTNode> ast) {
	shared_ptr<PrintASTNode> printNode = dynamic_pointer_cast<PrintASTNode>(ast);

	Entity printEntity = printNode->extractEntity();
	Entity printedVariable = printNode->getVariableToPrintNode()->extractEntity();
	return vector<Relationship>{ Relationship{ printEntity, printedVariable, RelationshipType::USES } };
}

vector<Relationship> UsesExtractor::handleWhile(shared_ptr<ASTNode> ast) {
	vector<Relationship> usesRelationships = vector<Relationship>();

	shared_ptr<WhileASTNode> whileNode = dynamic_pointer_cast<WhileASTNode>(ast);
	shared_ptr<ASTNode> child = whileNode->getStmtList();

	Entity leftHandSide = whileNode->extractEntity();

	vector<Entity> whileConditionVariables = extractVariables(whileNode->getCondition());
	addRelationshipsWithVariables(usesRelationships, leftHandSide, whileConditionVariables);

	// Recursively extract relations from the while block
	vector<Relationship> extractedChildRelationships = recursiveContainerExtract(leftHandSide, child);

	usesRelationships.insert(usesRelationships.end(), extractedChildRelationships.begin(), extractedChildRelationships.end());

	return usesRelationships;
}

vector<Relationship> UsesExtractor::handleIf(shared_ptr<ASTNode> ast) {
	vector<Relationship> usesRelationships = vector<Relationship>();

	shared_ptr<IfASTNode> ifASTNode = dynamic_pointer_cast<IfASTNode>(ast);
	shared_ptr<ASTNode> condition = ifASTNode->getCondition();
	shared_ptr<ASTNode> thenChild = ifASTNode->getThenStatements();
	shared_ptr<ASTNode> elseChild = ifASTNode->getElseStatements();

	Entity leftHandSide = ifASTNode->extractEntity();

	vector<Entity> conditionVariables = extractVariables(condition);
	addRelationshipsWithVariables(usesRelationships, leftHandSide, conditionVariables);

	// Recursively extract relations from Then and else containers
	vector<Relationship> extractedThenChildRelationships = recursiveContainerExtract(leftHandSide, thenChild);
	vector<Relationship> extractedElseChildRelationships = recursiveContainerExtract(leftHandSide, elseChild);

	usesRelationships.insert(usesRelationships.end(), extractedThenChildRelationships.begin(), extractedThenChildRelationships.end());
	usesRelationships.insert(usesRelationships.end(), extractedElseChildRelationships.begin(), extractedElseChildRelationships.end());

	return usesRelationships;
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

void UsesExtractor::addRelationshipsWithVariables(vector<Relationship>& relationships, Entity& LHS, vector<Entity>& variables) {
	for (int i = 0; i < variables.size(); i++) {
		Relationship toAdd = Relationship{ LHS, variables[i], RelationshipType::USES };
		relationships.push_back(toAdd);
	}
}

vector<Relationship> UsesExtractor::recursiveContainerExtract(Entity& LHS, shared_ptr<ASTNode> ast) {
	vector<Relationship> usesRelationships = vector<Relationship>();
	ASTNodeType type = ast->getType();

	switch (type) {
	case ASTNodeType::ASSIGN:
	{
		// Cast to AssignASTNode
		shared_ptr<AssignASTNode> assignNode = dynamic_pointer_cast<AssignASTNode>(ast);
		shared_ptr<ASTNode> rightChild = assignNode->getRightHandSide();

		vector<Relationship> assignRelationship = handleAssign(ast);

		vector<Entity> extractedVariables = extractVariables(rightChild);
		addRelationshipsWithVariables(usesRelationships, LHS, extractedVariables);

		usesRelationships.insert(usesRelationships.end(), assignRelationship.begin(), assignRelationship.end());
		break;
	}
	case ASTNodeType::PRINT:
	{
		// Cast to PrintASTNode
		shared_ptr<PrintASTNode> printNode = dynamic_pointer_cast<PrintASTNode>(ast);
		shared_ptr<ASTNode> child = printNode->getVariableToPrintNode();

		vector<Relationship> printRelationship = handlePrint(ast);

		Entity childEntity = child->extractEntity();
		Relationship toAdd = Relationship{ LHS, childEntity, RelationshipType::USES };

		usesRelationships.push_back(toAdd);
		usesRelationships.insert(usesRelationships.end(), printRelationship.begin(), printRelationship.end());
		break;
	}
	case ASTNodeType::WHILE:
	{
		// Cast to WhileASTNode
		shared_ptr<WhileASTNode> whileNode = dynamic_pointer_cast<WhileASTNode>(ast);
		shared_ptr<ASTNode> children = whileNode->getStmtList();

		vector<Entity> whileConditionVariables = extractVariables(whileNode->getCondition());
		addRelationshipsWithVariables(usesRelationships, LHS, whileConditionVariables);

		vector<Relationship> recursiveExtract = recursiveContainerExtract(LHS, children);
		vector<Relationship> recursiveWhile = handleWhile(ast);

		usesRelationships.insert(usesRelationships.end(), recursiveExtract.begin(), recursiveExtract.end());
		usesRelationships.insert(usesRelationships.end(), recursiveWhile.begin(), recursiveWhile.end());
		break;
	}
	case ASTNodeType::IF:
	{
		// Cast IfASTNode
		shared_ptr<IfASTNode> ifNode = dynamic_pointer_cast<IfASTNode>(ast);
		shared_ptr<ASTNode> condition = ifNode->getCondition();
		shared_ptr<ASTNode> thenChildren = ifNode->getThenStatements();
		shared_ptr<ASTNode> elseChildren = ifNode->getElseStatements();

		vector<Entity> conditionVariables = extractVariables(condition);
		addRelationshipsWithVariables(usesRelationships, LHS, conditionVariables);

		vector<Relationship> toAddThen = recursiveContainerExtract(LHS, thenChildren);
		vector<Relationship> toAddElse = recursiveContainerExtract(LHS, elseChildren);
		vector<Relationship> recursiveIfRelations = handleIf(ast);

		usesRelationships.insert(usesRelationships.end(), toAddThen.begin(), toAddThen.end());
		usesRelationships.insert(usesRelationships.end(), toAddElse.begin(), toAddElse.end());
		usesRelationships.insert(usesRelationships.end(), recursiveIfRelations.begin(), recursiveIfRelations.end());
		break;
	}
	default:
	{
		// Iterate through child nodes calling this function recursively
		vector<shared_ptr<ASTNode>> children = ast->getChildren();
		for (int i = 0; i < children.size(); i++) {
			shared_ptr<ASTNode> child = children[i];
			vector<Relationship> extractedUses = recursiveContainerExtract(LHS, child);
			usesRelationships.insert(usesRelationships.end(), extractedUses.begin(), extractedUses.end());
		}
	}
	}
	return usesRelationships;
}
