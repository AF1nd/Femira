#include <iostream>
#include <stack>
#include <vector>
#include <variant>
#include <any>
#include <unistd.h>

#include "fvm.h"

using namespace std;

FVM::FVM(vector<Instruction> bytecode) {
    this->bytecode = bytecode;
}

shared_ptr<InstructionOperrand> FVM::run() {
    for (Instruction code: bytecode) {
        switch (code.code) {
            case F_PUSH:
                {
                    bool hasVal = code.operrand.has_value();
                    shared_ptr<InstructionOperrand> val = code.operrand.value();
                    if (!hasVal) throw runtime_error("FVM: NO OPERRAND FOR PUSH");

                    push(code.operrand.value());
                }
                break;
            case F_DELAY:
                {
                    auto val = dynamic_pointer_cast<InstructionNumberOperrand>(pop());
                    if (!val) throw runtime_error("FVM: DELAY ERROR, NO NUMBER IN STACK");

                    usleep(val->operrand * 1000000);
                }
                break;
            case F_RETURN:
                {
                    shared_ptr<InstructionOperrand> val = pop();
                    return val;
                }
                break;
            case F_LOADFUNC:
                {
                    auto declr = dynamic_pointer_cast<InstructionFunctionLoadOperrand>(code.operrand.value());
                    FuncDeclaration fnDeclr = declr->operrand;
                    string id = fnDeclr.id;

                    functions.insert({id, fnDeclr});
                }
                break;
            case F_CALL:
                {
                    auto funcId = dynamic_pointer_cast<InstructionStringOperrand>(pop());

                    vector<shared_ptr<InstructionOperrand>> args;

                    FuncDeclaration funcDeclar = functions.at(funcId->operrand);
                    
                    for (size_t i = 0; i < funcDeclar.argsNum; ++i) {
                        shared_ptr<InstructionOperrand> arg = pop();
                        args.push_back(arg);
                    }

                    FVM funcVM(funcDeclar.bytecode);

                    for (size_t i = 0; i < funcDeclar.argsNum; i++) {
                        shared_ptr<InstructionOperrand> arg = args.at(i);
                        string id = funcDeclar.argsIds.at(i);
                        
                        funcVM.memory.insert({ id, arg });
                    }

                    push(funcVM.run());
                }
                break;
            case F_SETVAR:
                {
                    auto adr = dynamic_pointer_cast<InstructionStringOperrand>(code.operrand.value());
                    auto val = code.operrand2.value();

                    if (adr) memory.insert({adr->operrand, val});
                    else throw runtime_error("FVM: FOR SETVAR EXPECTED ADDERS (OPERRAND 1)");
                }
                break;
            case F_GETVAR:
                {
                    auto adr = dynamic_pointer_cast<InstructionStringOperrand>(code.operrand.value());
                    if (adr) {
                        auto val = memory.at(adr->operrand);
                        if (val) push(val);
                    } else throw runtime_error("FVM: FOR GETVAR EXPECTED ADDERS (OPERRAND)");
                }
                break;
            case F_OUTPUT:
                {
                    shared_ptr<InstructionOperrand> val = pop();
                    cout << val->tostring() << endl;
                }
                break;
            case F_ADD:
                {
                    auto val1 = dynamic_pointer_cast<InstructionNumberOperrand>(pop());
                    auto val2 = dynamic_pointer_cast<InstructionNumberOperrand>(pop());
                    push(make_shared<InstructionNumberOperrand>(val1->operrand + val2->operrand));
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

                    push(make_shared<InstructionNumberOperrand>(val1->operrand * val2->operrand));
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

    return 0;
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
        string opStr;
        string opStr2;

        if (code.operrand.has_value()) {
            opStr = code.operrand.value()->tostring();
        }

          if (code.operrand2.has_value()) {
            opStr2 = code.operrand2.value()->tostring();
        }

        str += "\n   > " + to_string(code.code) + " " + opStr + " " + opStr2;
    }

    return " > BYTECODE: " + str;
}