#include <memory>

#include <qps/query_parser/QueryParser.h>
#include <qps/query_parser/parsers/SelectParser.h>
#include <qps/query_parser/parsers/SelectBooleanParser.h>
#include <qps/query_parser/parsers/SelectSingleParser.h>
#include <qps/query_parser/parsers/SelectMultipleParser.h>
#include <qps/query_parser/parsers/DeclarationParser.h>
#include <qps/query_parser/parsers/CallsParser.h>
#include <qps/query_parser/parsers/FollowsParser.h>
#include <qps/query_parser/parsers/ModifiesParser.h>
#include <qps/query_parser/parsers/ParentParser.h>
#include <qps/query_parser/parsers/PatternParser.h>
#include <qps/query_parser/parsers/PatternAssignParser.h>
#include <qps/query_parser/parsers/UsesParser.h>

Query QueryParser::parse() {
    DeclarationParser declParser = DeclarationParser(this->tokens);
    unordered_map<string, ArgumentType> declarations = declParser.parse();
    this->setSemanticErrorFromParser(make_shared<SemanticChecker>(declParser));
	this->tokens = declParser.getRemainingTokens();

    shared_ptr<SelectClause> selectClause = parseSelect(declarations);

    if (!this->tokens.empty()) {
        parseConstraints(declarations);
    }
    
    if (!(this->isSemanticallyValid)) {
        throw PQLSemanticError(this->semanticErrorMessage);
    }

    return Query(selectClause, this->suchThatClauses, this->patternClauses);   
}

shared_ptr<SelectClause> QueryParser::parseSelect(unordered_map<string, ArgumentType> declarations) {
	if (this->tokens.empty() || !this->tokens.front().isSelect()) {
		throw PQLSyntaxError("Missing 'Select'");
	}
	this->tokens.pop_front();

	if (this->tokens.empty()) {
		throw PQLSyntaxError("Missing argument after 'Select'");
	}
	PQLToken token = this->tokens.front();

	shared_ptr<SelectParser> parserPointer;

	if (token.isBooleanKeyword() && declarations.count(token.getTokenString()) == 0) {
		parserPointer = shared_ptr<SelectParser>(new SelectBooleanParser(this->tokens, declarations));
	}
	else if (token.isName()) {
		parserPointer = shared_ptr<SelectParser>(new SelectSingleParser(this->tokens, declarations));
	}
	else if (token.isAngledOpenBracket()) {
		parserPointer = shared_ptr<SelectParser>(new SelectMultipleParser(this->tokens, declarations));
	}
	else {
		throw PQLSyntaxError("Expected BOOLEAN, synonym or tuple of synonyms after select, got: " + token.getTokenString());
	}

	shared_ptr<SelectClause> clause = parserPointer->parse();
	this->tokens = parserPointer->getRemainingTokens();
	this->setSemanticErrorFromParser(parserPointer);
	return clause;
}

void QueryParser::parseConstraints(unordered_map<string, ArgumentType> declarations) {

    PQLToken token = this->tokens.front();
    while (!this->tokens.empty()) {
        token = this->tokens.front();
        this->tokens.pop_front();
        if (token.isSuch()) {
            this->suchThatClauses.emplace_back(parseSuchThat(declarations));
        }
        else if (token.isPattern()) {
			this->patternClauses.emplace_back(parsePattern(declarations));
        }
        else {
            throw PQLSyntaxError("Only such that and pattern clause are supported.");
        }
    }
}

shared_ptr<RelationshipClause> QueryParser::parseSuchThat(unordered_map<string, ArgumentType> declarations) {
    if (this->tokens.empty() || !this->tokens.front().isThat()) {
        throw PQLSyntaxError("Missing 'that' after 'such'");
    }
    this->tokens.pop_front();
    if (this->tokens.empty() ) {
        throw PQLSyntaxError("Missing relRef after such that");
    }
    PQLToken token = this->tokens.front();
    shared_ptr<SuchThatClauseParser> parserPointer;

    if (token.isModifies()) {
        parserPointer = shared_ptr<SuchThatClauseParser>(new ModifiesParser(this->tokens, declarations));
    } 
    else if (token.isParent()) {
        parserPointer = shared_ptr<SuchThatClauseParser>(new ParentParser(this->tokens, declarations));
    } 
    else if (token.isUses()) {
        parserPointer = shared_ptr<SuchThatClauseParser>(new UsesParser(this->tokens, declarations));
    }
    else if (token.isFollows()) {
        parserPointer = shared_ptr<SuchThatClauseParser>(new FollowsParser(this->tokens, declarations));
    }
    else if (token.isCalls()) {
        parserPointer = shared_ptr<SuchThatClauseParser>(new CallsParser(this->tokens, declarations));
    }
    else {
        throw PQLSyntaxError("Only Modifies, Uses, Parent/Parent*, Follows/Follows* are supported as such that clauses.");
    }
    shared_ptr<RelationshipClause> clause = parserPointer->parse();
    this->tokens = parserPointer->getRemainingTokens();
    this->setSemanticErrorFromParser(parserPointer);
    return clause;
}

shared_ptr<PatternClause> QueryParser::parsePattern(unordered_map<string, ArgumentType> declarations) {
	if (this->tokens.empty() || !this->tokens.front().isName()) {
		throw PQLSyntaxError("Missing synonym after pattern");
	}

	PQLToken token = this->tokens.front();

	shared_ptr<PatternParser> parserPointer;

    //TODO (Milestone 3): Fix parsing of patterns to rely on syntax before semantics

    //first check is required to prevent .at from throwing when synonym is not declared
    bool isSynonymDeclared = declarations.count(token.getTokenString()) > 0;
	if (!isSynonymDeclared || !(declarations.at(token.getTokenString()) == ArgumentType::ASSIGN)) {
        this->isSemanticallyValid = false;
        this->semanticErrorMessage = "Invalid synonym after 'pattern'";
	}

    parserPointer = shared_ptr<PatternParser>(new PatternAssignParser(this->tokens, declarations));
	shared_ptr<PatternClause> clause = parserPointer->parse();
	this->tokens = parserPointer->getRemainingTokens(); 
    this->setSemanticErrorFromParser(parserPointer);
	return clause;
}

void QueryParser::setSemanticErrorFromParser(shared_ptr<SemanticChecker> parserPointer) {
    if (!(parserPointer->isSemanticallyValid())) {
        this->isSemanticallyValid = false;
        this->semanticErrorMessage = parserPointer->getSemanticErrorMessage();
    };
}
