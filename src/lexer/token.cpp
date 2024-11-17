#include <iostream>
#include "token.h"

using namespace std;

Token::Token(string value, TokenType type) {
    _value = value;
    _type = type;
}

TokenType Token::getType() {
    return _type;
}

string Token::getValue() {
    return _value;
}
