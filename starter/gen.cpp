#include "generator.hpp"

using namespace generator::all;
using ll = long long;

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    init_gen();

    fill_inputs(4, [&]() {
    });

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
            std::remove(std::format("data/{}.in", index).c_str());
        }
    }

    copy_input("./data/handmade");
    validate("./val");
    fill_outputs_exe("./std");
    show_output_first_line();
    compare(3000, mycmp, "./std", "./sol/wa");

    return 0;
}