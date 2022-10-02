#pragma once

#include <vector>
#include <sp/dataclasses/design_objects/Relationship.h>
#include <sp/design_extractor/Extractor.h>
#include <sp/dataclasses/ast/AST.h>
#include <memory>
#include <unordered_set>

using namespace std;

class CallsExtractor : public Extractor<Relationship> {

private:
	// Used to check for repeated procedure names.
	unordered_set<string> extractedProcedures;
	
	// Used to check for simple cyclic calls and to avoid repeats.
	unordered_set<string> extractedCalls;

public:
	/*
		This method is used to extract relationships from a provided abstract syntax tree. It is meant to be
		overriden for each specific relationship type.
	*/
	vector<Relationship> extract(shared_ptr<ASTNode> rootnode) override;

	/*
		This method extracts calls relationships from an Program node
	*/
	vector<Relationship> handleProgram(shared_ptr<ASTNode> ast);

	/*
		This method recursively extracts calls relationships from container statements.
	*/
	vector<Relationship> recursiveContainerExtract(Entity& leftHandSide, shared_ptr<ASTNode> ast);
};
