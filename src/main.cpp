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
        {F_SETVAR, make_shared<InstructionStringOperrand>("a"), make_shared<InstructionNumberOperrand>(5.0)},
        {F_SETVAR, make_shared<InstructionStringOperrand>("b"), make_shared<InstructionNumberOperrand>(5.0)},

        {F_GETVAR, make_shared<InstructionStringOperrand>("a")},
        {F_GETVAR, make_shared<InstructionStringOperrand>("b")},

        {F_ADD},

        {F_OUTPUT}
    });

    cout << vm.readBytecode() << endl;
    vm.run();

    return 0;
}