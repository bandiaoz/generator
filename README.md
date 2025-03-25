## starter

可以进一步使用一些脚本命令来简化操作，下面是一些例子，需要根据实际情况修改

`starter.py` 自动执行 `generator` 和 `compare` 命令

```python
import os
import sys

def compile_cpp(
    file_path
):
    """
    编译 cpp 文件，获得同名可执行文件；例如 std.cpp -> std, checker.cc -> checker
    """
    prefix = ".".join(file_path.split(".")[:-1])
    os.system(
        f"g++-14 {file_path} \
        -o {prefix} -O2 -std=c++20 \
        -Wl,-stack_size -Wl,0x10000000 \
        -I/Users/chenjinglong/algorithm_competition/nowcoder/generator \
        -I/Users/chenjinglong/algorithm_competition/template/CP-algo"
    )

cpp_files_prefix = []
def compile_all_cpp(exclude=[]):
    from concurrent.futures import ThreadPoolExecutor
    with ThreadPoolExecutor() as executor:
        for root, dirs, files in os.walk("."):
            for file in files:
                if file in exclude:
                    continue
                file_path = os.path.join(root, file)
                if file.endswith(".cpp") or file.endswith(".cc"):
                    cpp_files_prefix.append(".".join(file_path.split(".")[:-1]))
                    # 使用线程池编译
                    executor.submit(compile_cpp, file_path)
    

def remove(file_names):
    for file_name in file_names:
        os.system(f"rm {file_name} || true")


def clear():
    # 删除所有临时文件
    remove(["data/*.in", "data/*.out"])
    remove(["*.out", "*.ans"])
    remove(cpp_files_prefix)
    cpp_files_prefix.clear()


def generator():
    # 删除之前生成的文件
    clear()
    if not os.path.exists("./data"):
        os.makedirs("./data")
    remove(["./data/data.zip"])

    compile_all_cpp(["compare.cpp"])
    # 生成 *.in 和 *.out 文件
    os.system("./gen")

    # 打包成 data.zip
    os.system("zip -q ./data/data.zip ./data/*.in ./data/*.out checker.cc")
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

```

`tasks.json` 配置了 `Generate data`，`compare`，`copy gen.cpp`，`copy val.cpp`，`copy compare.cpp`，`copy checker.cc` 等命令

```json
{
  "version": "2.0.0",
  "tasks": [
    {
      "label": "copy starter.py",
      "type": "shell",
      "command": "cp /Users/chenjinglong/algorithm_competition/nowcoder/generator/starter/starter.py ${fileDirname}/starter.py"
    },
    {
      "label": "Generate data", // 自动生成数据
      "type": "shell",
      "command": "cd ${fileDirname} && python starter.py generator",
      "dependsOrder": "sequence",
      "dependsOn": [
        "copy starter.py"
      ],
      "problemMatcher": []
    },
    {
      "label": "copy gen.cpp", // 复制一份 gen.cpp 到当前文件夹
      "type": "shell",
      "command": "cp /Users/chenjinglong/algorithm_competition/nowcoder/generator/starter/gen.cpp ${fileDirname}/gen.cpp",
      "problemMatcher": []
    },
    {
      "label": "copy val.cpp", // 复制一份 val.cpp 到当前文件夹
      "type": "shell",
      "command": "cp /Users/chenjinglong/algorithm_competition/nowcoder/generator/starter/val.cpp ${fileDirname}/val.cpp",
      "problemMatcher": []
    },
    {
      "label": "copy compare.cpp", // 复制一份 compare.cpp 到当前文件夹
      "type": "shell",
      "command": "cp /Users/chenjinglong/algorithm_competition/nowcoder/generator/starter/compare.cpp ${fileDirname}/compare.cpp",
      "problemMatcher": []
    },
    {
      "label": "copy checker.cc", // 复制一份 checker.cc 到当前文件夹
      "type": "shell",
      "command": "cp /Users/chenjinglong/algorithm_competition/nowcoder/generator/starter/checker.cc ${fileDirname}/checker.cc",
      "problemMatcher": []
    },
    {
      "label": "compare", // 对拍
      "type": "shell",
      "command": "cd ${fileDirname} && python starter.py compare",
      "dependsOrder": "sequence",
      "dependsOn": [
        "copy starter.py"
      ],
      "problemMatcher": []
    }
  ]
}
```