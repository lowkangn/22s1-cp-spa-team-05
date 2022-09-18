// imported libraries
#include <string>
#include <list>
#include <fstream>
using namespace std;

// imported local files
#include <sp/dataclasses/tokens/Token.h>
#include <sp/lexer/Lexer.h>

const char NEWLINE_CHARACTER = '\n';

list<Token> Lexer::tokenize(istream &stream) {
	
    // we use linkedlist (since we appending)
    list<Token> linkedListOfTokens;
    int trueline = 1;
    while (stream.peek() != EOF) { // see https://cplusplus.com/reference/cstdio/EOF/

        // ===== tokenize =====
        char peeked = char(stream.peek());
        if (this->charIsAlphabetical(peeked)) { // starts with alphabet, is probably a name
            linkedListOfTokens.emplace_back(this->createKeywordOrNameTokenFromTraversingStream(stream));
        } 
        else if (this->charIsDigit(peeked)) { // starts with a digit, is a number
            linkedListOfTokens.emplace_back(this->createIntegerTokenFromTraversingStream(stream));
        }
        else if (this->charIsOperator(peeked)) { // is punctuation, we're looking out only for specific ones
            linkedListOfTokens.emplace_back(this->createOperatorTokenFromTraversingStream(stream));
        }
        else if (this->charIsDelimiter(peeked)) { // is punctuation, we're looking out only for specific ones
            linkedListOfTokens.emplace_back(this->createDelimiterTokenFromTraversingStream(stream));
        }
        else if (peeked == NEWLINE_CHARACTER) {
            stream.get(); // take the character out
            trueline += 1; // count true line thing was on
        }
        else if (this->charIsWhiteSpace(peeked)) {
            this->traverseStreamUntilNoWhiteSpace(stream);
        }
        
        else if (peeked == EOF) {
            break;
        }
        else {
            throw logic_error(string("Unknown character ") + peeked + string(" encountered!"));
        }
    }
    return linkedListOfTokens;
}

void Lexer::traverseStreamUntilNoWhiteSpace(istream& stream) {
    while (this->charIsWhiteSpace(char(stream.peek()))) {
        stream.get(); // just traverse
    }   

}

bool Lexer::charIsAlphabetical(char c) {
    return isalpha(c);
}

bool Lexer::charIsDigit(char c) {
    return isdigit(c);
}

bool Lexer::charIsWhiteSpace(char c) {
    return isspace(c);
}

bool Lexer::charIsDelimiter(char c) {
    switch (c) {
        case '{':
        case '}':
        case '(':
        case ')':
        case ';':
            return true;
        default:
            return false;
    }
}

bool Lexer::charIsOperator(char c) {
    switch (c) {
    case '+':
    case '-':
    case '*':
    case '/':
    case '>':
    case '<':
    case '=':
    case '!':
    case '&':
    case '|':
    case '%':
        return true;
    default:
        return false;
    }
}

Token Lexer::createKeywordOrNameTokenFromTraversingStream(istream& stream) {
    string name;
    while (isalnum(stream.peek())) { // while is alphanumeric
        name += char(stream.get());
    }
    return Token::createNameOrKeywordToken(name);
}

Token Lexer::createIntegerTokenFromTraversingStream(istream& stream) {
    string value;
    while (isdigit(stream.peek())) { // keep getting digits
        // TODO: we should include a check for how weird numbers like 001
        value += char(stream.get());
    }
    // cannot have letter immediately after 
    if (isalpha(stream.peek())) {
        throw logic_error(string("Alphabetical character ") + char(stream.peek()) + string(" immediately after number ") + value + string(" - did you forget a space ? "));
    }
    return Token::createIntegerToken(value);

}


Token Lexer::createDelimiterTokenFromTraversingStream(istream& stream) {
    string s;
    s += char(stream.get());
    return Token::createDelimiterToken(s);
}

Token Lexer::createOperatorTokenFromTraversingStream(istream& stream) {
    // check if is singleton operators
    char c = char(stream.get());
    string s;
    s += c;

    switch (c) {
    case '+':
    case '-':
    case '*':
    case '/':
    case '%':
        break; // is singleton
    case '>':
    case '<':
    case '=':
        if (char(stream.peek()) == '=') { // comparators can only be paired with =
            s += char(stream.get());
        }
        else if (this->charIsOperator(char(stream.peek()))) {
            throw logic_error(string("Invalid comparator operator! ") + s + string(" followed by ") + char(stream.peek()));
        }
        break;
    case '!':
        if (!(char(stream.peek()) == '=' || char(stream.peek()) == '(')) { // not operator must be paired with = or '('
            throw logic_error(string("Invalid comparator operator! ") + s + string(" followed by ") + char(stream.peek()));
        } 
        // Only join the string if it is paired with '='
        if (char(stream.peek()) == '=') {
            s += char(stream.get());
        }
        break;
    case '&':
    case '|':
        if (char(stream.peek()) == c) { // comparators can only be paired with =
            s += char(stream.get());
        }
        if (this->charIsOperator(char(stream.peek()))) {
            throw logic_error(string("Invalid logical operator! ") + s + string(" followed by ") + char(stream.peek()));
        }
        break;
    default:
        throw logic_error(string("Unknown operator: ") + s);
    }
    return Token::createOperatorToken(s);
    

}

