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

    unaryOperationsTokens = {
        RETURN,
        DELAY,
        OUTPUT,
    };

    binaryOperationsTokens = {
        MUL,
        DIV,
        PLUS, 
        MINUS, 
        EQ, 
        NOTEQ, 
        BIGGER, 
        SMALLER, 
        BIGGER_OR_EQ, 
        SMALLER_OR_EQ,
        ASSIGN,
        AND
    };

    literalTokens = {
        STRING,
        NUMBER,
        NULLT,
        TRUE,
        FALSE
    };
}

Token Parser::consume(vector<TokenType> tokenTypes) {
    vector<string> types = {};

    for (TokenType v: tokenTypes) {
        types.push_back(getTokenTypeString(v));
    }

    string expectedStr = "";

    for (string v: types) {
        expectedStr += v + " ";
    }

    if (_position >= _tokens.size()) {
        throw runtime_error("In the end of file expected token: " + expectedStr);
    }
    
    Token currentToken = _tokens.at(_position);
    if (find(tokenTypes.begin(), tokenTypes.end(), currentToken.getType()) != tokenTypes.end()) {
        _position++;
        return currentToken;
    }

    throw runtime_error("Unexpected token type, expected: " + expectedStr + ", given: " + getTokenTypeString(currentToken.getType()) + ", position: " + to_string(_position));
}

bool Parser::match(vector<TokenType> tokenTypes) {
    if (_position >= _tokens.size()) return false;
    
    Token currentToken = _tokens.at(_position);
    if (find(tokenTypes.begin(), tokenTypes.end(), currentToken.getType()) != tokenTypes.end()) {
        return true;
    }

    return false;
}

bool Parser::lookMatch(vector<TokenType> tokenTypes, int offset) {
    int position = _position + offset;

    if (position >= _tokens.size()) return false;
    
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

        if (match({ SEMICOLON })) consume({ SEMICOLON });
    }

    return block;
}

AstNode* Parser::parseExpression() {
    AstNode* node = nullptr;

    if (match({ DEF })) node = parseFunctionDefinition();
    if (match({ ID }) && !node) {
        node = parseIdentifier();
    };
    if (match(literalTokens) && !node) {
        node = parseLiteral();
    };
    if (match(unaryOperationsTokens) && !node) {
        node = parseUnaryOperation();
    };
    if (match({ LBRACKET }) && !node) node = parseParenthisized();

    if (node) {
        if (match({ LBRACKET })) node = parseCall(node);
        if (match(binaryOperationsTokens)) {
            node = parseBinaryOperation(node);
        }
    }

    return node;
}

ParenthisizedNode* Parser::parseParenthisized() {
    consume({ LBRACKET });

    AstNode* expr = parseExpression();

    ParenthisizedNode* node = new ParenthisizedNode();
    node->wrapped = expr;

    consume({ RBRACKET });

    return node;
}

LiteralNode* Parser::parseLiteral() {
    Token token = consume(literalTokens);

    LiteralNode* node = new LiteralNode();
    node->token = token;

    return node;
}

UnaryOperationNode* Parser::parseUnaryOperation() {
    Token operatorToken = consume(unaryOperationsTokens);
    AstNode* expr = parseExpression();

    UnaryOperationNode* node = new UnaryOperationNode();
    node->operrand = expr;
    node->operatorToken = operatorToken;

    return node;
}

BlockNode* Parser::parseBlock() {
    Token begin = consume({ BEGIN });

    vector<AstNode*> blockNodes = {};

    while (_tokens.at(_position).getType() != END) {
        AstNode* ptr = parseExpression();
        if (ptr == nullptr) break;

        blockNodes.push_back(ptr);

        if (match({ SEMICOLON })) consume({ SEMICOLON });
    }

    Token end = consume({ END });

    BlockNode* block = new BlockNode();
    block->nodes = blockNodes;

    return block;
};

BinaryOperationNode* Parser::parseBinaryOperation(AstNode* left) {
    Token operatorToken = consume(binaryOperationsTokens);

    AstNode* right = parseExpression();

    BinaryOperationNode* node = new BinaryOperationNode();
    
    node->left = left;
    node->operatorToken = operatorToken;
    node->right = right;
     
    if (BinaryOperationNode* binary = dynamic_cast<BinaryOperationNode*>(right)) {
        
    }

    return node;
};

CallNode* Parser::parseCall(AstNode* calling) {
    ArgsNode* args = parseArgs();

    CallNode* node = new CallNode();
    node->args = args;
    node->calling = calling;

    return node;
};

ArgsNode* Parser::parseArgs() {
    consume({ LBRACKET });

    vector<AstNode*> args = {};

    while (!match({ RBRACKET })) {
        AstNode* expr = parseExpression();
        
        if (expr != nullptr) args.push_back(expr);
        else break;

        if (match({ COMMA })) consume({ COMMA });
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

    cout << id->tostr() << endl;

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