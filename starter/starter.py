import os


def compile_cpp(file_name, path="./"):
    """
    编译 cpp 文件，获得同名可执行文件；例如 std.cpp -> std, checker.cc -> checker
    """
    prefix = file_name.split(".")[0]
    os.system(
        f"g++-14 ./{file_name} \
        -o {prefix} -O2 -std=c++20 \
        -Wl,-stack_size -Wl,0x10000000\
        -I{path}"
    )


if __name__ == "__main__":
    # 删除之前生成的文件
    os.system("rm data.zip || true")
    os.system("rm *.in || true")
    os.system("rm *.out || true")
    os.system("rm gen || true")

    # 生成输入输出文件 .in .out
    compile_cpp("gen.cpp", path="/Users/chenjinglong/algorithm_competition/nowcoder/template/generator")
    compile_cpp("std.cpp")
    if os.path.exists("checker.cc"):
        compile_cpp("checker.cc", path="/Users/chenjinglong/algorithm_competition/nowcoder/template/generator")
    
    os.system("./gen")
    
    # 打包成 data.zip
    os.system("zip -q data.zip *.in *.out checker.cc")
    # 删除中间文件
    os.system("rm gen || true")
    os.system("rm std || true")
    os.system("rm checker || true")
    os.system("rm *.in || true")
    os.system("rm *.out || true")
    os.system("rm starter.py || true")