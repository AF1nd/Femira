#include <iostream>
#include <stack>
#include <vector>
#include <variant>
#include <any>
#include <memory>
#include <optional>

using namespace std;

enum Bytecode {
    F_PUSH,

    F_OUTPUT,

    F_ADD,
    F_MUL,
    F_DIV,
    F_SUB,
};

struct InstructionOperrand {
    any operrand;
    virtual void print() const = 0; // Виртуальная функция для печати
};

struct InstructionNumberOperrand : InstructionOperrand {
    double operrand;

    InstructionNumberOperrand(double operrand) { this->operrand = operrand; };

    void print() const override {
        cout << operrand;
    }
};

struct InstructionStringOperrand : InstructionOperrand {
    string operrand;

    InstructionStringOperrand(string operrand) { this->operrand = operrand; };

    void print() const override  {
        cout << operrand;
    }
};


struct InstructionBoolOperrand : InstructionOperrand {
    bool operrand;

    InstructionBoolOperrand(bool operrand) { this->operrand = operrand; };

    void print() const override  {
        cout << operrand;
    }
};


struct Instruction {
    optional<shared_ptr<InstructionOperrand>> operrand;
    Bytecode code;

    Instruction(Bytecode code, optional<shared_ptr<InstructionOperrand>> operrand = make_shared<InstructionNumberOperrand>(0)) { this->code = code; this->operrand = operrand; };;
    Instruction() = default;
};

class FVM {
    public:
        vector<Instruction> bytecode;
        stack<shared_ptr<InstructionOperrand>> vmStack;
        
        void run();
        FVM(vector<Instruction> bytecode);

        void push(shared_ptr<InstructionOperrand> operrand);

        shared_ptr<InstructionOperrand> pop();

        string readBytecode();
};