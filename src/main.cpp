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
            println(5 + 5 + 5)
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

    cout << "<VM STARTED RUNNING:>" << endl;

    cout << " > BYTECODE: " << vm.readBytecode() << endl;

    vm.run();

    return 0;
}