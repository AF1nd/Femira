#ifndef COMPILER_H
#define COMPILER_H

#include <iostream>
#include <vector>

#include "../parser/parser.h"
#include "vm.h"

using namespace std;

class Compiler {
    public:
        BlockNode* ast;
        Compiler(BlockNode* ast);

        vector<Instruction> generateBytecode();
};

#endif