#ifndef __CORE_MODE__
#define __CORE_MODE__

#include <iostream>
#include <string>
#include <tuple>
#include <vector>
#include <type_traits>
#include <queue>
#include <cassert>

struct ListNode {
    int val;
	struct ListNode *next;
	ListNode(int x) : val(x), next(nullptr) {}
};

struct TreeNode {
	int val;
    struct TreeNode *left;
    struct TreeNode *right;
    TreeNode(int x) : val(x), left(nullptr), right(nullptr) {}
};

/**
 * @brief 核心模式输入输出
 * @note 不允许字符串中出现引号，否则会导致解析错误。不要有多余的空格，仅允许在字符串变量内部出现空格。
 * @example 自动读取 Solution::twoSum 函数的参数，并运行该函数，打印函数的返回结果
 * auto args = CoreMode::read_function_args<decltype(&Solution::twoSum)>();
 * CoreMode::println(std::apply([](auto&&... params) {
 *     return Solution().twoSum(params...);
 * }, args));
 */
namespace CoreMode {
    template <typename T>
    struct is_vector : std::false_type {};
    template <typename U, typename Alloc>
    struct is_vector<std::vector<U, Alloc>> : std::true_type {};

    struct CoreModeInputHelper {
        static const std::string left_bracket;
        static const std::string right_bracket;
        static const std::string comma;

