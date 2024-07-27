#include "testlib.h"

#include <bits/stdc++.h>
#include <sys/stat.h>

namespace generator {

namespace __msg {
class Path;
class OutStream {
public:
    OutStream() { init(); }
    OutStream(Path& path);
    OutStream(const std::string& filename) { open(filename); }
    ~OutStream() { close(); }

    void init() {
        output_stream_ptr_ = &std::cerr;
        path_ = "std::cerr";
    }
    void open(const std::string &filename) {
        close();
        if (filename.empty()) {
            init();
        } else {
            file_.open(filename);
            if (!file_.is_open()) {
                std::cerr << "Can't open file " << filename << std::endl;
                init();
                std::cerr << "Using std::cerr" << std::endl;
            } else {
                output_stream_ptr_ = &file_;
                path_ = filename;
            }
        }
    }
    void close() {
        if (file_.is_open()) {
            file_.close();
            init();
        }
    }
    template <typename T>
    OutStream& operator<<(const T& data) {
        *output_stream_ptr_ << data;
        return *this;
    }
    OutStream& operator<<(std::ostream& (*manipulator)(std::ostream&)) {
        manipulator(*output_stream_ptr_);
        return *this;
    }
    void printf(const char* format, ...) {
        FMT_TO_RESULT(format, format, _format);
        *output_stream_ptr_ << _format;
    }
    void println(const char* format, ...) {
        FMT_TO_RESULT(format, format, _format);
        *output_stream_ptr_ << _format << std::endl;
    }
    std::string path() { return path_; }
    const char* cpath() { return path_.c_str(); }

private:
    std::ostream* output_stream_ptr_;
    std::ofstream file_;
    std::string path_;
};
OutStream _err("");

enum Color {
    Green,
    Red,
    Yellow,
    None
};

std::string color(const char *text, Color color) {
    if (color == None) {
        return text;
    } else if (color == Red) {
        return std::format("\033[1;31m{0}\033[0m", text);
    } else if (color == Green) {
        return std::format("\033[32m{0}\033[0m", text);
    } else if (color == Yellow) {
        return std::format("\033[1;33m{0}\033[0m", text);
    } 
    assert(false);
}
template <typename... Args>
void __fail_msg(OutStream& out, const char* msg, Args... args) {
    out.printf("%s ", color("FAIL", Red).c_str());
    out.println(msg, args..., nullptr);
    exit(EXIT_FAILURE);
}
template <typename... Args>
void __success_msg(OutStream& out, const char* msg, Args... args) {
    out.printf("%s ", color("SUCCESS", Green).c_str());
    out.println(msg, args..., nullptr);
    return;
}
template <typename... Args>
void __info_msg(OutStream& out, const char* msg, Args... args) {
    out.println(msg, args..., nullptr);
    return;
}
template <typename... Args>
void __warn_msg(OutStream& out, const char* msg, Args... args) {
    out.printf("%s ", color("WARN", Yellow).c_str());
    out.println(msg, args..., nullptr);
    return;
}
template <typename... Args>
void __error_msg(OutStream& out, const char* msg, Args... args) {
    out.printf("%s ", color("ERROR", Red).c_str());
    out.println(msg, args...,nullptr);
    exit(EXIT_FAILURE);
}

#ifdef WIN32
    char _path_split = '\\';
    char _other_split = '/';
#else
    char _path_split = '/';
    char _other_split = '\\';
#endif

template <typename U> struct IsPath;
class Path {
private:
    std::string _path;
public:
    Path() : _path("") {}
    Path(std::string s) : _path(s) {}
    Path(const char *s) : _path(std::string(s)) {}

    std::string path() const { return _path; }
    const char* cname() const { return _path.c_str(); }

    void change(std::string s) { _path = s; }
    void change(const char *s) { _path = std::string(s); }
    void change(Path other_path) { _path = other_path.path(); }

    bool __file_exists() {
        std::ifstream f(_path);
        return f.is_open();
    }
    bool __directory_exists() {
    #ifdef _WIN32
        struct _stat path_stat;
        if (_stat(_path.c_str(), &path_stat) != 0) {
            return false;
        }
        return (path_stat.st_mode & _S_IFDIR) != 0;
    #else
        struct stat path_stat;
        if (stat(_path.c_str(), &path_stat) != 0) {
            return false;
        }
        return S_ISDIR(path_stat.st_mode);
    #endif
    }
    /**
     * @brief 统一路径分隔符
     */
    void __unify_split() {
        for (auto &c : _path) {
            if (c == _other_split) {
                c = _path_split;
            }
        }
    }
    Path __folder_path() {
        if (this->__directory_exists()) {
            return Path(_path);
        } else {
            __unify_split();
            size_t pos = _path.find_last_of(_path_split);
            if (pos == std::string::npos) {
                return Path("");
            } else {
                return Path(_path.substr(0, pos));
            }
        }
    }
    /**
     * @brief 获得文件名，不包括路径和扩展名
     * @example 例如 /usr/local/test.cpp -> test
     */
    std::string __file_name() {
        if (!this->__file_exists()) {
            __msg::__fail_msg(__msg::_err, std::format("{0} is not a file path or the file doesn't exist.", _path).c_str());
            return "";
        }
        __unify_split();
        size_t pos = _path.find_last_of(_path_split);
        std::string file_full_name = (pos == std::string::npos) ? _path : _path.substr(pos + 1);
        size_t pos_s = file_full_name.find_first_of('.');
        std::string file_name = (pos_s == std::string::npos) ? file_full_name : file_full_name.substr(0, pos_s);
        return file_name;
    }
    /**
     * @brief 获得文件的完整的绝对路径
     */
    void full() {
    #ifdef _WIN32
        char buffer[MAX_PATH];
        if (GetFullPathNameA(_path.c_str(), MAX_PATH, buffer, nullptr) == 0) {
            __msg::__fail_msg(__msg::_err, std::format("can't find full path :{0}.", _path).c_str());
        }
    #else
        char buffer[1024];
        if (realpath(_path.c_str(), buffer) == nullptr) {
            __msg::__fail_msg(__msg::_err, std::format("can't find full path :{0}.", _path).c_str());
        }
    #endif
        _path = std::string(buffer);
    }
    void __delete_file() {
        if (this->__file_exists()) {
            std::remove(_path.c_str());
        }
    }
    template <typename T>
    Path __join_helper(const T &arg) const {
        std::string new_path = _path;
    #ifdef _WIN32
        if (!new_path.empty() && new_path.back() != _path_split)  {
            new_path += _path_split;
        }
    #else
        if (new_path.empty() || new_path.back() != _path_split)  {
            new_path += _path_split;
        }
    #endif
        return Path(new_path + arg);
    }
};

OutStream::OutStream(Path& path) {
    open(path.path());
}
}

namespace io {
char** __split_string_to_char_array(const char* input) {
    char** char_array = nullptr;
    char* cinput = const_cast<char*>(input);
    char* token = strtok(cinput, " ");
    int count = 0;

    while (token != nullptr) {
        char_array = (char**)realloc(char_array, (count + 1) * sizeof(char*));
        char_array[count] = strdup(token);
        ++count;
        token = strtok(nullptr, " ");
    }

    char_array = (char**)realloc(char_array, (count + 1) * sizeof(char*));
    char_array[count] = nullptr;
    return char_array;
}

void __fake_arg(std::string args = "", bool need_seed = false) {
    args = "generator " + args;
    auto _fake_argvs = __split_string_to_char_array(args.c_str());
    int _fake_argc = 0;
    while (_fake_argvs[_fake_argc] != nullptr) {
        ++_fake_argc;
    }
    if (need_seed) {
        rnd.setSeed(_fake_argc, _fake_argvs);
    }
    prepareOpts(_fake_argc, _fake_argvs);
}

/**
 * @brief 初始化随机数生成器，等价于 registerGen(argc, argv, 1)
 */
void init_gen(int argc, char* argv[]) {
    registerGen(argc, argv, 1);
}

/**
 * @brief 无参数的 registerGen
 */
void init_gen() {
    char * __fake_argvs[] = {(char*)"generator"};
    registerGen(1, __fake_argvs , 1);
}

/**
 * @brief 将标准输出重定向回控制台或终端
 */
void __close_output_file_to_console() {
#ifdef _WIN32
    freopen("CON", "w", stdout);
#else
    freopen("/dev/tty", "w", stdout);
#endif
}

/**
 * @brief 将标准输入重定向回控制台或终端
 */
void __close_input_file_to_console() {
#ifdef _WIN32
    freopen("CON", "r", stdin);
#else
    freopen("/dev/tty", "r", stdin);
#endif
}

/**
 * @brief 根据 func 生成输入文件，编号为 index，并检查输入文件非空
 */
void __write_input_file(int index, std::function<void()> func, std::string format, bool need_seed) {
    std::string filename = std::to_string(index) + ".in";
    freopen(filename.c_str(), "w", stdout);
    __fake_arg(format, need_seed);
    try {
        func();
    } catch (...) {
        __msg::__fail_msg(__msg::_err, "An exception occurred while writing the input file.");
    }
    __close_output_file_to_console();

    std::ifstream file(filename);
    if (file.peek() == std::ifstream::traits_type::eof()) {
        __msg::__fail_msg(__msg::_err, std::format("input file {0} is empty.", filename).c_str());
    }
}
void __make_inputs_impl(int start, int end, std::function<void()> func, std::string format, bool need_seed) {
    for (int i = start; i <= end; i++) {
        __write_input_file(i, func, format, need_seed);
    }
}
/**
 * @brief 根据 func 生成输入文件，编号从 start 到 end
 */
void make_inputs(int start, int end, std::function<void()> func, const char* format = "", ...) {
    FMT_TO_RESULT(format, format, _format);
    __make_inputs_impl(start, end, func, _format, false);
}
/**
 * @brief 根据 func 生成输入文件，编号为 index
 */
void make_input(int index, std::function<void()> func, const char* format = "", ...) {
    FMT_TO_RESULT(format, format, _format);
    __make_inputs_impl(index, index, func, _format, false);
}
/**
 * @brief 根据 func 生成输入文件，编号为 index，同时使用随机种子
 */
void make_input_seed(int index, std::function<void()> func, const char* format = "", ...) {
    FMT_TO_RESULT(format, format, _format);
    __make_inputs_impl(index, index, func, _format, true);
}
/**
 * @brief 获得所有输入文件的编号
 */
std::vector<int> __get_inputs() {
    std::vector<int> inputs;
    for (int i = 1; i <= 100; i++) {
        if (__msg::Path(std::to_string(i) + ".in").__file_exists()) {
            inputs.push_back(i);
        }
    }
    return inputs;
}
/**
 * @brief 根据 func 生成输出文件，编号为 index
 */
void __write_output_file(int index, std::function<void()> std_func) {
    if (!__msg::Path(std::to_string(index) + ".in").__file_exists()) {
        __msg::__fail_msg(__msg::_err, std::format("{0}.in doesn't exist.", index).c_str());
    }
    std::string filename_in = std::to_string(index) + ".in";
    std::string filename_out = std::to_string(index) + ".out";
    freopen(filename_in.c_str(), "r", stdin);
    freopen(filename_out.c_str(), "w", stdout);
    try {
        std_func();
    } catch (...) {
        __msg::__fail_msg(__msg::_err, "An exception occurred in std code.");
    }
    __close_input_file_to_console();
    __close_output_file_to_console();
}
/**
 * @brief 根据 std_func 生成 index.out 输出文件
 * @return 返回输出文件的文件流
 */
std::ifstream make_output(int index, std::function<void()> std_func) {
    __write_output_file(index, std_func);
    std::ifstream file(std::to_string(index) + ".out");
    if (!file.is_open()) {
        __msg::__fail_msg(__msg::_err, std::format("output file {0}.out is empty.", index).c_str());
    }
    return file;
}
/**
 * @brief 根据 std_func 生成 .out 输出文件，编号从 start 到 end
 */
void make_outputs(int start, int end, std::function<void()> std_func) {
    for (int i = start; i <= end; i++) {
        __write_output_file(i, std_func);
    }
}
/**
 * @brief 根据 std_func 生成 .in 对应的 .out 输出文件，
 * @param std_func 使用标准输入和标准输出，注意使用 std::endl 而不是 '\n'
 * @param cover_exist 是否覆盖已存在的输出文件
 */
void fill_outputs(std::function<void()> std_func, bool cover_exist = true) {
    std::vector<int> inputs = __get_inputs();
    for (int i : inputs) {
        if (!cover_exist && __msg::Path(std::to_string(i) + ".out").__file_exists()) {
            continue;
        }
        __write_output_file(i, std_func);
    }
}
/**
 * @brief 根据 ./std 可执行文件生成 .out 输出文件
 */
void __make_output_exe(int index, __msg::Path std_path) {
    std_path.full();
    if (!std_path.__file_exists()) {
        __msg::__fail_msg(__msg::_err, std::format("{0} doesn't exist.", std_path.cname()).c_str());
    }
    std::string filename_in = std::to_string(index) + ".in";
    if (!__msg::Path(filename_in).__file_exists()) {
        __msg::__fail_msg(__msg::_err, std::format("{0} doesn't exist.", filename_in).c_str());
    }
    std::string filename_out = std::to_string(index) + ".out";
    std::string command = std::format("{0} < {1} > {2}", std_path.path(), filename_in, filename_out);
    if (int return_code = system(command.c_str()); return_code != 0) {
        __msg::__fail_msg(__msg::_err, std::format("An exception occurred while creating the {}.", filename_out).c_str());
    }
}
/**
 * @brief 根据 std 可执行文件生成 index.out 输出文件
 * @param path 可执行文件的路径，可以使用字符串或者 Path 类型
 * @return 返回输出文件的文件流
 */
template <typename T>
std::ifstream make_output_exe(int index, T path) {
    __make_output_exe(index, __msg::Path(path));
    std::ifstream file(std::to_string(index) + ".out");
    if (!file.is_open()) {
        __msg::__fail_msg(__msg::_err, std::format("output file {0}.out is empty.", index).c_str());
    }
    return file;
}
/**
 * @brief 根据 std 可执行文件生成 .out 输出文件
 * @param path 可执行文件的路径，可以使用字符串或者 Path 类型
 */
template <typename T>
void make_outputs_exe(int start, int end, T path) {
    for (int index = start; index <= end; index++) {
        __make_output_exe(index, __msg::Path(path));
    }
}
/**
 * @brief 根据 std 可执行文件生成所有 .in 对应的 .out 输出文件
 * @param path 可执行文件的路径，可以使用字符串或者 Path 类型
 * @param cover_exist 是否覆盖已存在的输出文件
 */
template <typename T>
void fill_outputs_exe(T path, bool cover_exist = true) {
    __msg::Path std_path(path);
    std::vector<int> inputs = __get_inputs();
    for (int index : inputs) {
        if (!cover_exist && __msg::Path(std::to_string(index) + ".out").__file_exists()) {
            continue;
        }
        __make_output_exe(index, __msg::Path(path));
    }
}
/**
 * @brief 返回 index.out 文件的第一行，最大长度为 LENGTH
 */
std::string __first_line_output(int index, int LENGTH) {
    std::string first_line_output;
    std::ifstream file(std::to_string(index) + ".out");
    std::getline(file, first_line_output);
    if (first_line_output.empty()) {
        __msg::__fail_msg(__msg::_err, std::format("output file {0}.out is empty.", index).c_str());
    } else if (int(first_line_output.size()) > LENGTH) {
        first_line_output = first_line_output.substr(0, LENGTH) + "...";
    }
    return first_line_output;
}
/**
 * @brief 展示所有输出文件的第一行，最大长度为 LENGTH
 * @param LENGTH 最大展示长度，默认为 20
 */
void show_output_first_line(int LENGTH = 20) {
    for (int i = 1; i <= 100; i++) {
        if (__msg::Path(std::to_string(i) + ".out").__file_exists()) {
            __msg::__success_msg(__msg::_err, std::format("{0}.out: {1}", i, __first_line_output(i, LENGTH)).c_str());
        }
    }
}
}

namespace checker {
using __msg::Path;
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
std::string checker_name[MaxChecker] = {
    "checker",
    "lcmp",
    "ncmp",
    "yesno",
    "rcmp4", 
    "rcmp6",
    "rcmp9",
    "wcmp"
};
void __check_output(std::string prefix, Path check_path) {
    std::string filename_in = prefix + ".in";
    std::string filename_out = prefix + ".out";
    if (__msg::Path(filename_in).__file_exists() && __msg::Path(filename_out).__file_exists()) {
        check_path.full();
        std::string command = std::format("{0} {1} {2} {2}", check_path.path(), filename_in, filename_out);
        std::cerr << std::format("Case {0} : ", prefix);
        system(command.c_str());
    }
}
/**
 * @param 如果当前目录有 ./checker 可执行文件，判断输出是否正确
 */
void check_output() {
    if (!__msg::Path("checker").__file_exists()) {
        __msg::__warn_msg(__msg::_err, "checker doesn't exist.");
        return;
    }
    Path checker_path("checker");
    checker_path.full();
    for (int i = 1; i <= 100; i++) {
        __check_output(std::to_string(i), checker_path);
    }
}
}

namespace rand_numeric {

/**
 * @brief 生成 [0, n) 范围内的随机整数，等价于 rnd.next(n)
 * @tparam T 整数类型
 */
template <typename T = int>
typename std::enable_if<std::is_integral<T>::value, T>::type 
rand_int(T n) {
    return rnd.next(n);
}

/**
 * @brief 生成 [from, to] 范围内的随机整数，等价于 rnd.next(from, to)
 * @tparam T 要求是一个整数类型
 */
template <typename T = int>
typename std::enable_if<std::is_integral<T>::value, T>::type
rand_int(T from, T to) {
    return rnd.next(from, to);
}

/**
 * @brief 生成给定范围 [from, to] 内的随机整数，等价于 rnd.next(from, to)
 * @tparam T 和 U 都要求可以转换为 long long 类型
 */
template <typename T = long long, typename U = long long>
typename std::enable_if<
    std::is_convertible<T, long long>::value && 
    std::is_convertible<U, long long>::value, long long>::type
rand_int(T from, U to) {
    return rnd.next((long long)from, (long long)to);
}

/**
 * @brief 将 "[1,10]" 格式的字符串转换为 std::pair(1, 10)
 */
std::pair<long long, long long> __format_to_int_range(std::string s) {
    size_t open = s.find_first_of("[(");
    size_t close = s.find_first_of(")]");
    size_t comma = s.find(',');
    auto string_to_int = [&](size_t from, size_t to) -> long long {
        std::string str = s.substr(from + 1, to - from - 1);
        return std::stoll(str);
    };
    if (open == std::string::npos || close == std::string::npos || comma == std::string::npos) {
        __msg::__fail_msg(__msg::_err, std::format("{} is an invalid format. Example: [1, 10)", s).c_str());
    }
    assert(open != std::string::npos && close != std::string::npos && comma != std::string::npos);
    long long left = string_to_int(open, comma);
    long long right = string_to_int(comma, close);
    if (s[open] == '(') {
        left++;
    }
    if (s[close] == ')') {
        right--;
    }
    return std::pair(left, right);
}

/**
 * @brief 生成给定范围内的随机整数，格式为 "[from, to]"
 * @param format 格式字符串，例如 "[1, 10)"
 * @note 注意，如果 from 和 to 是小数类型，会先强制转换再判断括号的开闭；
 * @note 例如 rand_int("[1.0, 10.2)") 会生成 [1, 10) 范围内的随机整数，不会生成 10。
 */
long long rand_int(const char* format, ...) {
    FMT_TO_RESULT(format, format, _format);
    std::pair<long long, long long> range = __format_to_int_range(_format);
    long long x = rnd.next(range.first, range.second);
    return x;
}

/**
 * @brief 生成给定 [from, to] 范围内的随机奇数
 */
template <typename T = long long, typename U = long long>
typename std::enable_if<std::is_integral<T>::value && std::is_integral<U>::value, long long>::type
rand_odd(T from, U to) {
    long long froml = (long long)from;
    long long tol = (long long)to;
    long long l = (froml + (froml % 2 == 0) - 1) / 2;
    long long r = (tol - (tol % 2 == 0) - 1) / 2;
    if (l > r) {
        __msg::__fail_msg(__msg::_err, std::format("There is no odd number between [{0}, {1}].", froml, tol).c_str());
    }
    return 2 * rnd.next(l, r) + 1;
}

/**
 * @brief 生成给定范围内的随机奇数
 */
long long rand_odd(const char* format, ...) {
    FMT_TO_RESULT(format, format, _format);
    std::pair<long long, long long> range = __format_to_int_range(_format);
    long long x = rand_odd(range.first, range.second);
    return x;
}

/**
 * @brief 生成给定 [from, to] 范围内的随机偶数
 */
template <typename T = long long, typename U = long long>
typename std::enable_if<std::is_integral<T>::value && std::is_integral<U>::value, long long>::type
rand_even(T from, U to) {
    long long froml = (long long)from;
    long long tol = (long long)to;
    long long l = (froml + std::abs(froml % 2)) / 2;
    long long r = (tol - std::abs(tol % 2)) / 2;
    if (l > r) {
        __msg::__fail_msg(__msg::_err, std::format("There is no even number between [{0}, {1}].", froml, tol).c_str());
    }
    return 2 * rnd.next(l, r);
}

/**
 * @brief 生成给定范围内的随机偶数
 */
long long rand_even(const char* format, ...) {
    FMT_TO_RESULT(format, format, _format);
    std::pair<long long, long long> range = __format_to_int_range(_format);
    return rand_even(range.first, range.second);
}

/**
 * @brief 判断 T 是否是浮点类型（float、double、long double）或可以转换为 double 类型
 */
template <typename T>
constexpr bool is_double_valid() {
    return std::is_floating_point<T>::value || std::is_convertible<T, double>::value;
}

template <typename T>
double __change_to_double(T n){
    double _n;
    if (std::is_floating_point<T>::value){
        _n = n;
    } else if (std::is_convertible<T, double>::value){
        _n = static_cast<double>(n);
        __msg::__warn_msg(__msg::_err, "Input is not a real number, change it to %lf. Please ensure it's correct.", _n);
    } else {
        __msg::__fail_msg(__msg::_err, "Input is not a real number, and can't be changed to it.");
    }
    return _n;
}

/**
 * @brief 生成 [0, 1) 范围内的随机实数，等价于 rnd.next()
 */
double rand_real() {
    return rnd.next();
}
/**
 * @brief 生成 [0, n) 范围内的随机实数
 * @tparam T 浮点类型或者可以转换为 double 类型
 */
template <typename T = double>
double rand_real(T n) {
    double _n = __change_to_double(n);
    return rnd.next(_n);
}
/**
 * @brief 生成给定范围内 [from, to) 的随机实数
 * @tparam T 和 U 都要求是浮点类型或者可以转换为 double 类型
 */
template <typename T = double, typename U = double>
typename std::enable_if<is_double_valid<T>() && is_double_valid<U>(), double>::type
rand_real(T from, U to) {
    double _from = __change_to_double(from);
    double _to = __change_to_double(to);
    return rnd.next(_from, _to);
}

/**
 * @brief 将 "[1.0, 10.0]" 格式的字符串转换为 std::pair(1.0, 10.0)
 */
std::pair<double, double> __format_to_double_range(std::string s) {
    size_t open = s.find_first_of("[(");
    size_t close = s.find_first_of(")]");
    size_t comma = s.find(',');
    auto string_to_double = [&](size_t from, size_t to) -> double {
        std::string str = s.substr(from + 1, to - from - 1);
        return std::stod(str);
    };
    if (open == std::string::npos || close == std::string::npos || comma == std::string::npos) {
        __msg::__fail_msg(__msg::_err, std::format("{} is an invalid format. Example: [1.0, 10.0)", s).c_str());
    }
    double left = string_to_double(open, comma);
    double right = string_to_double(comma, close);
    double eps = 1e-8;
    if (s[open] == '(') {
        left += eps;
    }
    if (s[close] == ']') {
        right += eps;
    }
    return std::pair(left, right);
}

/**
 * @brief 生成给定范围内的随机实数，格式为 "[from, to]", "[from, to)"
 * @param format 格式字符串，可以使用科学计数法，例如 "[1e-2, 1E2)"
 */
double rand_real(const char* format, ...) {
    FMT_TO_RESULT(format, format, _format);
    std::pair<double,double> range = __format_to_double_range(_format);
    return rnd.next(range.first, range.second);
}

/**
 * @brief 根据概率选择一个元素
 * @param map 一个 map 或 unordered_map 容器，其 value 类型必须为整数，表示每个 key 有 value / sum(value) 的概率被选中
 * @return 选中的元素
 */
template <typename Con>
typename std::enable_if<
        (std::is_same<Con, std::map<typename Con::key_type, typename Con::mapped_type>>::value ||
        std::is_same<Con, std::unordered_map<typename Con::key_type, typename Con::mapped_type>>::value) &&
        std::is_integral<typename Con::mapped_type>::value,
        typename Con::key_type
>::type
rand_prob(const Con& map) {
    using KeyType = typename Con::key_type;
    using ValueType = typename Con::mapped_type;
    std::vector<KeyType> elements;
    std::vector<ValueType> probs;
    long long sum = 0;
    for (auto it : map){
        elements.emplace_back(it.first);
        ValueType value = it.second;
        assert(value >= 0);
        sum += value;
        probs.emplace_back(sum);
    }
    assert(sum > 0);
    long long p = rand_int(1LL, sum);
    auto pos = lower_bound(probs.begin(), probs.end(), p) - probs.begin();
    return *(elements.begin() + pos);
}
}

namespace rand_string {
enum CharType {
    LowerLetter,
    UpperLetter,
    Letter,
    Number,
    LetterNumber,
    ZeroOne,
    MAX
};
const std::string _PATTERN[MAX] = {
    "[a-z]",
    "[A-Z]",
    "[a-zA-Z]",
    "[0-9]",
    "[a-zA-Z0-9]",
    "[01]"
};
/**
 * @brief 生成一个随机字符，字符类型由 type 指定，默认为小写字母
 */
char rand_char(CharType type = LowerLetter) {
    std::string s = rnd.next(_PATTERN[type]);
    return s.c_str()[0];
}
/**
 * @brief 生成一个随机字符，字符类型由极简正则表达式 format 指定
 */
char rand_char(const char* format, ...) {
    FMT_TO_RESULT(format, format, _format);
    std::string s = rnd.next(_format);
    assert(!s.empty());
    return s.c_str()[0];
}
/**
 * @brief 生成一个随机字符，字符类型由极简正则表达式 format 指定
 */
char rand_char(std::string format) {
    return rand_char(format.c_str());
}
/**
 * @brief 生成长度为 n 的随机字符串，字符类型由 type 指定，默认为小写字母
 */
std::string rand_string(int n, CharType type = LowerLetter) {
    return rnd.next("%s{%d}", _PATTERN[type].c_str(), n);
}
/**
 * @brief 生成长度为 [from, to] 的随机字符串，字符类型由 type 指定，默认为小写字母
 */
std::string rand_string(int from, int to, CharType type = LowerLetter) {
    return rnd.next("%s{%d,%d}", _PATTERN[type].c_str(), from, to);
}
/**
 * @brief 生成长度为 n 的随机字符串，字符类型由极简正则表达式 format 指定
 */
std::string rand_string(int n, const char* format, ...) {
    FMT_TO_RESULT(format, format, _format);
    std::string s = rnd.next("%s{%d}", _format.c_str(), n);
    return s;
}
/**
 * @brief 生成长度为 [from, to] 的随机字符串，字符类型由极简正则表达式 format 指定
 */
std::string rand_string(int from, int to, const char* format, ...) {
    FMT_TO_RESULT(format, format, _format);
    std::string s = rnd.next("%s{%d,%d}", _format.c_str(), from, to);
    return s;
}
/**
 * @brief 生成长度为 n 的随机字符串，字符类型由极简正则表达式 format 指定
 */
std::string rand_string(int n, std::string format) {
    return rand_string(n, format.c_str());
}
/**
 * @brief 生成长度为 [from, to] 的随机字符串，字符类型由极简正则表达式 format 指定
 */
std::string rand_string(int from, int to, std::string format) {
    return rand_string(from, to, format.c_str());
}
/**
 * @brief 根据极简正则表达式 format 生成随机字符串
 */
std::string rand_string(const char* format, ...) {
    FMT_TO_RESULT(format, format, _format);
    return rnd.next(_format);
}
/**
 * @brief 根据极简正则表达式 format 生成随机字符串
 */
std::string rand_string(std::string format) {
    return rnd.next(format);
}
/**
 * @brief 生成长度为 n 的随机回文串，包含长度至少为 p 的回文子串
 */
std::string __rand_palindrome_impl(int n, int p, std::string char_type) {
    assert(n >= 0 && p >= 0 && p <= n);
    std::string palindrome_part(p, ' ');
    for (int i = 0; i < (p + 1) / 2; i++) {
        char c = rand_char(char_type);
        palindrome_part[i] = c;
        palindrome_part[p - i - 1] = c;
    }
    std::string result(n, ' ');
    int pos_l = rnd.next(0, n - p);
    int pos_r = pos_l + p - 1;
    for (int i = 0; i < n; i++) {
        if (i < pos_l || i > pos_r) {
            result[i] = rand_char(char_type);
        } else {
            result[i] = palindrome_part[i - pos_l];
        }
    }
    return result;
}
/**
 * @brief 生成长度为 n 的随机回文串，包含长度至少为 p 的回文子串，字符类型由 type 指定，默认为小写字母
 */
std::string rand_palindrome(int n, int p, CharType type = LowerLetter) {
    return __rand_palindrome_impl(n, p, _PATTERN[type]);
}
/**
 * @brief 生成长度为 n 的随机回文串，包含长度至少为 p 的回文子串，字符类型由极简正则表达式 format 指定
 */
std::string rand_palindrome(int n, int p, const char* format, ...) {
    FMT_TO_RESULT(format, format, _format);
    return __rand_palindrome_impl(n, p, _format);
}
/**
 * @brief 生成长度为 n 的随机回文串，包含长度至少为 p 的回文子串，字符类型由极简正则表达式 format 指定
 */
std::string rand_palindrome(int n, int p, std::string format) {
    return __rand_palindrome_impl(n, p, format);
}
}

namespace rand_vector {
/**
 * @brief 生成长度为 n 的随机排列，元素范围为 [0, n)，等价于 rnd.perm(n)
 */
template <typename T>
std::vector<T> rand_p(T n) {
    return rnd.perm(n, T(0));
}
/**
 * @brief 生成长度为 n 的随机排列，元素范围为 [s, s + n)，等价于 rnd.perm(n, s)
 */
template <typename T, typename E>
std::vector<E> rand_p(T n, E s = 0) {
    return rnd.perm(n, s);
}
/**
 * @brief 生成长度为 size 的随机整数数组，元素和为 sum，等价于 rnd.partition(size, sum)
 */
template<typename T>
std::vector<T> rand_sum(int size, T sum) {
    return rnd.partition(size, sum);
}
/**
 * @brief 生成长度为 size 的随机整数数组，元素和为 sum，且每个元素不小于 min_par；
 *        等价于 rnd.partition(size, sum, min_part)
 */
template<typename T>
std::vector<T> rand_sum(int size, T sum, T min_part) {
    return rnd.partition(size, sum, min_part);
}

template<typename T>
void __rand_small_sum(std::vector<T>&v, T sum, T limit) {
    int size = v.size();
    std::vector<int> rand_v(size);
    std::iota(rand_v.begin(), rand_v.end(), 0);
    shuffle(rand_v.begin(), rand_v.end());
    int last = size - 1;
    while (sum--) {
        int rand_pos = rnd.next(0, last);
        int add_pos = rand_v[rand_pos];
        v[add_pos]++;
        if(v[add_pos] >= limit) {
            std::swap(rand_v[last], rand_v[rand_pos]);
            last--;
        }
    }
    return;
}

template<typename T>
bool __rand_large_sum(std::vector<T> &v, T &sum, T limit) {
    int size = v.size();
    std::vector<int> rand_v(size);
    std::iota(rand_v.begin(), rand_v.end(), 0);
    shuffle(rand_v.begin(), rand_v.end());
    int last = size - 1;
    T once_add_base = sum / 1000000;
    while(sum > 1000000) {
        int rand_pos, add_pos, once_add;
        do {
            rand_pos = rnd.next(0, last);
            add_pos = rand_v[rand_pos];
            // 95% ~ 105% for once add
            T once_add_l = std::max(T(0), once_add_base - once_add_base / 20);
            T once_add_r = std::min(once_add_base + once_add_base / 20, sum);
            if (once_add_l > once_add_r) {
                return sum > 1000000;
            }
            once_add = rnd.next(once_add_l, once_add_r);
        } while(v[add_pos] + once_add > limit);         
        v[add_pos] += once_add;
        sum -= once_add;
        if (v[add_pos] >= limit) {
            std::swap(rand_v[last], rand_v[rand_pos]);
            last--;
        }
    }
    return false;
}
/**
 * @brief 生成长度为 size 的随机整数数组，元素范围为 [from, to]，元素和为 sum
 */
template<typename T>
std::vector<T> rand_sum(int size, T sum, T from, T to) {
    if (size < 0) {
        __msg::__fail_msg(__msg::_err, "Size of the vector can't less than zero.");
    } else if (size > 10000000) {
        __msg::__warn_msg(__msg::_err, std::format("Size of the vector is too large: {0}.", size).c_str());
    }
    if (from > to) {
        __msg::__fail_msg(__msg::_err, std::format("The range [{0}, {1}] is invalid.", from, to).c_str());
    }
    if (sum < from * size || sum > to * size) {
        __msg::__fail_msg(__msg::_err, std::format("Sum of the vector is in range [%s,%s], but need sum = %s.", from * size, to * size, sum).c_str());
    }
    if (size == 0) {
        if (sum != 0) {
            __msg::__fail_msg(__msg::_err, "Size of the vector is zero, but sum is not zero.");
        }
        return std::vector<T>();
    }

    T ask_sum = sum;
    std::vector<T> v(size, 0);
    sum -= from * size;
    T limit = to - from;
    if (sum <= 1000000) {
        __rand_small_sum(v, sum, limit);
    } else if (limit * size - sum <= 1000000) {
        __rand_small_sum(v, limit * size - sum, limit);
        for (int i = 0; i < size; i++) {
            v[i] = limit - v[i];
        }
    } else {
        while(__rand_large_sum(v, sum, limit));
        __rand_small_sum(v, sum, limit);
    }
    for (int i = 0; i < size; i++) {
        v[i] += from;
    }

    T result_sum = 0;
    for (int i = 0; i < size; i++) {
        if (v[i] < from || v[i] > to) {
            __msg::__error_msg(__msg::_err, std::format("The {0}{1} element is out of range [{2}, {3}].", v[i], englishEnding(i + 1), from, to).c_str());
        }
        result_sum += v[i];
    }
    if (result_sum != ask_sum) {
        __msg::__error_msg(__msg::_err, std::format("The sum of the vector is {0}, but need sum = {1}.", result_sum, ask_sum).c_str());
    }
    return v;
}
/**
 * @brief 生成长度为 size 的随机整数数组，每个元素由 func 生成
 */
template <typename T>
std::vector<T> rand_vector(int size, std::function<T()> func) {
    std::vector<T> v(size);
    for(int i = 0; i < size; i++) {
        v[i] = func();
    }
    return v;
}
/**
 * @brief 生成长度为 [from, to] 的随机整数数组，每个元素由 func 生成
 */
template <typename T>
std::vector<T> rand_vector(int from, int to, std::function<T()> func) {
    return rand_vector(rnd.next(from, to), func);
}

// use vector index + `offset` be new vector elements
// the value of the vector represents the number of times this index will appear in the new vector
// return the shuffled new vector
template<typename Iter>
std::vector<int> shuffle_index(Iter begin, Iter end, int offset = 0) {
    int tot = 0;
    std::vector<int> res;
    for (Iter i = begin; i != end; i++) {
        int x = *i;
        if (x < 0) {
            __msg::__fail_msg(__msg::_err, "Elements must be non negative number.");
        }
        tot += x;
        if (tot > 10000000) {
            __msg::__fail_msg(__msg::_err, "Sum of the elements must equal or less than 10^7");
        }
        while (x--) {
            res.emplace_back((i - begin) + offset);
        }   
    }
    shuffle(res.begin(), res.end());
    return res;
}

/**
 * @brief 生成长度为 sum(v[i]) 的随机整数数组，每个元素 i + offset 的个数为 v[i]
 * @example shuffle_index({1, 2, 3}, 1) -> {1, 2, 2, 3, 3, 3}
 */
std::vector<int> shuffle_index(std::vector<int> v, int offset = 0) {
    return shuffle_index(v.begin(), v.end(), offset);
}

}

namespace rand_graph {

namespace basic {
class _BasicEdge {
protected:
    int _u, _v;

public:
    _BasicEdge(int u, int v) : _u(u), _v(v) {}

