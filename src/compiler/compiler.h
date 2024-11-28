#ifndef COMPILER_H
#define COMPILER_H

#include <iostream>
#include <vector>

#include "../fvm/fvm.h"

using namespace std;

class Compiler {
    public:
        vector<Instruction> compile(string code);
};

#endif
