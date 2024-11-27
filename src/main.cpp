#include <iostream>
#include <vector>

#include "compiler/lexer/lexer.h"
#include "compiler/parser/parser.h"
#include "compiler/bytecodeGenerator.h"
#include "fvm/fvm.h"

using namespace std;

int main() {
    cout << "Femlang started work" << endl;

    const string code =
        R"(
            def foo(a, b, c):
                def helper(a, b):
                    output c
                    return a + b
                end
                return helper(a, b)
            end

            output foo(1, 2, 'zv')
        )";

    Lexer newLexer(code);

    vector<Token> tokens = newLexer.tokenize();

    Parser newParser(tokens);

    BlockNode* ast = newParser.parse();

    cout << "AST:" << endl;

    for (AstNode* v: ast->nodes) {
        cout << v->tostr() << endl;
    };

    // FVM

    auto bytecode = BytecodeGenerator(ast).generate();

    FVM vm(bytecode);

    cout << vm.readBytecode() << endl;

    cout << "<RESULT>" << endl;
    
    vm.run();

    return 0;
}