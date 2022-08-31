// imported libraries
#include <string>
#include <list>
#include <fstream>
using namespace std;

// imported local files
#include <sp/dataclasses/tokens/Token.h>
#include <sp/lexer/Lexer.h>

const char SINGLE_LINE_CONTINUATION_CHARACTER = '\\'; 
// see https://stackoverflow.com/questions/19405196/what-does-a-backslash-in-c-mean and https://www.quora.com/What-is-the-use-of-the-backslash-in-C
const char NEWLINE_CHARACTER = '\n';

list<Token> Lexer::tokenize(ifstream &stream) {
	
    // we use linkedlist (since we appending)
    list<Token> linkedListOfTokens;

    while (stream.peek() != EOF) { // see https://cplusplus.com/reference/cstdio/EOF/

        // ===== guard clause =====
        this->traverseStreamUntilNoNewLines(stream);
        

        // ===== tokenize =====
        char peeked = char(stream.peek());
        if (this->charIsAlphabetical(peeked)) { // starts with alphabet, is probably a name
            linkedListOfTokens.emplace_back(this->createNameTokenFromTraversingStream(stream));
        } 
        else if (this->charIsDigit(peeked)) { // starts with a digit, is a number
            linkedListOfTokens.emplace_back(this->createIntegerTokenFromTraversingStream(stream));
        }
        else if (this->isOperator(peeked)) { // is punctuation, we're looking out only for specific ones
            linkedListOfTokens.emplace_back(this->createOperatorTokenFromTraversingStream(stream));
        }
        else if (this->isDelimiter(peeked)) { // is punctuation, we're looking out only for specific ones
            linkedListOfTokens.emplace_back(this->createDelimiterTokenFromTraversingStream(stream));
        }
        else if (this->isWhiteSpace(peeked)) {
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
    while (isspace(stream.peek())) {
        char(stream.get()); // just traverse
    }
        
}

void Lexer::traverseStreamUntilNoNewLines(istream& stream) {
    char character = char(stream.get()); // get character
    if (character == SINGLE_LINE_CONTINUATION_CHARACTER) { // if line continuation
        if (stream.peek() == SINGLE_LINE_CONTINUATION_CHARACTER) { // and line continuatation again 
            // discard all newlines until not newline or EOF 
            // see https://stackoverflow.com/questions/25020129/cin-ignorenumeric-limitsstreamsizemax-n
            stream.ignore(numeric_limits<streamsize>::max(), NEWLINE_CHARACTER);
            if (!stream.eof()) {
                // if not eof, go back one character
                stream.unget();
            }
        }
        else {
            // some error, since after line continuation (\\) we expect another (\\)
            throw logic_error(string("Expected \\ after \\ for valid line continuation, got: ") + char(stream.peek()));
        }

    }
    else {
        stream.unget(); // is ok, can start lexing
    }
}

bool Lexer::charIsAlphabetical(char c) {
    return isalpha(c);
}

bool Lexer::charIsDigit(char c) {
    return isdigit(c);
}

bool Lexer::isWhiteSpace(char c) {
    return isspace(c);
}

bool Lexer::isDelimiter(char c) {
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

bool Lexer::isOperator(char c) {
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
        return true;
    default:
        return false;
    }
}

Token Lexer::createNameTokenFromTraversingStream(istream& stream) {
    string name;
    while (isalnum(stream.peek())) {
        name += char(stream.get());
    }
    return Token(name, TokenType::NAME);
}

Token Lexer::createIntegerTokenFromTraversingStream(istream& stream) {
    string value;
    while (isdigit(stream.peek())) { // keep getting digits
        value += char(stream.get());
    }
    // cannot have letter immediately after 
    if (isalpha(stream.peek())) {
        throw logic_error(string("Alphabetical character ") + char(stream.peek()) + string(" immediately after number ") + value + string(" - did you forget a space ? "));
    }
    return Token(value, TokenType::INTEGER);

}


Token Lexer::createDelimiterTokenFromTraversingStream(istream& stream) {
    string s = "" + char(stream.get());
    return Token(s, TokenType::DELIMITER);
}

Token Lexer::createOperatorTokenFromTraversingStream(istream& stream) {
    // check if is singleton operators
    char c = char(stream.get());
    string s = "" + c;

    switch (c) {
    case '+':
    case '-':
    case '*':
    case '/':
        break; // is singleton
    case '>':
    case '<':
    case '=':
    case '!':
        if (char(stream.peek()) == '=') { // comparators can only be paired with =
            s += char(stream.get());
            break;
        }
        else if (this->isOperator(char(stream.peek()))) {
            throw logic_error(string("Invalid comparator operator! ") + s + string(" followed by ") + char(stream.peek()));
        }

    case '&':
    case '|':
        if (char(stream.peek()) == c) { // comparators can only be paired with =
            s += char(stream.get());
            break;
        }
        else if (this->isOperator(char(stream.peek()))) {
            throw logic_error(string("Invalid logical operator! ") + s + string(" followed by ") + char(stream.peek()));
        }
    default:
        throw logic_error(string("Unknown operator: ") + s);
    }
    return Token(s, TokenType::OPERATOR);
    

}

