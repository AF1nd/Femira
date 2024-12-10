#include "include/parser.h"
#include "lexer/include/token.h"
#include "lexer/include/lexer.h"

#include <iostream>
#include <vector>
#include <algorithm>
#include <iostream>

using namespace std;

Parser::Parser(vector<Token*> tokens) {
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
    };

    conditionTokens = {
        EQ, 
        NOTEQ, 
        BIGGER, 
        SMALLER, 
        BIGGER_OR_EQ, 
        SMALLER_OR_EQ,
        AND,
        OR
    };

    literalTokens = {
        STRING,
        NUMBER,
        NULLT,
        TRUE,
        FALSE
    };
}

Token* Parser::eat(vector<TokenType> tokenTypes) {
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
    
    Token* currentToken = _tokens.at(_position);
    if (find(tokenTypes.begin(), tokenTypes.end(), currentToken->getType()) != tokenTypes.end()) {
        _position++;
        return currentToken;
    }

    throw runtime_error("Unexpected token type, expected: " + expectedStr + ", given: " + getTokenTypeString(currentToken->getType()) + ", position: " + to_string(_position));
}

bool Parser::match(vector<TokenType> tokenTypes) {
    if (_position >= _tokens.size()) return false;
    
    Token* currentToken = _tokens.at(_position);
    if (find(tokenTypes.begin(), tokenTypes.end(), currentToken->getType()) != tokenTypes.end()) {
        return true;
    }

    return false;
}

bool Parser::lookMatch(vector<TokenType> tokenTypes, int offset) {
    int position = _position + offset;

    if (position >= _tokens.size()) return false;
    
    Token* currentToken = _tokens.at(position);
    if (find(tokenTypes.begin(), tokenTypes.end(), currentToken->getType()) != tokenTypes.end()) {
        return true;
    }

    return false;
}

BlockNode* Parser::parse() {
    BlockNode* block = new BlockNode();

    while (_position < _tokens.size()) {
        AstNode* expr = parseExpression();
        if (expr == nullptr) break;

        if (auto fndef = dynamic_cast<FnDefineNode*>(expr)) {
            if (fndef->isLambda) throw runtime_error("Syntax error! Cannot define lambda function in block");
        }

        block->nodes.push_back(expr);

        if (match({ SEMICOLON })) eat({ SEMICOLON });
    }

    return block;
}

AstNode* Parser::repeat(AstNode* node, bool onlyAtom) {
    AstNode* newNode = nullptr;

    if (match(binaryOperationsTokens) && !onlyAtom) {
        newNode = parseBinaryOperation(node);
    }

    if (match({ LSQUARE_BRACKET, DOT }) && !newNode) {
        newNode = parseIndexation(node);
    }

    if (match({ LBRACKET }) && !newNode) newNode = parseCall(node);
    
    if (match(conditionTokens) && !newNode) {
        newNode = parseCondition(node);
    }

    if (match({ ASSIGN }) && !newNode) {
        newNode = parseAssignment(node);
    }

    if (newNode != nullptr) {
        AstNode* repeatNewNode = repeat(newNode, onlyAtom);
        if (repeatNewNode) newNode = repeatNewNode;
    }

    return newNode;
}

AstNode* Parser::parseExpression(bool onlyAtom, bool noParenthisized) {
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
    if (match({ LOBJECT_BRACKET }) && !node) node = parseObject();
    if (match({ IF }) && !node) node = parseIfStatement();
    if (match({ LBRACKET }) && !node) {
        if (!noParenthisized) node = parseParenthisized(onlyAtom, noParenthisized);
        else {
            _position++;
            node = parseExpression();
            if (match({RBRACKET})) _position++;
        }
    }
    if (match({ LSQUARE_BRACKET }) && !node) {
        node = parseArray();
    }

    if (node) {
        AstNode* repeated = repeat(node, onlyAtom);
        if (repeated != nullptr) node = repeated;
    }

    return node;
}


ObjectNode* Parser::parseObject() {
    eat({ LOBJECT_BRACKET });

    map<AstNode*, AstNode*> fields;

    while (!match({ ROBJECT_BRACKET })) {
        if (!fields.empty()) eat({ COMMA, SEMICOLON });

        AstNode* expr = parseExpression();
        if (auto assignment = dynamic_cast<AssignmentNode*>(expr)) {
            fields.insert({ assignment->id, assignment->value });
        } else break;
    }

    eat({ ROBJECT_BRACKET });

    ObjectNode* node = new ObjectNode();
    node->fields = fields;

    return node;
}

IndexationNode* Parser::parseIndexation(AstNode* where) {
    bool isSquarable = false;
    if (match({ LSQUARE_BRACKET })) isSquarable = true;

    eat({ LSQUARE_BRACKET, DOT });

    AstNode* index = isSquarable ? parseExpression() : parseIdentifier();

    if (isSquarable) eat({ RSQUARE_BRACKET });

    IndexationNode* node = new IndexationNode();
    node->index = index;
    node->where = where;

    if (auto identifier = dynamic_cast<IdentifierNode*>(index)) {
        LiteralNode* literal = new LiteralNode();
        literal->token = new Token(identifier->token->value, STRING, identifier->token->getPosition(), identifier->token->getEndPosition());

        node->index = literal;
    }

    return node;
}

