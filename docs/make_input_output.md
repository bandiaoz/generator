标准输入文件以 `.in`结尾，标准输出文件以 `.out`结尾。
### 注册数据生成器
在 `testlib.h`中，需要通过代码 `registerGen(argc, argv, 1)`注册数据生成器。
在 `generator.hpp`中，这一点被简化了。

1. `init_gen()`

如果不需要传入参数，可以直接使用这个函数更简便的注册数据生成器。

2. `init_gen(int argc, char* argv[])`

如果你想要和`testlib.h`一样，或者存在传入参数的需求，可以调用这个函数。
### 批量生成输入文件
在一个 `cpp`中实现编写数据生成器并批量生成，可以使用以下的函数：
```cpp
/**
 * @brief 根据 func 生成输入文件，编号从 start 到 end
 */
void make_inputs(int start, int end, std::function<void()> func, const char* format = "", ...);
/**
 * @brief 根据 func 生成输入文件，编号为 index
 */
void make_input(int index, std::function<void()> func, const char* format = "", ...);
/**
 * @brief 根据 func 生成输入文件，编号为 index，同时使用随机种子
 */
void make_input_seed(int index, std::function<void()> func, const char* format = "", ...);
```
### 批量生成输出文件
在一个 `cpp`中实现批量生成输出文件，可以使用以下的函数：
```cpp
/**
 * @brief 根据 std_func 生成 index.out 输出文件
 * @return 返回输出文件的文件流
 */
std::ifstream make_output(int index, std::function<void()> std_func);
/**
 * @brief 根据 std_func 生成 .out 输出文件，编号从 start 到 end
 */
void make_outputs(int start, int end, std::function<void()> std_func);
/**
 * @brief 根据 std_func 生成 .in 对应的 .out 输出文件，
 * @param std_func 使用标准输入和标准输出，注意使用 std::endl 而不是 '\n'
 * @param cover_exist 是否覆盖已存在的输出文件
 */
void fill_outputs(std::function<void()> std_func, bool cover_exist = true);
/**
 * @brief 根据 std 可执行文件生成 index.out 输出文件
 * @param path 可执行文件的路径，可以使用字符串或者 Path 类型
 * @return 返回输出文件的文件流
 */
template <typename T>
std::ifstream make_output_exe(int index, T path);
/**
 * @brief 根据 std 可执行文件生成 .out 输出文件
 * @param path 可执行文件的路径，可以使用字符串或者 Path 类型
 */
template <typename T>
void make_outputs_exe(int start, int end, T path);
/**
 * @brief 根据 std 可执行文件生成所有 .in 对应的 .out 输出文件
 * @param path 可执行文件的路径，可以使用字符串或者 Path 类型
 * @param cover_exist 是否覆盖已存在的输出文件
 */
template <typename T>
void fill_outputs_exe(T path, bool cover_exist = true);
/**
 * @brief 展示所有输出文件的第一行，最大长度为 LENGTH
 * @param LENGTH 最大展示长度，默认为 20
 */
void show_output_first_line(int LENGTH = 20);
```
### 注意：

1. 生成输入输出文件的 `func` **请使用** `std::endl`**进行换行**，不要使用 `"\n"`，或者使用 `println()`。
2. 使用文件名（例如`std.cpp`）生成输出文件可以使用 `"\n"`进行换行。
