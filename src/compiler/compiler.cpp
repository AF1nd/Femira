#include <iostream>
#include <fstream>

#include "compiler.h"
#include "lexer/lexer.h"
#include "parser/parser.h"
#include "bytecodeGenerator.h"
#include "../fvm/fvm.h"

using namespace std;

vector<Instruction> Compiler::compile(string code) {
    Lexer lexer(code);
    Parser parser(lexer.tokenize(false));

    BlockNode* ast = parser.parse();

    //for (AstNode* node: ast->nodes) cout << node->tostr() << endl;

    vector<Instruction> bytecode = BytecodeGenerator(ast).generate();

    return bytecode;
}