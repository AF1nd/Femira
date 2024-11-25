#include <iostream>
#include <vector>

#include "lexer/lexer.h"
#include "parser/parser.h"
#include "fvm/fvm.h"

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

    // FVM

    FVM vm({
        {F_PUSH, make_shared<InstructionNumberOperrand>(5.0)},
        {F_PUSH, make_shared<InstructionNumberOperrand>(2.0)},
        {F_ADD},
        {F_OUTPUT}
    });

    vm.readBytecode();
    vm.run();

    return 0;
}