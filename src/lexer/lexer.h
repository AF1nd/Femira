#ifndef LEXER_H
#define LEXER_H

#include <iostream>
#include <vector>
#include <regex>
#include "token.h"

using namespace std;

class Lexer {
    private:
        map<string, TokenType> _tokenTypesPatterns;

        vector<Token> _tokens;
        string _code;
    public: 
        Lexer(string code);

        void tokenize();
};

#endif