#include "generator.hpp"

using namespace generator::all;

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    init_gen();

    auto print = [&]() {
    };

    auto gen = [&](int max, ...) {
    };

    for (int i = 1; i <= 20; i++) {
        gen(i <= 4 ? 10 : 1e5);
        make_input(i, print);
        auto out = make_output_exe(i, "./std");
    }

    // fill_outputs_exe("./std");
    show_output_first_line();
    check_output();

    return 0;
}