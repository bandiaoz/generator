#include "generator.hpp"

using namespace generator::all;
using ll = long long;

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    init_gen();

    auto gen = [&]() {
    };

    compare(100, gen, "./ac", "./wa");
    system("rm hack.in");

    return 0;
}