    int u() const { return _u; }
    int v() const { return _v; }
    int& u_ref() { return _u; }
    int& v_ref() { return _v;}
    void set_u(int u) { _u = u; }
    void set_v(int v) { _v = v; } 

    friend bool operator<(const _BasicEdge a, const _BasicEdge b) {
        return a._u < b._u || (a._u == b._u && a._v < b._v);
    }
    friend bool operator<=(const _BasicEdge a, const _BasicEdge b) {
        return a._u <= b._u || (a._u == b._u && a._v <= b._v);
    }
    friend bool operator>(const _BasicEdge a, const _BasicEdge b) {
        return a._u > b._u || (a._u == b._u && a._v > b._v);
    }
    friend bool operator>=(const _BasicEdge a, const _BasicEdge b) {
        return a._u >= b._u || (a._u == b._u && a._v >= b._v);
    }
    friend bool operator==(const _BasicEdge a, const _BasicEdge b) {
        return a._u == b._u && a._v == b._v;
    }
    friend bool operator!=(const _BasicEdge a, const _BasicEdge b) {
        return !(a == b);
    }
};

/**
 * 定义边的输出函数
 */
#define _OUTPUT_FUNCTION(_TYPE) \
    typedef std::function<void(std::ostream&, const _TYPE&)> OutputFunction; \
    OutputFunction _output_function;

/**
 * 定义边的默认输出函数
 */
#define _COMMON_OUTPUT_FUNCTION_SETTING(_TYPE) \
    void set_output(OutputFunction func) { \
        _output_function = func; \
    } \
    friend std::ostream& operator<<(std::ostream& os, const _TYPE& type) { \
        type._output_function(os, type); \
        return os; \
    } \
    void println() { \
        std::cout<<*this<<std::endl; \
    } 

#define _OTHER_OUTPUT_FUNCTION_SETTING(_TYPE) \
    _COMMON_OUTPUT_FUNCTION_SETTING(_TYPE) \
    void set_output_default() { \
        _output_function = default_function(); \
    } \
    OutputFunction default_function() { \
        OutputFunction func =  \
            [](std::ostream& os, const _TYPE& type) { \
                type.default_output(os); \
            }; \
        return func; \
    } 

#define _EDGE_OUTPUT_FUNCTION_SETTING(_TYPE) \
    _COMMON_OUTPUT_FUNCTION_SETTING(_TYPE) \
    void set_output_default(bool swap_node = false) { \
        _output_function = default_function(swap_node); \
    } \
    OutputFunction default_function(bool swap_node = false) { \
        OutputFunction func =  \
            [swap_node](std::ostream& os, const _TYPE& type) { \
                type.default_output(os, swap_node); \
            }; \
        return func; \
    } \

template<typename T>
class _Edge : public _BasicEdge {
protected:
    T _w;
    _OUTPUT_FUNCTION(_Edge<T>)
public:
    _Edge(int u, int v, T w) : _BasicEdge(u, v), _w(w) {
        _output_function = default_function();
    }

    T w() const { return _w; }
    T& w_ref() { return _w; }
    void set_w(T w) { _w = w; }
    std::tuple<int, int, T> edge() const { return std::make_tuple(_u, _v, _w); }
    void default_output(std::ostream& os, bool swap_node = false) const {
        if (swap_node) {
            os << _v << " " << _u << " " << _w;
        } else {
            os << _u << " " << _v << " " << _w;
        }    
    }

    _EDGE_OUTPUT_FUNCTION_SETTING(_Edge<T>)
};


template<>
class _Edge<void> : public _BasicEdge {
protected:
    _OUTPUT_FUNCTION(_Edge<void>)
public:
    _Edge(int u, int v) : _BasicEdge(u, v) {
        _output_function = default_function();
    }

    std::tuple<int, int> edge() const { return std::make_tuple(_u, _v); }

    void default_output(std::ostream& os, bool swap_node = false) const {
        if (swap_node) {
            os << _v << " " << _u ;
        } else {
            os << _u << " " << _v ;
        }     
    }

    _EDGE_OUTPUT_FUNCTION_SETTING(_Edge<void>)
};

template<typename U>
class _Node {
protected:
    U _w;
    _OUTPUT_FUNCTION(_Node<U>)
public:
    _Node() : _w(U()) { _output_function = default_function(); }
    _Node(U w) : _w(w) { _output_function = default_function(); }
    
    U& w_ref() { return _w; }
    U w() const { return _w; }
    void set_w(U w) { _w = w; }
    
    void default_output(std::ostream& os) const { os << _w ; }

    _OTHER_OUTPUT_FUNCTION_SETTING(_Node<U>)
};

template<>
class _Node<void> {
public:
    _Node(){}
    friend std::ostream& operator<<(std::ostream& os, const _Node<void>&) {
        return os;
    }
};

class _BasicTree {
protected:
    int _node_count; // the number of nodes in the tree  
    int _begin_node; // index of the first node               
    bool _is_rooted;
    // use if `_is_rooted` is true,
    int _root;
    std::vector<int> _p;
    
    // output format
    bool _output_node_count;
    bool _output_root;
    bool _swap_node;// true means output `father son` or `son father` by random

    std::vector<int> _node_indices;
    
public:
    _BasicTree(
        int node_count, int begin_node, bool is_rooted, int root,
        bool output_node_count, bool output_root) :
        _node_count(node_count),
        _begin_node(begin_node),
        _is_rooted(is_rooted),
        _root(root - 1),
        _output_node_count(output_node_count),
        _output_root(output_root),
        _swap_node(_is_rooted ? false : true) {
        __init_node_indices();
    }
    
    void set_node_count(int node_count) {
        if (node_count != _node_count) {
            _node_count = node_count; 
            __init_node_indices();               
        }             
    }

    void set_is_rooted(bool is_rooted) { 
        if (_is_rooted != is_rooted) {
            _swap_node = is_rooted ? false : true;
            __msg::__warn_msg(__msg::_err, std::format("Setting `swap_node` to {}, because `is_rooted` changed!", (_swap_node ? "true" : "false")).c_str());
        }
        _is_rooted = is_rooted; 
    }

    void set_root(int root) {
        _root = root - 1;
        if (!_is_rooted) {
            __msg::__warn_msg(__msg::_err, "Unrooted Tree, set root is useless."); 
        }
    }

    void set_begin_node(int begin_node) { 
        if (begin_node != _begin_node) {
            _begin_node = begin_node; 
            __init_node_indices();                 
        }                 
    }

    void set_output_node_count(bool output_node_count) { _output_node_count = output_node_count; }

    void set_output_root(bool output_root) { _output_root = output_root; }

    void set_swap_node(bool swap_node) { _swap_node = swap_node; }
    
    bool is_rooted() const { return _is_rooted; }
    bool& is_rooted_ref() { return _is_rooted; }
    
    bool swap_node() const { return _swap_node; }
    bool& swap_node_ref() { return _swap_node; }
    
    int node_count() const { return _node_count; }         
    int& node_count_ref() { return _node_count; }
    
    int root() const {
        if (!_is_rooted) {
            __msg::__warn_msg(__msg::_err, "Unrooted Tree, root is useless.");
        }
        return _node_indices[_root];
    }
    
    int& root_ref() {
        if (!_is_rooted) {
            __msg::__warn_msg(__msg::_err, "Unrooted Tree, root is useless.");
        }
        return _root;
    }

    int begin_node() const { return _begin_node; }
    int& begin_node_ref() { return _begin_node; }

