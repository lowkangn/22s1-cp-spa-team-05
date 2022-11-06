#include <assert.h>

#include <sp/dataclasses/ast/AST.h>
#include <sp/dataclasses/ast/CallASTNode.h>
#include <sp/dataclasses/ast/IfASTNode.h>
#include <sp/dataclasses/ast/ProcedureASTNode.h>
#include <sp/dataclasses/ast/ProgramASTNode.h>
#include <sp/dataclasses/ast/WhileASTNode.h>
#include <sp/dataclasses/ast/exceptions/ASTException.h>
#include <sp/design_extractor/CallsAndCallsTExtractor.h>

#include <memory>
#include <vector>

using namespace std;

vector<Relationship> CallsAndCallsTExtractor::extract(shared_ptr<ASTNode> ast) {
    vector<Relationship> extractedRelationships;

    this->constructCallsGraph(ast);

    vector<Relationship> extractedCallsT = this->depthFirstExtract();


    // Filter out duplicates
    for (Relationship relationship : this->relationships) {
        if (find(extractedRelationships.begin(),
            extractedRelationships.end(),
            relationship) == extractedRelationships.end()) {
            extractedRelationships.push_back(relationship);
        }
    }

    for (Relationship extractedRelation : extractedCallsT) {
        if (find(extractedRelationships.begin(),
            extractedRelationships.end(),
            extractedRelation) == extractedRelationships.end()) {
            extractedRelationships.push_back(extractedRelation);
        }
    }

    return extractedRelationships;
}

void CallsAndCallsTExtractor::constructCallsGraph(shared_ptr<ASTNode> ast) {
    // If the AST is a procedure, traverse its children.
    assert(ast->isProgramNode());

    shared_ptr<ProgramASTNode> programNode = dynamic_pointer_cast<ProgramASTNode>(ast);
    vector<shared_ptr<ASTNode>> children = programNode->getChildren();

    // Check for any repeated declarations of procedure name.
    for (shared_ptr<ASTNode> child : children) {
        Entity procedure = child->extractEntity();
        string procedureName = procedure.getString();

        // Check if the procedure name was not already defined before.
        if (this->extractedProcedures.find(procedureName) == this->extractedProcedures.end()) {
            this->extractedProcedures.insert({procedureName, child});
        } else {
            throw ASTException(
                "Procedure " + procedureName + " was declared twice in the program");
        }
    }

    for (shared_ptr<ASTNode> child : programNode->getChildren()) {
        this->handleProcedure(child);
    }

    this->initializedCallGraph = true;
}

void CallsAndCallsTExtractor::handleProcedure(shared_ptr<ASTNode> procedureNode) {
    assert(procedureNode->isProcedureNode());
    shared_ptr<ProcedureASTNode> procedure = dynamic_pointer_cast<ProcedureASTNode>(procedureNode);

    shared_ptr<ASTNode> procStmtList = procedure->getStmtList();

    vector<shared_ptr<ASTNode>> children = procStmtList->getChildren();

    this->handleStatementList(procedure, children);
}

void CallsAndCallsTExtractor::handleStatementList(
    shared_ptr<ProcedureASTNode> calle, vector<shared_ptr<ASTNode>> children) {
    string procedureName = calle->extractEntity().toString();
    for (shared_ptr<ASTNode> child : children) {
        if (child->isCallNode()) {
            shared_ptr<CallASTNode> caller = static_pointer_cast<CallASTNode>(child);
            shared_ptr<ASTNode> calledProc = caller->getProcedure();
            string calledProcName = calledProc->extractEntity().toString();
            this->callsGraph[procedureName].push_back(calledProc);

            // Add Call(Call stmt, proc) statement relationship if not added
            Relationship callRelationship = Relationship::createCallStmtAttributeRelationship(
                caller->extractEntity(),
                calledProc->extractEntity());
            this->relationships.push_back(callRelationship);

            // Add Call(proc, proc) Relationship
            // Procedure calling itself
            if (procedureName == calledProcName) {
                throw ASTException("A procedure is calling itself!");
            } else if (this->extractedProcedures.find(calledProcName)
                    == this->extractedProcedures. end()) {
                throw ASTException("Trying to call a non-existent procedure " + calledProcName);
            } else {
                Relationship toAdd = Relationship::createCallsRelationship(
                    calle->extractEntity(),
                    calledProc->extractEntity());
                this->relationships.push_back(toAdd);
            }
        } else if (child->isIfNode()) {
            shared_ptr<IfASTNode> ifNode = static_pointer_cast<IfASTNode>(child);
            this->handleStatementList(calle, ifNode->getThenStatements()->getChildren());
            this->handleStatementList(calle, ifNode->getElseStatements()->getChildren());
        } else if (child->isWhileNode()) {
            shared_ptr<WhileASTNode> whileNode = static_pointer_cast<WhileASTNode>(child);
            this->handleStatementList(calle, whileNode->getStmtList()->getChildren());
        }
    }
}

vector<Relationship> CallsAndCallsTExtractor::depthFirstExtract() {
    vector<Relationship> relationships;

    if (!this->initializedCallGraph) {
        throw ASTException("Call graph was not initialized");
    }

    for (auto it = this->callsGraph.begin(); it != this->callsGraph.end(); it++) {
        string procedureName = it->first;
        vector<shared_ptr<ASTNode>> calledProcedures = it->second;

        vector<Relationship> extractedCallsT = this->recursiveExtractCallsT(
            procedureName,
            {this->extractedProcedures[procedureName]},
            calledProcedures);

        for (Relationship relationship : extractedCallsT) {
            relationships.push_back(relationship);
        }
    }

    return relationships;
}

vector<Relationship> CallsAndCallsTExtractor::recursiveExtractCallsT(string procedureName,
    vector<shared_ptr<ASTNode>> visited, vector<shared_ptr<ASTNode>> toAdd) {
    vector<Relationship> extractedCallsT;

    // First create CallT for the immediate neighbours
    for (shared_ptr<ASTNode> calledProc : toAdd) {
        Entity called = calledProc->extractEntity();
        Entity callee = this->extractedProcedures[procedureName]->extractEntity();

        extractedCallsT.push_back(Relationship::createCallsTRelationship(callee, called));
        visited.push_back(calledProc);

        // Check if the callee has any neighbours
        if (this->callsGraph.find(called.toString()) != this->callsGraph.end()) {
            vector<shared_ptr<ASTNode>> neighbours = this->callsGraph[called.toString()];

            // Check if the callee has any neighbours that have not been visited
            vector<shared_ptr<ASTNode>> unvisitedNeighbours;
            for (shared_ptr<ASTNode> neighbour : neighbours) {
                // Check for Recursive call to itself
                if (procedureName == neighbour->extractEntity().toString()) {
                    throw ASTException("Procedure " + procedureName + " is calling itself");
                }

                if (find(visited.begin(), visited.end(), neighbour) == visited.end()) {
                    unvisitedNeighbours.push_back(neighbour);
                }
            }

            // If there are unvisited neighbours, recursively call this function
            if (unvisitedNeighbours.size() > 0) {
                vector<Relationship> recursiveCallsT = this->recursiveExtractCallsT(
                    procedureName,
                    visited,
                    unvisitedNeighbours);
                extractedCallsT.insert(
                    extractedCallsT.end(),
                    recursiveCallsT.begin(),
                    recursiveCallsT.end());
            }
        }
    }
    return extractedCallsT;
}
