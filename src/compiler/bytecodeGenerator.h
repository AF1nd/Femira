#ifndef BGENERATOR_H
#define BGENERATOR_H

#include <iostream>
#include <vector>

#include "./parser/parser.h"
#include "../fvm/fvm.h"

using namespace std;

class BytecodeGenerator {
    public:
        vector<Instruction> bytecode;
        
        bool addAnd;
        
        BlockNode* root;
        BytecodeGenerator(BlockNode* root);
        
        void visitNode(AstNode* node);
        vector<Instruction> generate();
};

#endif