    std::vector<int> node_indices() const { return _node_indices; }
    std::vector<int>& node_indices_ref() { return _node_indices; }
    
    void set_node_indices(std::vector<int> node_indices) {
        if ((int)node_indices.size() != _node_count) {
            __msg::__warn_msg(
                __msg::_err, 
                "Node indices size must equal to node count %d, but found %d.", 
                _node_count, 
                node_indices.size());
            return;
        }
        _node_indices = node_indices;
    }
    
    void set_node_indices(int index, int number) {
        if (index < 1 || index > _node_count) {
            __msg::__warn_msg(
                __msg::_err,
                "Node index must in range [1, %d], but found %d.",
                _node_count,
                index);
            return;
        }
        _node_indices[index - 1] = number;
    }
    
protected:
    
    void __init_node_indices() {
        _node_indices.resize(_node_count);
        std::iota(_node_indices.begin(), _node_indices.end(), _begin_node);
    }
};

template<typename T, typename U>
using _IsBothWeight = typename std::enable_if<!std::is_void<T>::value &&
                                            !std::is_void<U>::value, int>::type;

template<typename T, typename U>
using _IsEdgeWeight = typename std::enable_if<std::is_void<T>::value &&
                                            !std::is_void<U>::value, int>::type;

template<typename T, typename U>
using _IsNodeWeight = typename std::enable_if<!std::is_void<T>::value &&
                                            std::is_void<U>::value, int>::type;

template<typename T, typename U>
using _IsUnweight = typename std::enable_if<std::is_void<T>::value &&
                                            std::is_void<U>::value, int>::type;

template<typename T>
using _HasT = typename std::enable_if<!std::is_void<T>::value, int>::type;

template<typename T>
using _NotHasT = typename std::enable_if<std::is_void<T>::value, int>::type;
    
#define _DEF_GEN_FUNCTION \
    typedef std::function<NodeType()> NodeGenFunction; \
    typedef std::function<EdgeType()> EdgeGenFunction;

template<typename NodeType, typename EdgeType>
class _RandomFunction {
protected:
    _DEF_GEN_FUNCTION
    NodeGenFunction _nodes_weight_function;
    EdgeGenFunction _edges_weight_function;
public:
    _RandomFunction(
        NodeGenFunction nodes_weight_function,
        EdgeGenFunction edges_weight_function) :
        _nodes_weight_function(nodes_weight_function),
        _edges_weight_function(edges_weight_function) {}
    
    template<typename T = NodeType, _HasT<T> = 0>
    void set_nodes_weight_function(NodeGenFunction nodes_weight_function) {
        _nodes_weight_function = nodes_weight_function;
    }
    
    template<typename T = EdgeType, _HasT<T> = 0>
    void set_edges_weight_function(EdgeGenFunction edges_weight_function) {
        _edges_weight_function = edges_weight_function;
    }
    
    NodeGenFunction nodes_weight_function() {
        return _nodes_weight_function;
    }
    
    EdgeGenFunction edges_weight_function() {
        return _edges_weight_function;
    }

protected:                                     
    template<typename T = NodeType, _HasT<T> = 0>
    void __check_nodes_weight_function() {
        if (_nodes_weight_function == nullptr) {
            __msg::__fail_msg(__msg::_err, "Nodes weight generator function is nullptr, please set it.");
        }
    }
    
    template<typename T = NodeType, _NotHasT<T> = 0>
    void __check_nodes_weight_function() {}
    
    template<typename T = EdgeType, _HasT<T> = 0>
    void __check_edges_weight_function() {
        if (_edges_weight_function == nullptr) {
            __msg::__fail_msg(__msg::_err, "Edges weight generator function is nullptr, please set it.");
        }
    }
    
    template<typename T = EdgeType, _NotHasT<T> = 0>
    void __check_edges_weight_function() {}
};

template<typename NodeType, typename EdgeType> 
class _LinkImpl;

template<typename NodeType, typename EdgeType>
class _TreeLinkImpl;

enum TreeGenerator {
    RandomFather,
    Pruefer
};

template<typename NodeType, typename EdgeType>
class _Tree : public _BasicTree, public _RandomFunction<NodeType, EdgeType> {
protected:
    typedef _Tree<NodeType,EdgeType> _Self;
    _OUTPUT_FUNCTION(_Self)
    _DEF_GEN_FUNCTION
    std::vector<_Edge<EdgeType>> _edges;
    std::vector<_Node<NodeType>> _nodes_weight; 
    TreeGenerator _tree_generator;
    
public:
    friend class _LinkImpl<NodeType, EdgeType>;
    friend class _TreeLinkImpl<NodeType, EdgeType>;
    
public:
    template<typename T = NodeType, typename U = EdgeType, _IsBothWeight<T, U> = 0>
    _Tree(
        int node_count = 1, int begin_node = 1, bool is_rooted = false, int root = 1,
        NodeGenFunction nodes_weight_function = nullptr,
        EdgeGenFunction edges_weight_function = nullptr,
        TreeGenerator tree_generator = RandomFather) :
        _BasicTree(node_count, begin_node, is_rooted, root, true, true),
        _RandomFunction<NodeType, EdgeType>(nodes_weight_function, edges_weight_function),
        _tree_generator(tree_generator) {
        _output_function = default_function();
    }
    
    template<typename T = NodeType, typename U = EdgeType, _IsEdgeWeight<T, U> = 0>
    _Tree(
        int node_count = 1, int begin_node = 1, bool is_rooted = false, int root = 1,
        EdgeGenFunction edges_weight_function = nullptr,
        TreeGenerator tree_generator = RandomFather) :
        _BasicTree(node_count, begin_node, is_rooted, root, true, true),
        _RandomFunction<NodeType, EdgeType>(nullptr, edges_weight_function),
        _tree_generator(tree_generator) {
        _output_function = default_function();
    }
    
    template<typename T = NodeType, typename U = EdgeType, _IsNodeWeight<T, U> = 0>
    _Tree(
        int node_count = 1, int begin_node = 1, bool is_rooted = false, int root = 1,
        NodeGenFunction nodes_weight_function = nullptr,
        TreeGenerator tree_generator = RandomFather) :
        _BasicTree(node_count, begin_node, is_rooted, root, true, true),
        _RandomFunction<NodeType, EdgeType>(nodes_weight_function, nullptr),
        _tree_generator(tree_generator) {
        _output_function = default_function();
    }
    
    template<typename T = NodeType, typename U = EdgeType, _IsUnweight<T, U> = 0>
    _Tree(
        int node_count = 1, int begin_node = 1, bool is_rooted = false, int root = 1,
        TreeGenerator tree_generator = RandomFather) :
        _BasicTree(node_count, begin_node, is_rooted, root, true, true),
        _RandomFunction<NodeType, EdgeType>(nullptr, nullptr),
        _tree_generator(tree_generator) {
        _output_function = default_function();
    }
    
    void set_tree_generator(TreeGenerator tree_generator) { _tree_generator = tree_generator; }
    void use_random_father() { _tree_generator = RandomFather; }
    void use_pruefer() { _tree_generator = Pruefer; }
    
    std::vector<_Edge<EdgeType>> edges() const { return __get_output_edges(); }
    std::vector<_Edge<EdgeType>> edges_ref() { return _edges; }

    template<typename T = NodeType, _HasT<T> = 0>
    std::vector<_Node<NodeType>> nodes_weight() const { return _nodes_weight; }
    template<typename T = NodeType, _HasT<T> = 0>
    std::vector<_Node<NodeType>>& nodes_weight_ref() { return _nodes_weight; }

    void gen() {           
        __init();
        __generate_tree();
        __generate_nodes_weight();
        shuffle(_edges.begin(),_edges.end());
    }
    
    void reroot(int root) {
        __reroot_set_check(root);
        __reroot();
    }
    
    void default_output(std::ostream& os) const {
        std::string first_line = "";
        if (_output_node_count) {
            first_line += std::to_string(_node_count);
        }
        if (_is_rooted && _output_root) {
            if (first_line != "") {
                first_line += " ";
            }
            first_line += std::to_string(root());
        }
        if (__output_node_weights(os, first_line)) {
            if (_node_count > 1) {
                os << "\n";
            }
        } else {
            if (first_line != "") {
                os << first_line;
                if (_node_count > 1) {
                    os << "\n";
                }
            }
        }
        int edge_cnt = 0;
        std::vector<_Edge<EdgeType>> output_edges = __get_output_edges();
        for (_Edge<EdgeType> e: output_edges) {
            if (_swap_node && rnd.next(2)) {
                e.set_output_default(true);
            }
            os << e;
            e.set_output_default();
            if (++edge_cnt < _node_count - 1) {
                os << "\n";
            }
        }
    }
    _OTHER_OUTPUT_FUNCTION_SETTING(_Self)

protected:
    void __reroot_set_check(int root) {
        if (!_is_rooted) {
            __msg::__warn_msg(__msg::_err, "unrooted tree can't re-root.");
            return;
        }
        if (root < 1 || root > _node_count) {
            __msg::__warn_msg(__msg::_err, "restriction of the root is [1, %d], but found %d.", _node_count, root);
            return;
        }
        if ((int)_edges.size() < _node_count - 1) {
            __msg::__warn_msg(__msg::_err, "Tree should generate first.");
            return;
        }
        _root = root - 1;
    }

    void __reroot() {
        std::vector<_Edge<EdgeType>> result;
        std::vector<std::vector<_Edge<EdgeType>>> node_edges(_node_count);
        for (auto edge : _edges) {
            node_edges[edge.u()].emplace_back(edge);
            node_edges[edge.v()].emplace_back(edge);
        }
        std::vector<int> visit(_node_count, 0);
        std::queue<int> q;
        q.push(_root);
        while(!q.empty()) {
            int u = q.front();
            q.pop();
            visit[u] = 1;
            for (auto& edge : node_edges[u]) {
                if (edge.u() != u) {
                    std::swap(edge.u_ref(), edge.v_ref());
                }
                int v = edge.v();
                if (visit[v]) {
                    continue;
                }
                result.emplace_back(edge);
                q.push(v);
            }
        }
        shuffle(result.begin(), result.end());
        _edges = result;       
    }

    template<typename T = NodeType, _HasT<T> = 0>
    bool __output_node_weights(std::ostream& os, std::string& first_line) const {
        int node_cnt = 0;
        for (_Node<NodeType> node : _nodes_weight) {
            node_cnt++;
            if(node_cnt == 1) {
                if (first_line != "") {
                    os << first_line << "\n";
                }
            }
            os << node << " ";
        }
        return node_cnt >= 1;
    }

    template<typename T = NodeType, _NotHasT<T> = 0>
    bool __output_node_weights(std::ostream&, std::string&) const {
        return false;
    }

    template<typename T = EdgeType, _NotHasT<T> = 0>
    std::vector<_Edge<EdgeType>> __get_output_edges() const {
        std::vector<_Edge<EdgeType>> output_edges;
        for (const auto& edge : _edges) {
            output_edges.emplace_back(_node_indices[edge.u()], _node_indices[edge.v()]);
        }
        return output_edges;
    }

    template<typename T = EdgeType, _HasT<T> = 0>
    std::vector<_Edge<EdgeType>> __get_output_edges() const {
        std::vector<_Edge<EdgeType>> output_edges;
        for (const auto& edge : _edges) {
            output_edges.emplace_back(_node_indices[edge.u()], _node_indices[edge.v()], edge.w());
        }
        return output_edges;
    }

    void __judge_comman_limit() {
        if (_node_count <= 0) {
            __msg::__fail_msg(__msg::_err, "Number of nodes must be a positive integer, but found %d.", _node_count);
        }

        if (_is_rooted && (_root < 0 || _root >= _node_count)) {
            __msg::__fail_msg(
                __msg::_err,
                "restriction of the root is [1, %d], but found %d.", 
                _node_count, 
                _root + 1);
        }
    }

    virtual void __judge_self_limit() {}

    void __judge_limits() {
        __judge_comman_limit();
        __judge_self_limit();
    }
    
    virtual void __self_init(){};
    
    void __init() {
        __self_init();
        this->__check_nodes_weight_function();
        this->__check_edges_weight_function();
        __judge_limits();
        _edges.clear();
        _nodes_weight.clear();
        _p = rnd.perm(_node_count, 0);
        if (_is_rooted) {
            for (int i = 1; i < _node_count; i++) {
                if (_p[i] == _root) {
                    std::swap(_p[0], _p[i]);
                    break;
                }
            }
        }    
        if ((int)_node_indices.size() != _node_count) {
            this->__init_node_indices();
        }
    }
    
    void __add_edge(_Edge<EdgeType> edge) {
        _edges.emplace_back(edge);
    }
    
    template<typename T = EdgeType, _NotHasT<T> = 0>
    void __add_edge(int u, int v) {
        __add_edge(_Edge<void>(u, v));

    }
    
    template<typename T = EdgeType, _HasT<T> = 0>
    void __add_edge(int u, int v) {
        EdgeType w = this->_edges_weight_function();
        __add_edge(_Edge<EdgeType>(u, v, w));
    }
    
    template<typename T = NodeType, _NotHasT<T> = 0>
    void __generate_nodes_weight() { return; }
    
    template<typename T = NodeType, _HasT<T> = 0>
    void __generate_nodes_weight() {
        for (int i = 0; i < _node_count ; i++) {
            NodeType w = this->_nodes_weight_function();
            _nodes_weight.emplace_back(w);
        }
    }
    
    void __random_father() {
        for (int i = 1; i < _node_count; i++) {
            int f = rnd.next(i);
            __add_edge(_p[f], _p[i]);
        }
    }

    void __pruefer_decode(std::vector<int> pruefer) {
        if (_node_count == 2) {
            int u = _is_rooted ? _root : 0;
            __add_edge(u, 1 ^ u);
            return;
        }

        if (_is_rooted) {
            int n = pruefer.size();
            bool exist = false;
            for (int i = 0; i < n; i++) {
                if (pruefer[i] == _root) {
                    std::swap(pruefer[i], pruefer[n - 1]);
                    exist = true;
                    break;
                }
            }
            if (!exist) {
                pruefer[n - 1] = _root;
            }
        }

        std::vector<int> degree(_node_count, 1);
        for (auto x: pruefer) {
            degree[x]++;
        }
        int ptr = 0;
        while (degree[ptr] != 1) {
            ptr++;
        }
        int leaf = ptr;
        for (auto u: pruefer) {
            __add_edge(u, leaf);
            degree[u]--;
            if (degree[u] == 1 && u < ptr) {
                leaf = u;
            } else {
                do {
                    ptr++;
                } while (degree[ptr] != 1);
                leaf = ptr;
            }
        }
        int u = leaf;
        int v = _node_count - 1;
        if (_is_rooted && v == _root) {
            std::swap(u, v);
        }
        __add_edge(u, v);
    }
    
    virtual void __generate_tree() {
        if (_tree_generator == Pruefer) {
            std::vector<int> times = rand_vector::rand_sum(_node_count, _node_count - 2, 0);
            std::vector<int> pruefer = rand_vector::shuffle_index(times);
            __pruefer_decode(pruefer);
        } else {
            __random_father();
        }
    }
};

#define _DISABLE_CHOOSE_GEN \
    void set_tree_generator(TreeGenerator tree_generator) = delete; \
    void use_random_father() = delete; \
    void use_pruefer() = delete; 

#define _MUST_IS_ROOTED \
    void set_is_rooted(int is_rooted) = delete; \
    bool& is_rooted_ref() = delete;

template<typename NodeType, typename EdgeType>
class _Chain : public _Tree<NodeType, EdgeType> {
protected:
    typedef _Chain<NodeType,EdgeType> _Self;
    _OUTPUT_FUNCTION(_Self)
    _DEF_GEN_FUNCTION
public:
    template<typename T = NodeType, typename U = EdgeType, _IsBothWeight<T, U> = 0>
    _Chain(
        int node = 1, int begin_node = 1, bool is_rooted = false, int root = 1,
        NodeGenFunction nodes_weight_function = nullptr,
        EdgeGenFunction edges_weight_function = nullptr) :
        _Tree<NodeType, EdgeType>(
            node, begin_node, is_rooted, root, 
            nodes_weight_function, edges_weight_function) {
        _output_function = this->default_function();
    }
    
    template<typename T = NodeType, typename U = EdgeType, _IsEdgeWeight<T, U> = 0>
    _Chain(
        int node = 1, int begin_node = 1, bool is_rooted = false, int root = 1,
        EdgeGenFunction edges_weight_function = nullptr) :
        _Tree<void, EdgeType>(node, begin_node, is_rooted, root, edges_weight_function) {
        _output_function = this->default_function();
    }
    
    template<typename T = NodeType, typename U = EdgeType, _IsNodeWeight<T, U> = 0>
    _Chain(
        int node = 1, int begin_node = 1, bool is_rooted = false, int root = 1,
        NodeGenFunction nodes_weight_function = nullptr) :
        _Tree<NodeType, void>(node, begin_node, is_rooted, root, nodes_weight_function) {
        _output_function = this->default_function();
    }
    
    template<typename T = NodeType, typename U = EdgeType, _IsUnweight<T, U> = 0>
    _Chain(
        int node = 1, int begin_node = 1, bool is_rooted = false, int root = 1) :
        _Tree<void, void>(node, begin_node, is_rooted, root) {
        _output_function = this->default_function();
    }

    _OTHER_OUTPUT_FUNCTION_SETTING(_Self)
    _DISABLE_CHOOSE_GEN
    
protected:                                       
    virtual void __generate_tree() override {
        for (int i = 1; i < this->_node_count; i++) {
            this->__add_edge(this->_p[i - 1], this->_p[i]);
        }
    }
};

template<typename NodeType, typename EdgeType>
class _Flower : public _Tree<NodeType, EdgeType> {
protected:
    typedef _Flower<NodeType,EdgeType> _Self;
    _OUTPUT_FUNCTION(_Self)
    _DEF_GEN_FUNCTION
public:
    template<typename T = NodeType, typename U = EdgeType, _IsBothWeight<T, U> = 0>
    _Flower(
        int node_count = 1, int begin_node = 1, bool is_rooted = false, int root = 1,
        NodeGenFunction nodes_weight_function = nullptr,
        EdgeGenFunction edges_weight_function = nullptr) :
        _Tree<NodeType, EdgeType>(
            node_count, begin_node, is_rooted, root, 
            nodes_weight_function, edges_weight_function) {
        _output_function = this->default_function();
    }
    
    template<typename T = NodeType, typename U = EdgeType, _IsEdgeWeight<T, U> = 0>
    _Flower(
        int node_count = 1, int begin_node = 1, bool is_rooted = false, int root = 1,
        EdgeGenFunction edges_weight_function = nullptr) :
        _Tree<void, EdgeType>(node_count, begin_node, is_rooted, root, edges_weight_function) {
        _output_function = this->default_function();
    }
    
    template<typename T = NodeType, typename U = EdgeType, _IsNodeWeight<T, U> = 0>
    _Flower(
        int node_count = 1, int begin_node = 1, bool is_rooted = false, int root = 1,
        NodeGenFunction nodes_weight_function = nullptr) :
        _Tree<NodeType, void>(node_count, begin_node, is_rooted, root, nodes_weight_function) {
        _output_function = this->default_function();
    }
    
    template<typename T = NodeType, typename U = EdgeType, _IsUnweight<T, U> = 0>
    _Flower(
        int node_count = 1, int begin_node = 1, bool is_rooted = false, int root = 1) :
        _Tree<void, void>(node_count, begin_node, is_rooted, root) {
        _output_function = this->default_function();
    }
    
    _OTHER_OUTPUT_FUNCTION_SETTING(_Self)
    _DISABLE_CHOOSE_GEN
    
protected:                         
    virtual void __generate_tree() override {
        for (int i = 1; i < this->_node_count; i++) {
            this->__add_edge(this->_p[0], this->_p[i]);
        }
    }
};

template<typename NodeType, typename EdgeType> 
class _HeightTree : public _Tree<NodeType, EdgeType> {
protected:
    typedef _HeightTree<NodeType,EdgeType> _Self;
    _OUTPUT_FUNCTION(_Self)
    _DEF_GEN_FUNCTION
    int _height;
public:

    template<typename T = NodeType, typename U = EdgeType, _IsBothWeight<T, U> = 0>
    _HeightTree(
        int node_count = 1, int begin_node = 1, int root = 1, int height = -1,
        NodeGenFunction nodes_weight_function = nullptr,
        EdgeGenFunction edges_weight_function = nullptr) :
        _Tree<NodeType, EdgeType>(
            node_count, begin_node, true, root, 
            nodes_weight_function, edges_weight_function),
        _height(height) {
        _output_function = this->default_function();
    }

    template<typename T = NodeType, typename U = EdgeType, _IsNodeWeight<T, U> = 0>
    _HeightTree(
        int node_count = 1, int begin_node = 1, int root = 1, int height = -1,
        NodeGenFunction nodes_weight_function = nullptr) :
        _Tree<NodeType, void>(node_count, begin_node, true, root, nodes_weight_function),
        _height(height) {
        _output_function = this->default_function();
    }

    template<typename T = NodeType, typename U = EdgeType, _IsEdgeWeight<T, U> = 0>
    _HeightTree(
        int node_count = 1, int begin_node = 1, int root = 1, int height = -1,
        EdgeGenFunction edges_weight_function = nullptr) :
        _Tree<void, EdgeType>(node_count, begin_node, true, root, edges_weight_function),
        _height(height) {
        _output_function = this->default_function();
    }

    template<typename T = NodeType, typename U = EdgeType, _IsUnweight<T, U> = 0>
    _HeightTree(int node_count = 1, int begin_node = 1, int root = 1, int height = -1) :
        _Tree<void, void>(node_count, begin_node, true, root),
        _height(height) {
        _output_function = this->default_function();
    }

    void set_height(int height) { _height = height; }
    int height() const { return _height; }
    int& height_ref() { return _height; }

    _OTHER_OUTPUT_FUNCTION_SETTING(_Self)
    _DISABLE_CHOOSE_GEN
    _MUST_IS_ROOTED

protected:
    virtual void __self_init() override {
        if (_height == -1) {
            _height = rnd.next(this->_node_count == 1 ? 1 : 2, this->_node_count);
        }
    }

