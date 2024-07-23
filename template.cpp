#include <bits/stdc++.h>

struct ListNode {
    int val;
    ListNode *next;
    ListNode(int val) : val(val), next(nullptr) {}
};

struct TreeNode {
    int val;
    struct TreeNode *left;
    struct TreeNode *right;
    TreeNode(int x) : val(x), left(nullptr), right(nullptr) {}
};

// 一维数组 [1,2,3], ["abc","def"], {1,2,3} -> std::vector
template<typename T>
std::vector<T> read(std::string s, std::vector<T> res) {
    std::string to_space = "[],\"{}";
    for (auto &ch : s) {
        if (to_space.find(ch) != std::string::npos) {
            ch = ' ';
        }
    }
    std::stringstream ss(s);
    T x;
    while (ss >> x) {
        res.push_back(x);
    }
    return res;
}

// 字符串中包含空格 ["abc","de f"]
std::vector<std::string> read_space(std::string s, std::vector<std::string> res) {
    s.erase(s.begin());
    s.erase(s.end() - 1);
    for (int l = 0, r; l < s.size(); l = r + 1) {
        r = s.find(',', l);
        if (r == std::string::npos) {
            r = s.size();
        }
        
        res.push_back(s.substr(l + 1, r - l - 2));
    }
    return res;
}

// 二维数组
template<typename T>
std::vector<std::vector<T>> read(std::string s, std::vector<std::vector<T>> res) {
    s.erase(s.begin());
    s.erase(s.end() - 1);
    for (int l = 0, r; l < s.size(); l = r + 1) {
        l = s.find('[', l);
        r = s.find(']', l);
        auto v = read(s.substr(l, r - l + 1), std::vector<T>());
        res.push_back(v);
    }
    return res;
}

// std::vector<int> -> ListNode*
ListNode* vector_to_ListNode(std::vector<int> v) {
    ListNode *res = new ListNode(-1);
    ListNode *p = res;
    for (auto i : v) {
        p->next = new ListNode(i);
        p = p->next;
    }
    return res->next;
}

// [{1,2,3},{4,5,6}] -> std::vector
std::vector<ListNode*> read(std::string s, std::vector<ListNode*> res) {
    s.erase(s.begin());
    s.erase(s.end() - 1);
    for (int l = 0, r; l < s.size(); l = r + 1) {
        l = s.find('{', l);
        r = s.find('}', l);
        auto v = read(s.substr(l, r - l + 1), std::vector<int>());
        res.push_back(vector_to_ListNode(v));
    }
    return res;
}

// 二叉树 {3,9,20,#,#,15,7}
TreeNode* read(std::string s, TreeNode* temp) {
    for (auto &ch : s) {
        if (ch == '{' || ch == '}' || ch == ',') {
            ch = ' ';
        }
    }

    std::stringstream ss(s);
    TreeNode* root = new TreeNode(-1);
    ss >> root->val;
    
    std::queue<TreeNode*> q;
    q.push(root);
    std::string num;
    for (int i = 2; ss >> num; i++) {
        if (num == "#") {
            if (i % 2 == 0) {
                q.front()->left = nullptr;
            } else {
                q.front()->right = nullptr;
                q.pop();
            }
            continue;
        }
        TreeNode* p = new TreeNode(std::stoi(num));
        if (i % 2 == 0) {
            q.front()->left = p;
        } else {
            q.front()->right = p;
            q.pop();
        }
        q.push(p);
    }
    return root;
}

// 输出一维数组
template<typename T>
void print(std::vector<T> v) {
    assert(!v.empty());
    std::cout << "[";
    for (int i = 0; i < v.size(); i++) {
        if (std::is_same<T, std::string>::value) {
            std::cout << "\"" << v[i] << "\"" << ",]"[i == v.size() - 1];
        } else {
            std::cout << v[i] << ",]"[i == v.size() - 1];
        }
    }
}

// 输出二维数组
template<typename T>
void print(std::vector<std::vector<T>> v) {
    assert(!v.empty());
    std::cout << "[";
    for (int i = 0; i < v.size(); i++) {
        print(v[i]);
        std::cout << ",]"[i == v.size() - 1];
    }
}

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    std::string s = "[[\"abc\",\"def\"], [\"ghi\",\"jkl\"]]";
    std::cout << s << "\n";
    
    auto res = read(s, std::vector<std::vector<std::string>>());
    for (auto v : res) {
        for (auto x : v) {
            std::cout << x << " ";
        }
        std::cout << "\n";
    }

    return 0;
}