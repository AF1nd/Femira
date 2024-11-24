#include <iostream>
#include <stack>
#include <map>
#include <vector>
#include <variant>

#include "vm.h"

using namespace std;

FVM::FVM(vector<Instruction> bytecode) {
    this->bytecode = bytecode;
}

void FVM::run() {
    for (int i = 0; i < bytecode.size(); i++) {
        Instruction instruction = bytecode.at(i);

        if (instruction.code == B_HALT) break;

        switch (instruction.code) {
            case B_PUSH:
                {
                    variant<double, string> value = instruction.arg;

                    vmStack.push(value);
                }
                break;
            case B_PRINT:
                {
                    variant<double, string> value = pop();
                    if (const double* pval = get_if<double>(&value))
                        cout << to_string(*pval) << endl;
                    else if (const string* pval = get_if<string>(&value))
                        cout << *pval << endl;
                }
                break;
            case B_ADD:
                {
                    variant<double, string> val1 = pop();
                    variant<double, string> val2 = pop();

                    if (const double* pval = get_if<double>(&val1)) {
                        if (const double* pval2 = get_if<double>(&val2)) {
                            vmStack.push(*pval + *pval2);
                        } else throw runtime_error("Add operation can called only on numbers");
                    } else throw runtime_error("Add operation can called only on numbers");
                }
                break;
            case B_SUB:
                {
                    variant<double, string> val1 = pop();
                    variant<double, string> val2 = pop();

                    if (const double* pval = get_if<double>(&val1)) {
                        if (const double* pval2 = get_if<double>(&val2)) {
                            vmStack.push(*pval - *pval2);
                        }
                    }
                }
                break;
            case B_MUL:
                {
                    variant<double, string> val1 = pop();
                    variant<double, string> val2 = pop();

                    if (const double* pval = get_if<double>(&val1)) {
                        if (const double* pval2 = get_if<double>(&val2)) {
                            vmStack.push(*pval * *pval2);
                        }
                    }
                }
                break;
            case B_DIV:
                {
                    variant<double, string> val1 = pop();
                    variant<double, string> val2 = pop();

                    if (const double* pval = get_if<double>(&val1)) {
                        if (const double* pval2 = get_if<double>(&val2)) {
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

variant<double, string> FVM::pop() {
    if (vmStack.empty()) {
        throw runtime_error("Attempted to pop from an empty stack.");
    }
    
    variant<double, string> val = vmStack.top();
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