    virtual void __judge_self_limit() override{
        if (_height > this->_node_count || (this->_node_count > 1 && _height <= 1) || _height < 1) {
            __msg::__fail_msg(
                __msg::_err, 
                "restriction of the height is [%d,%d].\n", 
                this->_node_count == 1 ? 1 : 2,
                this->_node_count);
        }
    }

    virtual void __generate_tree() override {
        std::vector<int> number(_height, 1);
        int w = this->_node_count - _height;
        for (int i = 1; i <= w; i++) {
            number[rnd.next(1, _height - 1)]++;
        }
        int l = 0, r = 0, k = 0;
        for (int i = 1; i < this->_node_count; i++) {
            if (r + number[k] == i) {
                l = r;
                r += number[k];
                k++;
            }
            int f = rnd.next(l, r - 1);
            this->__add_edge(this->_p[f], this->_p[i]);
        }
    }
};

template<typename NodeType, typename EdgeType>
class _DegreeTree : public _Tree<NodeType, EdgeType> {
protected:
    typedef _DegreeTree<NodeType,EdgeType> _Self;
    _OUTPUT_FUNCTION(_Self)
    _DEF_GEN_FUNCTION
    int _max_degree;
public:

    template<typename T = NodeType, typename U = EdgeType, _IsBothWeight<T, U> = 0>
    _DegreeTree(
        int node_count = 1, int begin_node = 1, bool is_rooted = false, int root = 1, int max_degree = -1,
        NodeGenFunction nodes_weight_function = nullptr,
        EdgeGenFunction edges_weight_function = nullptr) :
        _Tree<NodeType, EdgeType>(
            node_count, begin_node, is_rooted, root, 
            nodes_weight_function, edges_weight_function),
        _max_degree(max_degree) {
        _output_function = this->default_function();
    }

    template<typename T = NodeType, typename U = EdgeType, _IsNodeWeight<T, U> = 0>
    _DegreeTree(
        int node_count = 1, int begin_node = 1, bool is_rooted = false, int root = 1, int max_degree = -1,
        NodeGenFunction nodes_weight_function = nullptr) :
        _Tree<NodeType, void>(node_count, begin_node, is_rooted, root, nodes_weight_function),
        _max_degree(max_degree) {
        _output_function = this->default_function();
    }

    template<typename T = NodeType, typename U = EdgeType, _IsEdgeWeight<T, U> = 0>
    _DegreeTree(
        int node_count = 1, int begin_node = 1, bool is_rooted = false, int root = 1, int max_degree = -1,
        EdgeGenFunction edges_weight_function = nullptr) :
        _Tree<void, EdgeType>(node_count, begin_node, is_rooted, root,edges_weight_function),
        _max_degree(max_degree) {
        _output_function = this->default_function();
    }

    template<typename T = NodeType, typename U = EdgeType, _IsUnweight<T, U> = 0>
    _DegreeTree(
        int node_count = 1, int begin_node = 1, bool is_rooted = false, int root = 1, int max_degree = -1) :
        _Tree<void, EdgeType>(node_count, begin_node, is_rooted, root),
        _max_degree(max_degree) {
        _output_function = this->default_function();
    }

    void set_max_degree(int max_degree) { _max_degree = max_degree; }
    int max_degree() const { return max_degree; }
    int& max_degree_ref() { return max_degree; }

    _OTHER_OUTPUT_FUNCTION_SETTING(_Self)
    _DISABLE_CHOOSE_GEN
protected:
    virtual void __self_init() override {
        if (_max_degree == -1) {
            if (this->_node_count == 1) {
                _max_degree = 0;
            } else if (this->_node_count == 2) {
                _max_degree = 1;
            } else {
                _max_degree = rnd.next(2, this->_node_count - 1);
            }
        }
    }

    virtual void __judge_self_limit() override {
        if (_max_degree > this->_node_count - 1) {
            __msg::__warn_msg(__msg::_err,
                            "The max degree limit %d is greater than node - 1, equivalent to use Tree::gen_pruefer",
                            _max_degree);
        }
        int max_degree_limit = this->_node_count == 1 ? 0 : (this->_node_count == 2 ? 1 : 2);
        
        if (_max_degree < max_degree_limit) {
            __msg::__fail_msg(
                __msg::_err,
                "The max degree limit of %s node's tree is greater than or equal to %d, but found %d.",
                this->_node_count > 2 ? "3 or more" : std::to_string(this->_node_count).c_str(),
                max_degree_limit,
                _max_degree);
        }
    }

    virtual void __generate_tree() override {
        std::vector<int> times = rand_vector::rand_sum(this->_node_count, this->_node_count - 2, 0, _max_degree - 1);
        std::vector<int> pruefer = rand_vector::shuffle_index(times);
        this->__pruefer_decode(pruefer);
    }
};

template<typename NodeType, typename EdgeType>
class _SonTree : public _Tree<NodeType, EdgeType> {
protected:
    typedef _SonTree<NodeType,EdgeType> _Self;
    _OUTPUT_FUNCTION(_Self)
    _DEF_GEN_FUNCTION
    int _max_son;  
public:

    template<typename T = NodeType, typename U = EdgeType, _IsBothWeight<T, U> = 0>
    _SonTree(
        int node_count = 1, int begin_node = 1,  int root = 1, int max_son = -1,
        NodeGenFunction nodes_weight_function = nullptr,
        EdgeGenFunction edges_weight_function = nullptr) :
        _Tree<NodeType, EdgeType>(
            node_count, begin_node, true, root, 
            nodes_weight_function, edges_weight_function),
        _max_son(max_son) {
        _output_function = this->default_function();
    }

    template<typename T = NodeType, typename U = EdgeType, _IsNodeWeight<T, U> = 0>
    _SonTree(
        int node_count = 1, int begin_node = 1,  int root = 1, int max_son = -1,
        NodeGenFunction nodes_weight_function = nullptr) :
        _Tree<NodeType, void>(node_count, begin_node, true, root, nodes_weight_function),
        _max_son(max_son) {
        _output_function = this->default_function();
    }

    template<typename T = NodeType, typename U = EdgeType, _IsEdgeWeight<T, U> = 0>
    _SonTree(
        int node_count = 1, int begin_node = 1,  int root = 1, int max_son = -1,
        EdgeGenFunction edges_weight_function = nullptr) :
        _Tree<void, EdgeType>(node_count, begin_node, true, root, edges_weight_function),
        _max_son(max_son) {
        _output_function = this->default_function();
    }

    template<typename T = NodeType, typename U = EdgeType, _IsUnweight<T, U> = 0>
    _SonTree(int node_count = 1, int begin_node = 1,  int root = 1, int max_son = -1) :
        _Tree<void, void>(node_count, begin_node, true, root),
        _max_son(max_son) {
        _output_function = this->default_function();
    }

    void set_max_son(int max_son) { _max_son = max_son; }
    int max_son() const { return _max_son; }
    int& max_son_ref() { return _max_son; }

    _OTHER_OUTPUT_FUNCTION_SETTING(_Self)
    _DISABLE_CHOOSE_GEN
    _MUST_IS_ROOTED

protected:
    virtual void __self_init() override {
        if (_max_son == -1) {
            if (this->_node_count == 1) {
                _max_son = 0;
            } else if (this->_node_count == 2) {
                _max_son = 1;
            } else {
                _max_son = rnd.next(2, this->_node_count - 1);
            }
        }
    }

    virtual void __judge_self_limit() override {
        if (_max_son > this->_node_count - 1) {
            __msg::__warn_msg(
                __msg::_err,
                "The max son limit %d is greater than node - 1 (%d), equivalent to use Tree::gen_pruefer",
                this->_node_count - 1,
                _max_son);
        }

        int max_son_limit =
            this->_node_count == 1 ? 0 :
            (this->_node_count == 2 ? 1 : 2);
        
        if (_max_son < max_son_limit) {
            __msg::__fail_msg(
                __msg::_err,
                "The max son limit of %s node's tree is greater than or equal to %d, but found %d.",
                this->_node_count > 2 ? "3 or more" : std::to_string(this->_node_count).c_str(),
                max_son_limit,
                _max_son);
        }
    }

    virtual void __generate_tree() override {
        int max_degree = _max_son + 1;
        std::vector<int> times = rand_vector::rand_sum(this->_node_count, this->_node_count - 2, 0, max_degree - 1);
        if (times[this->_root] == max_degree - 1) {
            int p;
            do {
                p = rnd.next(0, this->_node_count - 1);
            } while (p == this->_root || times[p] == max_degree - 1);
            std::swap(times[this->_root], times[p]);
        }
        std::vector<int> pruefer = rand_vector::shuffle_index(times);
        this->__pruefer_decode(pruefer);
    }
}; 

class _BasicGraph {
protected:
    int _node_count;
    int _edge_count;
    int _begin_node;
    bool _direction;
    bool _multiply_edge;
    bool _self_loop;
    bool _connect;
    bool _swap_node;
    bool _output_node_count;
    bool _output_edge_count; 

    std::vector<int> _node_indices;

public:
    _BasicGraph(
        int node_count, int edge_count, int begin_node,
        bool direction, bool multiply_edge, bool self_loop, bool connect,
        bool output_node_count, bool output_edge_count) :
        _node_count(node_count),
        _edge_count(edge_count),
        _begin_node(begin_node),
        _direction(direction),
        _multiply_edge(multiply_edge),
        _self_loop(self_loop),
        _connect(connect),
        _swap_node(_direction ? false : true),
        _output_node_count(output_node_count),
        _output_edge_count(output_edge_count) {}

    _BasicGraph(
        int node_count, int edge_count, int begin_node,
        bool direction, bool multiply_edge, bool self_loop, bool connect,
        bool swap_node, bool output_node_count, bool output_edge_count) :
        _node_count(node_count),
        _edge_count(edge_count),
        _begin_node(begin_node),
        _direction(direction),
        _multiply_edge(multiply_edge),
        _self_loop(self_loop),
        _connect(connect),
        _swap_node(swap_node),
        _output_node_count(output_node_count),
        _output_edge_count(output_edge_count) {
        __init_node_indices();
    }

    void set_node_count(int node_count) { 
        if (node_count != _node_count) {
            _node_count = node_count; 
            __init_node_indices();                      
        }             
    }

    void set_edge_count(int edge_count) { _edge_count = edge_count; }

    void set_begin_node(int begin_node) { 
        if (begin_node != _begin_node) {
            _begin_node = begin_node; 
            __init_node_indices();                      
        }                 
    }

    void set_direction(bool direction) { 
        if (_direction != direction) {
            _swap_node = direction ? false : true;
            __msg::__warn_msg(__msg::_err, "Setting `swap_node` to %s, because `direction` changed!", (_swap_node ? "true" : "false"));
        }
        _direction = direction; 
    }

    void set_multiply_edge(bool multiply_edge) { _multiply_edge = multiply_edge; }

    void set_self_loop(bool self_loop) { _self_loop = self_loop; }

    void set_connect(bool connect) { _connect = connect; }

    void set_swap_node(bool swap_node) { _swap_node = swap_node; }

    void set_output_node_count(bool output_node_count) { _output_node_count = output_node_count; }
    void set_output_edge_count(bool output_edge_count) { _output_edge_count = output_edge_count; } 
    
    bool direction() const { return _direction; }
    bool& direction_ref() { return _direction; }
    
    bool multiply_edge() const { return _multiply_edge; }
    bool& multiply_edge_ref() { return _multiply_edge; }
    
    bool self_loop() const { return _self_loop; }
    bool& self_loop_ref() { return _self_loop; }
    
    bool connect() const { return _connect; }
    bool& connect_ref() { return _connect; } 
    
    int node_count() const { return _node_count; }
    int& node_count_ref() { return _node_count; }

    int edge_count() const { return _edge_count; }
    int& edge_count_ref() { return _edge_count; }

    int begin_node() const { return _begin_node; }
    int& begin_node_ref() { return _begin_node; }

    std::vector<int> node_indices() const { return _node_indices; }
    std::vector<int>& node_indices_ref() { return _node_indices; }
    
    void set_node_indices(std::vector<int> node_indices) {
        if ((int)node_indices.size() != _node_count) {
            __msg::__warn_msg(
                __msg::_err, 
                "Node indices size must equal to node count %d, but found %d.", 
                _node_count, 
                node_indices.size());
            return;
        }
        _node_indices = node_indices;
    }
    
    void set_node_indices(int index, int number) {
        if (index < 1 || index > _node_count) {
            __msg::__warn_msg(
                __msg::_err,
                "Node index must in range [1, %d], but found %d.",
                _node_count,
                index);
            return;
        }
        _node_indices[index - 1] = number;
    }
    
protected:
    void __init_node_indices() {
        _node_indices.resize(_node_count);
        std::iota(_node_indices.begin(), _node_indices.end(), _begin_node);
    }
};

template<typename NodeType, typename EdgeType>
class _Graph : public _BasicGraph, public _RandomFunction<NodeType, EdgeType> {
protected:
    typedef _Graph<NodeType,EdgeType> _Self;
    _OUTPUT_FUNCTION(_Self)
    _DEF_GEN_FUNCTION
    std::vector<_Edge<EdgeType>> _edges;
    std::vector<_Node<NodeType>> _nodes_weight; 
    std::map<_BasicEdge, bool> _e;

public:
    friend class _LinkImpl<NodeType, EdgeType>;
    friend class _TreeLinkImpl<NodeType, EdgeType>;

public:
    template<typename T = NodeType, typename U = EdgeType, _IsBothWeight<T, U> = 0>
    _Graph(
        int node_count = 1, int edge_count = 0, int begin_node = 1, 
        NodeGenFunction nodes_weight_function = nullptr,
        EdgeGenFunction edges_weight_function = nullptr) :
        _BasicGraph(
            node_count, edge_count, begin_node, 
            false, false, false, false,
            true, true),
        _RandomFunction<NodeType, EdgeType>(nodes_weight_function, edges_weight_function) {
        _output_function = default_function();
    }

    template<typename T = NodeType, typename U = EdgeType, _IsNodeWeight<T, U> = 0>
    _Graph(
        int node_count = 1, int edge_count = 0, int begin_node = 1, 
        NodeGenFunction nodes_weight_function = nullptr) :
        _BasicGraph(
            node_count, edge_count, begin_node, 
            false, false, false, false,
            true, true),
        _RandomFunction<NodeType, void>(nodes_weight_function, nullptr) {
        _output_function = default_function();
    }

    template<typename T = NodeType, typename U = EdgeType, _IsEdgeWeight<T, U> = 0>
    _Graph(
        int node_count = 1, int edge_count = 0, int begin_node = 1, 
        EdgeGenFunction edges_weight_function = nullptr) :
        _BasicGraph(
            node_count, edge_count, begin_node, 
            false, false, false, false,
            true, true),
        _RandomFunction<void, EdgeType>(nullptr, edges_weight_function) {
        _output_function = default_function();
    }

    template<typename T = NodeType, typename U = EdgeType, _IsUnweight<T, U> = 0>
    _Graph(int node_count = 1, int edge_count = 0, int begin_node = 1) :
        _BasicGraph(
            node_count, edge_count, begin_node, 
            false, false, false, false,
            true, true),
        _RandomFunction<void, void>(nullptr, nullptr) {
        _output_function = default_function();
    }

    std::vector<_Edge<EdgeType>> edges() const { return __get_output_edges(); }
    std::vector<_Edge<EdgeType>>& edges_ref() { return _edges; }

    template<typename T = NodeType, _HasT<T> = 0>
    std::vector<_Node<NodeType>>& nodes_weight(){ return _nodes_weight; }
    template<typename T = NodeType, _HasT<T> = 0>
    std::vector<_Node<NodeType>> nodes_weight_ref() const { return _nodes_weight; }

    void default_output(std::ostream& os) const {
        std::string first_line = "";
        first_line += __format_output_node();
        if (_output_edge_count) {
            if (first_line != "") {
                first_line += " ";
            }
            first_line += std::to_string(_edge_count);
        }
        
        if (__output_node_weights(os, first_line)) {
            if (_edge_count >= 1) {
                os << "\n";
            }
        } else {
            if (first_line != "") {
                os << first_line;
                if (_edge_count >= 1) {
                    os << "\n";
                }
            }
        }
        int edge_cnt = 0;
        for (_Edge<EdgeType> e: __get_output_edges()) {
            if (_swap_node && rnd.next(2)) {
                e.set_output_default(true);
            }
            os << e;
            e.set_output_default();
            if (++edge_cnt < _edge_count) {
                os << "\n";
            }
        }
    }

    void gen() {
        __init();
        __generate_graph();
        __generate_nodes_weight();
        shuffle(_edges.begin(),_edges.end());
    }

    _OTHER_OUTPUT_FUNCTION_SETTING(_Self)

protected:
    template<typename T = NodeType, _HasT<T> = 0>
    bool __output_node_weights(std::ostream& os, std::string& first_line) const {
        int node_cnt = 0;
        for (_Node<NodeType> node : _nodes_weight) {
            node_cnt ++;
            if(node_cnt == 1) {
                if (first_line != "") {
                    os << first_line << "\n";
                }
            }
            os << node << " ";
        }
        return node_cnt >= 1;
    }

    template<typename T = NodeType, _NotHasT<T> = 0>
    bool __output_node_weights(std::ostream&, std::string&) const {
        return false;
    }

    virtual std::string __format_output_node() const {
        if (_output_node_count) {
            return std::to_string(_node_count);
        }
        return "";
    }

    virtual void __judge_upper_limit() {
        if (!_multiply_edge) {
            long long limit = (long long) _node_count * (long long) (_node_count - 1) / 2;
            if (_direction) {
                limit *= 2;
            }
            if (_self_loop) {
                limit += _node_count;
            }
            if (_edge_count > limit) {
                __msg::__fail_msg(__msg::_err, "number of edges must less than or equal to %lld.", limit);
            }
        } else {
            if (_node_count == 1 && !_self_loop && _edge_count > 0) {
                __msg::__fail_msg(__msg::_err, "number of edges must equal to 0.");
            }
        }
    }

    virtual void __judge_lower_limit() {
        if (_edge_count < 0) {
            __msg::__fail_msg(__msg::_err, "number of edges must be a non-negative integer.");
        }
        if (_connect && _edge_count < _node_count - 1) {
            __msg::__fail_msg(__msg::_err, "number of edges must greater than or equal to %d.", _node_count - 1);
        }
    }

    virtual void __judge_self_limit() {}

    void __judge_limits() {
        __judge_upper_limit();
        __judge_lower_limit();
        __judge_self_limit();
    }
    
    virtual void __self_init(){};
    
    void __init() {
        __self_init();
        this->__check_nodes_weight_function();
        this->__check_edges_weight_function();
        __judge_limits();
        _edges.clear();
        _nodes_weight.clear();   
        if (!_multiply_edge) {
            _e.clear();
        }
        if ((int)_node_indices.size() != _node_count) {
            this->__init_node_indices();
        }
    }
    
    bool __judge_self_loop(int u, int v) {
        return !_self_loop && u == v;
    }

    bool __judge_multiply_edge(int u, int v) {
        if (_multiply_edge) {
            return false;
        }
        if (_e[{u, v}]) {
            return true;
        }
        return false;
    }

    void __add_edge_into_map(int u, int v) {
        if (!_multiply_edge) {
            _e[{u, v}] = true;
            if (!_direction) {
                _e[{v, u}] = true;
            }
        }
    }
    
    void __add_edge(_Edge<EdgeType> edge) {
        int u = edge.u();
        int v = edge.v();
        __add_edge_into_map(u, v);
        _edges.emplace_back(edge);
    }
    
    template<typename T = EdgeType, _NotHasT<T> = 0>
    void __add_edge(int u, int v) {
        __add_edge(_Edge<void>(u, v));
    }

    template<typename T = EdgeType, _HasT<T> = 0>
    void __add_edge(int u, int v) {
        EdgeType w = this->_edges_weight_function();
        __add_edge(_Edge<EdgeType>(u, v, w));
    }

    template<typename T = NodeType, _NotHasT<T> = 0>
    void __generate_nodes_weight() { return; }
    
    template<typename T = NodeType, _HasT<T> = 0>
    void __generate_nodes_weight() {
        for (int i = 0; i < _node_count ; i++) {
            NodeType w = this->_nodes_weight_function();
            _nodes_weight.emplace_back(w);
        }
    }

    template<typename T = EdgeType, _NotHasT<T> = 0>
    _Edge<void> __convert_edge(int u, int v) {
        return _Edge<void>(u, v);
    }

    template<typename T = EdgeType, _HasT<T> = 0>
    _Edge<EdgeType> __convert_edge(int u, int v) {
        EdgeType w = this->_edges_weight_function();
        return _Edge<EdgeType>(u, v, w);
    }

    virtual _Edge<EdgeType> __rand_edge() {
        int u, v;
        do {
            u = rnd.next(_node_count);
            v = rnd.next(_node_count);
        } while (__judge_self_loop(u, v) || __judge_multiply_edge(u, v));
        return this->__convert_edge(u, v);
    }

    virtual void __generate_connect() {
        _Tree<void, void> tree(_node_count, 0);
        tree.gen();
        std::vector <_Edge<void>> edge = tree.edges();
        for (auto e: edge) {
            __add_edge(e.u(), e.v());
        }
    }

    virtual void __generate_graph() {
        int m = _edge_count;
        if (_connect) {
            m -= _node_count - 1;
            __generate_connect();
        }
        while (m--) {
            __add_edge(__rand_edge());
        }
    }

protected:
    template<typename T = EdgeType, _NotHasT<T> = 0>
    std::vector<_Edge<EdgeType>> __get_output_edges() const {
        std::vector<_Edge<EdgeType>> output_edges;
        for (const auto& edge : _edges) {
            output_edges.emplace_back(_node_indices[edge.u()], _node_indices[edge.v()]);
        }
        return output_edges;
    }

    template<typename T = EdgeType, _HasT<T> = 0>
    std::vector<_Edge<EdgeType>> __get_output_edges() const {
        std::vector<_Edge<EdgeType>> output_edges;
        for (const auto& edge : _edges) {
            output_edges.emplace_back(_node_indices[edge.u()], _node_indices[edge.v()], edge.w());
        }
        return output_edges;
    }

