#include "generator.hpp"

using namespace generator::all;

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    init_gen();

    for (int i = 1; i <= 20; i++) {
        int max = i <= 4 ? 100 : 1e9;
        int b = rand_int(1, max);
        int a = rand_int(1, max);
        make_input(i, [&]() {
            std::cout << a << " " << b << std::endl;
            println(a, b);
        });
        make_output_exe(i, "./std");
    }

    // fill_outputs_exe("./std");
    show_output_first_line();

    return 0;
}