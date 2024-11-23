#include <iostream>
#include <stack>
#include <map>
#include <vector>
#include <variant>

#include "vm.h"

using namespace std;

FVM::FVM(vector<int> VM_BYTECODE) {
    bytecode = VM_BYTECODE;
}

void FVM::run() {
    for (int i = 0; i < bytecode.size(); i++) {
        int instruction = bytecode.at(i);

        switch (instruction) {
            case B_PUSH:
                {
                    int value = bytecode.at(i + 1);
                    i++;

                    vmStack.push(value);
                }
                break;
            case B_PRINT:
                cout << pop() << endl;
                break;
            case B_ADD:
                vmStack.push(pop() + pop());
                break;
            case B_SUB:
                vmStack.push(pop() - pop());
                break;
            case B_MUL:
                vmStack.push(pop() * pop());
                break;
            case B_DIV:
                vmStack.push(pop() / pop());
                break;
            default:
                break;
        }
    }
}

int FVM::pop() {
    int val = vmStack.top();
    vmStack.pop();

    return val;
}

string FVM::readBytecode() {
    string str = "";

    for (int code: bytecode) {
        str += to_string(code) + " ";
    }

    return str;
}