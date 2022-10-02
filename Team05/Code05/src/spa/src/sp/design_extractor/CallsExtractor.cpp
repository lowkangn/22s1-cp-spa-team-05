// imported libraries
#include <vector>
#include <memory>
#include <assert.h>

// imported locally
#include <sp/dataclasses/ast/AST.h>
#include <sp/design_extractor/CallsExtractor.h>

vector<Relationship> CallsExtractor::extract(shared_ptr<ASTNode> ast) {

	vector<Relationship> calls = vector<Relationship>();
	ASTNodeType type = ast->getType();

	return calls;
}
