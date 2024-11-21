#include <iostream>
#include "parser.h"
#include "../lexer/token.h"
#include "../lexer/lexer.h"
#include <vector>
#include <algorithm>

using namespace std;


Parser::Parser(vector<Token> tokens) {
    _tokens = tokens;
    _position = 0;
}

Token Parser::consume(vector<TokenType> tokenTypes) {
    if (_position >= _tokens.size()) {
        throw runtime_error("Position out of bounds");
    }
    
    Token currentToken = _tokens.at(_position);

    if (find(tokenTypes.begin(), tokenTypes.end(), currentToken.getType()) != tokenTypes.end()) {
        _position++;
        return currentToken;
    }

    vector<string> types = {};

    for (TokenType v: tokenTypes) {
        types.push_back(getTokenTypeString(v));
    }

    string str = "";

    for (string v: types) {
        str += v + " ";
    }
    
    throw runtime_error("Unexpected token type, expected: " + str + ", given: " + getTokenTypeString(currentToken.getType()) + ", position: " + to_string(_position));
}

bool Parser::match(vector<TokenType> tokenTypes) {
    if (_position >= _tokens.size()) {
        throw runtime_error("Position out of bounds");
    }
    
    Token currentToken = _tokens.at(_position);

    if (find(tokenTypes.begin(), tokenTypes.end(), currentToken.getType()) != tokenTypes.end()) {
        return true;
    }

    return false;
}

bool Parser::lookMatch(vector<TokenType> tokenTypes, int offset) {
    int position = _position + offset;

    if (position >= _tokens.size()) {
        throw runtime_error("Position out of bounds");
    }
    
    Token currentToken = _tokens.at(position);

    if (find(tokenTypes.begin(), tokenTypes.end(), currentToken.getType()) != tokenTypes.end()) {
        return true;
    }

    return false;
}

BlockNode* Parser::parse() {
    BlockNode* block = new BlockNode();

    while (_position < _tokens.size()) {
        AstNode* expr = parseExpression();
        if (expr == nullptr) break;

        block->nodes.push_back(expr);
    }

    return block;
}

AstNode* Parser::parseExpression(vector<string> exclude) {
    if (match({ STRING, NUMBER })) {
        if (lookMatch({ MUL, DIV, PLUS, MINUS, EQ, NOTEQ, BIGGER, SMALLER, BIGGER_OR_EQ, SMALLER_OR_EQ }, 1) && count(exclude.begin(), exclude.end(), "bin") <= 0) return parseBinaryOperation();
        else return parseLiteral();
    };
    if (match({ ID })) {
        if (lookMatch({ LBRACKET }, 1) && count(exclude.begin(), exclude.end(), "call") <= 0) return parseCall();
        else return parseIdentifier();
    };
    if (match({ DEF })) return parseFunctionDefinition();

    return 0;
}

AstNode* Parser::parseExpression() {
    if (match({ STRING, NUMBER })) {
        if (lookMatch({ MUL, DIV, PLUS, MINUS, EQ, NOTEQ, BIGGER, SMALLER, BIGGER_OR_EQ, SMALLER_OR_EQ }, 1)) return parseBinaryOperation();
        else return parseLiteral();
    };
    if (match({ ID })) {
        if (lookMatch({ LBRACKET }, 1)) return parseCall();
        else return parseIdentifier();
    };
    if (match({ DEF })) return parseFunctionDefinition();

    return 0;
}


LiteralNode* Parser::parseLiteral() {
    Token token = consume({ STRING, NUMBER });

    LiteralNode* node = new LiteralNode();
    node->token = token;
    
    return node;
}

BlockNode* Parser::parseBlock() {
    Token begin = consume({ BEGIN });

    vector<AstNode*> blockNodes = {};

    while (_tokens.at(_position).getType() != END) {
        AstNode* ptr = parseExpression();
        if (ptr == nullptr) break;

        blockNodes.push_back(ptr);
    }

    Token end = consume({ END });

    BlockNode* block = new BlockNode();
    block->nodes = blockNodes;

    return block;
};

BinaryOperationNode* Parser::parseBinaryOperation() {
    AstNode* left = parseExpression({ "bin" });

    Token operatorToken = consume({ MUL, DIV, PLUS, MINUS, EQ, NOTEQ, BIGGER, SMALLER, BIGGER_OR_EQ, SMALLER_OR_EQ });

    AstNode* right = parseExpression();

    BinaryOperationNode* node = new BinaryOperationNode();
    
    node->left = left;
    node->operatorToken = operatorToken;
    node->right = right;

    return node;
};

CallNode* Parser::parseCall() {
    AstNode* calling = parseExpression({ "call" });
    ArgsNode* args = parseArgs();

    CallNode* node = new CallNode();
    node->args = args;
    node->calling = calling;

    return node;
};

ArgsNode* Parser::parseArgs() {
    consume({ LBRACKET });

    vector<AstNode*> args = {};

    while (_tokens.at(_position).getType() != RBRACKET) {
        if (!args.empty()) consume({ COMMA });

        AstNode* ptr = parseExpression();
        if (ptr == nullptr) break;

        args.push_back(ptr);
    }

    consume({ RBRACKET });

    ArgsNode* node = new ArgsNode();
    node->nodes = args;

    return node;
}

FnDefineNode* Parser::parseFunctionDefinition() {
    consume({ DEF });

    IdentifierNode* id = parseIdentifier();
    ArgsNode* args = parseArgs();
    BlockNode* block = parseBlock();

    FnDefineNode* node = new FnDefineNode();
    node->args = args;
    node->id = id;
    node->block = block;

    return node;
}

IdentifierNode* Parser::parseIdentifier() {
    Token token = consume({ ID });

    IdentifierNode* node = new IdentifierNode();
    node->token = token;

    return node;
}