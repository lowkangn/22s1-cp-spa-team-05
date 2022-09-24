#include <memory>

#include <qps/query_parser/QueryParser.h>
#include <qps/query_parser/parsers/SelectParser.h>
#include <qps/query_parser/parsers/DeclarationParser.h>
#include <qps/query_parser/parsers/FollowsParser.h>
#include <qps/query_parser/parsers/ModifiesParser.h>
#include <qps/query_parser/parsers/ParentParser.h>
#include <qps/query_parser/parsers/PatternAssignParser.h>
#include <qps/query_parser/parsers/UsesParser.h>
#include <qps/query_parser/parsers/WithParser.h>

Query QueryParser::parse() {
    DeclarationParser declParser = DeclarationParser(this->tokens);
    unordered_map<string, ArgumentType> declarations = declParser.parse();

    SelectParser selParser = SelectParser(declParser.getRemainingTokens(), declarations);
    shared_ptr<SelectClause> selectClause = selParser.parse();

    this->tokens = selParser.getRemainingTokens();
    if (this->tokens.empty()) {
        return Query(selectClause, list<shared_ptr<RelationshipClause>>{}, list<shared_ptr<PatternClause>>{});
    }

    this->parseConstraints(declarations);

    return Query(selectClause, this->suchThatClauses, this->patternClauses);
}

void QueryParser::parseConstraints(unordered_map<string, ArgumentType>& declarations) {
    PQLToken token = this->tokens.front();
    while (!this->tokens.empty()) {
        token = this->tokens.front();
        this->tokens.pop_front();

        if (token.isSuch()) {
            this->suchThatClauses.push_back(parseSuchThat(declarations));
        }
        else if (token.isPattern()) {
            this->patternClauses.push_back(parsePattern(declarations));
        }
        else if (token.isWith()) {
            this->withClauses.push_back(parseWith(declarations));
        }
        else {
            throw PQLSyntaxError("Only such that, pattern and with clause are supported.");
        }
    }
}

shared_ptr<RelationshipClause> QueryParser::parseSuchThat(unordered_map<string, ArgumentType>& declarations) {
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
    else {
        throw PQLSyntaxError("Only Modifies, Uses, Parent/Parent*, Follows/Follows* are supported as such that clauses.");
    }
    shared_ptr<RelationshipClause> clause = parserPointer->parse();
    this->tokens = parserPointer->getRemainingTokens();
    return clause;
}

shared_ptr<PatternClause> QueryParser::parsePattern(unordered_map<string, ArgumentType>& declarations) {
	if (this->tokens.empty() || !this->tokens.front().isName()) {
		throw PQLSyntaxError("Missing synonym after pattern");
	}

	PQLToken token = this->tokens.front();

	shared_ptr<PatternParser> parserPointer;
    //first check is required to prevent .at from throwing when synonym is not declared
    bool isSynonymDeclared = declarations.count(token.getTokenString()) > 0;
	if (isSynonymDeclared && declarations.at(token.getTokenString()) == ArgumentType::ASSIGN) {
		parserPointer = shared_ptr<PatternParser>(new PatternAssignParser(this->tokens, declarations));
	} else {
		throw PQLSemanticError("Invalid synonym in pattern");
	}

	shared_ptr<PatternClause> clause = parserPointer->parse();
	this->tokens = parserPointer->getRemainingTokens();
	return clause;
}

shared_ptr<WithClause> QueryParser::parseWith(unordered_map<string, ArgumentType>& declarations) {
    if (this->tokens.empty()) {
        throw PQLSyntaxError("Query ended after with");
    }
    WithParser withParser = WithParser(this->tokens, declarations);
    return withParser.parse();
}
