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
    this->setSemanticErrorFromParser(make_shared<SemanticChecker>(declParser));

    SelectParser selParser = SelectParser(declParser.getRemainingTokens(), declarations);
    shared_ptr<SelectClause> selectClause = selParser.parse();
    this->setSemanticErrorFromParser(make_shared<SemanticChecker>(selParser));

    this->tokens = selParser.getRemainingTokens();
<<<<<<< HEAD
    if (this->tokens.empty()) {
        return Query(selectClause, list<shared_ptr<RelationshipClause>>{}, list<shared_ptr<PatternClause>>{});
    }

    this->parseConstraints(declarations);

    return Query(selectClause, this->suchThatClauses, this->patternClauses);
}

void QueryParser::parseConstraints(unordered_map<string, ArgumentType>& declarations) {
=======

    if (!this->tokens.empty()) {
        parseConstraints(declarations);
    }
    
    if (!(this->isSemanticallyValid)) {
        throw PQLSemanticError(this->semanticErrorMessage);
    }

    return Query(selectClause, this->suchThatClauses, this->patternClauses);   
}

void QueryParser::parseConstraints(unordered_map<string, ArgumentType> declarations) {
    

>>>>>>> master
    PQLToken token = this->tokens.front();
    while (!this->tokens.empty()) {
        token = this->tokens.front();
        this->tokens.pop_front();

        if (token.isSuch()) {
<<<<<<< HEAD
            this->suchThatClauses.push_back(parseSuchThat(declarations));
        }
        else if (token.isPattern()) {
            this->patternClauses.push_back(parsePattern(declarations));
        }
        else if (token.isWith()) {
            this->withClauses.push_back(parseWith(declarations));
=======
            this->suchThatClauses.emplace_back(parseSuchThat(declarations));
        }
        else if (token.isPattern()) {
			this->patternClauses.emplace_back(parsePattern(declarations));
>>>>>>> master
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
    this->setSemanticErrorFromParser(parserPointer);
    return clause;
}

shared_ptr<PatternClause> QueryParser::parsePattern(unordered_map<string, ArgumentType>& declarations) {
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

shared_ptr<WithClause> QueryParser::parseWith(unordered_map<string, ArgumentType>& declarations) {
    if (this->tokens.empty()) {
        throw PQLSyntaxError("Query ended after with");
    }
    WithParser withParser = WithParser(this->tokens, declarations);
    return withParser.parse();
}

void QueryParser::setSemanticErrorFromParser(shared_ptr<SemanticChecker> parserPointer) {
    if (!(parserPointer->isSemanticallyValid())) {
        this->isSemanticallyValid = false;
        this->semanticErrorMessage = parserPointer->getSemanticErrorMessage();
    };
}
