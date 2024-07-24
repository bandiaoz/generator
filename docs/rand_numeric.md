### 生成整数 `rand_int`
可以使用以下函数来生成一个整数：
```cpp
/**
 * @brief 生成 [0, n) 范围内的随机整数，等价于 rnd.next(n)
 * @tparam T 整数类型
 */
template <typename T = int>
typename std::enable_if<std::is_integral<T>::value, T>::type 
rand_int(T n);

/**
 * @brief 生成 [from, to] 范围内的随机整数，等价于 rnd.next(from, to)
 * @tparam T 要求是一个整数类型
 */
template <typename T = int>
typename std::enable_if<std::is_integral<T>::value, T>::type
rand_int(T from, T to);

/**
 * @brief 生成给定范围 [from, to] 内的随机整数，等价于 rnd.next(from, to)
 * @tparam T 和 U 都要求可以转换为 long long 类型
 */
template <typename T = long long, typename U = long long>
typename std::enable_if<
    std::is_convertible<T, long long>::value && 
    std::is_convertible<U, long long>::value, long long>::type
rand_int(T from, U to);

/**
 * @brief 生成给定范围内的随机整数，格式为 "[from, to]"
 * @param format 格式字符串，例如 "[1, 10)"
 * @note 注意，如果 from 和 to 是小数类型，会先强制转换再判断括号的开闭；
 * @note 例如 rand_int("[1.0, 10.2)") 会生成 [1, 10) 范围内的随机整数，不会生成 10。
 */
long long rand_int(const char* format, ...);

```
### 生成奇数 `rand_odd`和偶数 `rand_even`
可以使用以下函数来生成一个奇数：
```cpp
/**
 * @brief 生成给定 [from, to] 范围内的随机奇数
 */
template <typename T = long long, typename U = long long>
typename std::enable_if<std::is_integral<T>::value && std::is_integral<U>::value, long long>::type
rand_odd(T from, U to);

/**
 * @brief 生成给定范围内的随机奇数
 */
long long rand_odd(const char* format, ...);
```
类似地，可以使用以下函数来生成一个偶数：
```cpp
/**
 * @brief 生成给定 [from, to] 范围内的随机偶数
 */
template <typename T = long long, typename U = long long>
typename std::enable_if<std::is_integral<T>::value && std::is_integral<U>::value, long long>::type
rand_even(T from, U to);

/**
 * @brief 生成给定范围内的随机偶数
 */
long long rand_even(const char* format, ...);
```
### 生成实数 `rand_real`
```cpp
/**
 * @brief 生成 [0, 1) 范围内的随机实数，等价于 rnd.next()
 */
double rand_real();
/**
 * @brief 生成 [0, n) 范围内的随机实数
 * @tparam T 浮点类型或者可以转换为 double 类型
 */
template <typename T = double>
double rand_real(T n);

/**
 * @brief 生成给定范围内 [from, to) 的随机实数
 * @tparam T 和 U 都要求是浮点类型或者可以转换为 double 类型
 */
template <typename T = double, typename U = double>
typename std::enable_if<is_double_valid<T>() && is_double_valid<U>(), double>::type
rand_real(T from, U to);

/**
 * @brief 生成给定范围内的随机实数，格式为 "[from, to]", "[from, to)"
 * @param format 格式字符串，可以使用科学计数法，例如 "[1e-2, 1E2)"
 */
double rand_real(const char* format, ...);
```
### 给定概率生成元素
```cpp
/**
 * @brief 根据概率选择一个元素
 * @param map 一个 map 或 unordered_map 容器，其 value 类型必须为整数，表示每个 key 有 value / sum(value) 的概率被选中
 * @return 选中的元素
 */
template <typename Con>
typename std::enable_if<
        (std::is_same<Con, std::map<typename Con::key_type, typename Con::mapped_type>>::value ||
        std::is_same<Con, std::unordered_map<typename Con::key_type, typename Con::mapped_type>>::value) &&
        std::is_integral<typename Con::mapped_type>::value,
        typename Con::key_type
>::type
rand_prob(const Con& map);
```
