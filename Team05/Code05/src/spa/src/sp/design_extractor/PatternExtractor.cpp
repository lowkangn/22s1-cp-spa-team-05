// imported libraries
#include <vector>


// imported locally
#include <sp/design_extractor/PatternExtractor.h>
#include <sp/dataclasses/AST.h>
#include <sp/dataclasses/Pattern.h>

vector<Pattern> PatternExtractor::extract(shared_ptr<ASTNode> ast) override {
	return vector<Pattern>();
}