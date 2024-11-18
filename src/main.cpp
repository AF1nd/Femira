#include <iostream>
#include "lexer/lexer.h"

using namespace std;

int main() {
    const string code =
        R"(
            print('f', 5);
            call(5 + 5);
        )";

    Lexer newLexer(code);

    cout << "Femlang started work" << endl;
    newLexer.tokenize();

    return 0;
}