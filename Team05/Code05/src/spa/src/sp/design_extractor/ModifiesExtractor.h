#pragma once
#include <vector>
#include <sp/dataclasses/design_objects/Relationship.h>
#include <sp/design_extractor/Extractor.h>
#include <sp/dataclasses/ast/AST.h>
#include <memory>
using namespace std;

class ModifiesExtractor : public Extractor<Relationship> {

public:
	/*
		This method is used to extract entities from a provided abstract syntax tree. It is meant to be
		overriden for each specific entity type.
	*/
	vector<Relationship> extract(shared_ptr<ASTNode> ast) override;

	/*
		This method extracts a modifies relationship from an Procedure node
	*/
	vector<Relationship> handleProcedure(shared_ptr<ASTNode> ast);

	/*
		This method extracts a modifies relationship from an Assign node
	*/
	vector<Relationship> handleAssign(shared_ptr<ASTNode> ast);

	/*
	This method extracts a modifies relationship from an Read node
	*/
	vector<Relationship> handleRead(shared_ptr<ASTNode> ast);

	/*
		This method extracts a modifies relationship from an While node
	*/
	vector<Relationship> handleWhile(shared_ptr<ASTNode> ast);

	/*
		This method extracts a modifies relationship from an If node
	*/
	vector<Relationship> handleIf(shared_ptr<ASTNode> ast);

	/*
		This method extracts a modifies relationship from an call node
	*/
	vector<Relationship> handleCall(shared_ptr<ASTNode> ast);

	/*
		This method recursively traverses the node adding the Modifies relationship for procedures
	*/
	vector<Relationship> recursiveContainerExtract(Entity& LHS, shared_ptr<ASTNode> ast);
};
