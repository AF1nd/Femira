#ifndef PARSER_H
#define PARSER_H

#include <vector>
#include "../lexer/token.h"

using namespace std;

struct AstNode { 
    virtual ~AstNode() = default;
    virtual string tostr() { return "unknown"; };
};

struct ArgsNode: AstNode {
    vector<AstNode> nodes;
};

struct LiteralNode : AstNode {
    Token token;
    LiteralNode() = default;
};

struct BlockNode : AstNode {
    vector<AstNode> nodes;
    BlockNode() = default;
};

struct BinaryOperationNode : AstNode {
    AstNode left;
    AstNode right;
    Token operatorToken;
    BinaryOperationNode() = default;
};

struct CallNode : public AstNode {
    string tostr() override {
        return "call";
    };

    AstNode calling;
    ArgsNode args;
    CallNode() = default;
};

struct IdentifierNode : AstNode {
    Token token;
    IdentifierNode() = default;
};

class Parser {
    private:
        vector<Token> _tokens;
        int _position;
    public: 
        Parser(vector<Token> tokens);
        BlockNode parse();

        Token match(vector<TokenType> tokenTypes);
        bool isCurrentToken(vector<TokenType> tokenTypes);
        
        AstNode parseExpression();
        IdentifierNode parseIdentifier();
        LiteralNode parseLiteral();
        BlockNode parseBlock();
        BinaryOperationNode parseBinaryOperation();
        CallNode parseCall();
        ArgsNode parseArgs();
};

#endif