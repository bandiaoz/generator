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

namespace rand_string_test {
void rand_char_test() {
    // 随机生成一个字符，默认是小写字母
    char a = rand_char();
    assert(isalpha(a));
    // 随机生成一个数字字符
    char b = rand_char(Number);
    assert(isdigit(b));
    
    // 错误的极简正则 format
    char c = rand_char(std::string("abcd"));
    assert(c == 'a');
    // 正确的极简正则 format
    char d = rand_char("[a%cf]", 'd');
    assert(d == 'a' || d == 'd' || d == 'f');
}

void rand_string_test() {
    std::string a = rand_string(5);
    assert(a.size() == 5 && std::all_of(a.begin(), a.end(), islower));

    std::string b = rand_string(5, ZeroOne);
    assert(b.size() == 5 && std::all_of(b.begin(), b.end(), [](char c) {
        return c == '0' || c == '1';
    }));

    std::string c = rand_string(4, 5, Number);
    assert(c.size() >= 4 && c.size() <= 5 && std::all_of(c.begin(), c.end(), isdigit));

    std::string d = rand_string(5, "[ab]");
    assert(d.size() == 5 && std::all_of(d.begin(), d.end(), [](char c) {
        return c == 'a' || c == 'b';
    }));

    std::string e = rand_string(3, 5, "[a-z]");
    assert(e.size() >= 3 && e.size() <= 5 && std::all_of(e.begin(), e.end(), islower));

    std::string f = rand_string("[a-z]{5}");
    assert(f.size() == 5 && std::all_of(f.begin(), f.end(), islower));

    std::string g = rand_string("[a-z]{5, 10}");
    assert(g.size() >= 5 && g.size() <= 10 && std::all_of(g.begin(), g.end(), islower));
}

void rand_palindrome_test() {
    // 生成长度为 10 的小写字母串，至少包含长度为 5 的回文子串
    std::string a = rand_palindrome(10, 5);

    std::string b = rand_palindrome(10, 5, "[a-z]");
}

void rand_rand_bracket_seq_test() {
    // 生成长度为 10 的合法括号串
    std::string a = rand_bracket_seq(10, "[](){}");
    // 生成长度为 5 到 10 的合法括号串
    std::string b = rand_bracket_seq(5, 10, "[](){}");
}
}

namespace rand_vector_test {
void rand_p_test() {
    // 生成一个长度为 3 的排列
    std::vector<int> a = rand_p(3);
    std::sort(a.begin(), a.end());
    assert(a[0] == 0 && a[1] == 1 && a[2] == 2);

    std::vector<int> b = rand_p(3, 1);
    std::sort(b.begin(), b.end());
    assert(b[0] == 1 && b[1] == 0 && b[2] == 2);
}

void rand_sum_test() {
    std::vector<int> a = rand_sum(3, 5);
    assert(a.size() == 3 && std::accumulate(a.begin(), a.end(), 0) == 5);

    std::vector<int> b = rand_sum(3, 10, 2);
    assert(b.size() == 3 && std::accumulate(b.begin(), b.end(), 0) == 10
                        && std::ranges::min(b) >= 2);
    
    std::vector<int> c = rand_sum(3, 10, 2, 5);
    assert(c.size() == 3 && std::accumulate(c.begin(), c.end(), 0) == 10
                        && std::ranges::min(c) >= 2 && std::ranges::max(c) <= 5);
}

void rand_vector_test() {
    std::vector a = rand_vector<int>(5, []() {
        return rand_int(1, 100);
    });
    assert(a.size() == 5 && std::all_of(a.begin(), a.end(), [](int x) {
        return x >= 1 && x <= 100;
    }));

    std::vector b = rand_vector<int>(5, 10, []() {
        return rand_int(1, 100);
    });
    assert(b.size() >= 5 && b.size() <= 10 && std::all_of(b.begin(), b.end(), [](int x) {
        return x >= 1 && x <= 100;
    }));
}

void shuffle_index_test() {
    std::vector<int> a = {1, 2, 3};
    std::vector<int> b = shuffle_index(a, 1);
    std::sort(b.begin(), b.end());
    assert((b == std::vector{1, 2, 2, 3, 3, 3}));
}
}

namespace rand_graph_test {
/**
 * @brief 随机图测试
 */
void graph_constructor_test() {
    int n = 10;
    int m = 10;
    // 以带点权带边权的图为例，生成一个 n 个点 m 条边的图
    both_weight::Graph<int, int> g(n, m);
    g.set_begin_node(1); // 设置点的起始编号，默认为 1
    g.set_direction(false); // 设置是否有向，默认为 false
    g.set_multiply_edge(false); // 设置是否允许重边，默认为 false
    g.set_self_loop(false); // 设置是否允许自环，默认为 false
    g.set_connect(false); // 设置是否连通，默认为 false
    g.set_swap_node(false); // 设置是否随机交换 u, v，默认为无向图交换，有向图不交换，注意不要在有向图下设置为交换
    g.set_nodes_weight_function([]() { // 如果有点权，设置点权的生成函数
        return rand_int(1, 100);
    });
    g.set_edges_weight_function([]() { // 如果有边权，设置边权的生成函数
        return rand_int(1, 100);
    });
    g.set_output_node_count(true); // 设置是否输出点的数量，默认为 true
    g.set_output_edge_count(true); // 设置是否输出边的数量，默认为 true

    g.gen(); // 生成图
    println(g); // 输出图

    // 类似地，生成不带权、带点权、带边权的图
    unweight::Graph g_unweight(n, m);
    node_weight::Graph<int> g_node_weight(n, m);
    edge_weight::Graph<int> g_edge_weight(n, m);
}
}

int main() {
    init_gen();

    

    return 0;
}