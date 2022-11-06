#pragma once

#include <assert.h>

#include <sp/dataclasses/ast/AST.h>
#include <sp/dataclasses/ast/ProcedureASTNode.h>
#include <sp/dataclasses/ast/exceptions/ASTException.h>
#include <sp/dataclasses/design_objects/Entity.h>
#include <sp/dataclasses/design_objects/Relationship.h>
#include <sp/design_extractor/Extractor.h>

#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

using namespace std;


class CallsAndCallsTExtractor : public Extractor<Relationship> {
private:
    const string DELIMITER = ":";

    // Used to check for repeated procedure names. The procedure name is used as the key.
    unordered_map<string, shared_ptr<ASTNode>> extractedProcedures;

    // Used to avoid repeats.
    unordered_set<string> extractedCalls;

    // Used to check for cyclic calls and to extract callsT relationships.
    // The procedure name is used as the key.
    unordered_map<string, vector<shared_ptr<ASTNode>>> callsGraph;
    bool initializedCallGraph = false;

    // Extracted relationships
    vector<Relationship> relationships;

    vector<Relationship> depthFirstExtract();

    vector<Relationship> recursiveExtractCallsT(string procedureName,
        vector<shared_ptr<ASTNode>> visitedProcedures, vector<shared_ptr<ASTNode>> toAdd);

public:
    /*
        This method is used to extract relationships from a provided abstract syntax tree.
        It is meant to be overriden for each specific relationship type.
    */
    vector<Relationship> extract(shared_ptr<ASTNode> rootnode) override;

    /*
        Construct calls graph
    */
    void constructCallsGraph(shared_ptr<ASTNode> rootNode);

    /*
        Encapsulate logic for processing a procedure node
    */
    void handleProcedure(shared_ptr<ASTNode> procedureNode);


    /*
        Encapsulate logic for processing a statement node
    */
    void handleStatementList(shared_ptr<ProcedureASTNode> callee,
        vector<shared_ptr<ASTNode>> children);

    /*
		Extracts CallT from the CallGraph and checks for cycles as well.
	*/
    void extractCallsT();
};
