#ifndef TOKEN_H
#define TOKEN_H

#include <iostream>

using namespace std;

enum TokenType { 
    ID, 
    NUMBER, 
    STRING,
    TRUE,
    FALSE,
    NULLT,

    SEMICOLON, 
    WHITESPACE, 
    LBRACKET, 
    RBRACKET, 
    COMMA, 
    DOT, 
    NEWLINE, 
 
    DEF, 

    RETURN,
    DELAY,
    OUTPUT,

    ASSIGN, 
    PLUS,
    MINUS, 
    MUL, 
    DIV, 
    EQ, 
    NOTEQ, 
    BIGGER, 
    SMALLER,
    BIGGER_OR_EQ,
    SMALLER_OR_EQ,
    AND,
    OR,

    IF,
    ELSE,

    BEGIN,
    END,

    USING,
};

class Token {
    private:
        string _value;
        TokenType _type;
        int _position;
        int _endPosition;
    public:
        Token(string value, TokenType type, int position, int endPosition);
        Token() = default;
        
        string getValue();
        TokenType getType();

        int getPosition();
        int getEndPosition();
};

#endif