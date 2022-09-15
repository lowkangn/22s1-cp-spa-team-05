#include <vector>
#include <memory>
#include <sp/dataclasses/ast/AST.h>
#include <sp/dataclasses/ast/AssignASTNode.h>
#include <sp/dataclasses/ast/ExpressionASTNode.h>
#include <sp/dataclasses/ast/IfASTNode.h>
#include <sp/dataclasses/ast/PrintASTNode.h>
#include <sp/dataclasses/ast/ProcedureASTNode.h>
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
	}

	// If it is a container type we need to recursively extract children
	if (ast->hasContainer()) {
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
	return vector<Relationship>{ Relationship::createUsesRelationship(printEntity, printedVariable) };
}

vector<Relationship> UsesExtractor::handleWhile(shared_ptr<ASTNode> ast) {
	vector<Relationship> usesRelationships = vector<Relationship>();

	shared_ptr<WhileASTNode> whileNode = dynamic_pointer_cast<WhileASTNode>(ast);
	shared_ptr<ASTNode> childStmtLst = whileNode->getStmtList();

	Entity leftHandSide = whileNode->extractEntity();

	vector<Entity> whileConditionVariables = extractVariables(whileNode->getCondition());
	addRelationshipsWithVariables(usesRelationships, leftHandSide, whileConditionVariables);

	// Iterate through children and extract While Stmt relationships
	for (shared_ptr<ASTNode> child : childStmtLst->getChildren()) {
		vector <Relationship> extractedRelationships = recursiveContainerExtract(leftHandSide, child);
		usesRelationships.insert(usesRelationships.end(), extractedRelationships.begin(), extractedRelationships.end());
	}

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

	// Iterate through children and extract If Stmt relationships
	vector<shared_ptr<ASTNode>> childrenInThen = thenChild->getChildren();
	for (shared_ptr<ASTNode> child : childrenInThen) {
		vector <Relationship> extractedThenRelationships = recursiveContainerExtract(leftHandSide, child);
		usesRelationships.insert(usesRelationships.end(), extractedThenRelationships.begin(), extractedThenRelationships.end());
	}

	vector<shared_ptr<ASTNode>> childrenInElse = elseChild->getChildren();
	for (shared_ptr<ASTNode> child : childrenInElse) {
		vector <Relationship> extractedElseRelationships = recursiveContainerExtract(leftHandSide, child);
		usesRelationships.insert(usesRelationships.end(), extractedElseRelationships.begin(), extractedElseRelationships.end());
	}

	return usesRelationships;
}

// TODO in a future iteration
vector<Relationship> UsesExtractor::handleCall(shared_ptr<ASTNode> ast) {
	throw ASTException("Method not implemented!");
}

vector<Entity> UsesExtractor::extractVariables(shared_ptr<ASTNode> ast) {
	vector<Entity> variables = vector<Entity>();

	if (ast->isVariable()) {
		// Extract the variable and add it to the result.
		Entity variable = ast->extractEntity();
		variables.push_back(variable);
	}
	else if (ast->isExpression()) {
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

void UsesExtractor::addRelationshipsWithVariables(vector<Relationship>& relationships, Entity& leftHandSide, vector<Entity>& variables) {
	for (int i = 0; i < variables.size(); i++) {
		Relationship toAdd = Relationship::createUsesRelationship(leftHandSide, variables[i]);
		relationships.push_back(toAdd);
	}
}

vector<Relationship> UsesExtractor::recursiveContainerExtract(Entity& leftHandSide, shared_ptr<ASTNode> ast) {
	vector<Relationship> usesRelationships = vector<Relationship>();
	ASTNodeType type = ast->getType();

	switch (type) {
	case ASTNodeType::ASSIGN:
	{
		// Cast to AssignASTNode
		shared_ptr<AssignASTNode> assignNode = dynamic_pointer_cast<AssignASTNode>(ast);
		shared_ptr<ASTNode> rightChild = assignNode->getRightHandSide();

		vector<Entity> extractedVariables = extractVariables(rightChild);
		addRelationshipsWithVariables(usesRelationships, leftHandSide, extractedVariables);

		break;
	}
	case ASTNodeType::PRINT:
	{
		// Cast to PrintASTNode
		shared_ptr<PrintASTNode> printNode = dynamic_pointer_cast<PrintASTNode>(ast);
		shared_ptr<ASTNode> child = printNode->getVariableToPrintNode();

		Entity childEntity = child->extractEntity();
		Relationship toAdd = Relationship::createUsesRelationship(leftHandSide, childEntity);

		usesRelationships.push_back(toAdd);
		break;
	}
	case ASTNodeType::WHILE:
	{
		// Cast to WhileASTNode
		shared_ptr<WhileASTNode> whileNode = dynamic_pointer_cast<WhileASTNode>(ast);
		shared_ptr<ASTNode> condition = whileNode->getCondition();
		shared_ptr<ASTNode> childrenStmtLst = whileNode->getStmtList();
		
		vector<Entity> conditionVariables = extractVariables(condition);
		addRelationshipsWithVariables(usesRelationships, leftHandSide, conditionVariables);

		// Create relationship for parent of this container and the stmtlst inside this while node
		for (shared_ptr<ASTNode> child : childrenStmtLst->getChildren()) {
			vector<Relationship> toAdd = recursiveContainerExtract(leftHandSide, child);
			usesRelationships.insert(usesRelationships.end(), toAdd.begin(), toAdd.end());
		}
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
		addRelationshipsWithVariables(usesRelationships, leftHandSide, conditionVariables);

		// Create relationship for parent of this container and the then and else stmtlst inside this if node
		for (shared_ptr<ASTNode> child : thenChildren->getChildren()) {
			vector<Relationship> toAdd = recursiveContainerExtract(leftHandSide, child);
			usesRelationships.insert(usesRelationships.end(), toAdd.begin(), toAdd.end());
		}

		for (shared_ptr<ASTNode> child : elseChildren->getChildren()) {
			vector<Relationship> toAdd = recursiveContainerExtract(leftHandSide, child);
			usesRelationships.insert(usesRelationships.end(), toAdd.begin(), toAdd.end());
		}
		break;
	}
	}
	return usesRelationships;
}
