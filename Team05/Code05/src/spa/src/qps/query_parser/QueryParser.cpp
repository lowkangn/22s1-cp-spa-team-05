#include <memory>

#include <qps/query_parser/QueryParser.h>
#include <qps/query_parser/parsers/SelectParser.h>
#include <qps/query_parser/parsers/SelectBooleanParser.h>
#include <qps/query_parser/parsers/SelectSingleParser.h>
#include <qps/query_parser/parsers/SelectMultipleParser.h>
#include <qps/query_parser/parsers/DeclarationParser.h>
#include <qps/query_parser/parsers/PatternParser.h>
#include <qps/query_parser/parsers/WithParser.h>

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

    return Query(selectClause, this->suchThatClauses, this->patternClauses, this->withClauses);   
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
    assert(!this->tokens.empty());
    PQLToken token = this->tokens.front();
    while (!this->tokens.empty()) {
        token = this->tokens.front();
        this->tokens.pop_front();

        // Set state based on token, use previous state for 'and'
        if (token.isSuch() && !this->tokens.empty() && this->tokens.front().isThat()) {
            //pop 'that'
            this->tokens.pop_front();
            this->currentState = make_shared<SuchThatState>(this);
        }
        else if (token.isPattern()) {
            this->currentState = make_shared<PatternState>(this);
        }
        else if (token.isWith()) {
            this->currentState = make_shared<WithState>(this);
        }
        else if (token.isAnd() && this->currentState == nullptr) {
            throw PQLSyntaxError("'and' cannot be used without a preceding constraint clause");
        }
        else if (!token.isAnd()) {
            throw PQLSyntaxError("Expected 'such that', 'pattern', 'with' or 'and', got: " + token.getTokenString());
        }

        this->currentState->parseOneClause(this->tokens, declarations);
    }
}

void QueryParser::setSemanticErrorFromParser(shared_ptr<SemanticChecker> parserPointer) {
    if (!(parserPointer->isSemanticallyValid())) {
        this->isSemanticallyValid = false;
        this->semanticErrorMessage = parserPointer->getSemanticErrorMessage();
    };
}
