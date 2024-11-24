#include <iostream>
#include <vector>

#include "../parser/parser.h"
#include "vm.h"
#include "compiler.h"

vector<Instruction> bytecodeFromNode(AstNode* node) {
    if (auto* binaryOperation = dynamic_cast<BinaryOperationNode*>(node)) {
        vector<Instruction> left = bytecodeFromNode(binaryOperation->left);
        vector<Instruction> right = bytecodeFromNode(binaryOperation->right);

        vector<Instruction> bytecode = {};

        for (Instruction instr: left) {
            bytecode.push_back(instr);
        }

        for (Instruction instr: right) {
            bytecode.push_back(instr);
        }

        TokenType operatorType = binaryOperation->operatorToken.getType();
        
        if (operatorType == PLUS) {
            bytecode.push_back({
                code: B_ADD
            });
        } else if (operatorType == MINUS) {
            bytecode.push_back({
                code: B_SUB
            });
        } else if (operatorType == MUL) {
            bytecode.push_back({
                code: B_MUL
            });
        } else if (operatorType == DIV) {
            bytecode.push_back({
                code: B_DIV
            });
        }

        return bytecode;
    } else if (auto* literal = dynamic_cast<LiteralNode*>(node)) {
        string val = literal->token.getValue();

        if (literal->token.getType() == NUMBER) return { { code: B_PUSH, arg: stod(val) } };

        return { { code: B_PUSH, arg: val } };
    } else if (auto* unary = dynamic_cast<UnaryOperationNode*>(node)) {
        Token token = unary->operatorToken;
        if (token.getValue() == "delay") {
            vector<Instruction> bytecode = {};
            
            for (Instruction instr: bytecodeFromNode(unary->operrand)) bytecode.push_back(instr);
            bytecode.push_back({ code: B_DELAY });

            return bytecode;
        }
    } else if (auto* parenthisized = dynamic_cast<ParenthisizedNode*>(node)) {
        return bytecodeFromNode(parenthisized->wrapped);
    } else if (auto* args = dynamic_cast<ArgsNode*>(node)) {
        vector<Instruction> bytecode = {};

        for (AstNode* arg: args->nodes) {
            for (Instruction instr: bytecodeFromNode(arg)) {
                bytecode.push_back(instr);
            }
        }

        return bytecode;
    } else if (auto* call = dynamic_cast<CallNode*>(node)) {
        if (auto* calling = dynamic_cast<IdentifierNode*>(call->calling)) {
            if (calling->token.getValue() == "println") {
                vector<Instruction> bytecode = {};

                for (Instruction instr: bytecodeFromNode(call->args)) {
                    bytecode.push_back(instr);
                }

                bytecode.push_back({ code: B_PRINT });

                return bytecode;
            }
        }
    }

    return {};
}

Compiler::Compiler(BlockNode* ast) {
    this->ast = ast;
}

vector<Instruction> Compiler::generateBytecode() {
    vector<Instruction> bytecode = {};

    for (AstNode* node: ast->nodes) {
        for (Instruction instr: bytecodeFromNode(node)) bytecode.push_back(instr);
    }

    bytecode.push_back({ code: B_HALT });

    return bytecode;
}

using namespace std;