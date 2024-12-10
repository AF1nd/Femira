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

    LSQUARE_BRACKET,
    RSQUARE_BRACKET,

    LOBJECT_BRACKET,
    ROBJECT_BRACKET,
    
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
        TokenType _type;
        int _position;
        int _endPosition;
    public:
        string value;

        Token(string value, TokenType type, int position, int endPosition);
        Token() = default;
        
        TokenType getType();

        int getPosition();
        int getEndPosition();
};

#endif