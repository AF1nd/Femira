#include <iostream>
#include <fstream>

#include "include/runner.h"
#include "compiler/include/compiler.h"

using namespace std;

string read(string path) {
    ifstream file(path);

    string code;
    string line;

    while (getline(file, line)) {
        code += line + "\n";
    }

    file.close();

    return code;
}

void Runner::run(string path) {
    Compiler compiler;

    FVM fvm(false);
    fvm.run(compiler.compile(read(path)));
}