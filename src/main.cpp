#include <iostream>
#include <vector>

#include "compiler/lexer/lexer.h"
#include "compiler/parser/parser.h"
#include "compiler/bytecodeGenerator.h"
#include "fvm/fvm.h"

using namespace std;

int main() {
    const string code =
        R"(
            fn get(a, b):
                return a ? b
            end

            output get(null, 5)
            output get(10, 5)
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

    FVM vm(bytecode, false);

    cout << "<RESULT>" << endl;
    
    vm.run();

    return 0;
}