ConditionNode* Parser::parseCondition(AstNode* left) {
    Token* operatorToken = eat(conditionTokens);

    AstNode* right = parseExpression();

    ConditionNode* node = new ConditionNode();
    node->left = left;
    node->right = right;
    node->operatorToken = operatorToken;

    return node;
}

AssignmentNode* Parser::parseAssignment(AstNode* id) {
    eat({ ASSIGN });

    AstNode* expr = parseExpression();

    AssignmentNode* node = new AssignmentNode();
    node->id = id;
    node->value = expr;

    return node;
}

ArrayNode* Parser::parseArray() {
    eat({ LSQUARE_BRACKET });

    vector<AstNode*> elements;

    while (!match({ RSQUARE_BRACKET })) {
        AstNode* expr = parseExpression();
        if (expr != nullptr) {
            elements.push_back(expr);

            if (auto fndef = dynamic_cast<FnDefineNode*>(expr)) {
                if (!fndef->isLambda) throw runtime_error("Syntax error! Cannot define non-lambda function in array");
            }

            if (match({ COMMA })) eat({ COMMA });
        }
        else break;
    }

    eat({ RSQUARE_BRACKET });

    ArrayNode* node = new ArrayNode();
    node->elements = elements;

    return node;
}

IfStatementNode* Parser::parseIfStatement() {
    eat({ IF });

    AstNode* condition = parseExpression();

    if (!condition) throw runtime_error("Syntax error, after if needs condition");
    
    BlockNode* block = parseBlock();
    BlockNode* elseBlock = nullptr;

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
    Token* token = eat(literalTokens);

    LiteralNode* node = new LiteralNode();
    node->token = token;

    return node;
}

UnaryOperationNode* Parser::parseUnaryOperation() {
    Token* operatorToken = eat(unaryOperationsTokens);
    AstNode* expr = parseExpression();

    UnaryOperationNode* node = new UnaryOperationNode();
    node->operrand = expr;
    node->operatorToken = operatorToken;

    return node;
}

BlockNode* Parser::parseBlock() {
    Token* begin = eat({ BEGIN });

    vector<AstNode*> blockNodes = {};
    BlockNode* block = new BlockNode();

    while (_tokens.at(_position)->getType() != END) {
        AstNode* expr = parseExpression();
        if (expr == nullptr) break;

        if (auto fndef = dynamic_cast<FnDefineNode*>(expr)) {
            if (fndef->isLambda) throw runtime_error("Syntax error! Cannot define lambda function in block");
        }

        blockNodes.push_back(expr);

        if (match({ SEMICOLON })) eat({ SEMICOLON });
    }

    Token* end = eat({ END });

    block->nodes = blockNodes;

    return block;
};

AstNode* Parser::parseBinaryOperation(AstNode* left) {
    int lbrackets = 0;
    while (match({ LBRACKET })) {
        eat({ LBRACKET });
        lbrackets++;
    }

    bool isParsed = false;

    while (true) {
        if (match({ PLUS, MINUS })) {
            BinaryOperationNode* bin = new BinaryOperationNode();
            bin->left = left;
            bin->operatorToken = eat({ PLUS, MINUS });
            bin->right = prioritable();

            left = bin;

            isParsed = true;

            while (lbrackets > 0) {
                eat({ RBRACKET });
                lbrackets--;
            }

            continue;
        }
        break;
    }

    if (!isParsed) left = prioritable(left);
    
    return left;
};

AstNode* Parser::prioritable(AstNode* receivedLeft) {
    int lbrackets = 0;
    while (match({ LBRACKET })) {
        eat({ LBRACKET });
        lbrackets++;
    }

    AstNode* left = receivedLeft;
    if (!left) left = parseExpression(true, true);

    while (true) {
        if (match({ MUL, DIV })) {
            BinaryOperationNode* bin = new BinaryOperationNode();
            bin->left = left;
            bin->operatorToken = eat({ MUL, DIV });
            bin->right = parseExpression(true, true);

            left = bin;

            while (lbrackets > 0) {
                eat({ RBRACKET });
                lbrackets--;
            }

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
        
        if (expr != nullptr) {
            args.push_back(expr);
            if (auto fndef = dynamic_cast<FnDefineNode*>(expr)) {
                if (!fndef->isLambda) throw runtime_error("Syntax error! Cannot define non-lambda function in array");
            }
        } else break;

        if (match({ COMMA })) eat({ COMMA });
    }

    eat({ RBRACKET });

    ArgsNode* node = new ArgsNode();
    node->nodes = args;

    return node;
}

FnDefineNode* Parser::parseFunctionDefinition() {
    eat({ DEF });

    IdentifierNode* id = nullptr;

    if (!match({ LBRACKET })) {
       id = parseIdentifier();
    }

    ArgsNode* args = parseArgs();
    BlockNode* block = parseBlock();

    FnDefineNode* node = new FnDefineNode();
    node->args = args;
    if (id != nullptr) node->id = id;
    node->block = block;
    node->isLambda = id == nullptr;

    return node;
}

IdentifierNode* Parser::parseIdentifier() {
    Token* token = eat({ ID });

    IdentifierNode* node = new IdentifierNode();
    node->token = token;

    return node;
}