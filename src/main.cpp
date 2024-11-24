#include <iostream>
#include <vector>

#include "lexer/lexer.h"
#include "parser/parser.h"
#include "vm/vm.h"
#include "vm/compiler.h"

using namespace std;

int main() {
    cout << "Femlang started work" << endl;

    const string code =
        R"(
            a = 2

            delay a

            println(a)
            println('zадержка')
        )";

    Lexer newLexer(code);

    vector<Token> tokens = newLexer.tokenize();

    Parser newParser(tokens);

    BlockNode* ast = newParser.parse();

    cout << "AST:" << endl;

    for (AstNode* v: ast->nodes) {
        cout << v->tostr() << endl;
    }

    Compiler compiler(ast);
    FVM vm(compiler.generateBytecode());

    cout << vm.readBytecode() << endl;
    vm.run();

    return 0;
}