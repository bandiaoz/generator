一些预设的`checker`：

- `lcmp`：忽略多余的空格和空行，将文件当做若干单词进行比较；
- `ncmp`：按顺序比较 64 位整数；
- `nyesno`：输出仅包含一组或多组`YES/NO`，大小写不敏感；
- `rcmp4 / rcmp6 / rcmp9`：输出仅包含一组或多组`double`，符合精度要求即可；
- `wcmp`：按顺序比较字符串（不带空格，换行符等非空字符）。
```cpp
enum Checker {
    mycmp, // 自定义 checker
    lcmp,
    ncmp,
    yesno,
    rcmp4,
    rcmp6,
    rcmp9,
    wcmp,
    MaxChecker
};
```
### 相关函数
```cpp
/**
 * @param 如果当前目录有 ./checker 可执行文件，判断输出是否正确；否则警告 checker 不存在
 */
void check_output();
/**
 * @brief 对拍 num_case 组数据，使用 std_path 和 wa_path 两个可执行文件
 * @param std_path 相对可信的可执行文件
 * @param wa_path 待测的可执行文件
 * @param checker 使用的 checker，默认为 wcmp
 */
void compare(int num_case, std::function<void()> gen_func, Path std_path, Path wa_path, Checker checker = wcmp);
```
