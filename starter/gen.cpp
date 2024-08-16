#include "generator.hpp"

using namespace generator::all;
using ll = long long;

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    init_gen();

    int limit = 10;
    fill_inputs(4, [&]() {
    });

    limit = 1e5;
    fill_inputs(16, [&]() {
    });

    for (int cases = 0; cases < 3; cases++) {
        int index = fill_input([&]() {
        });
        auto out = make_output_exe(index, "./std");
        std::string ok;
        out >> ok;
        if (ok == "NO") {
            cases--;
        }
    }

    fill_outputs_exe("./std");
    validate("./val");
    show_output_first_line();
    check_output();

    return 0;
}