#ifndef TOKEN_H
#define TOKEN_H

#include <iostream>

using namespace std;

enum TokenType { ID, NUMBER, STRING, SEMICOLON, WHITESPACE, LBRACKET, RBRACKET, COMMA, DOT, NEWLINE };

class Token {
    private:
        string _value;
        TokenType _type;
        int _position;
        int _endPosition;
    public:
        Token(string value, TokenType type, int position, int endPosition);
        
        string getValue();
        TokenType getType();

        int getPosition();
        int getEndPosition();
};

#endif