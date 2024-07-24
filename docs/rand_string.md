字符以及字符串的生成包括了一些常用的字符集，用枚举类型`CharType`表示，类型包括：
```cpp
enum CharType {
    LowerLetter, // 小写字母
    UpperLetter, // 大写字母
    Letter, // 大小写字母
    Number, // 数字
    LetterNumber, // 数字和大小写字母
    ZeroOne, // 01
};
```
### 生成字符 `rand_char`
其中，极简正则表达式 `format`的例子：
`rand_string("[abcd]{5}")`：生成长度为 5 的字符串，字符为 a, b, c, d。
```cpp
/**
 * @brief 生成一个随机字符，字符类型由 type 指定，默认为小写字母
 */
char rand_char(CharType type = LowerLetter);
/**
 * @brief 生成一个随机字符，字符类型由极简正则表达式 format 指定
 */
char rand_char(const char* format, ...);
/**
 * @brief 生成一个随机字符，字符类型由极简正则表达式 format 指定
 */
char rand_char(std::string format);
```
### 生成字符串`rand_string`
生成随机字符串：
```cpp
/**
 * @brief 生成长度为 n 的随机字符串，字符类型由 type 指定，默认为小写字母
 */
std::string rand_string(int n, CharType type = LowerLetter);
/**
 * @brief 生成长度为 [from, to] 的随机字符串，字符类型由 type 指定，默认为小写字母
 */
std::string rand_string(int from, int to, CharType type = LowerLetter);
/**
 * @brief 生成长度为 n 的随机字符串，字符类型由极简正则表达式 format 指定
 */
std::string rand_string(int n, const char* format, ...);
/**
 * @brief 生成长度为 [from, to] 的随机字符串，字符类型由极简正则表达式 format 指定
 */
std::string rand_string(int from, int to, const char* format, ...);
/**
 * @brief 生成长度为 n 的随机字符串，字符类型由极简正则表达式 format 指定
 */
std::string rand_string(int n, std::string format);
/**
 * @brief 生成长度为 [from, to] 的随机字符串，字符类型由极简正则表达式 format 指定
 */
std::string rand_string(int from, int to, std::string format);
/**
 * @brief 根据极简正则表达式 format 生成随机字符串
 * @example rand_string("[abcd]{5}") 生成长度为 5 的字符串，字符为 a, b, c, d
 */
std::string rand_string(const char* format, ...);
/**
 * @brief 根据极简正则表达式 format 生成随机字符串
 */
std::string rand_string(std::string format);
```
生成字符串包含回文子串：
```cpp
/**
 * @brief 生成长度为 n 的随机回文串，包含长度至少为 p 的回文子串，字符类型由 type 指定，默认为小写字母
 */
std::string rand_palindrome(int n, int p, CharType type = LowerLetter);
/**
 * @brief 生成长度为 n 的随机回文串，包含长度至少为 p 的回文子串，字符类型由极简正则表达式 format 指定
 */
std::string rand_palindrome(int n, int p, const char* format, ...);
/**
 * @brief 生成长度为 n 的随机回文串，包含长度至少为 p 的回文子串，字符类型由极简正则表达式 format 指定
 */
std::string rand_palindrome(int n, int p, std::string format);
```
### 注意

1. 极简正则表达式 `format`参考`testlib`文档。
