
#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream> 
#include <string>

#include "lexer/include/lexer.h"
#include "parser/parser.h"
#include "../include/fvm.h"
#include "include/bytecodeGenerator.h"
#include "include/compiler.h"

string readFile(string path) {
    ifstream file(path);

    string code;
    string line;

    while (getline(file, line)) {
        code += line + "\n";
    }

    file.close();

    return code;
}

using namespace std;

BytecodeGenerator::BytecodeGenerator(BlockNode* root) {
    this->root = root;
}

void BytecodeGenerator::visitNode(AstNode* node) {
    if (BlockNode* block = dynamic_cast<BlockNode*>(node)) {
        for (AstNode* node: block->nodes) visitNode(node);
    } else {
        if (BinaryOperationNode* binary = dynamic_cast<BinaryOperationNode*>(node)) {
            Token operatorToken = binary->operatorToken;
            TokenType operatorType = operatorToken.getType();

            if (operatorType == ASSIGN) {
                AstNode* left = binary->left;
                if (IdentifierNode* identifier = dynamic_cast<IdentifierNode*>(left)) {
                    visitNode(binary->right);
                    
                    bytecode.push_back(Instruction(Bytecode(F_SETVAR), make_shared<InstructionStringOperrand>(identifier->token.getValue())));
                    return;
                } else throw runtime_error("Compile error! Left operrand of assign operration must be a identifier");
            }

            visitNode(binary->left);
            visitNode(binary->right);

            Instruction instr;

            if (operatorType == PLUS) instr = Instruction(Bytecode(F_ADD));
            else if (operatorType == MINUS) instr = Instruction(Bytecode(F_SUB));
            else if (operatorType == MUL) instr = Instruction(Bytecode(F_MUL));
            else if (operatorType == DIV) instr = Instruction(Bytecode(F_DIV));
            
            else if (operatorType == EQ) instr = Instruction(Bytecode(F_EQ));
            else if (operatorType == NOTEQ) instr = Instruction(Bytecode(F_NOTEQ));
            else if (operatorType == BIGGER) instr = Instruction(Bytecode(F_BIGGER));
            else if (operatorType == SMALLER) instr = Instruction(Bytecode(F_SMALLER));

            else if (operatorType == BIGGER_OR_EQ) instr = Instruction(Bytecode(F_BIGGER_OR_EQ));
            else if (operatorType == SMALLER_OR_EQ) instr = Instruction(Bytecode(F_SMALLER_OR_EQ));
            else if (operatorType == AND) instr = Instruction(Bytecode(F_AND));
            else if (operatorType == OR) instr = Instruction(Bytecode(F_OR));

            bytecode.push_back(instr);
        } else if (LiteralNode* literal = dynamic_cast<LiteralNode*>(node)) {
            Token token = literal->token;
            TokenType literalType = token.getType();

            if (literalType == NUMBER) {
                bytecode.push_back(Instruction(Bytecode(F_PUSH), make_shared<InstructionNumberOperrand>(stod(token.getValue()))));
            } else if (literalType == STRING) {
                bytecode.push_back(Instruction(Bytecode(F_PUSH), make_shared<InstructionStringOperrand>(token.getValue())));
            } else if (literalType == TRUE) {
                bytecode.push_back(Instruction(Bytecode(F_PUSH), make_shared<InstructionBoolOperrand>(true)));
            } else if (literalType == FALSE) {
                bytecode.push_back(Instruction(Bytecode(F_PUSH), make_shared<InstructionBoolOperrand>(false)));
            } else if (literalType == NULLT) {
                bytecode.push_back(Instruction(Bytecode(F_PUSH), make_shared<InstructionNullOperrand>()));
            }
        } else if (IfStatementNode* ifStatement = dynamic_cast<IfStatementNode*>(node)) {
            BytecodeGenerator bgen(ifStatement->block);

            visitNode(ifStatement->condition);

            if (ifStatement->elseBlock) {
                BytecodeGenerator bgenElse(ifStatement->elseBlock);
                
                bytecode.push_back(Instruction(Bytecode(F_IF), make_shared<InstructionIfStatementLoadOperrand>(
                    IfStatement(bgen.generate(), bgenElse.generate())
                )));
                return;
            };

            bytecode.push_back(Instruction(Bytecode(F_IF), make_shared<InstructionIfStatementLoadOperrand>(
                IfStatement(bgen.generate())
            )));
        } else if (UnaryOperationNode* unary = dynamic_cast<UnaryOperationNode*>(node)) {
            Token token = unary->operatorToken;
            TokenType unaryType = token.getType();

            if (unaryType == USING) {
                AstNode* operrand = unary->operrand;
                if (LiteralNode* operrandCasted = dynamic_cast<LiteralNode*>(operrand)) {
                    if (operrandCasted->token.getType() == STRING) {
                        string path = operrandCasted->token.getValue();
                        path = path.substr(1, path.length() - 2);

                        string code = readFile(path);

                        Compiler newCompiler;
                        vector<Instruction> importedBytecode = newCompiler.compile(code);

                        for (Instruction importedInstruction: importedBytecode) {
                            bytecode.insert(bytecode.begin(), importedInstruction);
                        }

                        return;
                    }
                }

                throw runtime_error("Compile error! Cant import module");
            }

            visitNode(unary->operrand);

            if (unaryType == RETURN) bytecode.push_back(Instruction(Bytecode(F_RETURN)));
            else if (unaryType == DELAY) bytecode.push_back(Instruction(Bytecode(F_DELAY)));
            else if (unaryType == OUTPUT) bytecode.push_back(Instruction(Bytecode(F_OUTPUT)));
        } else if (IdentifierNode* identifier = dynamic_cast<IdentifierNode*>(node)) {
            bytecode.push_back(Instruction(Bytecode(F_GETVAR), make_shared<InstructionStringOperrand>(identifier->token.getValue())));
        } else if (ParenthisizedNode* parenthisized = dynamic_cast<ParenthisizedNode*>(node)) {
            visitNode(parenthisized->wrapped);
        } else if (FnDefineNode* fnDefine = dynamic_cast<FnDefineNode*>(node)) {
            vector<string> argsIds;

            for (AstNode* arg: fnDefine->args->nodes) {
                if (IdentifierNode* id = dynamic_cast<IdentifierNode*>(arg)) argsIds.push_back(id->token.getValue());
                else throw runtime_error("Compile error! Argument in function define statement must be a identifier");
            }

            BytecodeGenerator bgen(fnDefine->block);
            
            bytecode.push_back(Instruction(Bytecode(F_LOADFUNC), make_shared<InstructionFunctionLoadOperrand>(
                FuncDeclaration(bgen.generate(), argsIds, fnDefine->id->token.getValue())
            )));
        } else if (CallNode* call = dynamic_cast<CallNode*>(node)) {
            reverse(call->args->nodes.begin(), call->args->nodes.end());
            
            for (AstNode* arg: call->args->nodes) {
                visitNode(arg);
            }

            string fnId;

            if (IdentifierNode* calling = dynamic_cast<IdentifierNode*>(call->calling)) fnId = calling->token.getValue();
            else if (FnDefineNode* calling = dynamic_cast<FnDefineNode*>(call->calling)) fnId = calling->id->token.getValue();
            else throw runtime_error("Compile error! Unknown object to call");

            bytecode.push_back(Instruction(Bytecode(F_CALL), make_shared<InstructionStringOperrand>(fnId)));
        }
    }
}

vector<Instruction> BytecodeGenerator::generate() {
    visitNode(root);
    
    return bytecode;
}