#include <iostream>
#include "token.h"

using namespace std;

Token::Token(string value, TokenType type, int position, int endPosition) {
    _value = value;
    _type = type;
    _position = position;
    _endPosition = endPosition;
}

TokenType Token::getType() {
    return _type;
}

string Token::getValue() {
    return _value;
}

int Token::getPosition() {
    return _position;
}

int Token::getEndPosition() {
    return _endPosition;
}
