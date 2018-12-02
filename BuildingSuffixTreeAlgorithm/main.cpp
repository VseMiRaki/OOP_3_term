#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <map>
#include <memory>
#include <stack>
#include <queue>

struct vertex {
    explicit vertex(std::weak_ptr<vertex> _par, int _len = 0, int _pos = 0) : parent(std::move(_par)), len(_len),
                                                                              position(_pos) {}

    std::weak_ptr<vertex> parent;
    int len;
    int position;
    std::map<char, std::shared_ptr<vertex>> next;
    std::map<char, std::weak_ptr<vertex>> pref_link;

};

class SuffixTree {
private:

    int current_size;
    std::string str;

    void print(std::shared_ptr<vertex> v);


    std::shared_ptr<vertex> zero;
    std::weak_ptr<vertex> last_added;
    std::stack<std::weak_ptr<vertex>> path;

    void add__suf(int i);

    void merge(const std::shared_ptr<vertex> &child, const std::weak_ptr<vertex> &parent, char c, int child_len);

public:
    explicit SuffixTree(std::string _str) : str(std::move(_str)) {
        current_size = 0;
        root = std::make_shared<vertex>(std::weak_ptr<vertex>(), 1, -1);
        zero = std::make_shared<vertex>(std::weak_ptr<vertex>(), 1);
        root->parent = zero;
        for (int c = 0; c < 256; c++) {
            zero->pref_link[c] = root;
        }
    }

    virtual void Print() { this->print(root); }

    virtual void BuildTree();

protected:
    std::shared_ptr<vertex> root;
};

void SuffixTree::BuildTree() {
    last_added = root;
    for (int i = str.size() - 1; i >= 0; i--) {
        add__suf(i);
    }

}

void SuffixTree::add__suf(int i) {
    std::weak_ptr<vertex> v;
    auto old_last_added = last_added;
    int v_len = str.size() - i;
    for (v = old_last_added; !v.lock()->pref_link.count(str[i]); v = v.lock()->parent) {
        v_len -= v.lock()->len;
        path.push(v);
    }
    auto w = v.lock()->pref_link[str[i]];
    std::shared_ptr<vertex> leaf = std::make_shared<vertex>(std::weak_ptr<vertex>());
    if (w.lock()->next.count(str[i + v_len])) {
        auto u = w.lock()->next[str[i + v_len]];
        std::shared_ptr<vertex> new_last_added = std::make_shared<vertex>(std::weak_ptr<vertex>());
        for (new_last_added->position = u->position - u->len;
             str[new_last_added->position] == str[i + v_len]; new_last_added->position += v.lock()->len) {
            v = path.top();
            path.pop();
            v_len += v.lock()->len;
        }
        this->merge(new_last_added, w, str[u->position - u->len], u->len - u->position + new_last_added->position);
        this->merge(u, new_last_added, str[new_last_added->position], u->position - new_last_added->position);
        w = new_last_added;
        v.lock()->pref_link[str[i]] = new_last_added;
        last_added = new_last_added;
    }
    old_last_added.lock()->pref_link[str[i]] = leaf;
    this->merge(leaf, w, str[i + v_len], str.size() - i - v_len);
    leaf->position = str.size();
    last_added = leaf;
}

void SuffixTree::merge(const std::shared_ptr<vertex> &child, const std::weak_ptr<vertex> &parent, char c, int child_len) {
    parent.lock()->next[c] = child;
    child->len = child_len;
    child->parent = parent;
}

void SuffixTree::print(std::shared_ptr<vertex> v) {
    if (v == zero || v == nullptr) {
        return;
    }
    std::string s = std::string(str.begin() + v->position - v->len, str.begin() + v->position);
    std::cout << s << " ";
    for (auto i : v->next) {
        print(i.second);
    }

}


class DoubleSufTree : public SuffixTree {
public:
    DoubleSufTree(const std::string &str1, const std::string &str2) : first_str_size(str1.size()),
                                                                      second_str_size(str2.size()),
                                                                      SuffixTree(str1 + str2) {}

    void BuildTree() override;

    void Print() override;

private:

    int first_str_size, second_str_size;

    long long number_of_vertex = 0;

};

void DoubleSufTree::BuildTree() {
    SuffixTree::BuildTree();

    std::stack<std::shared_ptr<vertex>> stack;
    stack.push(root);

    while (!stack.empty()) {
        number_of_vertex++;
        std::shared_ptr<vertex> cur_v = stack.top();
        stack.pop();
        if (cur_v->position - cur_v->len < first_str_size && cur_v->position > first_str_size && cur_v != root) {
            int new_len = cur_v->len - (cur_v->position - first_str_size);
            cur_v->position = first_str_size;
            cur_v->len = new_len;
        } else {
            for (auto i : cur_v->next) {
                stack.push(i.second);
            }
        }

    }
}

void DoubleSufTree::Print() {
    std::stack<std::pair<std::shared_ptr<vertex>, long long>> stack;
    stack.push({root, -1});
    long long number = 0;
    std::cout << this->number_of_vertex << "\n";
    while (!stack.empty()) {
        auto cur_v = stack.top();

        stack.pop();
        if (!cur_v.first->next.empty()) {
            auto i = cur_v.first->next.end();
            --i;
            for (; i != cur_v.first->next.begin(); --i) {
                stack.push({i->second, number});

            }
            stack.push({cur_v.first->next.begin()->second, number});
        }
        if (cur_v.first != root) {
            std::cout << cur_v.second << " ";

            if (cur_v.first->position <= first_str_size) {
                std::cout << 0 << " ";
                std::cout << cur_v.first->position - cur_v.first->len << " ";
                std::cout << cur_v.first->position << "\n";
            } else {
                std::cout << 1 << " ";
                std::cout << cur_v.first->position - cur_v.first->len - first_str_size << " ";
                std::cout << cur_v.first->position - first_str_size << "\n";
            }

        }
        number++;
    }

}

int main() {

    std::string str1, str2;
    std::cin >> str1 >> str2;
    DoubleSufTree tree(str1, str2);
    tree.BuildTree();
    tree.Print();
    return 0;
}