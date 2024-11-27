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
            def check(a):
                if (a > 2) & (a < 4):
                    return 'правда'
                end
                else:
                    return 'фейк'
                end
            end

            output check(3)
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