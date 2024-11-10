#include "generator.hpp"

using namespace generator::all;

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    init_gen();

    // 手捏数据
    std::vector<std::pair<int, int>> inputs {
        {1, 1}, {1, 2}, 
        {2, 1}, {2, 2}
    };
    for (auto [a, b] : inputs) {
        fill_input([&]() {
            println(a, b);
        });
    }

    int limit = 100;
    // 小数据
    fill_inputs(4, [&]() {
        int a = rand_int(1, limit);
        int b = rand_int(1, limit);
        println(a, b);
    });

    // 大数据
    fill_inputs(16, [&]() {
        int a = rand_int(1, limit);
        int b = rand_int(1, limit);
        println(a, b);
    });

    // 对输出有要求的数据
    int cases = 3;
    while (cases--) {
        int index = fill_input([&]() {
            int a = rand_int(1, limit);
            int b = rand_int(1, limit);
            println(a, b);
        });
        auto out = make_output_exe(index, "./std");
        int ans;
        out >> ans;
        if (ans % 2 == 1) {
            cases++;
        }
    }

    copy_input("./handmade");
    validate("./val");
    fill_outputs_exe("./std");
    show_output_first_line();
    compare(3000, mycmp, "./std", "./sol/wa");

    return 0;
}