#include "../generator.hpp"
#include <cassert>

using namespace generator::all;

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

void rand_prob_test() {
    std::map<char, int> probabilities = {
        {'A', 1},
        {'B', 2},
        {'C', 3}
    };

    char result = rand_prob(probabilities);

    assert(result == 'A' || result == 'B' || result == 'C');
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

int main() {
    init_gen();

    // __change_to_double_test();

    // rand_prob_test();

    // rand_char_test();

    rand_string();

    return 0;
}