#include "../generator.hpp"
#include <cassert>

bool is_equal(double a, double b) {
    return std::abs(a - b) < 1e-6;
}

void __change_to_double_test() {
    double result1 = generator::rand_numeric::__change_to_double(5);
    assert(is_equal(result1, 5.0));

    float result2 = generator::rand_numeric::__change_to_double(3.14f);
    assert(is_equal(result2, 3.14));

    long long result3 = generator::rand_numeric::__change_to_double(10LL);
    assert(result3 == 10.0);
}

int main() {
    __change_to_double_test();

    return 0;
}