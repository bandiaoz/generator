import os
import sys

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

cpp_files_prefix = []
def compile_all_cpp(exclude=[]):
    for file_name in os.listdir("."):
        if file_name in exclude:
            continue
        if file_name.endswith(".cpp"):
            cpp_files_prefix.append(file_name[:-4])
            compile_cpp(file_name)
        elif file_name.endswith(".cc"):
            cpp_files_prefix.append(file_name[:-3])
            compile_cpp(file_name)
    

def remove(file_names):
    for file_name in file_names:
        os.system(f"rm {file_name} || true")


def clear():
    # 删除所有临时文件
    remove(["*.in", "*.out"])
    remove(cpp_files_prefix)
    cpp_files_prefix.clear()


def generator():
    # 删除之前生成的文件
    clear()
    remove(["data.zip"])

    compile_all_cpp(["compare.cpp"])
    # 生成 *.in 和 *.out 文件
    os.system("./gen")

    # 打包成 data.zip
    os.system("zip -q data.zip *.in *.out checker.cc")
    clear()

def compare():
    # 编译所有 cpp 文件
    compile_all_cpp(["gen.cpp"])
    os.system("./compare")
    clear()


func = {
    "generator": generator,
    "compare": compare,   
}

if __name__ == "__main__":
    args = sys.argv
    if len(args) > 1:
        func[args[1]]()
        
    remove(["starter.py"])
