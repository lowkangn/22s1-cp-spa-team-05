#pragma once

#include <pkb/interfaces/PKBUpdateHandler.h>

#include <sp/SPException.h>
#include <sp/dataclasses/ast/AST.h>
#include <sp/dataclasses/tokens/Token.h>
#include <sp/design_extractor/CallsAndCallsTExtractor.h>
#include <sp/design_extractor/DesignExtractorManager.h>
#include <sp/design_extractor/EntityExtractor.h>
#include <sp/design_extractor/FollowsAndFollowsTExtractor.h>
#include <sp/design_extractor/ModifiesExtractor.h>
#include <sp/design_extractor/NextExtractor.h>
#include <sp/design_extractor/ParentExtractor.h>
#include <sp/design_extractor/ParentTExtractor.h>
#include <sp/design_extractor/PatternExtractor.h>
#include <sp/design_extractor/UsesExtractor.h>
#include <sp/lexer/Lexer.h>
#include <sp/parser/SimpleSyntaxParserManager.h>
#include <sp/parser/cfg_parser/ControlFlowGraphparser.h>

#include <list>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

class SourceProcessor {
private:
    bool isInitialized = false;
    shared_ptr<ASTNode> astRoot;
    vector<shared_ptr<CFGNode>> controlFlowGraphs;
    Lexer lexer = Lexer();
    ParserManager parser = ParserManager();
    DesignExtractorManager designManager{
        EntityExtractor(), PatternExtractor(), NextExtractor(),
        {
            make_shared<FollowsAndFollowsTExtractor>(),
            make_shared<ParentExtractor>(),
            make_shared<ParentTExtractor>(),
            make_shared<ModifiesExtractor>(false),
            make_shared<UsesExtractor>(false),
            make_shared<CallsAndCallsTExtractor>()
        }
    };
    ControlFlowParser cfgParser = ControlFlowParser();

public:
    SourceProcessor() {
    }

    void initialize(istream& sourceProgram);
    void extractAllAndAddToPkb(shared_ptr<PKBUpdateHandler> pkb);

    vector<Relationship> extractRelations();
    vector<Relationship> extractCFGRelations();
    vector<Pattern> extractPatterns();
    vector<Entity> extractEntities();
};
