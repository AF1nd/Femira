#include <iostream>
#include "token.h"
#include "lexer.h"
#include <vector>
#include <regex>

using namespace std;

struct TokenPositionBusy {
    int start;
    int end;
};

string getTokenTypeString(int type) {
    switch (type) {
        case ID:
            return "ID";
        case NUMBER:
            return "NUMBER";
        case STRING:
            return "STRING";
        case SEMICOLON:
            return "SEMICOLON";
        case WHITESPACE:
            return "WHITESPACE";
        case LBRACKET:
            return "LBRACKET";
        case RBRACKET:
            return "RBRACKET";
        case COMMA:
            return "COMMA";
        case DOT:
            return "DOT";
        case NEWLINE:
            return "NEWLINE";
        default:
            return "";
    }
}

bool compareTokens(Token first, Token second) {
    return first.getPosition() < second.getPosition();
}

Lexer::Lexer(string code) {
    _code = code;

    _tokenTypesPatterns = {
        { "[+-]?([0-9]*[.])?[0-9]+", NUMBER },
        {"'[^']*'", STRING },
        {";", SEMICOLON },
        { "\\s+", WHITESPACE },

        { "\\(", LBRACKET },
        { "\\)", RBRACKET },

        { ",", COMMA },
        { "\\.", DOT },

        { "[a-zA-Z_][a-zA-Z0-9_]*", ID },
        { "\n", NEWLINE },
    };
}

void Lexer::tokenize() {
    vector<TokenPositionBusy> busy;

    for (auto v: _tokenTypesPatterns) {
        string regexString = v.first;
        TokenType tokenType = v.second;
        regex rx(regexString);

        auto begin = sregex_iterator {_code.begin(), _code.end(), rx};
        auto end = sregex_iterator();

        for (sregex_iterator i = begin; i != end; ++i) {
            sort(_tokens.begin(), _tokens.end(), compareTokens);

            string str = i->str();
            int strLen = str.length();
            int pos = i->position();
            int endPos = pos + strLen;

            bool block = false;

            for (auto v: busy) {
                if (pos >= v.start && endPos <= v.end) block = true;
            }

            if (block) continue;

            cout << getTokenTypeString(tokenType) << " | " << pos << " | " << endPos << " | " << block << endl;

            TokenPositionBusy p;
            p.start = pos;
            p.end = endPos;

            busy.push_back(p);

            Token token(str, tokenType, pos, endPos);

            _tokens.push_back(token);
        }
    }

    for (Token v: _tokens) {
        cout << " [ " + getTokenTypeString(v.getType()) + " ] [ " + v.getValue() + " ] [ " + to_string(v.getPosition()) + " ] [ " + to_string(v.getEndPosition()) + " ] " << endl;
    }
}