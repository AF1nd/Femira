#include <iostream>
#include <fstream>

#include "include/compiler.h"
#include "lexer/include/lexer.h"
#include "include/parser.h"
#include "include/bytecodeGenerator.h"
#include "../include/fvm.h"

using namespace std;

vector<Instruction> Compiler::compile(string code) {
    Lexer lexer(code);
    Parser parser(lexer.tokenize(false));

    BlockNode* ast = parser.parse();

    // for (auto v: ast->nodes) cout << v->tostr() << endl;

    vector<Instruction> bytecode = BytecodeGenerator(ast).generate();

    return bytecode;
}