#include <iostream>
#include "include/parser.h"
#include "lexer/include/token.h"
#include "lexer/include/lexer.h"
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
        USING,
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
        AND,
        OR
    };

    prioritableBinOpTokens = {
        DIV,
        MUL,
        EQ, 
        NOTEQ, 
        BIGGER, 
        SMALLER, 
        BIGGER_OR_EQ, 
        SMALLER_OR_EQ,
        AND,
        OR,
    };

    literalTokens = {
        STRING,
        NUMBER,
        NULLT,
        TRUE,
        FALSE
    };
}

Token Parser::eat(vector<TokenType> tokenTypes) {
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

        if (match({ SEMICOLON })) eat({ SEMICOLON });
    }

    return block;
}

AstNode* Parser::parseExpression(bool onlyAtom, bool noParenthisized) {
    AstNode* node = nullptr;

    if (match({ DEF })) node = parseFunctionDefinition();
    if (match({ ID }) && !node) {
        if (lookMatch({ ASSIGN }, 1)) node = parseAssignment();

        if (!node) node = parseIdentifier();
    }; 
    if (match(literalTokens) && !node) {
        node = parseLiteral();
    };
    if (match(unaryOperationsTokens) && !node) {
        node = parseUnaryOperation();
    };
    if (match({ IF })) node = parseIfStatement();
    if (match({ LBRACKET }) && !node) {
        if (!noParenthisized) node = parseParenthisized(onlyAtom, noParenthisized);
        else {
            _position++;
            node = parseExpression();
            if (match({RBRACKET})) _position++;
        }
    }

    if (node) {
        if (match({ LBRACKET })) node = parseCall(node);
        if (match(binaryOperationsTokens) && !onlyAtom) {
            node = parseBinaryOperation(node);
        }
    }

    return node;
}

AssignmentNode* Parser::parseAssignment() {
    IdentifierNode* id = parseIdentifier();

    eat({ ASSIGN });

    AstNode* expr = parseExpression();

    AssignmentNode* node = new AssignmentNode();
    node->id = id;
    node->value = expr;

    return node;
}

IfStatementNode* Parser::parseIfStatement() {
    eat({ IF });

    AstNode* condition = parseExpression();

    if (!condition) throw runtime_error("Syntax error, after if needs condition");
    
    BlockNode* block = parseBlock();
    BlockNode* elseBlock;

    if (match({ ELSE })) {
        eat({ ELSE });

        elseBlock = parseBlock();
    }

    IfStatementNode* statement = new IfStatementNode();
    statement->block = block;
    statement->elseBlock = elseBlock;
    statement->condition = condition;

    return statement;
}

ParenthisizedNode* Parser::parseParenthisized(bool onlyAtom, bool noParenthisized)  {
    eat({ LBRACKET });

    AstNode* expr = parseExpression(onlyAtom, noParenthisized);

    ParenthisizedNode* node = new ParenthisizedNode();
    node->wrapped = expr;

    eat({ RBRACKET });

    return node;
}

LiteralNode* Parser::parseLiteral() {
    Token token = eat(literalTokens);

    LiteralNode* node = new LiteralNode();
    node->token = token;

    return node;
}

UnaryOperationNode* Parser::parseUnaryOperation() {
    Token operatorToken = eat(unaryOperationsTokens);
    AstNode* expr = parseExpression();

    UnaryOperationNode* node = new UnaryOperationNode();
    node->operrand = expr;
    node->operatorToken = operatorToken;

    return node;
}

BlockNode* Parser::parseBlock() {
    Token begin = eat({ BEGIN });

    vector<AstNode*> blockNodes = {};

    while (_tokens.at(_position).getType() != END) {
        AstNode* ptr = parseExpression();
        if (ptr == nullptr) break;

        blockNodes.push_back(ptr);

        if (match({ SEMICOLON })) eat({ SEMICOLON });
    }

    Token end = eat({ END });

    BlockNode* block = new BlockNode();
    block->nodes = blockNodes;

    return block;
};

AstNode* Parser::parseBinaryOperation(AstNode* left) {
    while (match({ LBRACKET })) eat({ LBRACKET });

    while (true) {
        if (match({ PLUS, MINUS })) {
            BinaryOperationNode* bin = new BinaryOperationNode();
            bin->left = left;
            bin->operatorToken = eat({ PLUS, MINUS });
            bin->right = prioritable();

            left = bin;

            while (match({ RBRACKET })) eat({ RBRACKET });

            continue;
        }
        break;
    }
    
    return left;
};

AstNode* Parser::prioritable(AstNode* receivedLeft) {
    while (match({ LBRACKET })) eat({ LBRACKET });

    AstNode* left = receivedLeft;
    if (!left) left = parseExpression(true, true);

    while (true) {
        if (match(prioritableBinOpTokens)) {
            BinaryOperationNode* bin = new BinaryOperationNode();
            bin->left = left;
            bin->operatorToken = eat(prioritableBinOpTokens);
            bin->right = parseExpression(true, true);

            left = bin;

            while (match({ RBRACKET })) eat({ RBRACKET });

            continue;
        }

        break;
    }

    return left;
}

CallNode* Parser::parseCall(AstNode* calling) {
    ArgsNode* args = parseArgs();

    CallNode* node = new CallNode();
    node->args = args;
    node->calling = calling;

    return node;
};

ArgsNode* Parser::parseArgs() {
    eat({ LBRACKET });

    vector<AstNode*> args = {};

    while (!match({ RBRACKET })) {
        AstNode* expr = parseExpression();
        
        if (expr != nullptr) args.push_back(expr);
        else break;

        if (match({ COMMA })) eat({ COMMA });
    }

    eat({ RBRACKET });

    ArgsNode* node = new ArgsNode();
    node->nodes = args;

    return node;
}

FnDefineNode* Parser::parseFunctionDefinition() {
    eat({ DEF });

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
    Token token = eat({ ID });

    IdentifierNode* node = new IdentifierNode();
    node->token = token;

    return node;
}