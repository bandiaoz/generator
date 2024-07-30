#include "generator.hpp"

using namespace generator::all;

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    init_gen();

    auto gen = [&]() {
        int a, b;
        a = rand_int(1, 1e9);
        b = rand_int(1, 1e9);
        println(a, b);
    };

    // compare(100, gen, "./std", "./wa", mycmp);
    compare(100, gen, "./std", "./wa");

    return 0;
}