#include <iostream>
#include <vector>

#include "compiler/lexer/lexer.h"
#include "compiler/parser/parser.h"
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
    };

    // FVM

    FuncDeclaration add = 
    {
        bytecode: {
            {F_GETVAR, make_shared<InstructionStringOperrand>("a")},
            {F_GETVAR, make_shared<InstructionStringOperrand>("b")},

            {F_ADD},
            
            {F_PUSH, make_shared<InstructionNumberOperrand>(3)},

            {F_SUB},
            
            {F_RETURN},
        },
        argsNum: 2,
        argsIds: { "a", "b" },
        id: "add",
    };

    FVM vm({
        {F_SETVAR, make_shared<InstructionStringOperrand>("a"), make_shared<InstructionNumberOperrand>(5.0)}, // var a
        {F_SETVAR, make_shared<InstructionStringOperrand>("b"), make_shared<InstructionNumberOperrand>(5.0)}, // var b

        {F_LOADFUNC, make_shared<InstructionFunctionLoadOperrand>(add)},

        {F_GETVAR, make_shared<InstructionStringOperrand>("a")}, // arg1 from var
        {F_GETVAR, make_shared<InstructionStringOperrand>("b")}, // arg2 from var

        {F_PUSH, make_shared<InstructionStringOperrand>("add")}, // func id
        {F_CALL}, // call

        {F_OUTPUT} // print
    });

    cout << vm.readBytecode() << endl;
    vm.run();

    return 0;
}