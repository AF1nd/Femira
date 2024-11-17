#ifndef TOKEN_H
#define TOKEN_H

#include <iostream>

using namespace std;

enum TokenType { ID, NUMBER, STRING, SEMICOLON, WHITESPACE };

class Token {
    private:
        string _value;
        TokenType _type;
    public:
        Token(string value, TokenType type);
        string getValue();
        TokenType getType();
};

#endif