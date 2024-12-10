#include <iostream>
#include <stack>
#include <vector>
#include <algorithm>
#include <string>
#include <chrono>
#include <thread>
#include <math.h>

#include "include/fvm.h"

using namespace std;

string opcodeToString(Bytecode opcode) {
    switch (opcode) {
        case F_PUSH:
            return "PUSH";
        case F_SETGLOBAL:
            return "SETGLOBAL";
        case F_GETGLOBAL:
            return "GETGLOBAL";
        case F_CALL:
            return "CALL";
        case F_RETURN:
            return "RETURN";
        case F_DELAY:
            return "DELAY";
        case F_OUTPUT:
            return "OUTPUT";
         case F_ADD:
            return "ADD";
        case F_SUB:
            return "SUB";
        case F_MUL:
            return "MUL";
        case F_DIV:
            return "DIV";
        case F_EQ:
            return "EQ";
        case F_NOTEQ:
            return "NOTEQ";
        case F_AND:
            return "AND";
        case F_BIGGER:
            return "BIGGER";
        case F_SMALLER:
            return "SMALLER";
        case F_BIGGER_OR_EQ:
            return "BIGGER_OR_EQ";
        case F_SMALLER_OR_EQ:
            return "SMALLER_OR_EQ";
        case F_INDEXATION:
            return "INDEXATION";
        case F_SETINDEX:
            return "SETINDEX";
        case F_LOADIFST:
            return "LOADIF";
        case F_IF:
            return "IF";
        default:
            break;
    }

    return "unknown";
};

bool isDoubleInt(double trouble) {
   double absolute = abs(trouble);

   return absolute == floor(absolute);
}

bool binaryNumbersCondition(shared_ptr<InstructionOperrand> one, shared_ptr<InstructionOperrand> two, Bytecode opcode) {
    auto oneCasted = dynamic_pointer_cast<InstructionNumberOperrand>(one);
    auto twoCasted = dynamic_pointer_cast<InstructionNumberOperrand>(two);

    if (oneCasted && twoCasted) {
        if (opcode == F_BIGGER) return oneCasted->operrand > twoCasted->operrand;
        if (opcode == F_SMALLER) return oneCasted->operrand < twoCasted->operrand;
        if (opcode == F_BIGGER_OR_EQ) return oneCasted->operrand >= twoCasted->operrand;
        if (opcode == F_SMALLER_OR_EQ) return oneCasted->operrand <= twoCasted->operrand;
    } else throw runtime_error("FVM: BINARY CONDITION WITH OPCODE " + opcodeToString(opcode) + " CAN WORK ONLY WITH NUMBERS!");

    return false;
}

FVM::FVM(bool logs) {
    this->logs = logs;
}

