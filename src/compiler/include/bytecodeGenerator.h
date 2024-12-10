#ifndef BGENERATOR_H
#define BGENERATOR_H

#include <vector>

#include "parser.h"
#include "../../include/fvm.h"

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