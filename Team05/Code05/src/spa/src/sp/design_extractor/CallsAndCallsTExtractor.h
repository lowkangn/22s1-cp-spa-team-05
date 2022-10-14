#pragma once

#include <vector>
#include <sp/dataclasses/ast/exceptions/ASTException.h>
#include <sp/dataclasses/design_objects/Relationship.h>
#include <sp/design_extractor/Extractor.h>
#include <sp/dataclasses/ast/AST.h>
#include <memory>
#include <unordered_map>
#include <unordered_set>

using namespace std;


class CallsAndCallsTExtractor : public Extractor<Relationship> {

private:
	const string DELIMITER = ":";

	// Used to check for repeated procedure names. The procedure name is used as the key.
	unordered_map<string, Entity> extractedProcedures;
	
	// Used to avoid repeats.
	unordered_set<string> extractedCalls;

	// Used to check for cyclic calls and to extract callsT relationships. The procedure name is used as the key.
	unordered_map<string, vector<Entity>> callsGraph;

	/*
	Traverses the graph to extract recursive callsT relationships and to check for cycles.
	If a cyclic call is detected, throws a semantic error.
	*/
	vector<Relationship> extractCallsT() {
		vector<Relationship> recursiveCallsT = vector<Relationship>{};

		for (pair<string, vector<Entity>> const& keyValuePair : this->callsGraph) {
			string procedureName = keyValuePair.first;
			vector<Entity> children = keyValuePair.second;

			Entity procedure = this->extractedProcedures.at(procedureName);

			// Traverse the procedures that the selected procedure calls.
			for (Entity child : children) {
				vector<Relationship> recursivelyExtracted = this->extractCallsTHelper(procedure, child);
				recursiveCallsT.insert(recursiveCallsT.end(), recursivelyExtracted.begin(), recursivelyExtracted.end());
			}
		}
		return recursiveCallsT;
	}

	vector<Relationship> extractCallsTHelper(Entity& leftHandSide, Entity& procedureCalledByLhs) {
		vector<Relationship> recursiveCallsT = vector<Relationship>{};
		string procedureName = procedureCalledByLhs.getString();

		// If the called procedure also calls any other procedure, form and extract the CallsT relationships.
		for (Entity& child : this->callsGraph[procedureName]) {
			/*
			If the LHS is visited again, that means a cyclic call exists in the program.
			Throw a semantic error.
			*/
			if (child == leftHandSide) {
				throw ASTException("The program contains cyclic procedure calls! This is not allowed");
			}

			string callerCalleeString = leftHandSide.getString() + DELIMITER + child.getString();

			// If relationship was extracted previously, ignore it and return. This path was already traversed before.
			if (this->extractedCalls.find(callerCalleeString) != this->extractedCalls.end()) {
				return recursiveCallsT;
			}
			else {
				Relationship toAdd = Relationship::createCallsTRelationship(leftHandSide, child);
				recursiveCallsT.push_back(toAdd);

				this->extractedCalls.insert(callerCalleeString);

				vector<Relationship> recursivelyExtracted = this->extractCallsTHelper(leftHandSide, child);
				recursiveCallsT.insert(recursiveCallsT.end(), recursivelyExtracted.begin(), recursivelyExtracted.end());
			}
		}
		return recursiveCallsT;
	}

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
