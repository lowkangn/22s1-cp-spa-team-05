#include <qps/query_parser/parsers/WithParser.h>

shared_ptr<WithClause> WithParser::parse() {
	// Check syntax and parse 'ref = ref' into ClauseArgs
	vector<ClauseArgument> lhsArgs = this->parseRef();
	this->consumeEquals();
	vector<ClauseArgument> rhsArgs = this->parseRef();

	// Verify the semantics
	this->checkAttrCompare(lhsArgs, rhsArgs);

	return this->createWithClause(lhsArgs, rhsArgs);
}

vector<ClauseArgument> WithParser::parseRef() {
	vector<ClauseArgument> args;
	ClauseArgument refOrSynonym = this->parseOneArgument();
	args.push_back(refOrSynonym);

	// a ref can be an '"' IDENT '"', INTEGER or a synonym '.' attrName
	if (!refOrSynonym.isSynonym() && !refOrSynonym.isLineNumber() && !refOrSynonym.isStringLiteral()) {
		throw PQLSyntaxError("ref of a with clause should be a '\"'IDENT'\"', INTEGER or an attrRef");
	}

	if (refOrSynonym.isSynonym()) {
		this->consumeDot();
		args.push_back(this->parseAttribute(refOrSynonym));
	}

	if (!this->tokens.empty() && this->tokens.front().isDot()) {
		throw PQLSyntaxError("Unexpected '.' after ref in with clause");
	}

	return args;
}

ClauseArgument WithParser::parseAttribute(ClauseArgument synonym) {
	PQLToken attrNameToken = this->tokens.front();
	this->tokens.pop_front();
	if (attrNameToken.isProcName()) {
		return ClauseArgument::createProcNameAttributeArg(synonym);
	}
	else if (attrNameToken.isVarName()) {
		return ClauseArgument::createVarNameAttributeArg(synonym);
	}
	else if (attrNameToken.isValue()) {
		return ClauseArgument::createValueAttributeArg(synonym);
	}
	else if (attrNameToken.isStmtNumStmt()) {
		if (this->tokens.empty()) {
			throw PQLSyntaxError("Query ended after 'stmt' in with clause");
		}
		attrNameToken = this->tokens.front();
		this->tokens.pop_front();
		if (!attrNameToken.isStmtNumHash()) {
			throw PQLSyntaxError("Expected '#' after 'stmt' in with clause, got: " + attrNameToken.getTokenString());
		}
		return ClauseArgument::createStmtNumAttributeArg(synonym);
	}
	else {
		throw PQLSyntaxError("Unkown attribute name: " + attrNameToken.getTokenString());
	}
}

void WithParser::checkAttrCompare(vector<ClauseArgument>& lhsArgs, vector<ClauseArgument>& rhsArgs) {
	// Check each ref is semantically correct
	this->checkRef(lhsArgs);
	this->checkRef(rhsArgs);

	/* Since an attribute value can only be an integer type or a name type,
	   it suffices to check if each ref is an integer type */
	vector<bool> areRefsIntegers;

	for (vector<ClauseArgument> refArgs : { lhsArgs, rhsArgs }) {
		if (refArgs.size() == 1) {
			areRefsIntegers.push_back(refArgs.front().isLineNumber());
		}
		else {
			areRefsIntegers.push_back(refArgs.back().isValueAttribute() || refArgs.back().isStmtNumAttribute());
		}
	}
	assert(areRefsIntegers.size() == 2);

	if (areRefsIntegers.front() != areRefsIntegers.back()) {
		this->semanticErrorMessage = "The two refs in an attrCompare must be of the same type (both NAME, or both INTEGER)";
	}
}

void WithParser::checkRef(vector<ClauseArgument>& args) {
	if (args.size() == 1) {
		// there is no semantics to check 
		return;
	}
	assert(args.size() == 2);
	assert(args.front().isSynonym());
	assert(args.back().isAttributeName());
	
	ClauseArgument synonym = args.front();
	ClauseArgument attrName = args.back();

	bool acceptsProcName = synonym.isProcedureSynonym() || synonym.isCallSynonym();
	bool acceptsVarName = synonym.isVariableSynonym() || synonym.isPrintSynonym() || synonym.isReadSynonym();
	bool acceptsValue = synonym.isConstantSynonym();
	bool acceptsStmtNum = synonym.isStmtRefNoWildcard();

	bool isSemanticallyCorrect = (acceptsProcName && attrName.isProcNameAttribute())
		|| (acceptsVarName && attrName.isVarNameAttribute())
		|| (acceptsValue && attrName.isValueAttribute())
		|| (acceptsStmtNum && attrName.isStmtNumAttribute());
	if (!isSemanticallyCorrect) {
		this->semanticErrorMessage = "With clause synonym and attribute name do not match";
	}
}

shared_ptr<WithClause> WithParser::createWithClause(vector<ClauseArgument>& lhsArgs, vector<ClauseArgument>& rhsArgs) {

	if (lhsArgs.size() == 1 && rhsArgs.size() == 1) {
		return make_shared<WithBothExactClause>(lhsArgs.front(), rhsArgs.front());
	}

	if (lhsArgs.size() == 2 && rhsArgs.size() == 2) {
		return make_shared<WithNoExactClause>(lhsArgs, rhsArgs);
	}
	
	ClauseArgument exactArg = lhsArgs.size() == 1 ? lhsArgs.front() : rhsArgs.front();
	vector<ClauseArgument> nonExactArgs = lhsArgs.size() == 2 ? lhsArgs : rhsArgs;
	assert(exactArg.isExactReference() && nonExactArgs.front().isSynonym() && nonExactArgs.back().isAttributeName());

	return make_shared<WithOneExactClause>(exactArg, nonExactArgs);

}

bool WithParser::isNonDefaultAttribute(vector<ClauseArgument>& args) {
	ClauseArgument syn = args.front();
	ClauseArgument attribute = args.back();
	if (syn.isCallSynonym() && attribute.isProcNameAttribute()) {
		return true;
	}
	else if (syn.isReadSynonym() && attribute.isVarNameAttribute()) {
		//equivalent to ModifiesS with syn as lhs
		return true;
	}
	else if (syn.isPrintSynonym() && attribute.isVarNameAttribute()) {
		//equivalent to UsesS with syn as lhs
		return true;
	}
	return false;
}

void WithParser::consumeDot() {
	if (this->tokens.empty() || !this->tokens.front().isDot()) {
		throw PQLSyntaxError("Expected '.'");
	}
	this->tokens.pop_front();
	if (this->tokens.empty()) {
		throw PQLSyntaxError("Expected argument after '.'");
	}
}

void WithParser::consumeEquals() {
	if (this->tokens.empty() || !this->tokens.front().isEquals()) {
		throw PQLSyntaxError("Expected '='");
	}
	this->tokens.pop_front();
	if (this->tokens.empty()) {
		throw PQLSyntaxError("Expected argument after '='");
	}
}