        /**
         * @brief 判断当前两个括号是否匹配
         */
        static bool _isBracketMatch(char left, char right) {
            int left_pos = left_bracket.find(left);
            int right_pos = right_bracket.find(right);
            return left_pos != std::string::npos && left_pos == right_pos;
        }
        /**
         * @brief 找到所有分割变量的逗号位置
         * @note 该函数扫描输入字符串，返回所有处于最外层（即不在任何括号内部）位置的逗号索引。
         */
        static std::vector<size_t> _findCommaPositions(const std::string &data) {
            std::vector<size_t> positions;
            std::vector<char> bracketStack;
            for (size_t i = 0; i < data.size(); i++) {
                char current = data[i];
                if (right_bracket.find(current) != std::string::npos) {
                    // 如果是右括号，则尝试弹出栈顶匹配的左括号
                    if (!bracketStack.empty() && _isBracketMatch(bracketStack.back(), current)) {
                        bracketStack.pop_back();
                        continue;
                    }
                }
                if (left_bracket.find(current) != std::string::npos) {
                    // 如果是左括号，则压入栈中
                    bracketStack.push_back(current);
                    continue;
                }
                if (current == comma[0] && bracketStack.empty()) {
                    // 如果当前字符是逗号，且不在任何括号内部，则记录其位置
                    positions.push_back(i);
                    continue;
                }
            }
            return positions;
        }
        /**
         * @brief 分割字符串，返回所有分割好的变量
         * @note 该函数会忽略所有在括号内部的逗号
         */
        static std::vector<std::string> _split(const std::string &data) {
            std::vector<size_t> commaPositions = _findCommaPositions(data);
            std::vector<std::string> result;
            size_t lastPos = 0;
            for (size_t pos : commaPositions) {
                result.push_back(data.substr(lastPos, pos - lastPos));
                lastPos = pos + 1;
            }
            result.push_back(data.substr(lastPos));
            return result;
        }
        /**
         * @brief 对基础变量进行解析
         */
        template <typename T>
        static T _parseSingleVariable(const std::string &data) {
            throw std::invalid_argument("Not implemented");
        }
        /**
         * @brief 对容器变量进行解析
         */
        template <typename T>
        static std::vector<T> _parseContainerVariable(const std::string &data) {
            if (data.size() < 2 || data.front() != '[' || data.back() != ']') {
                throw std::invalid_argument("Invalid container");
            }
            std::vector<T> result;
            std::vector<std::string> splitData = _split(data.substr(1, data.size() - 2));
            for (const auto &item : splitData) {
                if constexpr (is_vector<T>::value) {
                    using ElementType = typename T::value_type;
                    result.push_back(_parseContainerVariable<ElementType>(item));
                } else {
                    result.push_back(_parseSingleVariable<T>(item));
                }
            }
            return result;
        }
        /**
         * @brief 对链表进行解析
         * @note 字符串格式为：{1,2,3,4,5}
         */
        static ListNode* _parseList(const std::string &data) {
            if (data.size() < 2 || data.front() != '{' || data.back() != '}') {
                throw std::invalid_argument("Invalid list");
            }
            std::vector<std::string> splitData = _split(data.substr(1, data.size() - 2));
            ListNode *dummyRoot = new ListNode(0), *ptr = dummyRoot;
            for (const auto &item : splitData) {
                ptr->next = new ListNode(std::stoi(item));
                ptr = ptr->next;
            }
            ListNode *head = dummyRoot->next;
            delete dummyRoot;
            return head;
        }
        /**
         * @brief 对二叉树进行解析
         * @note 字符串格式为：{1,2,3,#,#,4,#,#,5,#,#}
         */
        static TreeNode* _parseTree(const std::string &data) {
            if (data.size() < 2 || data.front() != '{' || data.back() != '}') {
                throw std::invalid_argument("Invalid tree");
            }
            TreeNode *root = nullptr;
            std::queue<TreeNode *> nodeQueue;
            std::vector<std::string> splitData = _split(data.substr(1, data.size() - 2));
            for (int i = 0; i < splitData.size(); i++) {
                const auto &item = splitData[i];
                if (nodeQueue.empty()) {
                    root = new TreeNode(std::stoi(item));
                    nodeQueue.push(root);
                } else {
                    if (item != "#") {
                        TreeNode *ptr = new TreeNode(std::stoi(item));
                        nodeQueue.front()->left = ptr;
                        nodeQueue.push(ptr);
                    }
                    if (i + 1 < splitData.size()) {
                        const auto next_item = splitData[i + 1];
                        if (next_item != "#") {
                            TreeNode *ptr = new TreeNode(std::stoi(next_item));
                            nodeQueue.front()->right = ptr;
                            nodeQueue.push(ptr);
                        }
                    }
                    nodeQueue.pop();
                }
            }
            return root;
        }
    };
    const std::string CoreModeInputHelper::left_bracket = "{[(\"";
    const std::string CoreModeInputHelper::right_bracket = "}])\"";
    const std::string CoreModeInputHelper::comma = ",";
    template <>
    std::string CoreModeInputHelper::_parseSingleVariable<std::string>(const std::string &data) { 
        if (data.size() < 2 || data.front() != '\"' || data.back() != '\"') {
            throw std::invalid_argument("Invalid string");
        }
        return data.substr(1, data.size() - 2); 
    }
    template <>
    int CoreModeInputHelper::_parseSingleVariable<int>(const std::string &data) { return std::stoi(data); }
    template <>
    double CoreModeInputHelper::_parseSingleVariable<double>(const std::string &data) { return std::stod(data); }
    template <>
    float CoreModeInputHelper::_parseSingleVariable<float>(const std::string &data) { return std::stof(data); }
    template <>
    long long CoreModeInputHelper::_parseSingleVariable<long long>(const std::string &data) { return std::stoll(data); }
    template <>
    bool CoreModeInputHelper::_parseSingleVariable<bool>(const std::string &data) { return data == "true"; }
    template <>
    char CoreModeInputHelper::_parseSingleVariable<char>(const std::string &data) { return data[0]; }
    template <>
    ListNode* CoreModeInputHelper::_parseSingleVariable<ListNode*>(const std::string &data) { return _parseList(data); }
    template <>
    TreeNode* CoreModeInputHelper::_parseSingleVariable<TreeNode*>(const std::string &data) { return _parseTree(data); }

