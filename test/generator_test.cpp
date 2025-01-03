#include "../generator.hpp"
#include <cassert>

using namespace generator::all;

namespace rand_numeric_test {
/**
 * @brief 随机获得一个布尔值
 */
void rand_bool_test() {
    bool a = rand_bool();
    assert(a == true || a == false);
}

/**
 * @brief 随机获得一个整数
 * @param 参数只要可以转换为整型即可，或者是一个格式字符串
 */
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

/**
 * @brief 随机获得一个奇数
 */
void rand_odd_test() {
    int a = rand_odd(1, 5);
    assert(a % 2 == 1 && a >= 1 && a <= 5);

    int b = rand_odd("[1, 5]");
    assert(b % 2 == 1 && b >= 1 && b <= 5);
}

/**
 * @brief 随机获得一个偶数
 */
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

/**
 * @brief 随机获得一个实数
 */
void rand_real_test() {
    // 不带参数默认是 [0, 1)
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

/**
 * @brief 根据给定的概率随机生成
 */
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
/**
 * @brief 随机生成一个字符，可选的格式有小写字母、大写字母、字母、数字、字母数字、01
 */
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

/**
 * @brief 随机生成一个字符串，可选的格式有小写字母、大写字母、字母、数字、字母数字、01
 */
void rand_string_test() {
    // 默认生成小写字母串
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

    // 生成长度为 3 到 5 的小写字母串
    std::string e = rand_string(3, 5, "[a-z]");
    assert(e.size() >= 3 && e.size() <= 5 && std::all_of(e.begin(), e.end(), islower));

    // 生成长度为 5 的小写字母串
    std::string f = rand_string("[a-z]{5}");
    assert(f.size() == 5 && std::all_of(f.begin(), f.end(), islower));

    // 生成长度为 5 到 10 的小写字母串
    std::string g = rand_string("[a-z]{5, 10}");
    assert(g.size() >= 5 && g.size() <= 10 && std::all_of(g.begin(), g.end(), islower));
}

/**
 * @brief 随机生成一个字符串，包含回文子串
 */
void rand_palindrome_test() {
    // 生成长度为 10 的小写字母串，至少包含长度为 5 的回文子串
    std::string a = rand_palindrome(10, 5);

    std::string b = rand_palindrome(10, 5, "[a-z]");
}

/**
 * @brief 随机生成一个合法括号串
 */
void rand_rand_bracket_seq_test() {
    // 生成长度为 10 的合法括号串
    std::string a = rand_bracket_seq(10, "[](){}");
    // 生成长度为 5 到 10 的合法括号串
    std::string b = rand_bracket_seq(5, 10, "[](){}");
}
}

namespace rand_vector_test {
/**
 * @brief 随机生成一个排列
 */
void rand_p_test() {
    // 生成一个长度为 3 的排列，默认最小值为 0
    std::vector<int> a = rand_p(3);
    std::sort(a.begin(), a.end());
    assert(a[0] == 0 && a[1] == 1 && a[2] == 2);

    // 生成一个长度为 3 的排列，最小值为 1
    std::vector<int> b = rand_p(3, 1);
    std::sort(b.begin(), b.end());
    assert(b[0] == 1 && b[1] == 0 && b[2] == 2);
}

/**
 * @brief 随机生成一个数组，数组的元素和为 sum
 */
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

/**
 * @brief 随机生成一个数组，数组元素由 func 生成
 */
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
 * @brief 随机边权生成函数
 */
auto edges_weight_function = []() {
    return rand_int(1, 100);
};
/**
 * @brief 随机点权生成函数
 */
auto nodes_weight_function = []() {
    return rand_int(1, 100);
};
/**
 * @brief 随机图测试
 */
void graph_constructor_test() {
    int n = 10, m = 10, begin_node = 1;
    // 以带点权带边权的图为例，生成一个 n 个点 m 条边的图
    both_weight::Graph<int, int> g(n, m, begin_node, 
        nodes_weight_function, edges_weight_function);
    g.set_begin_node(begin_node); // 设置点的起始编号，默认为 1
    g.set_direction(false); // 设置是否有向，默认为 false
    g.set_multiply_edge(false); // 设置是否允许重边，默认为 false
    g.set_self_loop(false); // 设置是否允许自环，默认为 false
    g.set_connect(false); // 设置是否连通，默认为 false
    g.set_swap_node(false); // 设置是否随机交换 u, v，默认为无向图交换，有向图不交换，注意不要在有向图下设置为交换
    
    g.set_nodes_weight_function(nodes_weight_function); // 如果有点权，设置点权的生成函数
    g.set_edges_weight_function(edges_weight_function); // 如果有边权，设置边权的生成函数
    
    g.set_output_node_count(true); // 设置是否输出点的数量，默认为 true
    g.set_output_edge_count(true); // 设置是否输出边的数量，默认为 true

    g.gen(); // 生成图
    println(g); // 输出图

    // 类似地，生成不带权、带点权、带边权的图
    unweight::Graph g_unweight(n, m);
    node_weight::Graph<int> g_node_weight(n, m);
    edge_weight::Graph<int> g_edge_weight(n, m);
}
/**
 * @brief 随机有向无环图测试
 * @note 和随机图的区别在于，不允许设置有向/无向以及自环
 */
void DAG_constructor_test() {
    int n = 10, m = 10, begin_node = 1;
    // 以带点权带边权的有向无环图为例，生成一个 n 个点 m 条边的有向无环图
    both_weight::DAG<int, int> dag(n, m, begin_node, 
        nodes_weight_function, edges_weight_function);
    dag.set_begin_node(begin_node); // 设置点的起始编号，默认为 1
    dag.set_multiply_edge(false); // 设置是否允许重边，默认为 false
    dag.set_connect(false); // 设置是否连通，默认为 false
    
    dag.set_nodes_weight_function(nodes_weight_function); // 如果有点权，设置点权的生成函数
    dag.set_edges_weight_function(edges_weight_function); // 如果有边权，设置边权的生成函数

    dag.set_output_node_count(true); // 设置是否输出点的数量，默认为 true
    dag.set_output_edge_count(true); // 设置是否输出边的数量，默认为 true

    dag.gen(); // 生成图
    println(dag); // 输出图

    // 类似地，生成不带权、带点权、带边权的图
    unweight::DAG dag_unweight(n, m);
    node_weight::DAG<int> dag_node_weight(n, m);
    edge_weight::DAG<int> dag_edge_weight(n, m);
}
/**
 * @brief 随机环图测试
 * @note 环图是无重边，无自环，一定连通，边数等于点数的首尾相接的图
 */
void cycle_graph_constructor_test() {
    int n = 10, begin_node = 1;
    // 以带点权带边权的环图为例，生成一个 n 个点的环图
    both_weight::CycleGraph<int, int> cycle(n, begin_node, 
        nodes_weight_function, edges_weight_function);
    cycle.set_begin_node(begin_node); // 设置点的起始编号，默认为 1
    cycle.set_direction(false); // 设置是否有向，默认为 false
    cycle.set_nodes_weight_function(nodes_weight_function); // 如果有点权，设置点权的生成函数
    cycle.set_edges_weight_function(edges_weight_function); // 如果有边权，设置边权的生成函数

    cycle.set_output_node_count(true); // 设置是否输出点的数量，默认为 true
    cycle.set_output_edge_count(true); // 设置是否输出边的数量，默认为 true

    cycle.gen(); // 生成图
    println(cycle); // 输出图

    // 类似地，生成不带权、带点权、带边权的图
    unweight::CycleGraph cycle_unweight(n);
    node_weight::CycleGraph<int> cycle_node_weight(n);
    edge_weight::CycleGraph<int> cycle_edge_weight(n);
}
/**
 * @brief 随机基环树测试
 * @note 基环树是无重边，无自环，一定连通，边数一定的无向图
 * @note 环的大小是 `cycle <= n`
 * @note 注意默认输出方式和图相同，会输出点的数量和边的数量
 */
void pseudoTree_constructor_test() {
    int n = 10, begin_node = 1, cycle = -1;
    // 以带点权带边权的基环树为例，生成一个 n 个点的基环树
    both_weight::PseudoTree<int, int> pseudoTree(n, begin_node, cycle, 
        nodes_weight_function, edges_weight_function);
    pseudoTree.set_begin_node(begin_node); // 设置点的起始编号，默认为 1
    pseudoTree.set_swap_node(true); // 设置是否随机交换 u, v，默认为 true

    pseudoTree.set_cycle(cycle); // 设置环的大小，默认为 -1，表示随机大小

    pseudoTree.set_nodes_weight_function(nodes_weight_function); // 如果有点权，设置点权的生成函数
    pseudoTree.set_edges_weight_function(edges_weight_function); // 如果有边权，设置边权的生成函数

    pseudoTree.set_output_node_count(true); // 设置是否输出点的数量，默认为 true
    pseudoTree.set_output_edge_count(true); // 设置是否输出边的数量，默认为 true

    pseudoTree.gen(); // 生成图
    println(pseudoTree); // 输出图

    // 类似地，生成不带权、带点权、带边权的图
    unweight::PseudoTree pseudoTree_unweight(n, cycle);
    node_weight::PseudoTree<int> pseudoTree_node_weight(n, cycle);
    edge_weight::PseudoTree<int> pseudoTree_edge_weight(n, cycle);
}
/**
 * @brief 随机树测试
 */
void tree_constructor_test() {
    int n = 10, begin_node = 1;
    bool is_rooted = false;
    int root = 1;
    // 以带点权边权的树为例，生成一个 n 个点的树
    both_weight::Tree<int, int> tree(n, begin_node, is_rooted, root,
        nodes_weight_function, edges_weight_function, 
        RandomFather);
    tree.set_node_count(n); // 设置点的数量
    tree.set_begin_node(begin_node); // 设置点的起始编号，默认为 1
    tree.set_is_rooted(is_rooted); // 设置是否是有根树，默认为 false
    tree.set_root(root); // 设置根的编号，默认为 1，如果不是有根树，会有警告

    tree.set_output_node_count(false); // 设置是否输出点的数量，默认为 true
    tree.set_output_root(false); // 设置是否输出根的编号，默认为 true
    tree.set_swap_node(false); // 设置是否随机交换 fa, u，默认为有根树不交换，无根树交换，注意不要在有根树下设置为交换
    tree.set_node_indices(std::vector{1, 2, 3, 4, 5, 6, 7, 8, 9, 10}); // 设置点的编号
    tree.set_node_indices(1, 11); // 将 1 号点的编号设置为 11，注意 index 范围要在 [1, n] 之间
    tree.set_nodes_weight_function(nodes_weight_function); // 如果有点权，设置点权的生成函数
    tree.set_edges_weight_function(edges_weight_function); // 如果有边权，设置边权的生成函数

    tree.set_tree_generator(RandomFather); // 设置生成树的方式，默认为随机父亲，期望高度为 O(log n)
    tree.set_tree_generator(Pruefer); // 设置生成树的方式，期望高度为 O(sqrt(n))

    tree.gen(); // 生成树
    println(tree); // 输出树

    // 类似地，生成不带权、带点权、带边权的树
    unweight::Tree tree_unweight(n);
    node_weight::Tree<int> tree_node_weight(n);
    edge_weight::Tree<int> tree_edge_weight(n);
}
/**
 * @brief 菊花带链测试
 * @note 生成一个菊花带链，菊花的大小由 `_flower_size` 决定，链的大小由 `_chain_size` 决定。
 *       继承自 `_Tree`，所以可以使用 `_Tree` 的除了「设置生成树的方法」外的所有方法。
 */
void flowerChain_constructor_test() {
    int n = 10, begin_node = 1;
    bool is_rooted = false;
    int root = 1;
    int flower_size = -1; // 默认为-1，表示随机大小
    // 以带点权边权的树为例，生成一个 n 个点的树
    both_weight::FlowerChain<int, int> flowerChain(
        n, begin_node, is_rooted, root, flower_size,
        nodes_weight_function, edges_weight_function);
    flowerChain.set_flower_size(n / 10); // 设置菊花的大小，注意菊花大小加链大小等于 n
    flowerChain.set_nodes_weight_function(nodes_weight_function); // 如果有点权，设置点权的生成函数
    flowerChain.set_edges_weight_function(edges_weight_function); // 如果有边权，设置边权的生成函数

    flowerChain.gen(); // 生成树
    println(flowerChain); // 输出树

    // 类似地，生成不带权点权边权、带点权、带边权的树
    unweight::FlowerChain flowerChain_unweight(n);
    node_weight::FlowerChain<int> flowerChain_node_weight(n);
    edge_weight::FlowerChain<int> flowerChain_edge_weight(n);
}
/**
 * @brief 随机森林测试
 * @note 根据 `_trees_size` 来生成森林，每个树的大小由 `_trees_size` 决定。
 */
void forest_constructor_test() {
    int n = 10000, m = rand_int(n - 10, n - 1), begin_node = 1;
    // 以带点权边权的树为例，生成一个 n 个点 m 条边的森林，也就是有 n - m 个树
    both_weight::Forest<int, int> forest(n, m, begin_node, 
        nodes_weight_function, edges_weight_function);
    forest.set_node_count(n); // 设置点的数量
    forest.set_begin_node(1); // 设置点的起始编号，默认为 1
    // forest.add_tree_size(5); // 添加一棵树，大小为 5
    // forest.set_trees_size(std::vector{5, 3, 2}); // 设置每棵树的大小，会覆盖之前的设置
    forest.set_nodes_weight_function(nodes_weight_function); // 如果有点权，设置点权的生成函数
    forest.set_edges_weight_function(edges_weight_function); // 如果有边权，设置边权的生成函数

    forest.gen(); // 生成森林
    println(forest); // 输出森林

    // 类似地，生成不带权、带点权、带边权的森林
    unweight::Forest forest_unweight(n, m);
    node_weight::Forest<int> forest_node_weight(n, m);
    edge_weight::Forest<int> forest_edge_weight(n, m);
}
}

namespace rand_geometry_test {
/**
 * @brief 生成随机点测试
 */
void point_test() {
    Point<int> p1;
    p1.rand(1, 2, 3, 4); // 生成一个坐标在 x ∈ [1, 2], y ∈ [3, 4] 的点
    Point<int> p2;
    p2.rand("[-1,1]"); // 生成一个坐标在 x,y ∈ [-1, 1] 的点
    Point<double> p3;
    p3.rand("y[-1, 0] x(0, 1]"); // 生成一个坐标在 x ∈ (0, 1], y ∈ [-1, 0] 的点
    println(p1, p2, p3);

    Point<int> p4 = rand_point<int>(1, 2, 3, 4); // 生成一个坐标在 x ∈ [1, 2], y ∈ [3, 4] 的点
    Point<int> p5 = rand_point<int>("[-1,1]"); // 生成一个坐标在 x,y ∈ [-1, 1] 的点
    Point<double> p6 = rand_point<double>("y[-1, 0] x(0, 1]"); // 生成一个坐标在 x ∈ (0, 1], y ∈ [-1, 0] 的点
    Point<int> p7 = rand_point<int>(-1, 1); // 生成一个坐标在 x,y ∈ [-1, 1] 的点
}

/**
 * @brief 生成随机凸包测试
 */
void convexHull_test() {
    ConvexHull<int> c(10);
    c.set_xy_limit("[-20, 20]"); // 设置点的坐标范围
    c.gen();
    println(c);
}
}

int main() {
    init_gen();
    
    rand_geometry_test::point_test();
    rand_geometry_test::convexHull_test();

    return 0;
}