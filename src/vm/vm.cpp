#include <iostream>
#include <stack>
#include <map>
#include <vector>
#include <variant>
#include <unistd.h>

#include "vm.h"

using namespace std;

FVM::FVM(vector<Instruction> bytecode) {
    this->bytecode = bytecode;
}

void FVM::run() {
    cout << "<VM STARTED RUNNING:>" << endl;

    for (int i = 0; i < bytecode.size(); i++) {
        Instruction instruction = bytecode.at(i);

        if (instruction.code == B_HALT) break;

        switch (instruction.code) {
            case B_LOAD:
                {
                    variant<double, string> address = instruction.arg;

                    if (const string* pval = get_if<string>(&address)) {
                        vmStack.push(memory.at(*pval));
                    }
                }
                break;
            case B_ASSIGN:
                {
                    variant<double, string> val = pop();
                    variant<double, string> address = pop();

                    if (const string* pval = get_if<string>(&address)) {
                        memory.insert({ *pval, val });
                    }
                }
                break;
            case B_DELAY:
                { 
                    variant<double, string> val1 = pop();

                    if (const double* pval = get_if<double>(&val1)) usleep(*pval * 1000000);
                }
                break;
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
        throw runtime_error("Stack is empty");
    }
    
    variant<double, string> val = vmStack.top();
    vmStack.pop();

    return val;
}

string FVM::readBytecode() {
    string str = "";

    for (Instruction code: bytecode) {
        variant<double, string> value = code.arg;
        string str1 = "";

        if (const double* pval = get_if<double>(&value))
           str1 += to_string(*pval);
        else if (const string* pval = get_if<string>(&value))
            str1 += *pval;
            
        str += "\n" + to_string(code.code) + " " + str1;
    }

    return " > BYTECODE: " + str;
}