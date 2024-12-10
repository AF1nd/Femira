#include <vector>
#include <algorithm>
#include <fstream> 
#include <string>

#include "lexer/include/lexer.h"
#include "include/parser.h"
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

map<ArrayNode*, shared_ptr<InstructionArrayOperrand>> arrayLinks;

shared_ptr<InstructionOperrand> getOperrandFromNode(AstNode* node) {
    if (IdentifierNode* identifier = dynamic_cast<IdentifierNode*>(node)) {
        return make_shared<InstructionStringOperrand>(identifier->token->value);
    }  else if (LiteralNode* literal = dynamic_cast<LiteralNode*>(node)) {
        Token* token = literal->token;
        TokenType literalType = token->getType();

        if (literalType == NUMBER) {
            return make_shared<InstructionNumberOperrand>(stod(token->value));
        } else if (literalType == STRING) {
            return make_shared<InstructionStringOperrand>(token->value);
        } else if (literalType == TRUE) {
            return make_shared<InstructionBoolOperrand>(true);
        } else if (literalType == FALSE) {
            return make_shared<InstructionBoolOperrand>(false);
        } else if (literalType == NULLT) {
            return make_shared<InstructionNullOperrand>();
        }
    } else if (ArrayNode* array = dynamic_cast<ArrayNode*>(node)) {
        shared_ptr<vector<shared_ptr<InstructionOperrand>>> elements = make_shared<vector<shared_ptr<InstructionOperrand>>>();
        
        for (AstNode* element: array->elements) {
            elements->push_back(getOperrandFromNode(element));
        }

        shared_ptr<InstructionArrayOperrand> operrand = make_shared<InstructionArrayOperrand>(elements);

        return operrand;
    } else if (ObjectNode* object = dynamic_cast<ObjectNode*>(node)) {
        shared_ptr<map<string, shared_ptr<InstructionOperrand>>> fields = make_shared<map<string, shared_ptr<InstructionOperrand>>>();

        for (pair<AstNode*, AstNode*> field: object->fields) {
            shared_ptr<InstructionOperrand> index = getOperrandFromNode(field.first);
            shared_ptr<InstructionOperrand> value = getOperrandFromNode(field.second);
            
            if (auto indexCasted = dynamic_pointer_cast<InstructionStringOperrand>(index)) {
                fields->insert({ indexCasted->operrand, value });
            }
        }

        return make_shared<InstructionObjectOperrand>(fields);
    } else if (FnDefineNode* fnDefine = dynamic_cast<FnDefineNode*>(node)) {
        vector<string> argsIds;

        for (AstNode* arg: fnDefine->args->nodes) {
            if (IdentifierNode* id = dynamic_cast<IdentifierNode*>(arg)) argsIds.push_back(id->token->value);
            else throw runtime_error("Compile error! Argument in function define statement must be a identifier");
        }

        BytecodeGenerator bgen(fnDefine->block);

        shared_ptr<InstructionFunctionOperrand> operrand;
        if (!fnDefine->isLambda) operrand = make_shared<InstructionFunctionOperrand>(FuncDeclaration(bgen.generate(), argsIds, fnDefine->id->token->value));
        else operrand = make_shared<InstructionFunctionOperrand>(FuncDeclaration(bgen.generate(), argsIds));

        return operrand;
    }

    throw runtime_error("Compile error! Node " + node->tostr() + " can't return operrand");

    return nullptr;
}

