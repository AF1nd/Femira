#include <iostream>
#include <stack>
#include <vector>
#include <variant>
#include <any>
#include <memory>
#include <optional>
#include <map>

using namespace std;

enum Bytecode {
    F_PUSH,
    F_SETVAR,
    F_GETVAR,

    F_LOADFUNC,
    F_CALL,
    F_RETURN,
    F_DELAY,

    F_OUTPUT,

    F_ADD,
    F_MUL,
    F_DIV,
    F_SUB,
};

struct InstructionOperrand {
    any operrand;
    virtual void output() const = 0; // Виртуальная функция для печати
};

struct InstructionNumberOperrand : InstructionOperrand {
    double operrand;

    InstructionNumberOperrand(double operrand) { this->operrand = operrand; };

    void output() const override {
        cout << operrand;
    }
};

struct InstructionStringOperrand : InstructionOperrand {
    string operrand;

    InstructionStringOperrand(string operrand) { this->operrand = operrand; };

    void output() const override  {
        cout << operrand;
    }
};

struct InstructionBoolOperrand : InstructionOperrand {
    bool operrand;

    InstructionBoolOperrand(bool operrand) { this->operrand = operrand; };

    void output() const override  {
        string str = operrand == true ? "true" : "false";
        cout << str;
    }
};

struct Instruction {
    optional<shared_ptr<InstructionOperrand>> operrand;
    optional<shared_ptr<InstructionOperrand>> operrand2;
    Bytecode code;

    Instruction(Bytecode code, 
        optional<shared_ptr<InstructionOperrand>> operrand = make_shared<InstructionNumberOperrand>(0), 
        optional<shared_ptr<InstructionOperrand>> operrand2 = make_shared<InstructionNumberOperrand>(0)
    )
    { this->code = code; this->operrand = operrand; this->operrand2 = operrand2; };

    Instruction() = default;
};

struct FuncDeclaration {
    vector<Instruction> bytecode;
    int argsNum;
    vector<string> argsIds;
    string id;
};

struct InstructionFunctionLoadOperrand : InstructionOperrand {
    FuncDeclaration operrand;

    InstructionFunctionLoadOperrand(FuncDeclaration operrand) { this->operrand = operrand; };

    void output() const override {
        cout << "Function";
    }
};

class FVM {
    public:
        vector<Instruction> bytecode;
        stack<shared_ptr<InstructionOperrand>> vmStack;
        map<string, shared_ptr<InstructionOperrand>> memory;

        map<string, FuncDeclaration> functions;
        
        shared_ptr<InstructionOperrand> run();
        FVM(vector<Instruction> bytecode);

        void push(shared_ptr<InstructionOperrand> operrand);

        shared_ptr<InstructionOperrand> pop();

        string readBytecode();
};