#include <iostream>
#include "token.h"
#include "lexer.h"
#include <vector>
#include <regex>

using namespace std;

string getTokenTypeString(int type) {
    switch (type) {
        case 0:
            return "ID";
        case 1:
            return "NUMBER";
        case 2:
            return "STRING";
        case 3:
            return "SEMICOLON";
        case 4:
            return "WHITESPACE";
        default:
            return "";
    }
}

Lexer::Lexer(string code) {
    _code = code;
    _position = 0;

    _tokenTypesPatterns = {
        { regex("[+-]?([0-9]*[.])?[0-9]+"), NUMBER },
        { regex("'[^']*'"), STRING },
        { regex(";"), SEMICOLON },
        { regex("\\s+"), WHITESPACE },
        { regex("[a-zA-Z_][a-zA-Z0-9_]*"), ID },
    };
}

Token Lexer::nextToken() {
    if (_position >= _code.length()) return;

    for (auto v: _tokenTypesPatterns) {
        regex rx = v.first;
        TokenType tokenType = v.second;
        
    }
}

void Lexer::tokenize() {

}