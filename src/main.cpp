#include <iostream>
#include "lexer/lexer.h"

using namespace std;

int main() {
    Lexer newLexer("print(5);");

    cout << "Femlang started work" << endl;
    newLexer.tokenize();

    return 0;
}