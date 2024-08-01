#pragma once
#include <format>
#include <string>

/**
 * @file settings.hpp，和 generator.hpp 放在同一目录下
 * @brief 使用者可以在这里设置一些参数
 */
namespace settings {
/**
 * @brief checker 文件夹路径
 */
std::string checker_folder_path =
    "/Users/chenjinglong/algorithm_competition/nowcoder/generator/checker/";

std::vector<std::string> cpp_files_prefix;
/**
 * @brief 编译当前目录下的 cpp 文件
 * @param filename cpp 的完整文件名，包括后缀，不包括路径
 */
void compile(std::string filename,
             std::string path = "/Users/chenjinglong/algorithm_competition/nowcoder/generator") {
    std::string prefix = filename.substr(0, filename.find('.'));
    std::string command = std::format(
        "g++-14 ./{} "
        " -o {} -O2 -std=c++20 "
        "-Wl,-stack_size -Wl,0x10000000 "
        "-I{}",
        filename, prefix, path);
    std::cerr << command << '\n';
    if (int return_code = system(command.c_str()); return_code == 0) {
        cpp_files_prefix.push_back(prefix);
    } else {
        std::cerr << "Compile " << filename << " failed with return code "
                  << return_code << '\n';
    }
}

/**
 * @brief 清除当前目录下的所有 .out .ans .in 以及编译生成的文件
 */
void clear() {
    system("rm -f *.out *.ans *.in");
    for (const auto& prefix : cpp_files_prefix) {
        std::string command = std::format("rm -f {}", prefix);
        system(command.c_str());
    }
    cpp_files_prefix.clear();
}
}  // namespace settings