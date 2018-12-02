#include <iostream>
#include <vector>
#include <stack>
#include <queue>
#include <string>
#include <map>

class SuffixTree {
public:
    explicit SuffixTree(const std::string &str1, const std::string &str2);

    void build();

    std::vector<int> GetPos() const {
        return this->_pos;
    }

    std::vector<size_t> GetLen() const {
        return this->_len;
    }

    std::vector<std::map<char, size_t >> GetTo() const {
        return this->_to;
    }

    void Print();

private:
    std::string _str1;

    std::string _str2;

    std::string _str;

    size_t curNumOfVertices = 2;

    std::vector<int> _pos;

    std::stack<size_t> _path;

    std::vector<size_t> _len;

    std::vector<size_t> _parent;

    std::vector<std::map<char, size_t >> _to;

    std::vector<std::map<char, size_t>> _prefLink;

    std::vector<int> &getPos() {
        return this->_pos;
    }

    std::vector<size_t> &getLen() {
        return _len;
    }

    std::vector<std::map<char, size_t>> &getTo() {
        return _to;
    }

    void attach(size_t child, size_t parent, char c, size_t child_len);

    void extend(int i);

    void clear();

    void setNumOfCompressedTree();
};

SuffixTree::SuffixTree(const std::string &str1, const std::string &str2) {
    this->_str1 = str1;
    this->_str2 = str2;
    this->_str = _str1 + _str2;
    this->_len.assign(curNumOfVertices, 1);
    this->_pos.assign(curNumOfVertices, -1);
    this->_parent.assign(curNumOfVertices, 0);
    this->_prefLink.emplace_back(std::map<char, size_t>());
    this->_prefLink.emplace_back(std::map<char, size_t>());
    this->_to.emplace_back(std::map<char, size_t>());
    this->_to.emplace_back(std::map<char, size_t>());
    for (int c = 0; c < 256; ++c) {
        this->_prefLink[0][c] = 1;
    }
}

void SuffixTree::attach(size_t child, size_t parent, char c, size_t child_len) {
    _to[parent][c] = child;
    _len[child] = child_len;
    _parent[child] = parent;
}

void SuffixTree::extend(int i) {
    size_t v, vlen = _str.size() - i, old = curNumOfVertices - 1;
    for (v = old; !(_prefLink[v].count(_str[i])); v = _parent[v]) {
        vlen -= _len[v];
        _path.push(v);
    }
    size_t w = _prefLink[v][_str[i]];
    if (_to[w].count(_str[i + vlen])) {
        size_t u = _to[w][_str[i + vlen]];
        this->_prefLink.emplace_back(std::map<char, size_t>());
        this->_len.emplace_back();
        this->_parent.emplace_back();
        this->_pos.emplace_back();
        this->_to.emplace_back(std::map<char, size_t>());
        for (_pos[curNumOfVertices] = _pos[u] - _len[u]; _str[_pos[curNumOfVertices]] == _str[i + vlen];
             _pos[curNumOfVertices] += _len[v]) {
            v = _path.top();
            _path.pop();
            vlen += _len[v];
        }
        attach(curNumOfVertices, w, _str[_pos[u] - _len[u]], _len[u] - (_pos[u] - _pos[curNumOfVertices]));
        attach(u, curNumOfVertices, _str[_pos[curNumOfVertices]], _pos[u] - _pos[curNumOfVertices]);
        w = _prefLink[v][_str[i]] = curNumOfVertices++;
    }
    _prefLink[old][_str[i]] = curNumOfVertices;
    this->_prefLink.emplace_back(std::map<char, size_t>());
    this->_parent.emplace_back();
    this->_len.emplace_back();
    this->_pos.emplace_back();
    this->_to.emplace_back(std::map<char, size_t>());
    attach(curNumOfVertices, w, _str[i + vlen], _str.size() - (i + vlen));
    _pos[curNumOfVertices++] = _str.size();
}

void SuffixTree::build() {
    for (int i = _str.size() - 1; i >= 0; i--) {
        extend(i);
    }
    this->clear();
    this->curNumOfVertices = 0;
}

void SuffixTree::clear() {
    std::vector<size_t> &len = this->getLen();
    std::vector<int> &pos = this->getPos();
    std::vector<std::map<char, size_t >> &to = this->getTo();
    for (int i = 2; i < curNumOfVertices; ++i) {
        if (pos[i] - len[i] < this->_str1.size() && pos[i] > this->_str1.size()) {
            len[i] = len[i] - (pos[i] - _str1.size());
            pos[i] = this->_str1.size();
            to[i].clear();
        }
    }
}

void SuffixTree::setNumOfCompressedTree() {
    std::vector<std::map<char, size_t >> &to = this->getTo();
    std::stack<size_t> Stack;
    Stack.push(1);
    while (!Stack.empty()) {
        this->curNumOfVertices++;
        size_t cur = Stack.top();
        Stack.pop();
        for (auto i: to[cur]) {
            Stack.push(i.second);
        }
    }
}

void SuffixTree::Print() {
    this->setNumOfCompressedTree();
    std::vector<std::map<char, size_t >> &to = this->getTo();
    std::vector<size_t> &len = this->getLen();
    std::vector<int> &pos = this->getPos();
    std::stack<std::pair<size_t, size_t >> stackForDFS;
    stackForDFS.push(std::pair(1, 0));
    size_t counter = 0;
    std::cout << this->curNumOfVertices << std::endl;
    while (!stackForDFS.empty()) {
        auto current = stackForDFS.top();
        stackForDFS.pop();
        if (!to[current.first].empty()) {
            auto item = to[current.first].end();
            item--;
            for (; item != to[current.first].begin(); --item) {
                stackForDFS.push({(*item).second, counter});
            }
            stackForDFS.push({to[current.first].begin()->second, counter});
        }
        if (current.first != 1) {
            std::cout << current.second << " ";
            if (pos[current.first] <= this->_str1.size()) {
                std::cout << 0 << " " << pos[current.first] - len[current.first] << " " << pos[current.first] << "\n";
            } else {
                std::cout << 1 << " " << pos[current.first] - len[current.first] - this->_str1.size()
                          << " " << pos[current.first] - this->_str1.size() << "\n";
            }
        }
        counter++;
    }
}

int main() {
    std::string s, t;
    std::cin >> s >> t;
    std::string str = s + t;
    SuffixTree suffixTree(s, t);
    suffixTree.build();
    std::vector<std::map<char, size_t >> to;
    std::stack<int> st;
    st.push(1);
    /*while(!st.empty()) {
      int cur = st.top();
      st.pop();
      std::string q(str.begin() + suffixTree.GetPos()[cur] - suffixTree.GetLen()[cur], str.begin() + suffixTree.GetPos()[cur]);
      std::cout<< q<< " ";
      for (char i = 0; i < 127; i++) {
        if (suffixTree.GetTo()[cur].count(i))
          st.push(suffixTree.GetTo()[cur][i]);
      }
    }*/
    suffixTree.Print();
    return 0;
}