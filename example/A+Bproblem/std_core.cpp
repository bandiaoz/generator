#include "core_mode.hpp"

class Solution {
   public:
    int add(int a, int b) {
        return a + b;
    }
};

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    CoreMode::println(std::apply(
        [](auto &&...params) { return Solution().add(params...); },
        CoreMode::read_function_args<decltype(&Solution::add)>()
	));

    return 0;
}