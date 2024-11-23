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
    B_ADD,
    B_SUB,
    B_MUL,
    B_DIV,
    B_PRINT,
};

class FVM {
    private:
        vector<int> bytecode;
    public:
        stack<int> vmStack;

        FVM(vector<int> bytecode);
        
        void run();
        int pop();
};

#endif