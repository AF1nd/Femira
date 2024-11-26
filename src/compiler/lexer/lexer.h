#ifndef LEXER_H
#define LEXER_H

#include <iostream>
#include <vector>
#include <map>
#include "token.h"

using namespace std;

string getTokenTypeString(int type);

class Lexer {
    private:
        map<string, TokenType> _tokenTypesPatterns;

        vector<Token> _tokens;
        string _code;
    public: 
        Lexer(string code);

        vector<Token> tokenize();
};

#endif