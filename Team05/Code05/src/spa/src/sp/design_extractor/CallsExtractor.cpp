// imported libraries
#include <vector>
#include <memory>
#include <assert.h>

// imported locally
#include <sp/dataclasses/ast/AST.h>
#include <sp/dataclasses/ast/exceptions/ASTException.h>
#include <sp/dataclasses/ast/ProcedureASTNode.h>
#include <sp/dataclasses/ast/ProgramASTNode.h>
#include <sp/design_extractor/CallsExtractor.h>

vector<Relationship> CallsExtractor::extract(shared_ptr<ASTNode> ast) {
	vector<Relationship> calls = vector<Relationship>();
	ASTNodeType type = ast->getType();


	switch (type) {
	case ASTNodeType::PROGRAM:
	{
		vector<Relationship> extractedCalls = this->handleProgram(ast);
		calls.insert(calls.end(), extractedCalls.begin(), extractedCalls.end());
	}
	case ASTNodeType::PROCEDURE:

	}
	return calls;
}

vector<Relationship> CallsExtractor::handleProgram(shared_ptr<ASTNode> ast) {
	vector<Relationship> callsRelationships = vector<Relationship>();

	// Sanity check to prevent erroneous calls outside of extract() since handleProgram is public 
	ASTNodeType currNodeType = ast->getType();
	if (currNodeType != ASTNodeType::PROGRAM) {
		throw ASTException("handleProgram can only accept program AST nodes");
	}

	shared_ptr<ProgramASTNode> programNode = dynamic_pointer_cast<ProgramASTNode>(ast);
	vector<shared_ptr<ASTNode>> children = programNode->getChildren();

	// Check for any repeated declarations of procedure name.
	for (int i = 0; i < children.size(); i++) {
		Entity procedure = children[i]->extractEntity();
		string procedureName = procedure.getString();

		// Check if the procedure name was not already defined before.
		if (this->extractedProcedures.find(procedureName) == this->extractedProcedures.end()) {
			this->extractedProcedures.insert(procedureName);
		}
		else {
			throw ASTException("Procedure " + procedureName + " was declared twice in the program.");
		}
	}

	// Extract calls relationships from the procedures in the program.
	for (int i = 0; i < children.size(); i++) {
		vector<Relationship> extractedCalls = this->extract(children[i]);
		callsRelationships.insert(callsRelationships.end(), extractedCalls.begin(), extractedCalls.end());
	}
	return callsRelationships;
}
