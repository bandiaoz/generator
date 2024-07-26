#include "generator.hpp"

using namespace generator::all;

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    init_gen();

    int a, b;
    auto gen = [&]() {
        assert(1 <= a && a <= 1e9);
        assert(1 <= b && b <= 1e9);
        println(a, b);
    };

    for (int i = 1; i <= 20; i++) {
        int max = i <= 4 ? 100 : 1e9;
        a = rand_int(1, max);
        b = rand_int(1, max);
        make_input(i, gen);
        auto out = make_output_exe(i, "./std");
        int ans;
        out >> ans;
        if (ans % 2 == 1) {
            i--;
        }
    }

    // fill_outputs_exe("./std");
    show_output_first_line();

    return 0;
}