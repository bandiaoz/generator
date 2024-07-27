import os


def compile_cpp(
    file_name,
    path="/Users/chenjinglong/algorithm_competition/nowcoder/template/generator",
):
    """
    编译 cpp 文件，获得同名可执行文件；例如 std.cpp -> std, checker.cc -> checker
    """
    if not os.path.exists(file_name):
        return
    prefix = file_name.split(".")[0]
    os.system(
        f"g++-14 ./{file_name} \
        -o {prefix} -O2 -std=c++20 \
        -Wl,-stack_size -Wl,0x10000000\
        -I{path}"
    )


def remove(file_names):
    for file_name in file_names:
        os.system(f"rm {file_name} || true")


if __name__ == "__main__":
    # 删除之前生成的文件
    remove(["data.zip", "*.in", "*.out", "gen", "std", "checker"])

    # 生成输入输出文件 .in .out
    compile_cpp("gen.cpp")
    compile_cpp("std.cpp")
    compile_cpp("checker.cc")

    os.system("./gen")

    # 打包成 data.zip
    os.system("zip -q data.zip *.in *.out checker.cc")
    # 删除中间文件
    remove(["*.in", "*.out", "gen", "std", "checker"])
    remove([["starter.py"]])
