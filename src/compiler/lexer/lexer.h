#ifndef LEXER_H
#define LEXER_H

#include <iostream>
#include <vector>
#include <array>
#include "token.h"

using namespace std;

string getTokenTypeString(int type);

class Lexer {
    private:
        array<pair<string, TokenType>, 33> _tokenTypesPatterns;

        vector<Token> _tokens;
        string _code;
    public: 
        Lexer(string code);

        vector<Token> tokenize();
};

#endif