    template<typename T = NodeType, typename U = EdgeType, _IsBothWeight<T, U> = 0>
    _Graph(
        int node_count, int edge_count, int begin_node, 
        bool direction, bool multiply_edge, bool self_loop, bool connect, bool swap_node,
        NodeGenFunction nodes_weight_function,
        EdgeGenFunction edges_weight_function) :
        _BasicGraph(
            node_count, edge_count, begin_node, 
            direction, multiply_edge, self_loop, connect, swap_node, 
            true, true),
        _RandomFunction<NodeType, EdgeType>(nodes_weight_function, edges_weight_function) {
        _output_function = default_function();
    }

    template<typename T = NodeType, typename U = EdgeType, _IsNodeWeight<T, U> = 0>
    _Graph(
        int node_count, int edge_count, int begin_node, 
        bool direction, bool multiply_edge, bool self_loop, bool connect, bool swap_node,
        NodeGenFunction nodes_weight_function) :
        _BasicGraph(
            node_count, edge_count, begin_node, 
            direction, multiply_edge, self_loop, connect, swap_node, 
            true, true),
        _RandomFunction<NodeType, void>(nodes_weight_function, nullptr) {
        _output_function = default_function();
    }

    template<typename T = NodeType, typename U = EdgeType, _IsEdgeWeight<T, U> = 0>
    _Graph(
        int node_count, int edge_count, int begin_node, 
        bool direction, bool multiply_edge, bool self_loop, bool connect, bool swap_node,
        EdgeGenFunction edges_weight_function) :
        _BasicGraph(
            node_count, edge_count, begin_node, 
            direction, multiply_edge, self_loop, connect, swap_node, 
            true, true),
        _RandomFunction<void, EdgeType>(nullptr, edges_weight_function) {
        _output_function = default_function();
    }

    template<typename T = NodeType, typename U = EdgeType, _IsUnweight<T, U> = 0>
    _Graph(
        int node_count, int edge_count, int begin_node, 
        bool direction, bool multiply_edge, bool self_loop, bool connect, bool swap_node) :
        _BasicGraph(
            node_count, edge_count, begin_node, 
            direction, multiply_edge, self_loop, connect, swap_node, 
            true, true),
        _RandomFunction<void, void>(nullptr, nullptr) {
        _output_function = default_function();
    }
};

#define _DISABLE_EDGE_COUNT \
    void set_edge_count(int edge_count) = delete; \
    int& edge_count_ref() = delete;

#define _DISABLE_DIRECTION  \
    void set_direction(bool direction) = delete; \
    bool& direction_ref() = delete;

#define _DISABLE_MULTIPLY_EDGE \
    void set_multiply_edge(bool multiply_edge) = delete; \
    bool& multiply_edge_ref() = delete;

#define _DISABLE_SELF_LOOP \
    void set_self_loop(bool self_loop) = delete; \
    bool& self_loop_ref() = delete;

#define _DISABLE_CONNECT \
    void set_connect(bool connect) = delete; \
    bool& connect_ref() = delete;


template<typename NodeType, typename EdgeType>
class _BipartiteGraph : public _Graph<NodeType, EdgeType> {
public:
    enum NodeOutputFormat {
        Node,
        LeftRight,
        NodeLeft,
        NodeRight
    };
protected:       
    int _left, _right;
    bool _different_part;
    std::vector<int> _part[2];
    std::vector<int> _degree[2];
    int _d[2];
    NodeOutputFormat _node_output_format;
    typedef _BipartiteGraph<NodeType, EdgeType> _Self;
    _OUTPUT_FUNCTION(_Self)
    _DEF_GEN_FUNCTION
public :
    template<typename T = NodeType, typename U = EdgeType, _IsBothWeight<T, U> = 0>
    _BipartiteGraph(
        int node_count = 1, int edge_count = 0, int begin_node = 1, int left = -1,
        NodeGenFunction nodes_weight_function = nullptr,
        EdgeGenFunction edges_weight_function = nullptr) :
        _Graph<NodeType,EdgeType>(
            node_count, edge_count, begin_node, 
            false, false, false, false, false, 
            nodes_weight_function, edges_weight_function),
        _left(left),
        _different_part(false),
        _node_output_format(Node) {
        _output_function = default_function();
    }

    template<typename T = NodeType, typename U = EdgeType, _IsNodeWeight<T, U> = 0>
    _BipartiteGraph(
        int node_count = 1, int edge_count = 0, int begin_node = 1, int left = -1,
        NodeGenFunction nodes_weight_function = nullptr) :
        _Graph<NodeType, void>(
            node_count, edge_count, begin_node,
            false, false, false, false, false,
            nodes_weight_function),
        _left(left),
        _different_part(false),
        _node_output_format(Node) {
        _output_function = default_function();
    }

    template<typename T = NodeType, typename U = EdgeType, _IsEdgeWeight<T, U> = 0>
    _BipartiteGraph(
        int node_count = 1, int edge_count = 0, int begin_node = 1, int left = -1,
        EdgeGenFunction edges_weight_function = nullptr) :
        _Graph<void, EdgeType>(
            node_count, edge_count, begin_node,
            false, false, false, false, false,
            edges_weight_function),
        _left(left),
        _different_part(false),
        _node_output_format(Node) {
        _output_function = default_function();
    }

    template<typename T = NodeType, typename U = EdgeType, _IsUnweight<T, U> = 0>
    _BipartiteGraph(int node_count = 1, int edge_count = 0, int begin_node = 1, int left = -1) :
        _Graph<void, void>(
            node_count, edge_count, begin_node,
            false, false, false, false, false),
        _left(left),
        _different_part(false),
        _node_output_format(Node) {
        _output_function = default_function();
    }

    void set_different_part(bool different_part) { 
        if (_different_part != different_part) {
            _different_part = different_part; 
            __remark_node_indices();
        }             
    }

    void set_left(int left) {
        _left = left;
        _right = this->_node_count - _left;
    }

    void set_right(int right) {
        _right = right;
        _left = this->_node_count - _right;
    }

    void set_left_right(int left, int right) {
        if (left + right < 0) {
            __msg::__fail_msg(
                    __msg::_err,
                    "number of left part nodes add right part nodes must greater than 0."
                    "But found %d + %d = %d",
                    left, right, left + right);
        }
        _left = left;
        _right = right;
        int node_count = left + right;
        if (this->_node_count != node_count) {
            this->_node_count = node_count;
            this->__init_node_indices();
        }
        
    }

    int left() const { return _left; }
    int& left_ref() { return _left; }

    int right() const { return _right; }
    int& right_ref() { return _right; }

    void set_node_output_format(NodeOutputFormat format) { _node_output_format = format; }
    void use_format_node() {  _node_output_format = Node; }
    void use_format_left_right() { _node_output_format = LeftRight; }
    void use_format_node_left() { _node_output_format = NodeLeft; }
    void use_format_node_right() { _node_output_format = NodeRight; }
    
    _DISABLE_SELF_LOOP
    _DISABLE_DIRECTION
    _OTHER_OUTPUT_FUNCTION_SETTING(_Self)

protected:
    void __remark_node_indices_by_part(std::vector<int>& part) {
        int index = this->_begin_node;
        for (int x : part) {
            this->_node_indices[x] = index;
            index++;
        }
    }
    
    // node indices modified by different part change
    void __remark_node_indices() {
        if (_different_part) {
            if (_part[0].empty() && _part[1].empty()) {
                return;
            }
            this->_node_indices.resize(this->_node_count);
            __remark_node_indices_by_part(_part[0]);
            __remark_node_indices_by_part(_part[1]);
        } else {
            this->__init_node_indices();
        }
    }
    
    virtual std::string __format_output_node() const override {
        std::string str = "";
        if (this->_output_node_count) {
            if (_node_output_format == Node) {
                str += std::to_string(this->_node_count);
            } else if (_node_output_format == LeftRight) {
                str += std::to_string(_left);
                str += " ";
                str += std::to_string(_right);
            } else if (_node_output_format == NodeLeft) {
                str += std::to_string(this->_node_count);
                str += " ";
                str += std::to_string(_left);
            } else if (_node_output_format == NodeRight) {
                str += std::to_string(this->_node_count);
                str += " ";
                str += std::to_string(_right);
            }
        }
        return str;
    }

    void __rand_left() {
        if (_left >= 0) {
            return;
        }
        int node = this->_node_count, edge = this->_edge_count;
        int l = 0, r = node / 2, limit;
        if (!this->_multiply_edge) {
            if (edge > r * (node - r)) {
                __msg::__fail_msg(
                        __msg::_err,
                        "number of edges must less than or equal to %d.",
                        r * (node - r));
            }
            while (l <= r) {
                int mid = (l + r) / 2;
                if (mid * (node - mid) < edge) {
                    l = mid + 1;
                } else {
                    limit = r;
                    r = mid - 1;
                }
            }
        } else {
            limit = 1;
        }
        _left = rnd.next(limit, node / 2);
        _right = node - _left;
    }

    virtual void __self_init() override {
        __rand_left();
        int node = this->_node_count;
        _right = node - _left;
        for (int i = 0; i < 2; i++) {
            _part[i].clear();
        }
        std::vector<int> p = rnd.perm(node, 0);
        for (int i = 0; i < _left; i++) {
            _part[0].push_back(p[i]);
        }
        for (int i = _left; i < node; i++) {
            _part[1].push_back(p[i]);
        }
        if (_different_part) {
            __remark_node_indices();
        }
        if (this->_connect) {
            _degree[0].resize(_left, 1);
            _degree[1].resize(_right, 1);
            for (int i = _left; i < node - 1; i++) {
                _degree[0][rnd.next(_left)]++;
            }
            for (int i = _right; i < node - 1; i++) {
                _degree[1][rnd.next(_right)]++;
            }
            _d[0] = node - 1;
            _d[1] = node - 1;
        }
    }

    virtual void __judge_self_limit() override {
        if (_left < 0) {
            __msg::__fail_msg(__msg::_err, "Left part size must greater than or equal to 0, but found %d",_left);
        }
        if (_right < 0) {
            __msg::__fail_msg(__msg::_err, "Left part size must greater than or equal to 0, but found %d",_right);
        }
    }

    virtual void __judge_upper_limit() override {
        if (!this->_multiply_edge) {
            long long limit = (long long)_left * (long long)_right;
            if (limit < this->_edge_count) {
                __msg::__fail_msg(
                    __msg::_err, "number of edges must less than or equal to %lld, but found %d.",
                    limit, this->_edge_count);
            }
        } else {
            if (this->_node_count == 1 && this->_edge_count > 0) {
                __msg::__fail_msg(__msg::_err, "number of edges must less than or equal to 0, but found %d.", this->_edge_count);
            }
        }
    }

    virtual _Edge<EdgeType> __rand_edge() override{
        int u, v;
        do {
            u = rnd.any(_part[0]);
            v = rnd.any(_part[1]);
        } while (this->__judge_multiply_edge(u, v));
        return this->__convert_edge(u, v);
    }

    void __add_part_edge(int f, int i, int j) {
        int u = _part[f][i];
        int v = _part[f ^ 1][j];
        if (f == 1) {
            std::swap(u, v);
        }
        this->__add_edge(u, v);
        _d[0]--;
        _d[1]--;
        _degree[f][i]--;
        _degree[f ^ 1][j]--;
    }

    virtual void __generate_connect() override {
        int f = 0;
        while (_d[0] + _d[1] > 0) {
            for (int i = 0; i < (f == 0 ? _left : _right); i++) {
                if (_degree[f][i] == 1) {
                    if (_d[f] == 1) {
                        for (int j = 0; j < (f == 0 ? _right : _left); j++) {
                            if (_degree[f ^ 1][j] == 1) {
                                __add_part_edge(f, i, j);
                            }
                        }
                    } else {
                        int j;
                        do {
                            j = rnd.next(f == 0 ? _right : _left);
                        } while (_degree[f ^ 1][j] < 2);
                        __add_part_edge(f, i, j);
                    }
                }
            }
            f ^= 1;
        }
    }
};

template<typename NodeType, typename EdgeType>
class _DAG : public _Graph<NodeType, EdgeType> {
protected:
    std::vector<int> _p;
    typedef _DAG<NodeType, EdgeType> _Self;
    _OUTPUT_FUNCTION(_Self)
    _DEF_GEN_FUNCTION
public:
    template<typename T = NodeType, typename U = EdgeType, _IsBothWeight<T, U> = 0>
    _DAG(
        int node_count = 1, int edge_count = 0, int begin_node = 1,
        NodeGenFunction nodes_weight_function = nullptr,
        EdgeGenFunction edges_weight_function = nullptr) :
        _Graph<NodeType,EdgeType>(
            node_count, edge_count, begin_node, 
            true, false, false, false, false, 
            nodes_weight_function, edges_weight_function) {
        _output_function = default_function();
    }

    template<typename T = NodeType, typename U = EdgeType, _IsNodeWeight<T, U> = 0>
    _DAG(
        int node_count = 1, int edge_count = 0, int begin_node = 1,
        NodeGenFunction nodes_weight_function = nullptr) :
        _Graph<NodeType, void>(
            node_count, edge_count, begin_node,
            true, false, false, false, false,
            nodes_weight_function) {
        _output_function = default_function();
    }

    template<typename T = NodeType, typename U = EdgeType, _IsEdgeWeight<T, U> = 0>
    _DAG(
        int node_count = 1, int edge_count = 0, int begin_node = 1,
        EdgeGenFunction edges_weight_function = nullptr) :
        _Graph<void, EdgeType>(
            node_count, edge_count, begin_node,
            true, false, false, false, false,
            edges_weight_function) {
        _output_function = default_function();
    }

    template<typename T = NodeType, typename U = EdgeType, _IsUnweight<T, U> = 0>
    _DAG(int node_count = 1, int edge_count = 0, int begin_node = 1) :
        _Graph<void, void>(
            node_count, edge_count, begin_node,
            true, false, false, false, false) {
        _output_function = default_function();
    }

    _DISABLE_DIRECTION
    _DISABLE_SELF_LOOP
    _OTHER_OUTPUT_FUNCTION_SETTING(_Self)

protected:
    virtual void __self_init() override{
        _p = rnd.perm(this->_node_count, 0);
    }

    virtual void __generate_connect() override{
        for (int i = 1; i < this->_node_count; i++) {
            int f = rnd.next(i);
            this->__add_edge(_p[f], _p[i]);
        }
    }

    virtual _Edge<EdgeType> __rand_edge() override {
        int u, v;
        do {
            u = rnd.next(this->_node_count);
            v = rnd.next(this->_node_count);
            if (u > v) {
                std::swap(u, v);
            }
            u = _p[u];
            v = _p[v];
        } while (this->__judge_self_loop(u, v) || this->__judge_multiply_edge(u, v));
        return this->__convert_edge(u, v);
    }
};

template<typename NodeType, typename EdgeType>
class _CycleGraph : public _Graph<NodeType, EdgeType> {
protected:
    typedef _CycleGraph<NodeType, EdgeType> _Self;
    _OUTPUT_FUNCTION(_Self)
    _DEF_GEN_FUNCTION
public:

    template<typename T = NodeType, typename U = EdgeType, _IsBothWeight<T, U> = 0>
    _CycleGraph(
        int node_count = 3, int begin_node = 1,
        NodeGenFunction nodes_weight_function = nullptr,
        EdgeGenFunction edges_weight_function = nullptr) :
        _Graph<NodeType, EdgeType>(
            node_count, node_count, begin_node, 
            false, false, false, true, true, 
            nodes_weight_function, edges_weight_function)
    {
        _output_function = default_function();
    }

    template<typename T = NodeType, typename U = EdgeType, _IsNodeWeight<T, U> = 0>
    _CycleGraph(
        int node_count = 3, int begin_node = 1,
        NodeGenFunction nodes_weight_function = nullptr) :
        _Graph<NodeType, void>(
            node_count, node_count, begin_node, 
            false, false, false, true, true, 
            nodes_weight_function)
    {
        _output_function = default_function();
    }

    template<typename T = NodeType, typename U = EdgeType, _IsEdgeWeight<T, U> = 0>
    _CycleGraph(
        int node_count = 3, int begin_node = 1,
        EdgeGenFunction edges_weight_function = nullptr) :
        _Graph<void, EdgeType>(
            node_count, node_count, begin_node, 
            false, false, false, true, true, 
            edges_weight_function)
    {
        _output_function = default_function();
    }

    template<typename T = NodeType, typename U = EdgeType, _IsUnweight<T, U> = 0>
    _CycleGraph(int node_count = 3, int begin_node = 1) :
        _Graph<void, void>(
            node_count, node_count, begin_node, 
            false, false, false, true, true)
    {
        _output_function = default_function();
    }

    _DISABLE_CONNECT
    _DISABLE_MULTIPLY_EDGE
    _DISABLE_SELF_LOOP
    _DISABLE_EDGE_COUNT

    _OTHER_OUTPUT_FUNCTION_SETTING(_Self)

protected:

    virtual void __self_init() override {
        this->_edge_count = this->_node_count;
    }

    virtual void __judge_lower_limit() override {
        if (this->_node_count < 3) {
            __msg::__fail_msg(__msg::_err, "number of nodes must greater than or equal to 3, but found %d.", this->_node_count);
        }
    }

    virtual void __generate_graph() override {
        int node = this->_node_count;
        std::vector<int> p = rnd.perm(node, 0);
        for (int i = 0; i < node; i++) {
            this->__add_edge(p[i], p[(i + 1) % node]);
        }
    }
};

template<typename NodeType, typename EdgeType>
class _WheelGraph : public _Graph<NodeType, EdgeType> {
protected:
    typedef _WheelGraph<NodeType, EdgeType> _Self;
    _OUTPUT_FUNCTION(_Self)
    _DEF_GEN_FUNCTION

public:
    template<typename T = NodeType, typename U = EdgeType, _IsBothWeight<T, U> = 0>
    _WheelGraph(
        int node_count = 4, int begin_node = 1,
        NodeGenFunction nodes_weight_function = nullptr,
        EdgeGenFunction edges_weight_function = nullptr) :
        _Graph<NodeType, EdgeType>(
            node_count, 2 * node_count - 2, begin_node, 
            false, false, false, true, true, 
            nodes_weight_function, edges_weight_function)
    {
        _output_function = default_function();
    }

    template<typename T = NodeType, typename U = EdgeType, _IsNodeWeight<T, U> = 0>
    _WheelGraph(
        int node_count = 4, int begin_node = 1,
        NodeGenFunction nodes_weight_function = nullptr) :
        _Graph<NodeType, void>(
            node_count, 2 * node_count - 2, begin_node, 
            false, false, false, true, true, 
            nodes_weight_function)
    {
        _output_function = default_function();
    }

    template<typename T = NodeType, typename U = EdgeType, _IsEdgeWeight<T, U> = 0>
    _WheelGraph(
        int node_count = 4, int begin_node = 1,
        EdgeGenFunction edges_weight_function = nullptr) :
        _Graph<void, EdgeType>(
            node_count, 2 * node_count - 2, begin_node, 
            false, false, false, true, true, 
            edges_weight_function)
    {
        _output_function = default_function();
    }

    template<typename T = NodeType, typename U = EdgeType, _IsUnweight<T, U> = 0>
    _WheelGraph(int node_count = 4, int begin_node = 1) :
        _Graph<void, void>(
            node_count, 2 * node_count - 2, begin_node, 
            false, false, false, true, true)
    {
        _output_function = default_function();
    }

    _DISABLE_CONNECT
    _DISABLE_MULTIPLY_EDGE
    _DISABLE_SELF_LOOP
    _DISABLE_EDGE_COUNT

    _OTHER_OUTPUT_FUNCTION_SETTING(_Self)

protected:
    virtual void __self_init() override {
        this->_edge_count = 2 * this->_node_count - 2;
    }

    virtual void __judge_lower_limit() override {
        if (this->_node_count < 4) {
            __msg::__fail_msg(__msg::_err, "number of nodes must greater than or equal to 4, but found %d.", this->_node_count);
        }
    }

    virtual void __generate_graph() override {
        int node = this->_node_count;
        std::vector<int> p = rnd.perm(node, 0);
        for (int i = 0; i < node - 1; i++) {
            this->__add_edge(p[i], p[(i + 1) % (node - 1)]);
            this->__add_edge(p[i], p[node - 1]);
        }
    }
};

template<typename NodeType, typename EdgeType>
class _GridGraph : public _Graph<NodeType, EdgeType> {
protected:
    typedef _GridGraph<NodeType, EdgeType> _Self;
    _OUTPUT_FUNCTION(_Self)
    _DEF_GEN_FUNCTION 

    int _row, _column;
    std::vector<int> _p;
public:
    template<typename T = NodeType, typename U = EdgeType, _IsBothWeight<T, U> = 0>
    _GridGraph(
        int node_count = 1, int edge_count = 0, int begin_node = 1, int row = -1,
        NodeGenFunction nodes_weight_function = nullptr,
        EdgeGenFunction edges_weight_function = nullptr) :
        _Graph<NodeType, EdgeType>(
            node_count, edge_count, begin_node, 
            false, false, false, false, true, 
            nodes_weight_function, edges_weight_function),
        _row(row)
    {
        _output_function = default_function();
    }

    template<typename T = NodeType, typename U = EdgeType, _IsNodeWeight<T, U> = 0>
    _GridGraph(
        int node_count = 1, int edge_count = 0, int begin_node = 1, int row = -1,
        NodeGenFunction nodes_weight_function = nullptr) :
        _Graph<NodeType, void>(
            node_count, edge_count, begin_node, 
            false, false, false, false, true, 
            nodes_weight_function),
        _row(row)
    {
        _output_function = default_function();
    }

    template<typename T = NodeType, typename U = EdgeType, _IsEdgeWeight<T, U> = 0>
    _GridGraph(
        int node_count = 1, int edge_count = 0, int begin_node = 1, int row = -1,
        EdgeGenFunction edges_weight_function = nullptr) :
        _Graph<void, EdgeType>(
            node_count, edge_count, begin_node, 
            false, false, false, false, true, 
            edges_weight_function),
        _row(row)
    {
        _output_function = default_function();
    }

