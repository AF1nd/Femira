#ifndef PARSER_H
#define PARSER_H

#include <vector>
#include "../lexer/include/token.h"
#include <map>

using namespace std;

struct AstNode { 
    virtual ~AstNode() = default;
    virtual string tostr() { return "unknown"; };
};

struct UnaryOperationNode : AstNode {
    AstNode* operrand;
    Token operatorToken;

    UnaryOperationNode() = default;

    string tostr() override {
        return "[ unary: " + operatorToken.getValue() + ": " + operrand->tostr() + " ]";
    }
};

struct ArgsNode : AstNode {
    vector<AstNode*> nodes;
    string tostr() override {
        string str = "";

        for (AstNode* arg: nodes) {
            str += arg->tostr();
        }

        return str;
    }
};

struct LiteralNode : AstNode {
    Token token;
    LiteralNode() = default;

    string tostr() override {
        return "[ literal: " + token.getValue() + " ]";
    }
};

struct BlockNode : AstNode {
    vector<AstNode*> nodes;
    BlockNode() = default;

    string tostr() override {
        string str = "";

        for (AstNode* node: nodes) {
            str += "  > " + node->tostr() + "\n";
        }

        return "\n block: \n" + str + " end";
    }
};

struct BinaryOperationNode : AstNode {
    AstNode* left;
    AstNode* right;
    Token operatorToken;
    int priority;
    BinaryOperationNode() = default;

    string tostr() override {
        return "[ binary: "  + left->tostr() + " " + operatorToken.getValue() + " " + right->tostr() + + " | " + to_string(priority) + " ]";
    }
};

struct IdentifierNode : AstNode {
    Token token;
    IdentifierNode() = default;

    string tostr() override {
        return "[ id: " + token.getValue() + " ]";
    }
};

struct CallNode : AstNode {
    string tostr() override {
        return "[ call: " + calling->tostr() + " | " + args->tostr() + " ]";
    };

    AstNode* calling;
    ArgsNode* args;
    CallNode() = default;
};

struct ParenthisizedNode : AstNode {
    AstNode* wrapped;
    ParenthisizedNode() = default;

    string tostr() override {
        return "[ parenthisized: " + wrapped->tostr() + " ]";
    };
};

struct FnDefineNode : AstNode {
    IdentifierNode* id;
    ArgsNode* args;
    BlockNode* block;

    FnDefineNode() = default;

    string tostr() override {
        return "[ func: " + id->tostr() + " | " + args->tostr() + " | " + block->tostr() + " \n]";
    };
};

struct IfStatementNode : AstNode {
    BlockNode* block;
    BlockNode* elseBlock;
    AstNode* condition;

    IfStatementNode() = default;

    string tostr() override {
        return "[ if statement: " + condition->tostr() + " ]";
    }
};


class Parser {
    private:
        vector<Token> _tokens;
        int _position;

        vector<TokenType> unaryOperationsTokens;
        vector<TokenType> binaryOperationsTokens;
        vector<TokenType> literalTokens;

        map<TokenType, int> operatorPriorities;
    public: 
        Parser(vector<Token> tokens);
        BlockNode* parse();

        bool match(vector<TokenType> tokenTypes);
        bool lookMatch(vector<TokenType> tokenTypes, int offset);
        Token eat(vector<TokenType> tokenTypes);
        
        AstNode* parseExpression(bool isParenthisized = false);
        
        IdentifierNode* parseIdentifier();

        IfStatementNode* parseIfStatement();
        ParenthisizedNode* parseParenthisized();
        LiteralNode* parseLiteral();
        BlockNode* parseBlock();
        BinaryOperationNode* parseBinaryOperation(AstNode* left, bool isParenthisized);
        FnDefineNode* parseFunctionDefinition();
        CallNode* parseCall(AstNode* calling);
        ArgsNode* parseArgs();
        UnaryOperationNode* parseUnaryOperation();
};

#endif