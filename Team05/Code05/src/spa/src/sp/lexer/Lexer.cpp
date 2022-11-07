
#include <sp/SPException.h>
#include <sp/dataclasses/tokens/Token.h>
#include <sp/lexer/Lexer.h>

#include <fstream>
#include <list>
#include <string>

using namespace std;

const char NEWLINE_CHARACTER = '\n';

list<Token> Lexer::tokenize(istream& stream) {
    // we use linkedlist (since we are appending)
    list<Token> linkedListOfTokens;
    int trueline = 1;
    while (stream.peek() != EOF) {
        // see https://cplusplus.com/reference/cstdio/EOF/

        // ===== tokenize =====
        char peeked = static_cast<char>(stream.peek());
        if (this->charIsAlphabetical(peeked)) {
            // starts with alphabet, is probably a name
            linkedListOfTokens.emplace_back(
                this->createKeywordOrNameTokenFromTraversingStream(stream));
        } else if (this->charIsDigit(peeked)) {
            // starts with a digit, is a number
            linkedListOfTokens.emplace_back(this->createIntegerTokenFromTraversingStream(stream));
        } else if (this->charIsOperator(peeked)) {
            // is punctuation, we're looking out only for specific ones
            linkedListOfTokens.emplace_back(this->createOperatorTokenFromTraversingStream(stream));
        } else if (this->charIsDelimiter(peeked)) {
            // is punctuation, we're looking out only for specific ones
            linkedListOfTokens.emplace_back(
                this->createDelimiterTokenFromTraversingStream(stream));
        } else if (peeked == NEWLINE_CHARACTER) {
            stream.get(); // take the character out
            trueline += 1; // count true line thing was on
        } else if (this->charIsWhiteSpace(peeked)) {
            this->traverseStreamUntilNoWhiteSpace(stream);
        } else if (peeked == EOF) {
            break;
        } else {
            throw SPException(string("Unknown character ") + peeked + string(" encountered!"));
        }
    }
    return linkedListOfTokens;
}

void Lexer::traverseStreamUntilNoWhiteSpace(istream& stream) {
    while (this->charIsWhiteSpace(static_cast<char>(stream.peek()))) {
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
    while (isalnum(stream.peek())) {
        // while is alphanumeric
        name += static_cast<char>(stream.get());
    }
    return Token::createNameOrKeywordToken(name);
}

Token Lexer::createIntegerTokenFromTraversingStream(istream& stream) {
    string value;

    while (isdigit(stream.peek())) {
        // keep getting digits
        value += static_cast<char>(stream.get());
    }

    if (value.size() > 1 && value[0] == '0') {
        // value is a sequence of numbers of length > 1 and starts with 0
        throw SPException(value + string(" is an invalid integer"));
    }

    // cannot have letter immediately after
    if (isalpha(stream.peek())) {
        throw SPException(
            string("Alphabetical character ") + static_cast<char>(stream.peek()) +
                string("immediately after number ") + value +
                string(" - did you forget a space ? "));
    }
    return Token::createIntegerToken(value);
}


Token Lexer::createDelimiterTokenFromTraversingStream(istream& stream) {
    string s;
    s += static_cast<char>(stream.get());
    return Token::createDelimiterToken(s);
}

Token Lexer::createOperatorTokenFromTraversingStream(istream& stream) {
    // check if is singleton operators
    char c = static_cast<char>(stream.get());
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
        if (static_cast<char>(stream.peek()) == '=') {
            // comparators can only be paired with =
            s += static_cast<char>(stream.get());
        } else if (this->charIsOperator(static_cast<char>(stream.peek()))) {
            throw SPException(
                string("Invalid comparator operator! ") + s + string(" followed by ") +
                    static_cast<char>(stream.peek()));
        }
        break;
    case '!':
        // Only join the string if it is paired with '='
        if (static_cast<char>(stream.peek()) == '=') {
            s += static_cast<char>(stream.get());
            break;
        } else if (this->charIsWhiteSpace(static_cast<char>(stream.peek()))
                || static_cast<char>(stream.peek()) == '(') {
            this->traverseStreamUntilNoWhiteSpace(stream);
            if (static_cast<char>(stream.peek()) == '(') {
                // do nothing
                break;
            }
        } else {
            throw SPException(
                string("Invalid comparator operator! ") + s + string(" followed by ") +
                    static_cast<char>(stream.peek()));
        }
    case '&':
    case '|':
        if (static_cast<char>(stream.peek()) == c) {
            // comparators can only be paired with =
            s += static_cast<char>(stream.get());
        }
        if (this->charIsOperator(static_cast<char>(stream.peek()))) {
            throw SPException(
                string("Invalid logical operator! ") + s + string(" followed by ") +
                    static_cast<char>(stream.peek()));
        }
        break;
    default:
        throw SPException(string("Unknown operator: ") + s);
    }
    return Token::createOperatorToken(s);
}
