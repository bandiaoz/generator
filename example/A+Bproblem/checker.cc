#include "testlib.h"
#include <bits/stdc++.h>

using ll = long long;

int main(int argc, char* argv[]) {
    registerTestlibCmd(argc, argv);

    int std_ans = ans.readInt();
    int out_ans = ouf.readInt(1, 2e9, "out_ans");

    if (std_ans != out_ans) {
        quitf(_wa, "expected %d, found %d!", std_ans, out_ans);
    }
 
    quitf(_ok, "Accepted!");
}