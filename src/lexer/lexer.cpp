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
        case ASSIGN:
            return "ASSIGN";
        case PLUS:
            return "PLUS";
        case MINUS:
            return "MINUS";
        case DIV:
            return "DIV";
        case MUL:
            return "MUL";
        case VAR:
            return "VAR";
        case DEF:
            return "DEF";
        case EQ:
            return "EQ";
        case NOTEQ:
            return "NOTEQ";
        case BIGGER:
            return "BIGGER";
        case SMALLER:
            return "SMALLER";
        case BEGIN:
            return "BEGIN";
        case END:
            return "END";
        case RETURN:
            return "RETURN";
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

        { "\\+", PLUS },
        { "\\-", MINUS },
        { "\\/", DIV },
        { "\\*", MUL },

        { "!=", NOTEQ },
        { "==", EQ },
        { "=", ASSIGN },

        { ">=", BIGGER_OR_EQ },
        { "<=", SMALLER_OR_EQ },
 
        { ">", BIGGER },
        { "<", SMALLER },

        { ":", BEGIN },
        { "end", END },
        { "var", VAR },
        { "def", DEF },
        { "return", RETURN },

        { "[a-zA-Z_][a-zA-Z0-9_]*", ID },
    };
}

vector<Token> Lexer::tokenize() {
    vector<TokenPositionBusy> busy;

    for (pair<string, TokenType> v: _tokenTypesPatterns) {
        string regexString = v.first;
        TokenType tokenType = v.second;

        regex rx(regexString);

        auto begin = sregex_iterator {_code.begin(), _code.end(), rx};
        auto end = sregex_iterator();

        for (sregex_iterator i = begin; i != end; ++i) {
            sort(_tokens.begin(), _tokens.end(), compareTokens);

            TokenType tokenTypeDynamic = tokenType;

            string str = i->str();

            for (pair<string, TokenType> pattern: _tokenTypesPatterns) {
                if (pattern.first == str && pattern != v) { tokenTypeDynamic = pattern.second; }
            }

            int strLen = str.length();
            int pos = i->position();
            int endPos = pos + strLen;

            bool block = false;

            for (auto v: busy) {
                if (pos >= v.start && endPos <= v.end) block = true;
            }

            if (block) continue;

            TokenPositionBusy p;
            p.start = pos;
            p.end = endPos;

            busy.push_back(p);

            Token token(str, tokenTypeDynamic, pos, endPos);

            _tokens.push_back(token);
        }
    }

    vector<Token> tokens = {};
    copy_if(_tokens.begin(), _tokens.end(), std::back_inserter(tokens), [](Token token) {
        return token.getType() != WHITESPACE && token.getType() != NEWLINE && token.getType() != SEMICOLON;
    });

    for (Token v: tokens) {
        cout << " [ " + getTokenTypeString(v.getType()) + " ] [ " + v.getValue() + " ] [ " + to_string(v.getPosition()) + " ] [ " + to_string(v.getEndPosition()) + " ] " << endl;
    }

    return tokens;
}