#ifndef LEXER_H
#define LEXER_H

#include <iostream>
#include <vector>
#include <regex>
#include "token.h"

using namespace std;

class Lexer {
    private:
        map<regex, TokenType> _tokenTypesPatterns;

        vector<Token> _tokens;
        string _code;
        int _position;
    public: 
        Lexer(string code);
    
        Token nextToken();
        void tokenize();
};

#endif