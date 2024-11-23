#include <iostream>
#include <vector>

#include "lexer/lexer.h"
#include "parser/parser.h"
#include "vm/vm.h"

using namespace std;

int main() {
    cout << "Femlang started work" << endl;

    const string code =
        R"(
            a = 5

            def foo(arg):
                return arg
            end

            delay 5;

            (print)(foo(5))
        )";

    Lexer newLexer(code);

    vector<Token> tokens = newLexer.tokenize();

    Parser newParser(tokens);

    BlockNode* ast = newParser.parse();

    cout << "AST:" << endl;

    for (AstNode* v: ast->nodes) {
        cout << v->tostr() << endl;
    }

    cout << "VM:" << endl;

    FVM newVM({ B_PUSH, 5, B_PUSH, 5, B_PRINT });

    cout << "BYTECODE: " << newVM.readBytecode() << endl;

    newVM.run();

    return 0;
}