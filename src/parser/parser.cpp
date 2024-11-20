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

Token Parser::match(vector<TokenType> tokenTypes) {
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

BlockNode* Parser::parse() {
    BlockNode* block = new BlockNode();

    while (_position < _tokens.size()) {
        AstNode* ptr = parseExpression();

        if (ptr == NULL) {
            break;
        } else block->nodes.push_back(ptr);
    }

    return block;
}

AstNode* Parser::parseExpression() {
    try {
        return parseCall();
    } catch (const runtime_error&) {
      
    }

    try {
        return parseLiteral();
    } catch (const runtime_error&) {
      
    }

    throw runtime_error("Syntax error: Unable to parse expression at position " + to_string(_position));
}

LiteralNode* Parser::parseLiteral() {
    Token token = match({ STRING, NUMBER });

    LiteralNode* node = new LiteralNode();
    node->token = token;
    
    return node;
}

BlockNode* Parser::parseBlock() {
    Token begin = match({ BEGIN });

    vector<AstNode*> blockNodes = {};

    for (int i = _position; i < _tokens.size(); i++) {
        if ((_position) >= _tokens.size()) break;

        Token atPos = _tokens.at(_position);
        if (atPos.getType() == END) break;

        AstNode* ptr = parseExpression();

        blockNodes.push_back(ptr);
        _position++;
    }

    Token end = match({ END });

    BlockNode* block = new BlockNode();
    block->nodes = blockNodes;

    return block;
};

BinaryOperationNode* Parser::parseBinaryOperation() {
    AstNode* left = parseExpression();

    Token operatorToken = match({ MUL, DIV, PLUS, MINUS, EQ, NOTEQ, BIGGER, SMALLER, BIGGER_OR_EQ, SMALLER_OR_EQ });

    AstNode* right = parseExpression();

    BinaryOperationNode* node = new BinaryOperationNode();
    
    node->left = left;
    node->operatorToken = operatorToken;
    node->right = right;

    return node;
};

CallNode* Parser::parseCall() {
    AstNode* calling = parseIdentifier();
    ArgsNode* args = parseArgs();

    CallNode* node = new CallNode();
    node->args = args;
    node->calling = calling;

    return node;
};

ArgsNode* Parser::parseArgs() {
    Token lbracket = match({ LBRACKET });

    vector<AstNode*> args = {};

    for (int i = _position; i < _tokens.size(); i++) {
        Token atPos = _tokens.at(_position);
        if (atPos.getType() == RBRACKET) break;

        if (args.size() > 0) match({ COMMA });

        AstNode* ptr = parseExpression();
        if (ptr == NULL) break;

        args.push_back(ptr);
    }

    Token rbracket = match({ RBRACKET });

    ArgsNode* node = new ArgsNode();
    node->nodes = args;

    return node;
}

IdentifierNode* Parser::parseIdentifier() {
    Token token = match({ ID });

    IdentifierNode* node = new IdentifierNode();
    node->token = token;

    return node;
}