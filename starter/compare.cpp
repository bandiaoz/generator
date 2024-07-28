#include "generator.hpp"

using namespace generator::all;

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    init_gen();

    auto gen = [&]() {
    };

    compare(100, gen, "./ac", "./wa", wcmp, "/Users/chenjinglong/algorithm_competition/nowcoder/template/generator/checker/");
    system("rm hack.in");

    return 0;
}