bool FVM::run(vector<Instruction> bytecode, shared_ptr<Scope> scope, shared_ptr<Scope> parent) {
    if (logs) cout << getBytecodeString(bytecode) << endl;

    if (scope != nullptr && parent != nullptr) {
        for (pair<string, ScopeMember> member: parent->members) {
            if (scope->members.find(member.first) == scope->members.end()) scope->members.insert(member);
        }
    }

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
            case F_INDEXATION:
                {
                    shared_ptr<InstructionOperrand> index = pop();
                    shared_ptr<InstructionOperrand> where = pop();

                    if (auto casted = dynamic_pointer_cast<InstructionArrayOperrand>(where)) {
                        if (auto indexCasted = dynamic_pointer_cast<InstructionNumberOperrand>(index)) {
                            double indexOperrand = indexCasted->operrand;
                            if (!isDoubleInt(indexOperrand)) throw runtime_error("FVM: ARRAY INDEX MUST BE A INTEGER");

                            shared_ptr<vector<shared_ptr<InstructionOperrand>>> elements = casted->operrand;
                            if (elements->size() - 1 >= indexOperrand) {
                                auto val = elements->at(indexOperrand);
                                push(val);
                            } else push(make_shared<InstructionNullOperrand>());
                        } else throw runtime_error("FVM: ARRAY CAN BE INDEXED ONLY WITH INTEGERS");
                    } else if (auto casted = dynamic_pointer_cast<InstructionObjectOperrand>(where)) {
                        if (auto indexCasted = dynamic_pointer_cast<InstructionStringOperrand>(index)) {
                            shared_ptr<map<string, shared_ptr<InstructionOperrand>>> fields = casted->operrand;
                            auto val = fields->at(indexCasted->operrand);
                            if (val == nullptr) val = make_shared<InstructionNullOperrand>();
                            push(val);
                        } else throw runtime_error("FVM: INDEX FOR OBJECT INDEXATION MUST BE A STRING");
                    } else throw runtime_error("FVM: UNABLE TO INDEX UNKNOWN OPERRAND");
                }
                break;
            case F_SETINDEX:
                {
                    shared_ptr<InstructionOperrand> index = pop();
                    shared_ptr<InstructionOperrand> value = pop();
                    shared_ptr<InstructionOperrand> where = pop();

                    if (auto casted = dynamic_pointer_cast<InstructionArrayOperrand>(where)) {
                        if (auto indexCasted = dynamic_pointer_cast<InstructionNumberOperrand>(index)) {
                            shared_ptr<vector<shared_ptr<InstructionOperrand>>> elements = casted->operrand;
                            (*elements)[indexCasted->operrand] = value;
                        }
                    } else if (auto casted = dynamic_pointer_cast<InstructionObjectOperrand>(where)) {
                        if (auto indexCasted = dynamic_pointer_cast<InstructionStringOperrand>(index)) {
                            shared_ptr<map<string, shared_ptr<InstructionOperrand>>> fields = casted->operrand;
                            (*fields)[indexCasted->operrand] = value;
                        }
                    }
                }
                break;
            case F_IF:
                {
                    auto operrand = dynamic_pointer_cast<InstructionIfStatementLoadOperrand>(code.operrand.value());
                    if (!operrand) throw runtime_error("FVM: NO OPERRAND FOR IF INSTRUCTION");

                    IfStatement statement = operrand->operrand;

                    shared_ptr<InstructionOperrand> val = pop();

                    if (auto boolean = dynamic_pointer_cast<InstructionBoolOperrand>(val)) {
                        vector<Instruction> bytecode = statement.bytecode;
                        if (!boolean->operrand) bytecode = statement.elseBytecode;

                        if (!bytecode.empty()) {
                            shared_ptr<Scope> newScope = make_shared<Scope>();

                            if (run(bytecode, newScope, scope)) return true ;
                        }
                    }
                }
                break;
            case F_DELAY:
                {
                    auto val = dynamic_pointer_cast<InstructionNumberOperrand>(pop());
                    if (!val) throw runtime_error("FVM: DELAY ERROR, NO NUMBER IN STACK");

                    this_thread::sleep_for(chrono::duration<double>(val->operrand));
                }
                break;
            case F_RETURN:
                {
                    if (vmStack.empty()) push(make_shared<InstructionNullOperrand>());

                    for (auto v: scope->members) {
                        if (parent != nullptr && parent->members.find(v.first) != parent->members.end() && !v.second.isLocal) {
                            parent->members[v.first] = v.second;
                        }
                    }

                    return true;
                }
                break;
            case F_CALL:
                {
                    shared_ptr<InstructionFunctionOperrand> func = dynamic_pointer_cast<InstructionFunctionOperrand>(pop());
                    if (!func) break;

                    vector<shared_ptr<InstructionOperrand>> args;

                    FuncDeclaration funcDeclar = func->operrand;
                    string funcName = funcDeclar.id;

                    int argsNum = funcDeclar.argsIds.size();

                    for (size_t i = 0; i < argsNum; ++i) {
                        shared_ptr<InstructionOperrand> arg = pop();
                        args.push_back(arg);
                    }

                    shared_ptr<Scope> newScope = make_shared<Scope>();

                    for (size_t i = 0; i < argsNum; ++i) {
                        shared_ptr<InstructionOperrand> arg = args.at(i);
                        string id = funcDeclar.argsIds.at(i);
                        
                        newScope->members.insert({ id, ScopeMember(arg, true) });
                    }

                    run(funcDeclar.bytecode, newScope, funcDeclar.scope);
                }
                break;
            case F_SETGLOBAL:
                {
                    auto adr = dynamic_pointer_cast<InstructionStringOperrand>(code.operrand.value());
                    auto val = pop();

                    if (adr) {
                        scope->members[adr->operrand] = ScopeMember(val);
                    }
                    else throw runtime_error("FVM: FOR SETVAR EXPECTED ADDRESS (OPERRAND 1)");
                }
                break;
            case F_GETGLOBAL:
                {
                    auto adr = dynamic_pointer_cast<InstructionStringOperrand>(code.operrand.value());
                    if (adr) {
                        try {
                            ScopeMember val = scope->members.at(adr->operrand);
                            push(val.value);
                        }
                        catch (const exception& e) {
                            throw runtime_error("FVM: BY ADDRESS " + adr->operrand + " NOT FINDED ANYTHING");
                        }
                        
                    } else throw runtime_error("FVM: FOR GETVAR EXPECTED ADDERS (OPERRAND)");
                }
                break;
            case F_OUTPUT:
                {
                    shared_ptr<InstructionOperrand> val = pop();
                    cout << "OUTPUT: " + val->tostring() << endl;
                }
                break;
            case F_EQ:
                {
                    shared_ptr<InstructionOperrand> one = pop();
                    shared_ptr<InstructionOperrand> two = pop();

                    push(make_shared<InstructionBoolOperrand>(one->isEq(two)));
                }
                break;
            case F_NOTEQ:
                {
                    shared_ptr<InstructionOperrand> one = pop();
                    shared_ptr<InstructionOperrand> two = pop();

                    push(make_shared<InstructionBoolOperrand>(!(one->isEq(two))));
                }
                break;
            case F_BIGGER:
                {
                    shared_ptr<InstructionOperrand> one = pop();
                    shared_ptr<InstructionOperrand> two = pop();

                    push(make_shared<InstructionBoolOperrand>(binaryNumbersCondition(two, one, F_BIGGER)));
                }
                break;
            case F_SMALLER:
                {
                    shared_ptr<InstructionOperrand> one = pop();
                    shared_ptr<InstructionOperrand> two = pop();

                    push(make_shared<InstructionBoolOperrand>(binaryNumbersCondition(two, one, F_SMALLER)));
                }
                break;
            case F_BIGGER_OR_EQ:
                {
                    shared_ptr<InstructionOperrand> one = pop();
                    shared_ptr<InstructionOperrand> two = pop();

                    push(make_shared<InstructionBoolOperrand>(binaryNumbersCondition(two, one, F_BIGGER_OR_EQ)));
                }
                break;
            case F_SMALLER_OR_EQ:
                {
                    shared_ptr<InstructionOperrand> one = pop();
                    shared_ptr<InstructionOperrand> two = pop();

                    push(make_shared<InstructionBoolOperrand>(binaryNumbersCondition(two, one, F_SMALLER_OR_EQ)));
                }
                break;
            case F_AND:
                {
                    shared_ptr<InstructionOperrand> one = pop();
                    shared_ptr<InstructionOperrand> two = pop();

                    if (auto oneCasted = dynamic_pointer_cast<InstructionBoolOperrand>(one)) {
                        if (auto twoCasted = dynamic_pointer_cast<InstructionBoolOperrand>(two)) {
                            push(make_shared<InstructionBoolOperrand>(oneCasted->operrand == true && twoCasted->operrand == true));
                        }
                    }
                }
                break;
            case F_OR:
                {
                    shared_ptr<InstructionOperrand> one = pop();
                    shared_ptr<InstructionOperrand> two = pop();

                    bool isFalse = false;
                    if (auto casted = dynamic_pointer_cast<InstructionBoolOperrand>(two)) isFalse = !casted->operrand;

                    if (dynamic_pointer_cast<InstructionNullOperrand>(two) || isFalse) push(one);
                    else push(two);
                }
                break;
            case F_ADD:
                {
                    auto val1 = dynamic_pointer_cast<InstructionNumberOperrand>(pop());
                    auto val2 = dynamic_pointer_cast<InstructionNumberOperrand>(pop());

                    if (val1 && val2) push(make_shared<InstructionNumberOperrand>(val1->operrand + val2->operrand));
                    else throw runtime_error("FVM: ADD ERROR! OPERRANDS MUST BE A NUMBERS");
                }
                break;
            case F_SUB:
                {
                    auto val1 = dynamic_pointer_cast<InstructionNumberOperrand>(pop());
                    auto val2 = dynamic_pointer_cast<InstructionNumberOperrand>(pop());

                    if (val1 && val2) push(make_shared<InstructionNumberOperrand>(val2->operrand - val1->operrand));
                    else throw runtime_error("FVM: SUB ERROR! OPERRANDS MUST BE A NUMBERS");
                }
                break;
            case F_MUL:
                {
                    auto val1 = dynamic_pointer_cast<InstructionNumberOperrand>(pop());
                    auto val2 = dynamic_pointer_cast<InstructionNumberOperrand>(pop());

                    if (val1 && val2) push(make_shared<InstructionNumberOperrand>(val1->operrand * val2->operrand));
                    else throw runtime_error("FVM: MUL ERROR! OPERRANDS MUST BE A NUMBERS");
                }
                break;
            case F_DIV:
                {
                    auto val1 = dynamic_pointer_cast<InstructionNumberOperrand>(pop());
                    auto val2 = dynamic_pointer_cast<InstructionNumberOperrand>(pop());

                    if (val1 && val2) push(make_shared<InstructionNumberOperrand>(val2->operrand / val1->operrand));
                    else throw runtime_error("FVM: DIV ERROR! OPERRANDS MUST BE A NUMBERS");
                }
                break;
            default:
                break;
        }
    }

    for (auto v: scope->members) {
        if (parent != nullptr && parent->members.find(v.first) != parent->members.end() && !v.second.isLocal) {
            parent->members[v.first] = v.second;
        }
    }

    return false;
}

void FVM::push(shared_ptr<InstructionOperrand> operrand) {
    vmStack.push(operrand);
}

shared_ptr<InstructionOperrand> FVM::pop() {
    if (vmStack.empty() || vmStack.top() == nullptr) throw runtime_error("FVM: CANNOT POP FROM EMPTY STACK");

    auto top = vmStack.top();
    vmStack.pop();
    return top;
}

string FVM::getBytecodeString(vector<Instruction> bytecode) {
    string str = "";

    for (Instruction code: bytecode) {
        string opStr;
        string opStr2;

        if (code.operrand.has_value()) {
            opStr = code.operrand.value()->tostring();
        }

        string opcodeName = opcodeToString(code.code);

        str += "\n  > " + to_string(code.code) + " | " + (opcodeName != "unknown" ? opcodeName : to_string(code.code)) + " " + opStr + " " + opStr2;
    }

    return "[ BYTECODE ]" + str;
}