#ifndef COMPILER_H
#define COMPILER_H

#include <vector>

#include "../../include/fvm.h"

using namespace std;

class Compiler {
    public:
        vector<Instruction> compile(string code);
};

#endif
