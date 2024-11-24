#ifndef VM_H
#define VM_H

#include <iostream>
#include <stack>
#include <map>
#include <vector>
#include <variant>
#include <optional>

using namespace std;

enum Bytecode {
    B_PUSH,
    B_ADD,
    B_SUB,
    B_MUL,
    B_DIV,
    B_ASSIGN,
    B_PRINT,
    B_HALT,
    B_DELAY,
    B_LOAD,
};

struct Instruction {
    Bytecode code;
    optional<variant<double, string>> arg;
};

class FVM {
    private:
        vector<Instruction> bytecode;
    public:
        stack<variant<double, string>> vmStack;
        map<string, variant<double, string>> memory;

        FVM(vector<Instruction> bytecode);
        
        void run();
        variant<double, string> pop();
        string readBytecode();
};

#endif