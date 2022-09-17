#include <memory>

#include <qps/query_parser/QueryParser.h>
#include <qps/query_parser/parsers/SelectParser.h>
#include <qps/query_parser/parsers/DeclarationParser.h>
#include <qps/query_parser/parsers/ModifiesParser.h>
#include <qps/query_parser/parsers/ParentParser.h>
#include <qps/query_parser/parsers/PatternParser.h>
#include <qps/query_parser/parsers/PatternAssignParser.h>
#include <qps/query_parser/parsers/UsesParser.h>

Query QueryParser::parse() {
    DeclarationParser declParser = DeclarationParser(this->tokens);
    unordered_map<string, ArgumentType> declarations = declParser.parse();

    SelectParser selParser = SelectParser(declParser.getRemainingTokens(), declarations);
    shared_ptr<SelectClause> selectClause = selParser.parse();

    this->tokens = selParser.getRemainingTokens();
    if (this->tokens.empty()) {
        return Query(selectClause, list<shared_ptr<RelationshipClause>>{}, list<shared_ptr<PatternClause>>{});
    }

	shared_ptr<list<shared_ptr<RelationshipClause>>> suchThatClauses = make_shared<list<shared_ptr<RelationshipClause>>>();
	shared_ptr<list<shared_ptr<PatternClause>>> patternClauses =  make_shared<list<shared_ptr<PatternClause>>>();

	parseConstraints(suchThatClauses, patternClauses, declarations);

    return Query(selectClause, *suchThatClauses, *patternClauses);
}

void QueryParser::parseConstraints(shared_ptr<list<shared_ptr<RelationshipClause>>> suchThatClauses,
								   shared_ptr<list<shared_ptr<PatternClause>>> patternClauses,
								   unordered_map<string, ArgumentType> declarations) {

    PQLToken token = this->tokens.front();
    while (!this->tokens.empty()) {
        token = this->tokens.front();
        this->tokens.pop_front();
        if (token.isSuch()) {
            suchThatClauses->emplace_back(parseSuchThat(declarations));
        }
        else if (token.isPattern()) {
			patternClauses->emplace_back(parsePattern(declarations));
        }
    }
}

shared_ptr<RelationshipClause> QueryParser::parseSuchThat(unordered_map<string, ArgumentType> declarations) {
    if (this->tokens.empty() || !this->tokens.front().isThat()) {
        throw PQLError("Missing 'that' after 'such'");
    }
    this->tokens.pop_front();
    if (this->tokens.empty() ) {
        throw PQLError("Missing relRef after such that");
    }
    PQLToken token = this->tokens.front();
    shared_ptr<SuchThatClauseParser> parserPointer;

    if (token.isModifies()) {
        parserPointer = shared_ptr<SuchThatClauseParser>(new ModifiesParser(this->tokens, declarations));
    } else if (token.isParent()) {
        parserPointer = shared_ptr<SuchThatClauseParser>(new ParentParser(this->tokens, declarations));
    } else if (token.isUses()) {
        parserPointer = shared_ptr<SuchThatClauseParser>(new UsesParser(this->tokens, declarations));
    }
    else {
        //TODO: add more such that clauses
    }
    shared_ptr<RelationshipClause> clause = parserPointer->parse();
    this->tokens = parserPointer->getRemainingTokens();
    return clause;
}

shared_ptr<PatternClause> QueryParser::parsePattern(unordered_map<string, ArgumentType> declarations) {
	if (this->tokens.empty() ) {
		throw PQLError("Missing synonym after pattern");
	}

	PQLToken token = this->tokens.front();
	shared_ptr<PatternParser> parserPointer;

	if (declarations.at(token.getTokenString()) == ArgumentType::ASSIGN) {
		parserPointer = shared_ptr<PatternParser>(new PatternAssignParser(this->tokens, declarations));
	} else {
		throw PQLError("Invalid synonym in pattern");
	}

	shared_ptr<PatternClause> clause = parserPointer->parse();
	this->tokens = parserPointer->getRemainingTokens();
	return clause;
}
