#include <qps/query_parser/PQLToken.h>

string PQLToken::getToken() {
    return tokenString;
}

PQLTokenType PQLToken::getTokenType() {
    return tokenType;
}
