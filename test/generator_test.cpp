#include "../generator.hpp"
#include <cassert>

using namespace generator::all;

namespace rand_numeric_test {
void rand_int_test() {
    int a = rand_int(10);
    assert(a >= 0 && a < 10);

    int b = rand_int(5, 10);
    assert(b >= 5 && b <= 10);

    // 参数可以转换为 long long 类型即可
    long long c = rand_int(1e2, 1000);
    assert(c >= 100 && c <= 1000);

    // 根据格式字符串生成
    long long d = rand_int("[1, 10)");
    assert(d >= 1 && d < 10);

    // 这里先转换为整型，再判断边界
    long long e = rand_int("[1.0, 10.2)");
    assert(e >= 1 && e < 10);
}

void rand_odd_test() {
    int a = rand_odd(1, 5);
    assert(a % 2 == 1 && a >= 1 && a <= 5);

    int b = rand_odd("[1, 5]");
    assert(b % 2 == 1 && b >= 1 && b <= 5);
}

void rand_even_test() {
    int a = rand_even(1, 5);
    assert(a % 2 == 0 && a >= 1 && a <= 5);

    int b = rand_even("[1, 5]");
    assert(b % 2 == 0 && b >= 1 && b <= 5);
}

/**
 * @brief 定义一个浮点比较函数
 */
bool __is_equal(double a, double b) {
    return std::abs(a - b) < 1e-6;
}

void __change_to_double_test() {
    double result1 = generator::rand_numeric::__change_to_double(5);
    assert(__is_equal(result1, 5.0));

    float result2 = generator::rand_numeric::__change_to_double(3.14f);
    assert(__is_equal(result2, 3.14));

    long long result3 = generator::rand_numeric::__change_to_double(10LL);
    assert(__is_equal(result3, 10.0));
}

void rand_real_test() {
    double a = rand_real();
    assert(a >= 0 && a < 1);

    double b = rand_real(5.0);
    assert(b >= 0 && b < 5.0);

    // 参数是浮点类型或者可以转换为浮点类型即可
    double c = rand_real(1e2, 1000LL);
    assert(c >= 100 && c < 1000);
    
    // 根据格式字符串生成
    double d = rand_real("[1e-2, 1E2)");
    assert(d >= 1e-2 && d < 1e2);
}

void rand_prob_test() {
    std::map<char, int> probabilities = {
        {'A', 1},
        {'B', 2},
        {'C', 3}
    };

    // 生成的结果只能是 A, B, C 中的一个，概率分别是 1/6, 2/6, 3/6
    char result = rand_prob(probabilities);
    assert(result == 'A' || result == 'B' || result == 'C');
}
}

void rand_char_test() {
    char a = rand_char();
    char b = rand_char(Number);
    char c = rand_char("[a%cf]", 'd');

    assert(isalpha(a));
    assert(isdigit(b));
    std::cerr << rand_char("[a%cf]", 'd') << " " <<  
        rand_char("[a%cf]", 'd') << " " << 
        rand_char("[a%cf]", 'd') << " " <<
        rand_char("[a%cf]", 'd') << " " <<
        rand_char("[a%cf]", 'd') << " " <<
        rand_char("[a%cf]", 'd') << " " <<
        std::endl;

    std::cerr << rand_char(std::string("abcs")) << " " << 
        rand_char(std::string("abcs")) << " " << 
        rand_char(std::string("abcs")) << " " << 
        rand_char(std::string("abcs")) << " " <<
        rand_char(std::string("abcs")) << " " <<
        rand_char(std::string("abcs")) << " " <<
        std::endl;
}

void rand_string() {
    std::cout << rand_string("[abcd]{5}") << std::endl;
    std::cout << rand_string(5, "[abcd]") << std::endl;

    std::cout << rand_string("[abcd]{5, 10}") << std::endl;
    std::cout << rand_string(5, 10, "[abcd]") << std::endl;
}

void rand_vector() {
    std::vector a{1, 3, 3, 4, 8};
    auto b = shuffle_index(a);
    for (auto i : b) {
        std::cerr << i << " ";
    }
}

int main() {
    init_gen();

    // __change_to_double_test();

    // rand_prob_test();

    // rand_char_test();

    // rand_string();

    rand_vector();

    return 0;
}