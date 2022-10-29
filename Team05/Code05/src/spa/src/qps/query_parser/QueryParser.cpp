#include <qps/query_parser/QueryParser.h>

Query QueryParser::parse() {
    DeclarationParser declParser = DeclarationParser(this->tokens);
    unordered_map<string, ArgumentType> declarations = declParser.parse();
    this->setSemanticErrorFromParser(make_shared<SemanticChecker>(declParser));

    shared_ptr<SelectClause> selectClause = parseSelect(declarations);

    if (!this->tokens.empty()) {
        parseConstraints(declarations);
    }
    
    if (!(this->isSemanticallyValid)) {
        throw PQLSemanticError(this->semanticErrorMessage);
    }

    return Query(selectClause, this->suchThatClauses, this->patternClauses, this->withClauses);   
}

shared_ptr<SelectClause> QueryParser::parseSelect(const unordered_map<string, ArgumentType>& declarations) {
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
		parserPointer = make_shared<SelectBooleanParser>(this->tokens, declarations);
	}
	else if (token.isName()) {
		parserPointer = make_shared<SelectSingleParser>(this->tokens, declarations);
	}
	else if (token.isAngledOpenBracket()) {
		parserPointer = make_shared<SelectMultipleParser>(this->tokens, declarations);
	}
	else {
		throw PQLSyntaxError("Expected BOOLEAN, synonym or tuple of synonyms after select, got: " + token.getTokenString());
	}

	shared_ptr<SelectClause> clause = parserPointer->parse();
	this->setSemanticErrorFromParser(parserPointer);
	return clause;
}

void QueryParser::parseConstraints(const unordered_map<string, ArgumentType>& declarations) {
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