    template<typename T = NodeType, typename U = EdgeType, _IsUnweight<T, U> = 0>
    _GridGraph(
        int node_count = 1, int edge_count = 0, int begin_node = 1, int row = -1,
        EdgeGenFunction edges_weight_function = nullptr) :
        _Graph<void, void>(
            node_count, edge_count, begin_node, 
            false, false, false, false, true),
        _row(row)
    {
        _output_function = default_function();
    }

    void set_row(int row) {
        _row = row; 
        if (_row != 0) {
            _column = (this->_node_count + _row - 1) / _row;
        } 
    }

    void set_column(int column) {
        _column = column;
        if (_column != 0) {
            _row = (this->_node_count + _column - 1) / _column;
        }
    }

    void set_row_column(int row, int column, int ignore = 0) {
        long long node = (long long)row * (long long)column - (long long)ignore;
        if (ignore >= column) {
            __msg::__warn_msg(__msg::_err, "The ignored nodes is large than or equal to column number, will invalidate it.");
        }
        if (node > 100000000) {
            __msg::__warn_msg(
                __msg::_err, 
                "The number of nodes is %d * %d - %d = %lld, even greater than 10^8.",
                row,
                column,
                ignore,
                node);
        }
        _row = row;
        _column = column;
        if (this->_node_count != node) {
            this->_node_count = node;
            this->__init_node_indices();
        }                   
    }

    int row() const { return _row; }
    int& row_ref() { return _row; }

    int column() const { return _column; }
    int& column_ref() { return _column; }

    _DISABLE_SELF_LOOP
    _OTHER_OUTPUT_FUNCTION_SETTING(_Self)

protected:
    long long __count_edge_count(int row, int column) {
        long long xl = (long long) row;
        long long yl = (long long) column;
        long long sum = xl * (yl - 1) + yl * (xl - 1) - 2 * (xl * yl - this->_node_count);
        if (this->_direction) {
            sum *= 2;
        }
        return sum;
    }

    virtual void __judge_upper_limit() override {
        long long limit = 0;
        if (!this->_multiply_edge) {
            limit = __count_edge_count(_row, _column);
            if (this->_direction) {
                limit *= 2;
            }
            if (this->_edge_count > limit) {
                __msg::__warn_msg(
                    __msg::_err, 
                    "Number of edges count is %d, which is large than the maximum possible, use upper edges limit %d.",
                    this->_edge_count,
                    limit);
                this->_edge_count = limit;
            }
        }
        else {
            if (this->_node_count == 1 && this->_edge_count == 0) {
                __msg::__fail_msg(__msg::_err, "Number of edges count must equal to 0.");
            }
        }
    }

    virtual void __judge_self_limit() override{
        if (_row <= 0) {
            __msg::__fail_msg(__msg::_err, "Number of rows must greater than 0, but found %d.", _row);
        }
        if (_column <= 0) {
            __msg::__fail_msg(__msg::_err, "Number of columns must greater than 0, but found %d.", _column);
        }
    }

    void __rand_row() {
        if (!this->_multiply_edge) {
            std::pair<int, int> max = {0, 0};
            std::vector<int> possible;
            for (int i = 1; i <= this->_node_count; i++) {
                int x = i, y = (this->_node_count + i - 1) / i;
                long long w = __count_edge_count(x, y);
                if (this->_direction) {
                    w *= 2;
                }
                if (w > max.first) {
                    max = {w, i};
                }
                if (w >= this->_edge_count) {
                    possible.push_back(i);
                }
            }
            if (possible.size() == 0) {
                this->_edge_count = max.first;
                __msg::__warn_msg(
                    __msg::_err,
                    "number of edges is large than the maximum possible, use upper edges limit %d.",
                    this->_edge_count);
                _row = max.second;
            } else {
                _row = rnd.any(possible);
            }
        } else {
            _row = rnd.next(1, this->_node_count);
        }
    }

    virtual void __self_init() override{
        _p = rnd.perm(this->_node_count, 0);
        if (_row == -1) {
            __rand_row();
        }
        _column = (this->_node_count + _row - 1) / _row;
    }

    virtual void __generate_connect() override {
        for (int i = 0; i < _row; i++) {
            for (int j = 1; j < _column; j++) {
                int x = i * _column + j, y = x - 1;
                if (x >= this->_node_count) continue;
                this->__add_edge(_p[x], _p[y]);
            }
            int x = i * _column, y = (i + 1) * _column;
            if (x < this->_node_count && y < this->_node_count) {
                this->__add_edge(_p[x], _p[y]);
            }
        }
    }

    virtual _Edge<EdgeType> __rand_edge() override {
        int d[4][2] = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}};
        int pos, k, px, py, nxt;
        do {
            pos = rnd.next(this->_node_count);
            k = rnd.next(4);
            px = pos / _column + d[k][0];
            py = pos % _column + d[k][1];
            nxt = px * _column + py;
        } while (px < 0 || px >= _row || py < 0 || py >= _column || nxt >= this->_node_count ||
                    this->__judge_multiply_edge(_p[pos], _p[nxt]));
        return this->__convert_edge(_p[pos], _p[nxt]);
    }
};

template<typename NodeType, typename EdgeType>
class _PseudoTree : public _Graph<NodeType, EdgeType> {
protected:
    typedef _PseudoTree<NodeType, EdgeType> _Self;
    _OUTPUT_FUNCTION(_Self)
    _DEF_GEN_FUNCTION
    int _cycle;
    std::vector<int> _p;

public:
    template<typename T = NodeType, typename U = EdgeType, _IsBothWeight<T, U> = 0>
    _PseudoTree(
        int node_count = 1, int begin_node = 1, int cycle = -1,
        NodeGenFunction nodes_weight_function = nullptr,
        EdgeGenFunction edges_weight_function = nullptr) :
        _Graph<NodeType, EdgeType>(
            node_count, node_count, begin_node, 
            false, false, false, true, true, 
            nodes_weight_function, edges_weight_function),
        _cycle(cycle)
    {
        _output_function = default_function();
    }

    template<typename T = NodeType, typename U = EdgeType, _IsNodeWeight<T, U> = 0>
    _PseudoTree(
        int node_count = 1, int begin_node = 1, int cycle = -1,
        NodeGenFunction nodes_weight_function = nullptr) :
        _Graph<NodeType, void>(
            node_count, node_count, begin_node, 
            false, false, false, true, true, 
            nodes_weight_function),
        _cycle(cycle)
    {
        _output_function = default_function();
    }

    template<typename T = NodeType, typename U = EdgeType, _IsEdgeWeight<T, U> = 0>
    _PseudoTree(
        int node_count = 1, int begin_node = 1, int cycle = -1,
        EdgeGenFunction edges_weight_function = nullptr) :
        _Graph<void, EdgeType>(
            node_count, node_count, begin_node, 
            false, false, false, true, true, 
            edges_weight_function),
        _cycle(cycle)
    {
        _output_function = default_function();
    }

    template<typename T = NodeType, typename U = EdgeType, _IsUnweight<T, U> = 0>
    _PseudoTree(int node_count = 1, int begin_node = 1, int cycle = -1) :
        _Graph<void, void>(
            node_count, node_count, begin_node, 
            false, false, false, true, true),
        _cycle(cycle)
    {
        _output_function = default_function();
    }

    void set_cycle(int cycle) { _cycle = cycle; }
    int cycle() const { return _cycle; }
    int& cycle_ref() { return _cycle; }

    _DISABLE_EDGE_COUNT
    _DISABLE_CONNECT
    _DISABLE_DIRECTION
    _DISABLE_SELF_LOOP
    _DISABLE_MULTIPLY_EDGE

    _OTHER_OUTPUT_FUNCTION_SETTING(_Self)

protected :
    virtual void __self_init() override {
        this->_edge_count = this->_node_count;
        _p = rnd.perm(this->_node_count, 0);
        if (_cycle == -1) {
            _cycle = rnd.next(3, this->_node_count);
        }
    }

    virtual void __judge_self_limit() override {
        if (_cycle < 3 || _cycle > this->_node_count) {
            __msg::__fail_msg(__msg::_err, "cycle size must in range [3, %d], but found %d.", this->_node_count, _cycle);
        }
    }

    virtual void __judge_lower_limit() override {
        if (this->_node_count < 3) {
            __msg::__fail_msg(__msg::_err, "number of nodes in cycle graph must greater than or equal to 3, but found %d.", this->_node_count);
        }
    }

    template<typename T = NodeType, typename U = EdgeType, _IsBothWeight<T, U> = 0>
    _CycleGraph<NodeType, EdgeType> __get_cycle_graph() {
        _CycleGraph<NodeType, EdgeType> cycle(
            _cycle, 0, this->_nodes_weight_function, this->_edges_weight_function);
        return cycle;
    }

    template<typename T = NodeType, typename U = EdgeType, _IsNodeWeight<T, U> = 0>
    _CycleGraph<NodeType, void> __get_cycle_graph() {
        _CycleGraph<NodeType, void> cycle(_cycle, 0, this->_nodes_weight_function);
        return cycle;
    }

    template<typename T = NodeType, typename U = EdgeType, _IsEdgeWeight<T, U> = 0>
    _CycleGraph<void, EdgeType> __get_cycle_graph() {
        _CycleGraph<void, EdgeType> cycle(_cycle, 0, this->_edges_weight_function);
        return cycle;
    }

    template<typename T = NodeType, typename U = EdgeType, _IsUnweight<T, U> = 0>
    _CycleGraph<void, void> __get_cycle_graph() {
        _CycleGraph<void, void> cycle(_cycle, 0);
        return cycle;
    }

    void __generate_cycle() {
        _CycleGraph<NodeType, EdgeType> cycle = __get_cycle_graph();
        cycle.set_swap_node(this->_swap_node);
        cycle.gen(); 
        std::vector <_Edge<EdgeType>> edge = cycle.edges();
        for (_Edge<EdgeType>& e: edge) {
            int& u = e.u_ref();
            int& v = e.v_ref();
            u = _p[u];
            v = _p[v];
            this->__add_edge(e);
        }
    }

    virtual void __generate_other_edges() {
        for (int i = _cycle; i < this->_node_count; i++) {
            int f = rnd.next(i);
            this->__add_edge(_p[i], _p[f]);
        }
    }

    virtual void __generate_graph() override {
        __generate_cycle();
        __generate_other_edges();
    }       
protected:

    template<typename T = NodeType, typename U = EdgeType, _IsBothWeight<T, U> = 0>
    _PseudoTree(
        int node_count, int begin_node, int cycle, int direction,
        NodeGenFunction nodes_weight_function,
        EdgeGenFunction edges_weight_function) :
        _Graph<NodeType, EdgeType>(
            node_count, node_count, begin_node, 
            direction, false, false, true, direction ? false : true, 
            nodes_weight_function, edges_weight_function),
        _cycle(cycle)
    {
        _output_function = default_function();
    }

    template<typename T = NodeType, typename U = EdgeType, _IsNodeWeight<T, U> = 0>
    _PseudoTree(
        int node_count, int begin_node, int cycle, int direction,
        NodeGenFunction nodes_weight_function) :
        _Graph<NodeType, void>(
            node_count, node_count, begin_node, 
            direction, false, false, true, direction ? false : true, 
            nodes_weight_function),
        _cycle(cycle)
    {
        _output_function = default_function();
    }

    template<typename T = NodeType, typename U = EdgeType, _IsEdgeWeight<T, U> = 0>
    _PseudoTree(
        int node_count, int begin_node, int cycle, int direction,
        EdgeGenFunction edges_weight_function) :
        _Graph<void, EdgeType>(
            node_count, node_count, begin_node, 
            direction, false, false, true, direction ? false : true, 
            edges_weight_function),
        _cycle(cycle)
    {
        _output_function = default_function();
    }

    template<typename T = NodeType, typename U = EdgeType, _IsUnweight<T, U> = 0>
    _PseudoTree(int node_count, int begin_node, int cycle, int direction) :
        _Graph<void, void>(
            node_count, node_count, begin_node, 
            direction, false, false, true, direction ? false : true),
        _cycle(cycle)
    {
        _output_function = default_function();
    }
};

template<typename NodeType, typename EdgeType>
class _PseudoInTree : public _PseudoTree<NodeType, EdgeType> {
protected:
    typedef _PseudoInTree<NodeType, EdgeType> _Self;
    _OUTPUT_FUNCTION(_Self);
    _DEF_GEN_FUNCTION

public:
    template<typename T = NodeType, typename U = EdgeType, _IsBothWeight<T, U> = 0>
    _PseudoInTree(
        int node_count = 3, int begin_node = 1, int cycle = -1,
        NodeGenFunction nodes_weight_function = nullptr,
        EdgeGenFunction edges_weight_function = nullptr) :
        _PseudoTree<NodeType, EdgeType>(
            node_count, begin_node, cycle, true,
            nodes_weight_function, edges_weight_function)
    {
        _output_function = default_function();
    }

    template<typename T = NodeType, typename U = EdgeType, _IsNodeWeight<T, U> = 0>
    _PseudoInTree(
        int node_count = 3, int begin_node = 1, int cycle = -1,
        NodeGenFunction nodes_weight_function = nullptr) :
        _PseudoTree<NodeType, void>(
            node_count, begin_node, cycle, true,
            nodes_weight_function)
    {
        _output_function = default_function();
    }

    template<typename T = NodeType, typename U = EdgeType, _IsEdgeWeight<T, U> = 0>
    _PseudoInTree(
        int node_count = 3, int begin_node = 1, int cycle = -1,
        EdgeGenFunction edges_weight_function = nullptr) :
        _PseudoTree<void, EdgeType>(
            node_count, begin_node, cycle, true,
            edges_weight_function)
    {
        _output_function = default_function();
    }

    template<typename T = NodeType, typename U = EdgeType, _IsUnweight<T, U> = 0>
    _PseudoInTree(int node_count = 3, int begin_node = 1, int cycle = -1) :
        _PseudoTree<void, EdgeType>(node_count, begin_node, cycle, true)
    {
        _output_function = default_function();
    }

    _OTHER_OUTPUT_FUNCTION_SETTING(_Self)
};

template<typename NodeType, typename EdgeType>
class _PseudoOutTree : public _PseudoTree<NodeType, EdgeType> {
protected:
    typedef _PseudoOutTree<NodeType, EdgeType> _Self;
    _OUTPUT_FUNCTION(_Self);
    _DEF_GEN_FUNCTION

public:
    template<typename T = NodeType, typename U = EdgeType, _IsBothWeight<T, U> = 0>
    _PseudoOutTree(
        int node_count = 3, int begin_node = 1, int cycle = -1,
        NodeGenFunction nodes_weight_function = nullptr,
        EdgeGenFunction edges_weight_function = nullptr) :
        _PseudoTree<NodeType, EdgeType>(
            node_count, begin_node, cycle, true,
            nodes_weight_function, edges_weight_function)
    {
        _output_function = default_function();
    }

    template<typename T = NodeType, typename U = EdgeType, _IsNodeWeight<T, U> = 0>
    _PseudoOutTree(
        int node_count = 3, int begin_node = 1, int cycle = -1,
        NodeGenFunction nodes_weight_function = nullptr) :
        _PseudoTree<NodeType, void>(
            node_count, begin_node, cycle, true,
            nodes_weight_function)
    {
        _output_function = default_function();
    }

    template<typename T = NodeType, typename U = EdgeType, _IsEdgeWeight<T, U> = 0>
    _PseudoOutTree(
        int node_count = 3, int begin_node = 1, int cycle = -1,
        EdgeGenFunction edges_weight_function = nullptr) :
        _PseudoTree<void, EdgeType>(
            node_count, begin_node, cycle, true,
            edges_weight_function)
    {
        _output_function = default_function();
    }

    template<typename T = NodeType, typename U = EdgeType, _IsUnweight<T, U> = 0>
    _PseudoOutTree(int node_count = 3, int begin_node = 1, int cycle = -1) :
        _PseudoTree<void, EdgeType>(node_count, begin_node, cycle, true)
    {
        _output_function = default_function();
    }

    _OTHER_OUTPUT_FUNCTION_SETTING(_Self)
protected:

    virtual void __generate_other_edges() override {
        for (int i = this->_cycle; i < this->_node_count; i++) {
            int f = rnd.next(i);
            this->__add_edge(this->_p[f], this->_p[i]);
        }
    }
};

template<typename NodeType, typename EdgeType> 
class _Cactus : public _Graph<NodeType, EdgeType> {
protected:
    typedef _Cactus<NodeType, EdgeType> _Self;
    _OUTPUT_FUNCTION(_Self);
    _DEF_GEN_FUNCTION
    std::vector<int> _p;

public:
    template<typename T = NodeType, typename U = EdgeType, _IsBothWeight<T, U> = 0>
    _Cactus(
        int node_count = 1, int edge_count = 0, int begin_node = 1, 
        NodeGenFunction nodes_weight_function = nullptr,
        EdgeGenFunction edges_weight_function = nullptr) :
        _Graph<NodeType, EdgeType>(
            node_count, edge_count, begin_node,
            false, false, false, true, true,
            nodes_weight_function, edges_weight_function)
    {
        _output_function = default_function();
    }

    template<typename T = NodeType, typename U = EdgeType, _IsNodeWeight<T, U> = 0>
    _Cactus(
        int node_count = 1, int edge_count = 0, int begin_node = 1, 
        NodeGenFunction nodes_weight_function = nullptr) :
        _Graph<NodeType, void>(
            node_count, edge_count, begin_node,
            false, false, false, true, true,
            nodes_weight_function)
    {
        _output_function = default_function();
    }

    template<typename T = NodeType, typename U = EdgeType, _IsEdgeWeight<T, U> = 0>
    _Cactus(
        int node_count = 1, int edge_count = 0, int begin_node = 1,
        EdgeGenFunction edges_weight_function = nullptr) :
        _Graph<void, EdgeType>(
            node_count, edge_count, begin_node,
            false, false, false, true, true,
            edges_weight_function)
    {
        _output_function = default_function();
    }

    template<typename T = NodeType, typename U = EdgeType, _IsUnweight<T, U> = 0>
    _Cactus(int node_count = 1, int edge_count = 0, int begin_node = 1) :
        _Graph<void, EdgeType>(
            node_count, edge_count, begin_node,
            false, false, false, true, true)
    {
        _output_function = default_function();
    }

    _DISABLE_DIRECTION
    _DISABLE_CONNECT
    _DISABLE_SELF_LOOP
    _DISABLE_MULTIPLY_EDGE
    _OTHER_OUTPUT_FUNCTION_SETTING(_Self)

protected:
    virtual void __self_init() override{
        _p = rnd.perm(this->_node_count, 0);
    }

    virtual void __judge_upper_limit() override {
        int limit = this->_node_count - 1 + (this->_node_count - 1) / 2;
        if (this->_edge_count > limit) {
            __msg::__fail_msg(
                __msg::_err, 
                "number of edges must less than or equal to %d, but found %d.",
                limit, 
                this->_edge_count);
        }
    }

    template<typename T = NodeType, typename U = EdgeType, _IsBothWeight<T, U> = 0>
    _CycleGraph<NodeType, EdgeType> __get_cycle_graph(int size) {
        _CycleGraph<NodeType, EdgeType> cycle(
            size, 0, this->_nodes_weight_function, this->_edges_weight_function);
        return cycle;
    }

    template<typename T = NodeType, typename U = EdgeType, _IsNodeWeight<T, U> = 0>
    _CycleGraph<NodeType, void> __get_cycle_graph(int size) {
        _CycleGraph<NodeType, void> cycle(size, 0, this->_nodes_weight_function);
        return cycle;
    }

    template<typename T = NodeType, typename U = EdgeType, _IsEdgeWeight<T, U> = 0>
    _CycleGraph<void, EdgeType> __get_cycle_graph(int size) {
        _CycleGraph<void, EdgeType> cycle(size, 0, this->_edges_weight_function);
        return cycle;
    }

    template<typename T = NodeType, typename U = EdgeType, _IsUnweight<T, U> = 0>
    _CycleGraph<void, void> __get_cycle_graph(int size) {
        _CycleGraph<void, void> cycle(size, 0);
        return cycle;
    }

    virtual void __generate_graph() override {
        std::vector<std::vector<int>> cycles;
        int m = this->_edge_count - (this->_node_count - 1);
        for (int i = 2; i <= 2 * m; i += 2) {
            std::vector<int> pre;
            if (i == 2) {
                pre.emplace_back(0);
            }
            pre.emplace_back(i);
            pre.emplace_back(i - 1);
            cycles.emplace_back(pre);
        }
        int len = cycles.size();
        int add = len == 0 ? 0 : rnd.next(0, this->_node_count - (2 * m + 1));
        for (int i = 2 * m + 1; i <= 2 * m + add; i++) {
            int w = rnd.next(len);
            cycles[w].emplace_back(i);
        }
        for (int i = 2 * m + add + (len != 0); i < this->_node_count; i++) {
            cycles.emplace_back(1, i);
        }
        shuffle(cycles.begin() + 1, cycles.end());
        for(size_t i = 0; i < cycles.size(); i++) {
            std::vector<int> current = cycles[i];
            if (i != 0) {
                int w = rnd.next(i);
                current.push_back(rnd.any(cycles[w]));
            }
            if(current.size() == 1) {
                continue;
            }
            else if(current.size() == 2) {
                this->__add_edge(_p[current[0]], _p[current[1]]);
            }
            else {
                _CycleGraph<NodeType, EdgeType> cycle = __get_cycle_graph(current.size());
                cycle.gen();
                std::vector<_Edge<EdgeType>> edge = cycle.edges();
                for(_Edge<EdgeType>& e : edge) {
                    int& u = e.u_ref();
                    int& v = e.v_ref();
                    u = _p[current[u]];
                    v = _p[current[v]];
                    this->__add_edge(e);
                }
            }
        }
    }
};

enum class LinkType {
    Direct,
    Increase,
    Shuffle,           
    Dedupe
};

using MergeType = LinkType;

enum class TreeLinkType {
    Direct,
    Increase,
    Shuffle            
};

template<typename NodeType, typename EdgeType> 
class _LinkImpl {
private:
    _Graph<NodeType, EdgeType> _result;
    std::vector<_Edge<EdgeType>> _edges[2];
    std::vector<_Node<NodeType>> _nodes_weight[2];
    std::vector<int> _node_indices[2];
    LinkType _link_type;
    int _extra_edge_count;
    std::vector<int> _father;
    std::map<int, std::vector<int>> _connect_parts;
    std::map<std::pair<int, int>, int> _node_merge_map;
    int _node_count[2];

public:
    template<template<typename, typename> class TG1, template<typename, typename> class TG2>
    _LinkImpl(
        _Graph<NodeType, EdgeType> result_graph,
        TG1<NodeType, EdgeType> source1,
        TG2<NodeType, EdgeType> source2,
        int extra_edge_count,
        LinkType link_type) :
        _result(result_graph),
        _link_type(link_type),
        _extra_edge_count(extra_edge_count)
    {
        __init_result_graph();
        __dump_source_data(0, source1);
        __dump_source_data(1, source2);
    }
    
