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
#include <sp/dataclasses/ast/CallASTNode.h>
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
		vector<Relationship> extractedModifies = this->handleCall(ast);
		modifies.insert(modifies.end(), extractedModifies.begin(), extractedModifies.end());
		break;
	}

	if (ast->hasContainer()) {
		vector<shared_ptr<ASTNode>> children = ast->getChildren();

		// If program node, we look ahead and store the procedures in the program
		if (ast->isProgramNode()) {
			for (int i = 0; i < children.size(); i++) {
				shared_ptr<ASTNode> child = children[i];
				assert(child->isProcedureNode());

				/* 
					We look ahead from root program node and add all procedures to
					extract on demand
				*/
				allProcedures.push_back(child);
			}
		}

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

	// if entry already exists in DP map, just return the mapped relationships
	string procedureName = ast->extractEntity().getString();

	// checks if procedure name is already in entry
	if (procedureNameToRelationshipMap.find(procedureName) != procedureNameToRelationshipMap.end()) {
		return procedureNameToRelationshipMap.at(procedureName);
	}


	Entity leftHandSide = ast->extractEntity();

	vector<Relationship> extractedChildRelationships;
	shared_ptr<ProcedureASTNode> procedureNode = dynamic_pointer_cast<ProcedureASTNode>(ast);
	shared_ptr<ASTNode> childContainer = procedureNode->getStmtList();

	vector<Relationship> collectedRelationships;

	// Iterate through children and extract Procedure Stmt relationships
	for (shared_ptr<ASTNode> child : childContainer->getChildren()) {
		vector <Relationship> extractedRelationships = recursiveContainerExtract(leftHandSide, child);
		extractedChildRelationships.insert(extractedChildRelationships.end(), extractedRelationships.begin(), extractedRelationships.end());
	}

	// Update procedureToModifiedVariablesMap for DP purposes so future calls can refer to the result
	procedureNameToRelationshipMap.insert(make_pair(procedureName, extractedChildRelationships));

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

vector<Relationship> ModifiesExtractor::handleCall(shared_ptr<ASTNode> ast) {
	ASTNodeType currNodeType = ast->getType();
	if (currNodeType != ASTNodeType::CALL) {
		throw ASTException("handleCall can only accept call AST nodes");
	}

	vector<Relationship> extractedRelationships;

	// We check that procedure call only has one child, that is the procedure it is calling
	assert(ast->getChildren().size() == 1);
	shared_ptr<ASTNode> procedureCalled = ast->getChildren()[0];

	assert(procedureCalled->isProcedureNode());
	string procedureCalledName = procedureCalled->extractEntity().getString();

	// checks if procedure name is already in DP entry
	if (procedureNameToRelationshipMap.find(procedureCalledName) != procedureNameToRelationshipMap.end()) {
		vector<Relationship> procedureCalledRelationships = procedureNameToRelationshipMap.at(procedureCalledName);
		
		// get the call entity in question
		Entity leftHandSide = ast->extractEntity();
		assert(leftHandSide.getType() == EntityType::CALL);

		/*
			The DP relationships are in the form of procedure p : variable v
			Take variable v (from rhs) and create relationship call c : variable v
		*/
		for (int i = 0; i < procedureCalledRelationships.size(); i++) {
			Relationship currRelation = procedureCalledRelationships[i];
			Entity rightHandSide = currRelation.getRhs();
			assert(rightHandSide.getType() == EntityType::VARIABLE);

			Relationship toAdd = Relationship::createModifiesRelationship(leftHandSide, rightHandSide);
			extractedRelationships.push_back(toAdd);
		}
		return extractedRelationships;
	}
	else {
		/*
			Procedures from root program node added to allProcedures vector

			We find the name of the called procedure, and string match it to the corect index in 
			allProcedures
			
			e.g.

						program
					   /      \
				 proc:first  proc:second
					/           \
				  stmtLst       ...
				  /
				 call
				 /
			proc:second
			    /
			empty stmtLst

			allProcedures: vect {index 0: first, index 1: second}
			In this case, the correct index is 1
			We need to do this because both proc:second AST nodes share the same name, but are different nodes (i.e. diff children and parents)

		*/
		int index = -1;
		for (int i = 0; i < allProcedures.size(); i++) {
			shared_ptr<ASTNode> currProcedure = allProcedures[i];
			assert(currProcedure->isProcedureNode());

			string currProcedureName = currProcedure->extractEntity().getString();

			if (currProcedureName == procedureCalledName) {
				index = i;
			}
		}

		if (index == -1) {
			throw ASTException("Could not find name of procedure called in program");
		}

		// Now we know the index of the called procedure in allProcedures
		shared_ptr<ASTNode> procedureToExtract = allProcedures[index];

		// We use handleProcedure() to get relationships if entries are not in DP map
		vector<Relationship> procedureCalledRelationships = this->handleProcedure(procedureToExtract);
		
		// Get the call entity in question
		Entity leftHandSide = ast->extractEntity();
		assert(leftHandSide.getType() == EntityType::CALL);

		/*
			The DP relationships are in the form of procedure p : variable v
			Take variable v (from rhs) and create relationship call c : variable v
		*/
		for (int i = 0; i < procedureCalledRelationships.size(); i++) {
			Relationship currRelation = procedureCalledRelationships[i];
			Entity rightHandSide = currRelation.getRhs();
			assert(rightHandSide.getType() == EntityType::VARIABLE);

			Relationship toAdd = Relationship::createModifiesRelationship(leftHandSide, rightHandSide);
			extractedRelationships.push_back(toAdd);
		}
		return extractedRelationships;
	}
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
	case ASTNodeType::CALL: { // helps handle indirect procedure call (e.g. A calls B which calls C)
		// Cast CallASTNode
		shared_ptr<CallASTNode> callNode = dynamic_pointer_cast<CallASTNode>(ast);
		
		// Get referenced procedure
		assert(callNode->getChildren().size() == 1);
		shared_ptr<ASTNode> calledProcedure = callNode->getChildren()[0];
		string calledProcName = calledProcedure->extractEntity().getString();

		/*
			Procedures from root program node added to allProcedures vector

			We find the name of the called procedure, and string match it to the corect index in
			allProcedures

			e.g.

						program
						/      \
					proc:first  proc:second
					/           \
					stmtLst       ...
					/
					call
					/
			proc:second
				/
			empty stmtLst

			allProcedures: vect {index 0: first, index 1: second}
			In this case, the correct index is 1.
			We need to do this because both proc:second AST nodes share the same name, but are different nodes (i.e. diff children and parents)
		*/
		int index = -1;
		for (int i = 0; i < allProcedures.size(); i++) {
			shared_ptr<ASTNode> currProcedure = allProcedures[i];
			assert(currProcedure->isProcedureNode());

			string currProcedureName = currProcedure->extractEntity().getString();

			// once we find the index of the referenced procedure in allProcedures, we can extract the procedure AST node
			if (currProcedureName == calledProcName) {
				index = i;
			}
		}

		if (index == -1) {
			throw ASTException("Could not find name of procedure called in program");
		}

		// Now we know the index of the called procedure in allProcedures
		shared_ptr<ASTNode> procedureToExtract = allProcedures[index];

		// Type-cast procedureToExtract to ProcedureASTNode
		shared_ptr<ProcedureASTNode> calledProcNode = dynamic_pointer_cast<ProcedureASTNode>(procedureToExtract);

		shared_ptr<ASTNode> childrenStmtLst = calledProcNode->getStmtList();

		for (shared_ptr<ASTNode> child : childrenStmtLst->getChildren()) {
			vector<Relationship> toAdd = recursiveContainerExtract(leftHandSide, child);
			modifiesRelationships.insert(modifiesRelationships.end(), toAdd.begin(), toAdd.end());

		}
	}
	}
	return modifiesRelationships;
}
