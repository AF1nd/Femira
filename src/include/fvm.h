#ifndef FVM_H
#define FVM_H

#include <stack>
#include <vector>
#include <any>
#include <memory>
#include <optional>
#include <map>
#include <variant>

using namespace std;

enum Bytecode {
    F_PUSH,

    F_SETENV,
    F_GETENV,

    F_LOADIFST,
    F_CALL,
    F_RETURN,
    F_DELAY,

    F_OUTPUT,

    F_ADD,
    F_MUL,
    F_DIV,
    F_SUB,

    F_EQ,
    F_NOTEQ,
    F_BIGGER,
    F_SMALLER,

    F_BIGGER_OR_EQ,
    F_SMALLER_OR_EQ,

    F_IF,
    F_ELSE,

    F_AND,
    F_OR,

    F_INDEXATION,
    F_SETINDEX,
};

struct InstructionOperrand {
    any operrand;

    virtual string tostring() { return "unknown"; }; 

    virtual bool isEq(shared_ptr<InstructionOperrand> toEq) { return false; };
};

struct InstructionNullOperrand : InstructionOperrand {
    bool operrand = true;

    InstructionNullOperrand() = default;

    string tostring() override {
        return "NULL";
    }

    bool isEq(shared_ptr<InstructionOperrand> toEq) override {
        if (dynamic_pointer_cast<InstructionNullOperrand>(toEq)) return true;
        
        return false;
    }
};

struct InstructionNumberOperrand : InstructionOperrand {
    double operrand;

    InstructionNumberOperrand(double operrand) { this->operrand = operrand; };

    string tostring() override {
        return to_string(operrand);
    }
    
    bool isEq(shared_ptr<InstructionOperrand> toEq) override {
        if (auto casted = dynamic_pointer_cast<InstructionNumberOperrand>(toEq)) return operrand == casted->operrand;
        return false;
    }
};

struct InstructionStringOperrand : InstructionOperrand {
    string operrand;

    InstructionStringOperrand(string operrand) { this->operrand = operrand; };

    string tostring() override  {
        return operrand;
    }

    bool isEq(shared_ptr<InstructionOperrand> toEq) override {
        if (auto casted = dynamic_pointer_cast<InstructionStringOperrand>(toEq)) return operrand == casted->operrand;
        return false;
    }
};

struct InstructionBoolOperrand : InstructionOperrand {
    bool operrand;

    InstructionBoolOperrand(bool operrand) { this->operrand = operrand; };

    string tostring() override  {
        string str = operrand == true ? "true" : "false";
        return str;
    }

    bool isEq(shared_ptr<InstructionOperrand> toEq) override {
        if (auto casted = dynamic_pointer_cast<InstructionBoolOperrand>(toEq)) return operrand == casted->operrand;
        return false;
    }
};

struct Instruction {
    optional<shared_ptr<InstructionOperrand>> operrand;
    Bytecode code;

    Instruction(Bytecode code, 
        optional<shared_ptr<InstructionOperrand>> operrand
    ) { this->code = code; this->operrand = operrand; };

    Instruction(Bytecode code) { this->code = code; };

    Instruction() = default;
};

struct Scope;

struct FuncDeclaration {
    vector<Instruction> bytecode;
    vector<string> argsIds;
    string id;

    bool isLambda;

    shared_ptr<Scope> scope;

    FuncDeclaration(vector<Instruction> bytecode, vector<string> argsIds, string id) { this->bytecode = bytecode; this->argsIds = argsIds, this->id = id; };
    FuncDeclaration(vector<Instruction> bytecode, vector<string> argsIds) { this->bytecode = bytecode; this->argsIds = argsIds, this->isLambda = true; };
    FuncDeclaration() = default;
};

struct IfStatement {
    vector<Instruction> bytecode;
    vector<Instruction> elseBytecode;

    IfStatement(vector<Instruction> bytecode) { this->bytecode = bytecode; };
    IfStatement(vector<Instruction> bytecode, vector<Instruction> elseBytecode)  { this->bytecode = bytecode; this->elseBytecode = elseBytecode; };
    IfStatement() = default;
};

struct InstructionFunctionOperrand : InstructionOperrand {
    FuncDeclaration operrand;

    InstructionFunctionOperrand(FuncDeclaration operrand) { this->operrand = operrand; };
    InstructionFunctionOperrand() = default;

    string tostring() override {
        return !this->operrand.isLambda ? this->operrand.id : "function";
    }
};

struct InstructionIfStatementLoadOperrand : InstructionOperrand {
    IfStatement operrand;

    InstructionIfStatementLoadOperrand(IfStatement operrand) { this->operrand = operrand; };

    string tostring() override {
        return "IF";
    }
};

struct InstructionArrayOperrand : InstructionOperrand {
    shared_ptr<vector<shared_ptr<InstructionOperrand>>> operrand;

    InstructionArrayOperrand(shared_ptr<vector<shared_ptr<InstructionOperrand>>> operrand) { this->operrand = operrand; };
    
    string tostring() override {
        string str = "array: ";

        for (shared_ptr<InstructionOperrand> op: *operrand) {
            str += op->tostring() + " ";
        }

        return str;
    }
};

struct InstructionObjectOperrand : InstructionOperrand {
    shared_ptr<map<string, shared_ptr<InstructionOperrand>>> operrand;

    InstructionObjectOperrand(shared_ptr<map<string, shared_ptr<InstructionOperrand>>> fields) { this->operrand = fields; };

    string tostring() override {
        string str = "object: \n";

        for (pair<string, shared_ptr<InstructionOperrand>> op: *operrand) {
            str += op.first + ": " + op.second->tostring() + " \n";
        }
 
        return str;
    }
};

struct ScopeMember {
    shared_ptr<InstructionOperrand> value;
    bool isLocal;

    ScopeMember(shared_ptr<InstructionOperrand> value, bool isLocal = false) { this->value = value; this->isLocal = isLocal; };
    ScopeMember() = default;
};

struct Scope {
    map<string, ScopeMember> members;
};

class FVM {
    public:
        stack<shared_ptr<InstructionOperrand>> vmStack;
  
        bool run(vector<Instruction> bytecode, shared_ptr<Scope> scope = make_shared<Scope>(), shared_ptr<Scope> parent = make_shared<Scope>());
        FVM(bool logs);

        void push(shared_ptr<InstructionOperrand> operrand);

        shared_ptr<InstructionOperrand> pop();

        string getBytecodeString(vector<Instruction> bytecode);

        void printStack();

        bool logs;
};

#endif