#include <iostream>
#include <fstream>

#include "include/compiler.h"
#include "lexer/include/lexer.h"
#include "parser/parser.h"
#include "include/bytecodeGenerator.h"
#include "../include/fvm.h"

using namespace std;

vector<Instruction> Compiler::compile(string code) {
    Lexer lexer(code);
    Parser parser(lexer.tokenize(false));

    BlockNode* ast = parser.parse();

    //for (AstNode* node: ast->nodes) cout << node->tostr() << endl;

    vector<Instruction> bytecode = BytecodeGenerator(ast).generate();

    return bytecode;
}