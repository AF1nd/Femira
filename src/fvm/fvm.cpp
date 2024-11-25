#include <iostream>
#include <stack>
#include <vector>
#include <variant>
#include <any>

#include "fvm.h"

using namespace std;

FVM::FVM(vector<Instruction> bytecode) {
    this->bytecode = bytecode;
}

void FVM::run() {
    cout << "<VM STARTED RUNNING>" << endl;

    for (Instruction code: bytecode) {
        switch (code.code) {
            case F_PUSH:
                {
                    push(code.operrand.value());
                }
                break;  
            case F_OUTPUT:
                {
                    shared_ptr<InstructionOperrand> val = pop();
                    val->print();
                    cout << endl;
                }
                break;
            case F_ADD:
                {
                    auto val1 = dynamic_pointer_cast<InstructionNumberOperrand>(pop());
                    auto val2 = dynamic_pointer_cast<InstructionNumberOperrand>(pop());
                    push(make_shared<InstructionNumberOperrand>(val2->operrand + val1->operrand));
                }
                break;
            case F_SUB:
                {
                    auto val1 = dynamic_pointer_cast<InstructionNumberOperrand>(pop());
                    auto val2 = dynamic_pointer_cast<InstructionNumberOperrand>(pop());

                    push(make_shared<InstructionNumberOperrand>(val2->operrand - val1->operrand));
                }
                break;
            case F_MUL:
                {
                    auto val1 = dynamic_pointer_cast<InstructionNumberOperrand>(pop());
                    auto val2 = dynamic_pointer_cast<InstructionNumberOperrand>(pop());

                    push(make_shared<InstructionNumberOperrand>(val2->operrand * val1->operrand));
                }
                break;
            case F_DIV:
                {
                    auto val1 = dynamic_pointer_cast<InstructionNumberOperrand>(pop());
                    auto val2 = dynamic_pointer_cast<InstructionNumberOperrand>(pop());
                    push(make_shared<InstructionNumberOperrand>(val2->operrand / val1->operrand));
                }
                break;
            default:
                break;
        }
    }

    cout << "<VM ENDED RUNNING>" << endl;
}

void FVM::push(shared_ptr<InstructionOperrand> operrand) {
    vmStack.push(operrand);
}

shared_ptr<InstructionOperrand> FVM::pop() {
    if (vmStack.empty()) throw runtime_error("FVM: CANNOT POP FROM EMPTY STACK");

    auto top = vmStack.top();
    vmStack.pop();
    return top;
}

string FVM::readBytecode() {
    string str = "";

    for (Instruction code: bytecode) {
        str += "\n" + to_string(code.code);
    }

    return " > BYTECODE: 00000" + str;
}