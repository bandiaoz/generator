#include "generator.hpp"

using namespace generator::all;

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    init_gen();

    for (int index = 1; index <= 20; index++) {
        int n = index <= 4 ? 10 : 1e5;
        int max = index <= 4 ? 10 : 1e9;
        make_input(index, [&]() {
        });
        auto out = make_output_exe(index, "./std");
    }

    // fill_outputs_exe("./std");
    validate(1, 20, "./val");
    show_output_first_line();
    check_output();

    return 0;
}