#ifndef VM_H
#define VM_H

#include <iostream>
#include <stack>
#include <map>
#include <vector>
#include <variant>

using namespace std;

enum Bytecode {
    B_PUSH,
    B_TOSTRING,
    B_ADD,
    B_SUB,
    B_MUL,
    B_DIV,
    B_PRINT,
    B_HALT
};

struct Instruction {
    Bytecode code;
    variant<int, string> arg;
};

class FVM {
    private:
        vector<Instruction> bytecode;
    public:
        stack<variant<int, string>> vmStack;

        FVM(vector<Instruction> bytecode);
        
        void run();
        variant<int, string> pop();
        string readBytecode();
};

#endif