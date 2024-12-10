#include <iostream>
#include "include/token.h"


using namespace std;

Token::Token(string value, TokenType type, int position, int endPosition) {
    this->value = value;
    _type = type;
    _position = position;
    _endPosition = endPosition;
}

TokenType Token::getType() {
    return _type;
}

int Token::getPosition() {
    return _position;
}

int Token::getEndPosition() {
    return _endPosition;
}
