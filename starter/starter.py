import os
import sys

def compile_cpp(
    file_path,
    path="/Users/chenjinglong/algorithm_competition/nowcoder/generator",
):
    """
    编译 cpp 文件，获得同名可执行文件；例如 std.cpp -> std, checker.cc -> checker
    """
    prefix = ".".join(file_path.split(".")[:-1])
    os.system(
        f"g++-14 {file_path} \
        -o {prefix} -O2 -std=c++20 \
        -Wl,-stack_size -Wl,0x10000000\
        -I{path}"
    )

cpp_files_prefix = []
def compile_all_cpp(exclude=[]):
    for root, dirs, files in os.walk("."):
        for file in files:
            if file in exclude:
                continue
            file_path = os.path.join(root, file)
            if file.endswith(".cpp"):
                cpp_files_prefix.append(".".join(file_path.split(".")[:-1]))
                compile_cpp(file_path)
            elif file.endswith(".cc"):
                cpp_files_prefix.append(".".join(file_path.split(".")[:-1]))
                compile_cpp(file_path)
    

def remove(file_names):
    for file_name in file_names:
        os.system(f"rm {file_name} || true")


def clear():
    # 删除所有临时文件
    for file_name in os.listdir("."):
        if file_name.endswith(".in") and file_name[:-3] != "hack":
            os.system(f"rm {file_name}")
    remove(["*.out", "*.ans"])
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
