#include <iostream>
#include <vector>
#include <string.h>

#include "include/runner.h"

using namespace std;

int main(int argc, char * argv[]) {
    for (int i = 1; i < argc; ++i) {
        Runner newRunner;
        newRunner.run(argv[i]);
    }

    return 0;
}
