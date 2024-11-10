#include <bits/stdc++.h>

using ll = long long;

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int a, b;
    std::cin >> a >> b;

    std::srand(std::time(nullptr));
    if (b % 2 == 0) {
        std::cout << a + b + 1 << '\n';
    } else {
        std::cout << a + b << '\n';
    }

    return 0;
}