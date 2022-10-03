// imported libraries
#include <vector>
#include <memory>
#include <assert.h>

// imported locally
#include <sp/dataclasses/ast/AST.h>
#include <sp/dataclasses/ast/exceptions/ASTException.h>
#include <sp/dataclasses/ast/CallASTNode.h>
#include <sp/dataclasses/ast/IfASTNode.h>
#include <sp/dataclasses/ast/ProcedureASTNode.h>
#include <sp/dataclasses/ast/ProgramASTNode.h>
#include <sp/dataclasses/ast/WhileASTNode.h>
#include <sp/design_extractor/CallsAndCallsTExtractor.h>

vector<Relationship> CallsAndCallsTExtractor::extract(shared_ptr<ASTNode> ast) {
	vector<Relationship> calls = vector<Relationship>();
	ASTNodeType type = ast->getType();

	switch (type) {
	case ASTNodeType::PROGRAM:
	{
		vector<Relationship> extractedCalls = this->handleProgram(ast);
		calls.insert(calls.end(), extractedCalls.begin(), extractedCalls.end());
		break;
	}
	case ASTNodeType::PROCEDURE:
	{
		shared_ptr<ProcedureASTNode> procedureNode = dynamic_pointer_cast<ProcedureASTNode>(ast);

		Entity leftHandSide = procedureNode->extractEntity();
		shared_ptr<ASTNode> childContainer = procedureNode->getStmtList();

		for (shared_ptr<ASTNode> child : childContainer->getChildren()) {
			vector <Relationship> extractedCalls = recursiveContainerExtract(leftHandSide, child);
			calls.insert(calls.end(), extractedCalls.begin(), extractedCalls.end());
		}
		break;
	}
	}
	vector<Relationship> recursiveCallsT = this->extractCallsT();
	calls.insert(calls.end(),recursiveCallsT.begin(),recursiveCallsT.end());

	return calls;
}

vector<Relationship> CallsAndCallsTExtractor::handleProgram(shared_ptr<ASTNode> ast) {
	vector<Relationship> callsRelationships = vector<Relationship>();

	// Sanity check to prevent erroneous calls outside of extract() since handleProgram is public 
	ASTNodeType currNodeType = ast->getType();
	if (currNodeType != ASTNodeType::PROGRAM) {
		throw ASTException("handleProgram can only accept program AST nodes");
	}

	shared_ptr<ProgramASTNode> programNode = dynamic_pointer_cast<ProgramASTNode>(ast);
	vector<shared_ptr<ASTNode>> children = programNode->getChildren();

	// Check for any repeated declarations of procedure name.
	for (shared_ptr<ASTNode> child : children) {
		Entity procedure = child->extractEntity();
		string procedureName = procedure.getString();

		// Check if the procedure name was not already defined before.
		if (this->extractedProcedures.find(procedureName) == this->extractedProcedures.end()) {
			this->extractedProcedures.insert({ procedureName, procedure });
		}
		else {
			throw ASTException("Procedure " + procedureName + " was declared twice in the program");
		}
	}

	// Extract calls relationships from the procedures in the program.
	for (shared_ptr<ASTNode> child : children) {
		vector<Relationship> extractedCalls = this->extract(child);
		callsRelationships.insert(callsRelationships.end(), extractedCalls.begin(), extractedCalls.end());
	}
	return callsRelationships;
}

vector<Relationship> CallsAndCallsTExtractor::recursiveContainerExtract(Entity& leftHandSide, shared_ptr<ASTNode> ast) {
	vector<Relationship> callsRelationships = vector<Relationship>();
	ASTNodeType type = ast->getType();

	switch (type) {
	case ASTNodeType::CALL:
	{
		shared_ptr<CallASTNode> callNode = dynamic_pointer_cast<CallASTNode>(ast);
		Entity procedureCalled = callNode->getProcedureName()->extractEntity();
		string caller = leftHandSide.getString();
		string callee = procedureCalled.getString();

		if (caller == callee) {
			throw ASTException("A procedure is calling itself!");
		}
		else if (this->extractedProcedures.find(callee) == this->extractedProcedures.end()) {
			throw ASTException("Trying to call a non-existent procedure " + callee);
		}
		else {
			string callerCalleeString = caller + callee;

			// If this calls relationship was not extracted previously, extract and add it to the extracted relationships.
			if (this->extractedCalls.find(callerCalleeString) == this->extractedCalls.end()) {
				this->extractedCalls.insert(callerCalleeString);
				callsRelationships.push_back(Relationship::createCallsRelationship(leftHandSide, procedureCalled));
				callsRelationships.push_back(Relationship::createCallsTRelationship(leftHandSide, procedureCalled));

				// If this procedure was not added to the graph before, create a new entry.
				if (this->callsGraph.find(caller) == this->callsGraph.end()) {
					vector<Entity> children = { procedureCalled };
					this->callsGraph.insert({ caller, children });
				}
				else {
					// Add on to the existing children.
					this->callsGraph.at(caller).push_back(procedureCalled);
				}
			}
		}
		break;
	}
	case ASTNodeType::IF:
	{
		shared_ptr<IfASTNode> ifNode = dynamic_pointer_cast<IfASTNode>(ast);
		shared_ptr<ASTNode> thenChild = ifNode->getThenStatements();
		shared_ptr<ASTNode> elseChild = ifNode->getElseStatements();

		for (shared_ptr<ASTNode> child : thenChild->getChildren()) {
			vector <Relationship> extractedThenRelationships = recursiveContainerExtract(leftHandSide, child);
			callsRelationships.insert(callsRelationships.end(), extractedThenRelationships.begin(), extractedThenRelationships.end());
		}

		for (shared_ptr<ASTNode> child : elseChild->getChildren()) {
			vector <Relationship> extractedElseRelationships = recursiveContainerExtract(leftHandSide, child);
			callsRelationships.insert(callsRelationships.end(), extractedElseRelationships.begin(), extractedElseRelationships.end());
		}
		break;
	}
	case ASTNodeType::WHILE:
	{
		shared_ptr<WhileASTNode> whileNode = dynamic_pointer_cast<WhileASTNode>(ast);
		shared_ptr<ASTNode> childrenStmtLst = whileNode->getStmtList();

		for (shared_ptr<ASTNode> child : childrenStmtLst->getChildren()) {
			vector<Relationship> toAdd = recursiveContainerExtract(leftHandSide, child);
			callsRelationships.insert(callsRelationships.end(), toAdd.begin(), toAdd.end());
		}
		break;
	}
	}

	return callsRelationships;
}