    template<template<typename, typename> class TG1, template<typename, typename> class TG2>
    _LinkImpl(
        TG1<NodeType, EdgeType> source1,
        TG2<NodeType, EdgeType> source2,
        int extra_edge_count,
        LinkType link_type) :
        _link_type(link_type),
        _extra_edge_count(extra_edge_count)
    {
        __init_result_graph(source1);
        __dump_source_data(0, source1);
        __dump_source_data(1, source2);
    }

    _Graph<NodeType, EdgeType> get_result() {
        __check_weight_functions();
        __merge_node_indices();
        __reset_node_count();
        __merge_nodes_weight();
        __merge_edges();
        __reset_edge_count();
        __divide_connection_part();
        __judge_limits();
        __generate_extra_edges();
        shuffle(_result._edges.begin(), _result._edges.end());
        return _result;
    }

private:
    
    void __init_result_graph() {
        _result._edge_count = 0;
        _result._node_count = 0;
        _result._edges.clear();
        _result._nodes_weight.clear();
        _result._e.clear();
        _result._node_indices.clear();
    }
    
    void __init_result_graph(_Graph<NodeType, EdgeType> graph) {
        _result = graph;
        __init_result_graph();
    }
    
    void __init_result_graph(_Tree<NodeType, EdgeType> tree) {
        _result = _Graph<NodeType, EdgeType>();
        __init_result_graph();
        _result._direction = tree._is_rooted;
        _result._swap_node = tree._swap_node;
        _result._begin_node = tree._begin_node;
        _result._nodes_weight_function = tree._nodes_weight_function;
        _result._edges_weight_function = tree._edges_weight_function;
    }

    template<template<typename, typename> class TG>
    void __dump_source_data(int index, TG<NodeType, EdgeType> source) {
        _edges[index] = source._edges;
        int node_count = source._node_count;
        _node_count[index] = node_count;
        if ((int)source._nodes_weight.size() != node_count && source._nodes_weight_function != nullptr) {
            __msg::__warn_msg(__msg::_err, "Found node weights size is not equal to node count, re-generate it.");
            source.__check_nodes_weight_function();
            source.__generate_nodes_weight();
        }
        _nodes_weight[index] = source._nodes_weight;
        if ((int)source._node_indices.size() != node_count) {
            __msg::__warn_msg(__msg::_err, "Found node indices size is not equal to node count, re-init it.");
            source.__init_node_indices();
        }
        _node_indices[index] = source._node_indices;           
    }

    void __check_weight_functions() {
        _result.__check_edges_weight_function();
        _result.__check_nodes_weight_function();
    }

    void __merge_node_indices() {
        _node_merge_map.clear();     
        if (_link_type == LinkType::Dedupe) {
            std::map<int, int> first_appear;
            int cnt = 0;
            for (int i = 0; i < 2; i++) {
                for (int j = 0; j < _node_count[i]; j++ ) {
                    int x = _node_indices[i][j];
                    if (first_appear.find(x) == first_appear.end()) {
                        _result._node_indices.emplace_back(x);
                        first_appear[x] = cnt;
                        cnt++;
                    }
                    _node_merge_map[std::make_pair(i, j)] = first_appear[x];
                }
            }
        }
        else {   
            if (_link_type == LinkType::Shuffle) {
                std::vector<int> p = rnd.perm(_node_count[0] + _node_count[1], 0);
                int cnt = 0;
                for (int i = 0; i < 2; i++) {
                    for (int j = 0; j < _node_count[i]; j++) {
                        _node_merge_map[std::make_pair(i, j)] = p[cnt];
                        cnt++;
                    }
                }
            }   
            else {
                for (int i = 0; i < _node_count[0]; i++) {
                    _node_merge_map[std::make_pair(0, i)] = i;
                }
                for (int i = 0; i < _node_count[1]; i++) {
                    _node_merge_map[std::make_pair(1, i)] = i + _node_count[0];
                }                            
            }                 
            
            if (_link_type == LinkType::Direct) {
                for (int i = 0; i < 2; i++) {
                    for (auto x : _node_indices[i]) {
                        _result._node_indices.emplace_back(x);
                    }
                }
            }
            else {
                for (int i = 0; i < _node_count[0] + _node_count[1]; i++) {
                    _result._node_indices.emplace_back(i + _result._begin_node);
                }
            }
        }                
    }
    
    void __reset_node_count() {
        _result._node_count = (int)_result._node_indices.size();
    }
    
    void __merge_nodes_weight() {
        if (_link_type == LinkType::Dedupe) {
            std::set<int> appear;
            _result._nodes_weight.resize(_result._node_count);
            for (auto it : _node_merge_map) {
                if (appear.find(it.second) == appear.end()) {
                    appear.insert(it.second);
                    _result._nodes_weight[it.second] = _nodes_weight[it.first.first][it.first.second];
                }
            }
        }
        else {
            for (int i = 0 ; i < 2; i++) {
                for (auto x : _nodes_weight[i]) {
                    _result._nodes_weight.emplace_back(x);
                }
            }
        }
    }
    
    void __reset_edge_count() {
        _result._edge_count = (int)_result._edges.size() + _extra_edge_count;
    }
    
    void __merge_edges() {
        int ignore_edges = 0;
        for (int i = 0; i < 2; i++) {
            int sz = _edges[i].size();
            for (int j = 0; j < sz; j++) {
                _Edge<EdgeType> edge = _edges[i][j];
                int& u = edge.u_ref();
                int& v = edge.v_ref();
                u = _node_merge_map[std::make_pair(i, u)];
                v = _node_merge_map[std::make_pair(i, v)];
                if (_result.__judge_multiply_edge(u, v) || _result.__judge_self_loop(u, v)) {
                    ignore_edges++;
                } 
                else {
                    _result.__add_edge(edge);
                }
            }
        }
        if (ignore_edges) {
            __msg::__warn_msg(__msg::_err, "Ignore %d edge(s) due to the graph's attribute-based conditions.", ignore_edges);
        }
    }
    
    int __find(int x) {
        if (_father[x] != x) {
            _father[x] = __find(_father[x]);
        }
        return _father[x];
    }
    
    void __divide_connection_part() {
        if (!_result._connect) { return; }
        _father.clear();
        _connect_parts.clear();
        for (int i = 0; i < _result._node_count; i++) {
            _father.emplace_back(i);
        }
        for (auto edge : _result._edges) {
            int u = edge.u();
            int v = edge.v();
            int t1 = __find(u);
            int t2 = __find(v);
            if (t1 != t2) {
                _father[t1] = t2;
            }
        }
        for (int i = 0; i < _result._node_count; i++) {
            _connect_parts[__find(i)].emplace_back(i);
        }
    }
    
    void __judge_limits() {
        if (_result._connect) {
            int need_edge_count = _connect_parts.size() - 1;
            if (_extra_edge_count < need_edge_count) {
                __msg::__fail_msg(
                    __msg::_err, 
                    "At least %d edges are needed to connect %d connected components, but found %d.",
                    need_edge_count,
                    _connect_parts.size(),
                    _extra_edge_count);
            }
        }
        _result.__judge_limits();
    }
    
    void __generate_connect_part() {
        std::vector<int> mark_indices;
        for (auto it : _connect_parts) {
            mark_indices.emplace_back(it.first);
        }
        _Tree<void, void> tree(_connect_parts.size(), 0);
        tree.gen();
        std::vector<_Edge<void>> edges = tree.edges();
        for (_Edge<void> edge : edges) {
            int u = rnd.any(_connect_parts[mark_indices[edge.u()]]);
            int v = rnd.any(_connect_parts[mark_indices[edge.v()]]);
            _result.__add_edge(u, v);
        }
    }
    
    void __generate_extra_edges() {
        int m = _extra_edge_count;
        if (_result._connect) {
            m -= _connect_parts.size() - 1;
            __generate_connect_part();
        }
        while (m--){
            _result.__add_edge(_result.__rand_edge());
        }         
    }
};

template<typename NodeType, typename EdgeType>
class _TreeLinkImpl {
private:
    _Tree<NodeType, EdgeType> _result, _source1, _source2;
    TreeLinkType _link_type;
    
public:
    _TreeLinkImpl(
        _Tree<NodeType, EdgeType> result,
        _Tree<NodeType, EdgeType> source1,
        _Tree<NodeType, EdgeType> source2,
        TreeLinkType link_type) :
        _result(result),
        _source1(source1),
        _source2(source2),
        _link_type(link_type)
    {
        __init_result_tree();
    }
    
    _TreeLinkImpl(
        _Tree<NodeType, EdgeType> source1,
        _Tree<NodeType, EdgeType> source2,
        TreeLinkType link_type) :
        _source1(source1),
        _source2(source2),
        _link_type(link_type)
    {
        __init_result_tree(source1);
    }
    
    _Tree<NodeType, EdgeType> get_result() {
        __link_tree();
        return _result;
    }
    
private:
    void __init_result_tree() {
        _result._node_count = 0;
        _result._edges.clear();
        _result._nodes_weight.clear();
        _result._node_indices.clear();
    }
    
    void __init_result_tree(_Tree<NodeType, EdgeType> tree) {
        _result = tree;
        __init_result_tree();
    }   
    
    LinkType __convert_to_link_type() {
        if (_link_type == TreeLinkType::Direct) {
            return LinkType::Direct;
        }
        else if(_link_type == TreeLinkType::Increase) {
            return LinkType::Increase;
        }
        else {
            return LinkType::Shuffle;
        }
    }
    
    template<typename T = NodeType, _HasT<T> = 0>
    void __reset_nodes_weight_function(_Graph<NodeType, EdgeType>& graph) {
        _result.__check_nodes_weight_function();
        auto func = _result.nodes_weight_function();
        graph.set_nodes_weight_function(func);
    }
    
    template<typename T = NodeType, _NotHasT<T> = 0>
    void __reset_nodes_weight_function(_Graph<NodeType, EdgeType>&) {
        return;
    }
    
    template<typename T = EdgeType, _HasT<T> = 0>
    void __reset_edges_weight_function(_Graph<NodeType, EdgeType>& graph) {
        _result.__check_edges_weight_function();
        auto func = _result.edges_weight_function();
        graph.set_edges_weight_function(func);
    }
    
    template<typename T = EdgeType, _NotHasT<T> = 0>
    void __reset_edges_weight_function(_Graph<NodeType, EdgeType>&) {
        return;
    }
    
    void __convert_graph_to_tree(_Graph<NodeType, EdgeType>& graph) {
        _result._node_count = graph._node_count;
        _result._edges = graph._edges;
        _result._nodes_weight = graph._nodes_weight;
        _result._node_indices = graph._node_indices;
        if (_result._is_rooted) {
            _result.__reroot();
        }
    }
    
    void __link_tree() {
        _Graph<NodeType, EdgeType> empty_graph;
        empty_graph.set_connect(true);
        empty_graph.set_swap_node(_result._swap_node);
        __reset_nodes_weight_function(empty_graph);
        __reset_edges_weight_function(empty_graph);
        _LinkImpl<NodeType, EdgeType> impl(empty_graph, _source1, _source2, 1, __convert_to_link_type());
        _Graph<NodeType, EdgeType> graph = impl.get_result();
        __convert_graph_to_tree(graph);
    } 
};

template<typename NodeType, typename EdgeType>
class _FlowerChain : public _Tree<NodeType, EdgeType> {
protected:
    typedef _FlowerChain<NodeType,EdgeType> _Self;
    _OUTPUT_FUNCTION(_Self)
    _DEF_GEN_FUNCTION
    int _flower_size, _chain_size;

public:
    template<typename T = NodeType, typename U = EdgeType, _IsBothWeight<T, U> = 0>
    _FlowerChain(
        int node = 1, int begin_node = 1, bool is_rooted = false, int root = 1, int flower_size = -1,
        NodeGenFunction nodes_weight_function = nullptr,
        EdgeGenFunction edges_weight_function = nullptr) :
        _Tree<NodeType, EdgeType>(
            node, begin_node, is_rooted, root, 
            nodes_weight_function, edges_weight_function),
        _flower_size(flower_size)
    {
        _output_function = this->default_function();
    }
    
    template<typename T = NodeType, typename U = EdgeType, _IsEdgeWeight<T, U> = 0>
    _FlowerChain(
        int node = 1, int begin_node = 1, bool is_rooted = false, int root = 1, int flower_size = -1,
        EdgeGenFunction edges_weight_function = nullptr) :
        _Tree<void, EdgeType>(node, begin_node, is_rooted, root, edges_weight_function),
        _flower_size(flower_size)
    {
        _output_function = this->default_function();
    }
    
    template<typename T = NodeType, typename U = EdgeType, _IsNodeWeight<T, U> = 0>
    _FlowerChain(
        int node = 1, int begin_node = 1, bool is_rooted = false, int root = 1, int flower_size = -1,
        NodeGenFunction nodes_weight_function = nullptr) :
        _Tree<NodeType, void>(node, begin_node, is_rooted, root, nodes_weight_function),
        _flower_size(flower_size)
    {
        _output_function = this->default_function();
    }
    
    template<typename T = NodeType, typename U = EdgeType, _IsUnweight<T, U> = 0>
    _FlowerChain(
        int node = 1, int begin_node = 1, bool is_rooted = false, int root = 1, int flower_size = -1) :
        _Tree<void, void>(node, begin_node, is_rooted, root),
        _flower_size(flower_size)
    {
        _output_function = this->default_function();
    }
    
    void set_flower_size(int flower_size) {
        _flower_size = flower_size;
        _chain_size = this->_node_count - flower_size;
    }
    
    void set_chain_size(int chain_size) {
        _chain_size = chain_size;
        _flower_size = this->_node_count - _chain_size;
    }
    
    void set_flower_chain_size(int flower_size, int chain_size) {
        _flower_size = flower_size;
        _chain_size = chain_size;
        this->_node_count = flower_size + chain_size;
        this->__init_node_indices();
    } 
    
    int flower_size() const { return _flower_size; }
    int& flower_size_ref() { return _flower_size; }
    
    int chain_size() const { return _chain_size; }
    int& chain_size_ref() { return _chain_size; }
    
    _OTHER_OUTPUT_FUNCTION_SETTING(_Self)
    _DISABLE_CHOOSE_GEN
protected:
    
    template<typename T = EdgeType, _HasT<T> = 0>
    void __reset_edges_weight_function(_Tree<void, EdgeType>& tree) {
        auto func = this->edges_weight_function();
        tree.set_edges_weight_function(func);
    }
    
    template<typename T = EdgeType, _NotHasT<T> = 0>
    void __reset_edges_weight_function(_Tree<void, EdgeType>&) {
        return;
    }
    
    virtual void __self_init() override {
        if (_flower_size == -1) {
            _flower_size = rnd.next(0, this->_node_count);
        }
        _chain_size = this->_node_count - _flower_size;
    }
    
    virtual void __judge_self_limit() override {
        if (_flower_size < 0) {
            __msg::__fail_msg(__msg::_err, "Flower size must greater than or equal to 0, but found %d.", _flower_size);
        }
        if (_chain_size < 0) {
            __msg::__fail_msg(__msg::_err, "Chain size must greater than or equal to 0, but found %d.", _chain_size);
        }
    }
    
    void __dump_result(_Tree<void, EdgeType>& tree) {
        this->_edges = tree.edges_ref();
        this->_node_indices = tree.node_indices_ref();
    }
    
    virtual void __generate_tree() override {                   
        _Flower<void, EdgeType> _flower;
        _Chain<void, EdgeType> _chain;
        _flower.set_node_count(_flower_size);
        __reset_edges_weight_function(_flower);
        _chain.set_node_count(_chain_size);
        __reset_edges_weight_function(_chain);               
        if (_flower_size != 0) {
            _flower.gen();
        }
        if (_chain_size != 0) {
            _chain.gen();
        }

        if (_flower_size == 0) {
            __dump_result(_chain);
        }
        else if (_chain_size == 0) {
            __dump_result(_flower);
        }
        else {
            _TreeLinkImpl<void, EdgeType> impl(_flower, _chain, TreeLinkType::Shuffle);
            auto res = impl.get_result();     
            __dump_result(res);                        
        }

        if (this->_is_rooted) {
            this->__reroot();
        }       
    }
        
};

template<typename NodeType,typename EdgeType>
class _Forest : public _Graph<NodeType, EdgeType> {
protected:
    typedef _Forest<NodeType,EdgeType> _Self;
    _OUTPUT_FUNCTION(_Self)
    _DEF_GEN_FUNCTION
    std::vector<int> _trees_size;

public:

    template<typename T = NodeType, typename U = EdgeType, _IsBothWeight<T, U> = 0>
    _Forest(
        int node_count = 1, int edge_count = 0, int begin_node = 1, 
        NodeGenFunction nodes_weight_function = nullptr,
        EdgeGenFunction edges_weight_function = nullptr) :
        _Graph<NodeType, EdgeType>(
            node_count, edge_count, begin_node,
            false, false, false, edge_count == node_count - 1, true,
            nodes_weight_function, edges_weight_function)
    {
        _output_function = default_function();
    }

    template<typename T = NodeType, typename U = EdgeType, _IsNodeWeight<T, U> = 0>
    _Forest(
        int node_count = 1, int edge_count = 0, int begin_node = 1, 
        NodeGenFunction nodes_weight_function = nullptr) :
        _Graph<NodeType, void>(
            node_count, edge_count, begin_node,
            false, false, false, edge_count == node_count - 1, true,
            nodes_weight_function)
    {
        _output_function = default_function();
    }

    template<typename T = NodeType, typename U = EdgeType, _IsEdgeWeight<T, U> = 0>
    _Forest(
        int node_count = 1, int edge_count = 0, int begin_node = 1, 
        EdgeGenFunction edges_weight_function = nullptr) :
        _Graph<void, EdgeType>(
            node_count, edge_count, begin_node,
            false, false, false, edge_count == node_count - 1, true,
            edges_weight_function)
    {
        _output_function = default_function();
    }

    template<typename T = NodeType, typename U = EdgeType, _IsUnweight<T, U> = 0>
    _Forest(int node_count = 1, int edge_count = 0, int begin_node = 1) :
        _Graph<void, void>(
            node_count, edge_count, begin_node,
            false, false, false, edge_count == node_count - 1, true)
    {
        _output_function = default_function();
    }
    
    std::vector<int> trees_size() const { return _trees_size; }
    std::vector<int>& tree_size_ref() { return _trees_size; }
    
    void add_tree_size(int tree_size) {
        if (tree_size > 0) {
            _trees_size.emplace_back(tree_size);
        }
        else {
            __msg::__warn_msg(__msg::_err, "Tree size must greater than 0, but found %d.", tree_size);
        }
    }
    void set_trees_size(std::vector<int> trees_size) {
        _trees_size.clear();
        for (int tree_size : trees_size) {
            add_tree_size(tree_size);
        }
        __reset_node_edge_count();
    }

    _DISABLE_MULTIPLY_EDGE
    _DISABLE_SELF_LOOP
    _DISABLE_CONNECT
    _OTHER_OUTPUT_FUNCTION_SETTING(_Self)

protected:
    
    virtual void __judge_upper_limit() override {
        if (this->_edge_count > this->_node_count - 1) {
            __msg::__fail_msg(__msg::_err, "number of edges must less than %d.", this->_node_count - 1);
        }
    }
    
    virtual void __self_init() override {
        this->_connect = (this->_edge_count == this->_node_count - 1);
    }
    
    void __reset_node_edge_count() {
        int count = 0;
        for (int tree_size : _trees_size) {
            count += tree_size;
        }
        if (count != this->_node_count) {
            __msg::__info_msg(
                __msg::_err, 
                "Node count will be changed because the sum of Trees' size %d is not equal to node count %d.",
                count,
                this->_node_count);
            this->set_node_count(count);
        }
        if (count - (int)_trees_size.size() != this->_edge_count) {
            __msg::__info_msg(
                __msg::_err, 
                "Edge count will be changed because the sum of Trees' edges %d is not equal to edge count %d.",
                count - _trees_size.size(),
                this->_edge_count);
            this->set_edge_count(count - _trees_size.size());                       
        }
    }
    
    void __generate_trees_size() {
        int tree_count = this->_node_count - this->_edge_count;
        _trees_size = rand_vector::rand_sum(tree_count, this->_node_count, 1);
    }
    
    void __dump_result(_Graph<void, EdgeType>& graph) {
        this->_edges = graph.edges_ref();
        this->_node_indices = graph.node_indices_ref();
    }
    
    void __reset_connect() {
        this->_connect = this->_node_count == this->_edge_count - 1;
    }
    
    virtual void __generate_graph() override {
        if (_trees_size.empty()) {
            __generate_trees_size();
        }
        else {
            __reset_node_edge_count();
        }
        __reset_connect();
        _Graph<void, EdgeType> result_graph(0);
        __reset_edges_weight_function(result_graph);
        for (int tree_size : _trees_size) {
            _Tree<void, EdgeType> tree(tree_size);
            __reset_edges_weight_function(tree);
            tree.gen();
            _LinkImpl<void, EdgeType> impl(result_graph, tree, 0, LinkType::Shuffle);
            result_graph = impl.get_result();
        }
        __dump_result(result_graph);
    }
    
    template<typename T = EdgeType, _HasT<T> = 0>
    void __reset_edges_weight_function(_Tree<void, EdgeType>& tree) {
        auto func = this->edges_weight_function();
        tree.set_edges_weight_function(func);
    }
    
    template<typename T = EdgeType, _NotHasT<T> = 0>
    void __reset_edges_weight_function(_Tree<void, EdgeType>&) {
        return;
    }
    
