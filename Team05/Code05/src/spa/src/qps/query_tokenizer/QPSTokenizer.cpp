#include <qps/query_tokenizer/QPSTokenizer.h>

list<PQLToken> QPSTokenizer::tokenize(istream& stream) {

    list<PQLToken> tokenLst;

    while (stream.peek() != EOF) {
        char currentChar = char(stream.peek());

        if (this->isAlphabetical(currentChar)) {
            tokenLst.emplace_back(extractNameFromStream(stream));
        } else if (this->isDigit(currentChar)) {
            tokenLst.emplace_back(extractIntegerFromStream(stream));
        } else if (this->isDelimiter(currentChar)) {
            tokenLst.emplace_back(extractDelimiterFromStream(stream));
        } else if (this->isOperator(currentChar)) {
            tokenLst.emplace_back(extractOperatorFromStream(stream));
        } else if (this->isWhitespaceOrNewline(currentChar)) {
            skipWhitespaceOrNewlineInStream(stream);
        } else if (currentChar == EOF) {
            break;
        } else {
            string s;
            s += currentChar;
            throw PQLSyntaxError("Invalid character: " + s);
        }
    }

    return tokenLst;
}

bool QPSTokenizer::isAlphabetical(char c) {
    return isalpha(c);
}

bool QPSTokenizer::isDigit(char c) {
    return isdigit(c);
}

bool QPSTokenizer::isDelimiter(char c) {
    switch (c) {
        case '(':
        case ')':
		case '<':
		case '>':
        case ',':
        case ';':
        case '\"':
        case '_':
        case '#':
            return true;
        default:
            return false;
    }
}

bool QPSTokenizer::isOperator(char c) {
    switch (c) {
        case '+':
        case '-':
        case '*':
        case '/':
        case '%':
        case '=':
        case '.':
            return true;
        default:
            return false;
    }
}

bool QPSTokenizer::isWhitespaceOrNewline(char c) {
    return isspace(c) || c == '\n';
}

PQLToken QPSTokenizer::extractNameFromStream(istream& stream) {
    string word;
    while (isalnum(stream.peek())) {
        word += char(stream.get());
    }
    return PQLToken::createNameToken(word);
}

PQLToken QPSTokenizer::extractIntegerFromStream(istream& stream) {
    string integer;
    while (isdigit(stream.peek())) {
        integer += char(stream.get());
    }

    if (isAlphabetical(char(stream.peek()))) {
        string next;
        next += char(stream.peek());
        throw PQLSyntaxError("Alphabetical character cannot follow integer: " + integer + " followed by " + next);
    }

    return PQLToken::createIntegerToken(integer);
}

PQLToken QPSTokenizer::extractDelimiterFromStream(istream& stream) {
    string del;
    del += char(stream.get());
    return PQLToken::createDelimiterToken (del);
}

PQLToken QPSTokenizer::extractOperatorFromStream(istream& stream) {
    string op;
    op += char(stream.get());
    return PQLToken::createOperatorToken(op);
}

void QPSTokenizer::skipWhitespaceOrNewlineInStream(istream& stream) {
    while (isspace(stream.peek()) || stream.peek() == '\n') {
        stream.get();
    }
}
