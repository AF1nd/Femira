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
            str += " >" + node->tostr() + "\n";
        }

        return "\n[ block: \n" + str + " \n]";
    }
};

struct BinaryOperationNode : AstNode {
    AstNode* left;
    AstNode* right;
    Token operatorToken;
    BinaryOperationNode() = default;

    string tostr() override {
        return "[ binary: " + operatorToken.getValue() + " | " + left->tostr() + " | " + right->tostr() + " ]";
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
        return "[ func: " + id->tostr() + " | " + args->tostr() + " | " + block->tostr() + " ]";
    };
};

class Parser {
    private:
        vector<Token> _tokens;
        int _position;
    public: 
        Parser(vector<Token> tokens);
        BlockNode* parse();

        bool match(vector<TokenType> tokenTypes);
        bool lookMatch(vector<TokenType> tokenTypes, int offset);
        Token consume(vector<TokenType> tokenTypes);
        
        AstNode* parseExpression();
        AstNode* parseExpression(vector<string> exclude);
        
        IdentifierNode* parseIdentifier();
        LiteralNode* parseLiteral();
        BlockNode* parseBlock();
        BinaryOperationNode* parseBinaryOperation();
        FnDefineNode* parseFunctionDefinition();
        CallNode* parseCall();
        ArgsNode* parseArgs();
};

#endif