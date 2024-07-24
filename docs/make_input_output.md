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
 * @brief 根据 std_func 生成 .out 输出文件
 * @param std_func 使用标准输入和标准输出，注意使用 std::endl 而不是 '\n'
 * @param LENGTH 第一行的最大的展示长度
 * @param cover_exist 是否覆盖已存在的输出文件
 */
void fill_outputs(std::function<void()> std_func, int LENGTH = 20, bool cover_exist = true);
/**
 * @brief 根据 std 生成 .out 输出文件，并展示第一行
 * @param filename std 程序的文件名，例如 std.cpp
 * @param LENGTH 第一行的最大的展示长度
 * @param cover_exist 是否覆盖已存在的输出文件
 */
void fill_outputs(std::string filename, int LENGTH = 20, bool cover_exist = true);
```
### 注意：

1. 生成输入输出文件的 `func` **请使用 **`**std::endl**`**进行换行**，不要使用 `"\n"`；
2. 使用文件名（例如`std.cpp`）生成输出文件可以使用 `"\n"`进行换行。
