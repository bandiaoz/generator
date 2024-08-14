#include "generator.hpp"

using namespace generator::all;

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    init_gen();

    for (int index = 1; index <= 20; index++) {
        int max = index <= 4 ? 100 : 1e9;
        make_input(index, [&]() {
            int a = rand_int(1, 1e9);
            int b = rand_int(1, 1e9);
            println(a, b);
        });
        auto out = make_output_exe(index, "./std");
        int ans;
        out >> ans;
        if (ans % 2 == 1) {
            index--;
        }
    }

    // fill_outputs_exe("./std");
    validate(1, 20, "./val");
    show_output_first_line();
    check_output();

    return 0;
}