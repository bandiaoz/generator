#include "testlib.h"

using ll = long long;

int main(int argc, char* argv[]) {
    registerValidation(argc, argv);

    int a = inf.readInt(1, 1e9, "a");
    inf.readSpace();
    int b = inf.readInt(1, 1e9, "b");
    inf.readEoln();
    inf.readEof();

    return 0;
}