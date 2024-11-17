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
        { "[+-]?([0-9]*[.])?[0-9]+", NUMBER },
        {"'[^']*'", STRING },
        {";", SEMICOLON },
        { "\\s+", WHITESPACE },
        { "[a-zA-Z_][a-zA-Z0-9_]*", ID },

        { "\\(", LBRACKET },
        { "\\)", RBRACKET },

        { ",", COMMA },
        { "\\.", DOT },
    };
}

bool Lexer::nextToken() {
    if (_position < _code.length()) {
        for (auto v: _tokenTypesPatterns) {
            string regexString = v.first;
            TokenType tokenType = v.second;
            regex rx(regexString);

            string code = _code.substr(_position);

            auto begin = sregex_iterator {code.begin(), code.end(), rx};
            auto end = sregex_iterator();

            for (sregex_iterator i = begin; i != end; ++i)
            {
                cout << "Found " << i->str() << " at position: " << i->position() << endl;
                
                string str = i->str();
                int strLen = str.length();
                int pos = i->position();
                int idx = pos + strLen;

               // _tokens.insert(_tokens.begin() + idx, Token(str, tokenType));
            }
        }
    };

    return false;
}

void Lexer::tokenize() {
    while (nextToken()) {}

    for (Token v: _tokens) {
        cout << getTokenTypeString(v.getType()) << " | " << v.getValue() << endl;
    }
}