    template <typename T>
    typename std::enable_if<!is_vector<T>::value, T>::type parse_helper(const std::string &token) {
        return CoreModeInputHelper::_parseSingleVariable<T>(token);
    }
    template <typename T>
    typename std::enable_if<is_vector<T>::value, T>::type parse_helper(const std::string &token) {
        using ElementType = typename T::value_type;
        return CoreModeInputHelper::_parseContainerVariable<ElementType>(token);
    }
    template <typename... Args, std::size_t... Is>
    std::tuple<Args...> read_impl(const std::vector<std::string>& tokens, std::index_sequence<Is...>) {
        return std::make_tuple(parse_helper<Args>(tokens[Is])...);
    }
    /**
     * @brief 使用标准输入读取变量，将所有的变量解析为元组返回
     * @example auto [n, m] = CoreMode::read<int, int>();
     * @example auto [s, v] = CoreMode::read<std::string, std::vector<int>>();
     */
    template <typename... Args>
    std::tuple<Args...> read() {
        std::string data;
        std::getline(std::cin, data);
        auto tokens = CoreModeInputHelper::_split(data);
        if (tokens.size() != sizeof...(Args)) {
            throw std::invalid_argument("Argument count mismatch");
        }
        return read_impl<Args...>(tokens, std::make_index_sequence<sizeof...(Args)>{});
    }

    /**
     * @brief 处理单个参数：输出到标准输出
     */
    template <typename T>
    void print(T value) { std::cout << value; }
    void print(const std::string &value) { std::cout << "\"" << value << "\""; }
    template <>
    void print<bool>(bool value) { std::cout << (value ? "true" : "false"); }
    template <>
    void print<ListNode*>(ListNode *value) {
        std::cout << "{";
        while (value) {
            std::cout << value->val;
            value = value->next;
            if (value) std::cout << ",";
        }
        std::cout << "}";
    }
    template <>
    void print<TreeNode*>(TreeNode *value) {
        std::cout << '{';
        std::queue<TreeNode *> q;
        int number = 0;
        if (value) {
            q.push(value);
            number++;
        }
        while (number) {
            TreeNode *node = q.front();
            q.pop();
            if (node) number--;
            if (node == nullptr) {
                std::cout << "#";
            } else {
                std::cout << node->val;
                q.push(node->left);
                if (node->left) number++;
                q.push(node->right);
                if (node->right) number++;
            }
            if (number) std::cout << ",";
        }
        std::cout << '}';
    }
    template <typename T>
    void print(const std::vector<T> &value) {
        std::cout << "[";
        for (size_t i = 0; i < value.size(); i++) {
            if (i > 0) std::cout << ",";
            print(value[i]);
        }
        std::cout << "]";
    }
    template <typename... Args>
    static void print(const Args&... args) {
        bool first = true;
        ((first ? (first = false, print(args)) : (std::cout << ",", print(args))), ...);
    }
    template<typename... Args>
    static void println(const Args&... args) {
        print(args...);
        std::cout << std::endl;
    }

    // 函数特征辅助结构
    template<typename T>
    struct function_traits;
    // 成员函数特化版本
    template<typename ClassType, typename ReturnType, typename... Args>
    struct function_traits<ReturnType(ClassType::*)(Args...)> {
        using return_type = ReturnType;
        using args_tuple = std::tuple<std::remove_reference_t<Args>...>;
        static constexpr size_t arity = sizeof...(Args);
    };
    // 为 const 成员函数特化
    template<typename ClassType, typename ReturnType, typename... Args>
    struct function_traits<ReturnType(ClassType::*)(Args...) const> {
        using return_type = ReturnType;
        using args_tuple = std::tuple<std::remove_reference_t<Args>...>;
        static constexpr size_t arity = sizeof...(Args);
    };
    // 自动读取函数参数的辅助函数
    template<typename Func>
    auto read_function_args() {
        using traits = function_traits<Func>;
        using args_tuple = typename traits::args_tuple;
        return std::apply([](auto... types) {
            return read<std::remove_reference_t<decltype(types)>...>();
        }, args_tuple{});
    }
}  // namespace CoreMode

#endif