    template<typename T = EdgeType, _HasT<T> = 0>
    void __reset_edges_weight_function(_Graph<void, EdgeType>& graph) {
        auto func = this->edges_weight_function();
        graph.set_edges_weight_function(func);
    }
    
    template<typename T = EdgeType, _NotHasT<T> = 0>
    void __reset_edges_weight_function(_Graph<void, EdgeType>&) {
        return;
    }
};

template <typename U>
struct IsTreeOrGraph {
    template <typename V>
    static constexpr auto check(V *)
    -> decltype(std::declval<V>().edges(), std::true_type());

    template <typename V>
    static constexpr std::false_type check(...);

    static constexpr bool value =
            decltype(check<U>(nullptr))::value;
};

// merge 2 tree or graph to a graph
template<
    template<typename, typename> class TreeOrGraph1, 
    template<typename, typename> class TreeOrGraph2, 
    typename NodeType, 
    typename EdgeType>
    typename std::enable_if<
        IsTreeOrGraph<TreeOrGraph1<NodeType, EdgeType>>::value && 
        IsTreeOrGraph<TreeOrGraph2<NodeType, EdgeType>>::value,
_Graph<NodeType, EdgeType>>::type
__merge(
    _Graph<NodeType, EdgeType> setting_graph,
    TreeOrGraph1<NodeType, EdgeType> graph1,
    TreeOrGraph2<NodeType, EdgeType> graph2,
    MergeType merge_type) {
    _LinkImpl<NodeType, EdgeType> impl(setting_graph, graph1, graph2, 0, merge_type);
    return impl.get_result();
}

template<
    template<typename, typename> class TreeOrGraph1, 
    template<typename, typename> class TreeOrGraph2, 
    typename NodeType, 
    typename EdgeType>
    typename std::enable_if<
        IsTreeOrGraph<TreeOrGraph1<NodeType, EdgeType>>::value && 
        IsTreeOrGraph<TreeOrGraph2<NodeType, EdgeType>>::value,
_Graph<NodeType, EdgeType>>::type
__merge(
    TreeOrGraph1<NodeType, EdgeType> graph1,
    TreeOrGraph2<NodeType, EdgeType> graph2,
    MergeType merge_type)
{
    _LinkImpl<NodeType, EdgeType> impl(graph1, graph2, 0, merge_type);
    return impl.get_result();
}

// link 2 tree or graph to a graph, add extra edges
template<
    template<typename, typename> class TreeOrGraph1, 
    template<typename, typename> class TreeOrGraph2, 
    typename NodeType, 
    typename EdgeType>
    typename std::enable_if<
        IsTreeOrGraph<TreeOrGraph1<NodeType, EdgeType>>::value && 
        IsTreeOrGraph<TreeOrGraph2<NodeType, EdgeType>>::value,
_Graph<NodeType, EdgeType>>::type
__link(
    _Graph<NodeType, EdgeType> setting_graph,
    TreeOrGraph1<NodeType, EdgeType>  graph1,
    TreeOrGraph2<NodeType, EdgeType>  graph2,
    int extra_edges,
    LinkType link_type) {
    _LinkImpl<NodeType, EdgeType> impl(setting_graph, graph1, graph2, extra_edges, link_type);
    return impl.get_result();
}

template<
    template<typename, typename> class TreeOrGraph1, 
    template<typename, typename> class TreeOrGraph2, 
    typename NodeType, 
    typename EdgeType>
    typename std::enable_if<
        IsTreeOrGraph<TreeOrGraph1<NodeType, EdgeType>>::value && 
        IsTreeOrGraph<TreeOrGraph2<NodeType, EdgeType>>::value,
_Graph<NodeType, EdgeType>>::type
__link(
    TreeOrGraph1<NodeType, EdgeType>  graph1,
    TreeOrGraph2<NodeType, EdgeType>  graph2,
    int extra_edges,
    LinkType link_type) {
    _LinkImpl<NodeType, EdgeType> impl(graph1, graph2, extra_edges, link_type);
    return impl.get_result();
}

// special for 2 tree link to a tree

template<typename NodeType, typename EdgeType> 
_Tree<NodeType, EdgeType> __link(
    _Tree<NodeType, EdgeType> setting_tree,
    _Tree<NodeType, EdgeType> tree1,
    _Tree<NodeType, EdgeType> tree2,
    TreeLinkType link_type) {
    _TreeLinkImpl<NodeType, EdgeType> impl(setting_tree, tree1, tree2, link_type);
    return impl.get_result();
}

template<typename NodeType, typename EdgeType> 
_Tree<NodeType, EdgeType> __link(
    _Tree<NodeType, EdgeType> tree1,
    _Tree<NodeType, EdgeType> tree2,
    TreeLinkType link_type) {
    _TreeLinkImpl<NodeType, EdgeType> impl(tree1, tree2, link_type);
    return impl.get_result();
}

#undef _OTHER_OUTPUT_FUNCTION_SETTING
#undef _OUTPUT_FUNCTION
#undef _DEF_GEN_FUNCTION
#undef _DISABLE_CHOOSE_GEN
#undef _MUST_IS_ROOTED
#undef _DISABLE_EDGE_COUNT
#undef _DISABLE_DIRECTION
#undef _DISABLE_MULTIPLY_EDGE
#undef _DISABLE_SELF_LOOP
#undef _DISABLE_CONNECT
}

namespace unweight {
    using Edge = basic::_Edge<void>;
    using NodeWeight = basic::_Node<void>;
    using TreeGenerator = basic::TreeGenerator;

    using Tree = basic::_Tree<void, void>;
    using Chain = basic::_Chain<void, void>;
    using Flower = basic::_Flower<void, void>;
    using HeightTree = basic::_HeightTree<void, void>;
    using DegreeTree = basic::_DegreeTree<void, void>;
    using SonTree = basic::_SonTree<void, void>;
    using Graph = basic::_Graph<void, void>;
    using BipartiteGraph = basic::_BipartiteGraph<void, void>;
    using DAG = basic::_DAG<void, void>;
    using CycleGraph = basic::_CycleGraph<void, void>;
    using WheelGraph = basic::_WheelGraph<void, void>;
    using GridGraph = basic::_GridGraph<void, void>;
    using PseudoTree = basic::_PseudoTree<void, void>;
    using PseudoInTree = basic::_PseudoInTree<void, void>;
    using PseudoOutTree = basic::_PseudoOutTree<void, void>;
    using Cactus = basic::_Cactus<void, void>;
    using FlowerChain = basic::_FlowerChain<void, void>;
    using Forest = basic::_Forest<void, void>;

    using LinkType = basic::LinkType;
    using MergeType = basic::MergeType;
    using TreeLinkType = basic::TreeLinkType;
                
    template<typename TreeOrGraph1, typename TreeOrGraph2>
    Graph merge(Graph setting_graph, TreeOrGraph1 source1, TreeOrGraph2 source2, MergeType merge_type = MergeType::Shuffle) {
        return basic::__merge(setting_graph, source1, source2, merge_type);
    } 
    
    template<typename TreeOrGraph1, typename TreeOrGraph2>
    Graph merge(TreeOrGraph1 source1, TreeOrGraph2 source2, MergeType merge_type = MergeType::Shuffle) {
        return basic::__merge(source1, source2, merge_type);
    } 

    template<typename TreeOrGraph1, typename TreeOrGraph2>
    Graph link(Graph setting_graph, TreeOrGraph1 source1, TreeOrGraph2 source2, int extra_edges, LinkType link_type = LinkType::Shuffle) {
        return basic::__link(setting_graph, source1, source2, extra_edges, link_type);
    } 
    
    template<typename TreeOrGraph1, typename TreeOrGraph2>
    Graph link(TreeOrGraph1 source1, TreeOrGraph2 source2, int extra_edges, LinkType link_type = LinkType::Shuffle) {
        return basic::__link(source1, source2, extra_edges, link_type);
    } 

    Tree link(Tree setting_tree, Tree source1, Tree source2, TreeLinkType link_type = TreeLinkType::Shuffle) {
        return basic::__link(setting_tree, source1, source2, link_type);
    }
    Tree link(Tree source1, Tree source2, TreeLinkType link_type = TreeLinkType::Shuffle) {
        return basic::__link(source1, source2, link_type);
    }
}

namespace edge_weight{
    template<typename EdgeType> using Edge = basic::_Edge<EdgeType>;
    using NodeWeight = basic::_Node<void>;
    using TreeGenerator = basic::TreeGenerator;

    template<typename EdgeType> using Tree = basic::_Tree<void, EdgeType>;
    template<typename EdgeType> using Chain = basic::_Chain<void, EdgeType>;
    template<typename EdgeType> using Flower = basic::_Flower<void, EdgeType>;
    template<typename EdgeType> using HeightTree = basic::_HeightTree<void, EdgeType>;
    template<typename EdgeType> using DegreeTree = basic::_DegreeTree<void, EdgeType>;
    template<typename EdgeType> using SonTree = basic::_SonTree<void, EdgeType>;
    template<typename EdgeType> using Graph = basic::_Graph<void, EdgeType>;
    template<typename EdgeType> using BipartiteGraph = basic::_BipartiteGraph<void, EdgeType>;
    template<typename EdgeType> using DAG = basic::_DAG<void, EdgeType>;
    template<typename EdgeType> using CycleGraph = basic::_CycleGraph<void, EdgeType>;
    template<typename EdgeType> using WheelGraph = basic::_WheelGraph<void, EdgeType>;
    template<typename EdgeType> using GridGraph = basic::_GridGraph<void, EdgeType>;
    template<typename EdgeType> using PseudoTree = basic::_PseudoTree<void, EdgeType>;
    template<typename EdgeType> using PseudoInTree = basic::_PseudoInTree<void, EdgeType>;
    template<typename EdgeType> using PseudoOutTree = basic::_PseudoOutTree<void, EdgeType>;
    template<typename EdgeType> using Cactus = basic::_Cactus<void, EdgeType>;
    template<typename EdgeType> using FlowerChain = basic::_FlowerChain<void, EdgeType>;
    template<typename EdgeType> using Forest = basic::_Forest<void, EdgeType>;
    using LinkType = basic::LinkType;
    using MergeType = basic::MergeType;
    using TreeLinkType = basic::TreeLinkType;
    
    template<
        template<typename, typename> class TreeOrGraph1, 
        template<typename, typename> class TreeOrGraph2, 
        typename EdgeType>
    Graph<EdgeType> merge(
        Graph<EdgeType> setting_graph, 
        TreeOrGraph1<void, EdgeType> source1, 
        TreeOrGraph2<void, EdgeType> source2, 
        MergeType merge_type = MergeType::Shuffle) {
        return basic::__merge(setting_graph, source1, source2, merge_type);
    } 

    template<
        template<typename, typename> class TreeOrGraph1, 
        template<typename, typename> class TreeOrGraph2, 
        typename EdgeType>
    Graph<EdgeType> merge(
        TreeOrGraph1<void, EdgeType> source1, 
        TreeOrGraph2<void, EdgeType> source2, 
        MergeType merge_type = MergeType::Shuffle) {
        return basic::__merge(source1, source2, merge_type);
    } 

    template<
        template<typename, typename> class TreeOrGraph1, 
        template<typename, typename> class TreeOrGraph2, 
        typename EdgeType>
    Graph<EdgeType> link(
        Graph<EdgeType> setting_graph, 
        TreeOrGraph1<void, EdgeType> source1, 
        TreeOrGraph2<void, EdgeType> source2, 
        int extra_edges, 
        LinkType link_type = LinkType::Shuffle) {
        return basic::__link(setting_graph, source1, source2, extra_edges, link_type);
    } 

    template<
        template<typename, typename> class TreeOrGraph1, 
        template<typename, typename> class TreeOrGraph2, 
        typename EdgeType>
    Graph<EdgeType> link(
        TreeOrGraph1<void, EdgeType> source1,
        TreeOrGraph2<void, EdgeType> source2, 
        int extra_edges, 
        LinkType link_type = LinkType::Shuffle) {
        return basic::__link(source1, source2, extra_edges, link_type);
    } 

    template<typename EdgeType>
    Tree<EdgeType> link(
        Tree<EdgeType> setting_tree, 
        Tree<EdgeType> source1, 
        Tree<EdgeType> source2, 
        TreeLinkType link_type = TreeLinkType::Shuffle) {
        return basic::__link(setting_tree, source1, source2, link_type);
    }

    template<typename EdgeType>
    Tree<EdgeType> link(
        Tree<EdgeType> source1, 
        Tree<EdgeType> source2, 
        TreeLinkType link_type = TreeLinkType::Shuffle) {
        return basic::__link(source1, source2, link_type);
    }           
}

namespace node_weight{
    using Edge = basic::_Edge<void>;
    template<typename NodeType> using NodeWeight = basic::_Node<NodeType>;
    using TreeGenerator = basic::TreeGenerator;
    
    template<typename NodeType> using Tree = basic::_Tree<NodeType, void>;
    template<typename NodeType> using Chain = basic::_Chain<NodeType, void>;
    template<typename NodeType> using Flower = basic::_Flower<NodeType, void>;
    template<typename NodeType> using HeightTree = basic::_HeightTree<NodeType, void>;
    template<typename NodeType> using DegreeTree = basic::_DegreeTree<NodeType, void>;
    template<typename NodeType> using SonTree = basic::_SonTree<NodeType, void>;
    template<typename NodeType> using Graph = basic::_Graph<NodeType, void>;
    template<typename NodeType> using BipartiteGraph = basic::_BipartiteGraph<NodeType, void>;
    template<typename NodeType> using DAG = basic::_DAG<NodeType, void>;
    template<typename NodeType> using CycleGraph = basic::_CycleGraph<NodeType, void>;
    template<typename NodeType> using WheelGraph = basic::_WheelGraph<NodeType, void>;
    template<typename NodeType> using GridGraph = basic::_GridGraph<NodeType, void>;
    template<typename NodeType> using PseudoTree = basic::_PseudoTree<NodeType, void>;
    template<typename NodeType> using PseudoInTree = basic::_PseudoInTree<NodeType, void>;
    template<typename NodeType> using PseudoOutTree = basic::_PseudoOutTree<NodeType, void>;
    template<typename NodeType> using Cactus = basic::_Cactus<NodeType, void>;
    template<typename NodeType> using FlowerChain = basic::_FlowerChain<NodeType, void>;
    template<typename NodeType> using Forest = basic::_Forest<NodeType, void>;

    using LinkType = basic::LinkType;
    using MergeType = basic::MergeType;
    using TreeLinkType = basic::TreeLinkType;
    
    template<
        template<typename, typename> class TreeOrGraph1, 
        template<typename, typename> class TreeOrGraph2, 
        typename NodeType>
    Graph<NodeType> merge(
        Graph<NodeType> setting_graph, 
        TreeOrGraph1<NodeType, void> source1, 
        TreeOrGraph2<NodeType, void> source2, 
        MergeType merge_type = MergeType::Shuffle) {
        return basic::__merge(setting_graph, source1, source2, merge_type);
    } 

    template<
        template<typename, typename> class TreeOrGraph1, 
        template<typename, typename> class TreeOrGraph2, 
        typename NodeType>
    Graph<NodeType> merge(
        TreeOrGraph1<NodeType, void> source1, 
        TreeOrGraph2<NodeType, void> source2, 
        MergeType merge_type = MergeType::Shuffle) {
        return basic::__merge(source1, source2, merge_type);
    } 

    template<
        template<typename, typename> class TreeOrGraph1, 
        template<typename, typename> class TreeOrGraph2, 
        typename NodeType>
    Graph<NodeType> link(
        Graph<NodeType> setting_graph, 
        TreeOrGraph1<NodeType, void> source1, 
        TreeOrGraph2<NodeType, void> source2, 
        int extra_edges, 
        LinkType link_type = LinkType::Shuffle) {
        return basic::__link(setting_graph, source1, source2, extra_edges, link_type);
    } 

    template<
        template<typename, typename> class TreeOrGraph1, 
        template<typename, typename> class TreeOrGraph2, 
        typename NodeType>
    Graph<NodeType> link(
        TreeOrGraph1<NodeType, void> source1, 
        TreeOrGraph2<NodeType, void> source2, 
        int extra_edges, 
        LinkType link_type = LinkType::Shuffle) {
        return basic::__link(source1, source2, extra_edges, link_type);
    } 

    template<typename NodeType>
    Tree<NodeType> link(
        Tree<NodeType> setting_tree, 
        Tree<NodeType> source1, 
        Tree<NodeType> source2, 
        TreeLinkType link_type = TreeLinkType::Shuffle) {
        return basic::__link(setting_tree, source1, source2, link_type);
    }

    template<typename NodeType>
    Tree<NodeType> link(
        Tree<NodeType> source1, 
        Tree<NodeType> source2, 
        TreeLinkType link_type = TreeLinkType::Shuffle) {
        return basic::__link(source1, source2, link_type);
    }           
}

namespace both_weight{
    template<typename EdgeType> using Edge = basic::_Edge<EdgeType>;
    template<typename NodeType> using NodeWeight = basic::_Node<NodeType>;
    using TreeGenerator = basic::TreeGenerator;
    
    template<typename NodeType, typename EdgeType> using Tree = basic::_Tree<NodeType, EdgeType>;
    template<typename NodeType, typename EdgeType> using Chain = basic::_Chain<NodeType, EdgeType>;
    template<typename NodeType, typename EdgeType> using Flower = basic::_Flower<NodeType, EdgeType>;
    template<typename NodeType, typename EdgeType> using HeightTree = basic::_HeightTree<NodeType, EdgeType>;
    template<typename NodeType, typename EdgeType> using DegreeTree = basic::_DegreeTree<NodeType, EdgeType>;
    template<typename NodeType, typename EdgeType> using SonTree = basic::_SonTree<NodeType, EdgeType>;
    template<typename NodeType, typename EdgeType> using Graph = basic::_Graph<NodeType, EdgeType>;
    template<typename NodeType, typename EdgeType> using BipartiteGraph = basic::_BipartiteGraph<NodeType, EdgeType>;
    template<typename NodeType, typename EdgeType> using DAG = basic::_DAG<NodeType, EdgeType>;
    template<typename NodeType, typename EdgeType> using CycleGraph = basic::_CycleGraph<NodeType, EdgeType>;
    template<typename NodeType, typename EdgeType> using WheelGraph = basic::_WheelGraph<NodeType, EdgeType>;
    template<typename NodeType, typename EdgeType> using GridGraph = basic::_GridGraph<NodeType, EdgeType>;
    template<typename NodeType, typename EdgeType> using PseudoTree = basic::_PseudoTree<NodeType, EdgeType>;
    template<typename NodeType, typename EdgeType> using PseudoInTree = basic::_PseudoInTree<NodeType, EdgeType>;
    template<typename NodeType, typename EdgeType> using PseudoOutTree = basic::_PseudoOutTree<NodeType, EdgeType>;
    template<typename NodeType, typename EdgeType> using Cactus = basic::_Cactus<NodeType, EdgeType>;
    template<typename NodeType, typename EdgeType> using FlowerChain = basic::_FlowerChain<NodeType, EdgeType>;
    template<typename NodeType, typename EdgeType> using Forest = basic::_Forest<NodeType, EdgeType>;

    using LinkType = basic::LinkType;
    using MergeType = basic::MergeType;
    using TreeLinkType = basic::TreeLinkType;
    
    template<
        template<typename, typename> class TreeOrGraph1, 
        template<typename, typename> class TreeOrGraph2, 
        typename NodeType, 
        typename EdgeType>
    Graph<NodeType, EdgeType> merge(
        Graph<NodeType, EdgeType> setting_graph, 
        TreeOrGraph1<NodeType, EdgeType> source1, 
        TreeOrGraph2<NodeType, EdgeType> source2, 
        MergeType merge_type = MergeType::Shuffle) {
        return basic::__merge(setting_graph, source1, source2, merge_type);
    } 

    template<
        template<typename, typename> class TreeOrGraph1, 
        template<typename, typename> class TreeOrGraph2, 
        typename NodeType, 
        typename EdgeType>
    Graph<NodeType, EdgeType> merge(
        TreeOrGraph1<NodeType, EdgeType> source1, 
        TreeOrGraph2<NodeType, EdgeType> source2, 
        MergeType merge_type = MergeType::Shuffle) {
        return basic::__merge(source1, source2, merge_type);
    } 

    template<
        template<typename, typename> class TreeOrGraph1, 
        template<typename, typename> class TreeOrGraph2, 
        typename NodeType, 
        typename EdgeType>
    Graph<NodeType, EdgeType> link(
        Graph<NodeType, EdgeType> setting_graph, 
        TreeOrGraph1<NodeType, EdgeType> source1, 
        TreeOrGraph2<NodeType, EdgeType> source2, 
        int extra_edges, 
        LinkType link_type = LinkType::Shuffle) {
        return basic::__link(setting_graph, source1, source2, extra_edges, link_type);
    } 

    template<
        template<typename, typename> class TreeOrGraph1, 
        template<typename, typename> class TreeOrGraph2, 
        typename NodeType, 
        typename EdgeType>
    Graph<NodeType, EdgeType> link(
        TreeOrGraph1<NodeType, EdgeType> source1, 
        TreeOrGraph2<NodeType, EdgeType> source2, 
        int extra_edges, 
        LinkType link_type = LinkType::Shuffle) {
        return basic::__link(source1, source2, extra_edges, link_type);
    } 

    template<typename NodeType, typename EdgeType>
    Tree<NodeType, EdgeType> link(
        Tree<NodeType, EdgeType> setting_tree, 
        Tree<NodeType, EdgeType> source1, 
        Tree<NodeType, EdgeType> source2, 
        TreeLinkType link_type = TreeLinkType::Shuffle) {
        return basic::__link(setting_tree, source1, source2, link_type);
    }

    template<typename NodeType, typename EdgeType>
    Tree<NodeType, EdgeType> link(
        Tree<NodeType, EdgeType> source1, 
        Tree<NodeType, EdgeType> source2, 
        TreeLinkType link_type = TreeLinkType::Shuffle) {
        return basic::__link(source1, source2, link_type);
    }     
}

}

namespace all {
using namespace io;
using namespace checker;
using namespace rand_numeric;
using namespace rand_string;
using namespace rand_vector;
using namespace rand_graph;
}
}