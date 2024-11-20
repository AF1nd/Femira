#include <iostream>
#include "lexer/lexer.h"
#include "lexer/token.h"
#include <vector>
#include "parser/parser.h"

using namespace std;

int main() {
    cout << "Femlang started work" << endl;

    const string code =
        R"(
            call();
        )";

    Lexer newLexer(code);

    vector<Token> tokens = newLexer.tokenize();

    Parser newParser(tokens);

    BlockNode ast = newParser.parse();

    cout << "AST:" << endl;

    for (AstNode v: ast.nodes) {
        cout << v.tostr() << endl;
    }

    return 0;
}