void BytecodeGenerator::visitNode(AstNode* node) {
    if (BlockNode* block = dynamic_cast<BlockNode*>(node)) {
        for (AstNode* node: block->nodes) visitNode(node);
    } else {
        if (BinaryOperationNode* binary = dynamic_cast<BinaryOperationNode*>(node)) {
            Token* operatorToken = binary->operatorToken;
            TokenType operatorType = operatorToken->getType();

            visitNode(binary->left);
            visitNode(binary->right);

            Instruction instr;

            if (operatorType == PLUS) instr = Instruction(Bytecode(F_ADD));
            else if (operatorType == MINUS) instr = Instruction(Bytecode(F_SUB));
            else if (operatorType == MUL) instr = Instruction(Bytecode(F_MUL));
            else if (operatorType == DIV) instr = Instruction(Bytecode(F_DIV));

            bytecode.push_back(instr);
        } else if (ConditionNode* condition = dynamic_cast<ConditionNode*>(node)) {
            Token* operatorToken = condition->operatorToken;
            TokenType operatorType = operatorToken->getType();

            visitNode(condition->left);
            visitNode(condition->right);

            Instruction instr;

            if (operatorType == EQ) instr = Instruction(Bytecode(F_EQ));
            else if (operatorType == NOTEQ) instr = Instruction(Bytecode(F_NOTEQ));
            else if (operatorType == BIGGER) instr = Instruction(Bytecode(F_BIGGER));
            else if (operatorType == SMALLER) instr = Instruction(Bytecode(F_SMALLER));

            else if (operatorType == BIGGER_OR_EQ) instr = Instruction(Bytecode(F_BIGGER_OR_EQ));
            else if (operatorType == SMALLER_OR_EQ) instr = Instruction(Bytecode(F_SMALLER_OR_EQ));
            else if (operatorType == AND) instr = Instruction(Bytecode(F_AND));
            else if (operatorType == OR) instr = Instruction(Bytecode(F_OR));

            bytecode.push_back(instr);
        } else if (AssignmentNode* assignment = dynamic_cast<AssignmentNode*>(node)) {
            AstNode* id = assignment->id;
            if (IdentifierNode* identifier = dynamic_cast<IdentifierNode*>(id)) {
                visitNode(assignment->value);
                bytecode.push_back(Instruction(Bytecode(F_SETGLOBAL), make_shared<InstructionStringOperrand>(identifier->token->value)));
            } else if (IndexationNode* indexation = dynamic_cast<IndexationNode*>(id)) {
                visitNode(indexation->where);
                visitNode(assignment->value);
                visitNode(indexation->index);
                bytecode.push_back(Instruction(Bytecode(F_SETINDEX)));
            }
        } else if (LiteralNode* literal = dynamic_cast<LiteralNode*>(node)) {
             bytecode.push_back(Instruction(Bytecode(F_PUSH), getOperrandFromNode(literal)));
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
            Token* token = unary->operatorToken;
            TokenType unaryType = token->getType();

            if (unaryType == USING) {
                AstNode* operrand = unary->operrand;
                if (LiteralNode* operrandCasted = dynamic_cast<LiteralNode*>(operrand)) {
                    if (operrandCasted->token->getType() == STRING) {
                        string path = operrandCasted->token->value;
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
            bytecode.push_back(Instruction(Bytecode(F_GETGLOBAL), getOperrandFromNode(identifier)));
        } else if (ParenthisizedNode* parenthisized = dynamic_cast<ParenthisizedNode*>(node)) {
            visitNode(parenthisized->wrapped);
        } else if (FnDefineNode* fnDefine = dynamic_cast<FnDefineNode*>(node)) {
            bytecode.push_back(Instruction(Bytecode(F_PUSH), getOperrandFromNode(fnDefine)));

            if (!fnDefine->isLambda) bytecode.push_back(Instruction(Bytecode(F_SETGLOBAL), make_shared<InstructionStringOperrand>(fnDefine->id->token->value)));
        } else if (CallNode* call = dynamic_cast<CallNode*>(node)) {
            reverse(call->args->nodes.begin(), call->args->nodes.end());
            
            for (AstNode* arg: call->args->nodes) {
                visitNode(arg);
            }

            visitNode(call->calling);

            bytecode.push_back(Instruction(Bytecode(F_CALL)));
        } else if (ArrayNode* array = dynamic_cast<ArrayNode*>(node)) {
            bytecode.push_back(Instruction(Bytecode(F_PUSH), getOperrandFromNode(array)));
        } else if (ObjectNode* object = dynamic_cast<ObjectNode*>(node)) {
            bytecode.push_back(Instruction(Bytecode(F_PUSH), getOperrandFromNode(object)));
        } else if (IndexationNode* indexation = dynamic_cast<IndexationNode*>(node)) {
            visitNode(indexation->where);
            visitNode(indexation->index);
        
            bytecode.push_back(Instruction(Bytecode(F_INDEXATION)));
        }
    }
}

vector<Instruction> BytecodeGenerator::generate() {
    visitNode(root);
    
    return bytecode;
}