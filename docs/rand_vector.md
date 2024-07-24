### 生成随机数组
```cpp
/**
 * @brief 生成长度为 n 的随机排列，元素范围为 [0, n)，等价于 rnd.perm(n)
 */
template <typename T>
std::vector<T> rand_p(T n);
/**
 * @brief 生成长度为 n 的随机排列，元素范围为 [s, s + n)，等价于 rnd.perm(n, s)
 */
template <typename T, typename E>
std::vector<E> rand_p(T n, E s = 0);
/**
 * @brief 生成长度为 size 的随机整数数组，元素和为 sum，等价于 rnd.partition(size, sum)
 */
template<typename T>
std::vector<T> rand_sum(int size, T sum);
/**
 * @brief 生成长度为 size 的随机整数数组，元素和为 sum，且每个元素不小于 min_par；
 *        等价于 rnd.partition(size, sum, min_part)
 */
template<typename T>
std::vector<T> rand_sum(int size, T sum, T min_part);
/**
 * @brief 生成长度为 size 的随机整数数组，元素范围为 [from, to]，元素和为 sum
 */
template<typename T>
std::vector<T> rand_sum(int size, T sum, T from, T to);
/**
 * @brief 生成长度为 size 的随机整数数组，每个元素由 func 生成
 */
template <typename T>
std::vector<T> rand_vector(int size, std::function<T()> func);
/**
 * @brief 生成长度为 [from, to] 的随机整数数组，每个元素由 func 生成
 */
template <typename T>
std::vector<T> rand_vector(int from, int to, std::function<T()> func);
/**
 * @brief 生成长度为 sum(v[i]) 的随机整数数组，每个元素 i + offset 的个数为 v[i]
 * @example shuffle_index({1, 2, 3}, 1) -> {1, 2, 2, 3, 3, 3}
 */
std::vector<int> shuffle_index(std::vector<int> v, int offset = 0);
```
