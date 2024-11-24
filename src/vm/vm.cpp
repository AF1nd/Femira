#include <iostream>
#include <stack>
#include <map>
#include <vector>
#include <variant>

#include "vm.h"

using namespace std;

FVM::FVM(vector<Instruction> VM_BYTECODE) {
    bytecode = VM_BYTECODE;
}

void FVM::run() {
    for (int i = 0; i < bytecode.size(); i++) {
        Instruction instruction = bytecode.at(i);

        if (instruction.code == B_HALT) break;

        switch (instruction.code) {
            case B_PUSH:
                {
                    variant<int, string> value = instruction.arg;

                    vmStack.push(value);
                }
                break;
            case B_PRINT:
                {
                    variant<int, string> value = pop();
                    if (const int* pval = get_if<int>(&value))
                        cout << to_string(*pval) << endl;
                    else if (const string* pval = get_if<string>(&value))
                        cout << *pval << endl;
                }
                break;
            case B_ADD:
                {
                    variant<int, string> val1 = pop();
                    variant<int, string> val2 = pop();

                    if (const int* pval = get_if<int>(&val1)) {
                        if (const int* pval2 = get_if<int>(&val2)) {
                            vmStack.push(*pval + *pval2);
                        }
                    }
                }
                break;
            case B_SUB:
                {
                    variant<int, string> val1 = pop();
                    variant<int, string> val2 = pop();

                    if (const int* pval = get_if<int>(&val1)) {
                        if (const int* pval2 = get_if<int>(&val2)) {
                            vmStack.push(*pval - *pval2);
                        }
                    }
                }
                break;
            case B_MUL:
                {
                    variant<int, string> val1 = pop();
                    variant<int, string> val2 = pop();

                    if (const int* pval = get_if<int>(&val1)) {
                        if (const int* pval2 = get_if<int>(&val2)) {
                            vmStack.push(*pval * *pval2);
                        }
                    }
                }
                break;
            case B_DIV:
                {
                    variant<int, string> val1 = pop();
                    variant<int, string> val2 = pop();

                    if (const int* pval = get_if<int>(&val1)) {
                        if (const int* pval2 = get_if<int>(&val2)) {
                            vmStack.push(*pval / *pval2);
                        }
                    }
                }
                break;
            default:
                break;
        }
    }

    cout << "<VM ENDED RUNNING>" << endl;
}

variant<int, string> FVM::pop() {
    variant<int, string> val = vmStack.top();
    vmStack.pop();

    return val;
}

string FVM::readBytecode() {
    string str = "";

    for (Instruction code: bytecode) {
        str += to_string(code.code) + " ";
    }

    return str;
}