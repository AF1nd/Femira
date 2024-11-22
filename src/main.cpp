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
            printf(5 + (5 - 5))

            def foo():
                printf(2 + 5)
            end
        )";

    Lexer newLexer(code);

    vector<Token> tokens = newLexer.tokenize();

    Parser newParser(tokens);

    BlockNode* ast = newParser.parse();

    cout << "AST:" << endl;

    for (AstNode* v: ast->nodes) {
        cout << v->tostr() << endl;
    }

    return 0;
}