#include <iostream>
#include <string>
#include <vector>
std::vector<int> check(std::string &s, std::string &text) {
    std::vector<int> answer;
    for (int start = 0; start < text.size() - s.size() + 1; start++) {
        int len = 0;
        for (; (len < s.size()) && (s[len] == text[start + len] || s[len] == '?'); len++);
        if (len == s.size()) {
            answer.push_back(start);
        }
    }
    return answer;
}


class Trie {
public:
    explicit Trie();

    virtual void Add_string(const std::string &str, int start_index);

    virtual bool Find(const std::string &str);

    //void Print() { for (const std::string &s : patterns) { std::cout << s << " "; }}

    void Find_all_position(const std::string &str);

    std::vector<int> Get_answer();

    std::vector<int> answer;        // Только для ответа

private:
    struct vertex {
        std::vector<int> next_vertices;
        std::vector<int> pattern_index;     // Номер строки, если ничего тут не заканчивается, то -1
        bool flag;              // Заканчивается ли тут строка
        int parent;             // Номер родителя
        std::vector<int> move;  // Номера следующих вершин
        int suffix;             // Номер его суффиксной ссылки
        char c;                 // Буква перехода из родителя
        int good_suffix;        // Номер его хорошего суффикса

        explicit vertex(int _parent, char _c, bool _flag = false);
    };

    int get_next_move(int number_of_vertex, char c);

    int get_suffix(int number_of_vertex);

    int get_good_suffix(int number_of_vertex);

    void check(int number_of_vertex, int i);

    std::vector<vertex> trie_vertex;
    std::vector<std::pair<std::string, int>> patterns;  // Храним подстроку между ? и индекс начала в изначальном паттерне
};

Trie::vertex::vertex(int _parent, char _c, bool _flag) : flag(_flag),
                                                                              parent(_parent), c(_c), suffix(-1),
                                                                              good_suffix(-1) {
    next_vertices.resize(26, -1);
    move.resize(26, -1);
}

Trie::Trie() {
    trie_vertex.emplace_back(vertex(0, '0'));
}

void Trie::Add_string(const std::string &str, int start_index) {
    int number_of_vertex = 0;
    for (char i : str) {
        int c = i - 'a';
        if (trie_vertex[number_of_vertex].next_vertices[c] == -1) {
            trie_vertex.emplace_back(vertex(number_of_vertex, i));
            trie_vertex[number_of_vertex].next_vertices[c] = trie_vertex.size() - 1;

        }
        number_of_vertex = trie_vertex[number_of_vertex].next_vertices[c];
    }
    trie_vertex[number_of_vertex].flag = true;
    patterns.emplace_back(str, start_index);
    trie_vertex[number_of_vertex].pattern_index.push_back(patterns.size() - 1);
}

bool Trie::Find(const std::string &str) {
    int number_of_vertex = 0;
    for (char i : str) {
        char c = i - 'a';
        if (trie_vertex[number_of_vertex].next_vertices[c] == -1) {
            return false;
        }
        number_of_vertex = trie_vertex[number_of_vertex].next_vertices[c];
    }
    return true;
}

int Trie::get_suffix(int number_of_vertex) {
    if (trie_vertex[number_of_vertex].suffix == -1) {
        if (number_of_vertex == 0 || trie_vertex[number_of_vertex].parent == 0) {
            trie_vertex[number_of_vertex].suffix = 0;
        } else {
            trie_vertex[number_of_vertex].suffix = get_next_move(get_suffix(trie_vertex[number_of_vertex].parent),
                                                                 trie_vertex[number_of_vertex].c);
        }
    }
    return trie_vertex[number_of_vertex].suffix;
}

int Trie::get_next_move(int number_of_vertex, char c) {
    if (trie_vertex[number_of_vertex].move[c - 'a'] == -1) {
        if (trie_vertex[number_of_vertex].next_vertices[c - 'a'] != -1) {
            trie_vertex[number_of_vertex].move[c - 'a'] = trie_vertex[number_of_vertex].next_vertices[c - 'a'];
        } else {
            if (number_of_vertex == 0) {
                trie_vertex[number_of_vertex].move[c - 'a'] = 0;
            } else {
                trie_vertex[number_of_vertex].move[c - 'a'] = get_next_move(get_suffix(number_of_vertex), c);
            }
        }
    }
    return trie_vertex[number_of_vertex].move[c - 'a'];
}

int Trie::get_good_suffix(int number_of_vertex) {
    if (trie_vertex[number_of_vertex].good_suffix == -1) {
        int index = get_suffix(number_of_vertex);
        if (index == 0) {
            trie_vertex[number_of_vertex].good_suffix = 0;
        } else {
            trie_vertex[number_of_vertex].good_suffix = (trie_vertex[index].flag) ? index : get_good_suffix(index);
        }
    }
    return trie_vertex[number_of_vertex].good_suffix;
}

void Trie::Find_all_position(const std::string &str) {
    answer.resize(str.size(), 0);
    int number_of_vertex = 0;
    for (int i = 0; i < str.size(); i++) {
        number_of_vertex = get_next_move(number_of_vertex, str[i]);
        check(number_of_vertex, i + 1);
    }
}

void Trie::check(int number_of_vertex, int i) {
    for (int index = number_of_vertex; index != 0; index = get_good_suffix(index)) {
        if (trie_vertex[index].flag) {
            for (int j : trie_vertex[index].pattern_index) {
                int index_in_str = i - patterns[j].first.length();
                // std::cout << i - patterns[trie_vertex[index].pattern_number].first.length() << " " << patterns[trie_vertex[index].pattern_number].first << "\n";
                if (index_in_str - patterns[j].second >= 0) {
                    answer[index_in_str - patterns[j].second]++;
                }
            }
        }
    }
}


std::vector<int> Trie::Get_answer() {
    std::vector<int> ans;
    for (int i = 0; i < answer.size(); i ++) {
        if (answer[i] == patterns.size()) {
            ans.push_back(i);
        }
    }
    return ans;
}

struct substr {
    substr(std::string::iterator _begin, std::string::iterator _end, int _i_begin);

    std::string::iterator begin;
    std::string::iterator end;
    int i_begin;
};

substr::substr(std::string::iterator _begin, std::string::iterator _end, int _i_begin) : begin(_begin), end(_end),
                                                                                         i_begin(_i_begin) {}


std::vector<int> algorithm(std::string pattern, std::string string) {
    std::vector<substr> substr_of_maskless_part;
    bool part_type = false; // 1, если сейчас считываем безмасочный кусок
    std::string::iterator begin;
    std::string::iterator end;
    int i_begin = 0, i = 0;
    for (auto it = pattern.begin(); it != pattern.end(); ++it) {
        if (*it != '?' && part_type) {
            i++;
            continue;
        }
        if (*it != '?' && !part_type) {
            part_type = true;
            begin = it;
            i_begin = i;
            i++;
            continue;
        }
        if (*it == '?' && part_type) {
            end = it;
            substr_of_maskless_part.emplace_back(begin, end, i_begin);
            part_type = false;
            i++;
            continue;
        }
        if (*it == '?' && !part_type) {
            i++;
            continue;
        }
    }
    if (part_type) {
        substr_of_maskless_part.emplace_back(begin, pattern.end(), i_begin);
    }
    Trie trie;
    for (auto k : substr_of_maskless_part) {
        trie.Add_string(std::string(k.begin, k.end), k.i_begin);
    }
    auto it = string.end();
    int k;
    for (k = pattern.size() - 1; pattern[k] == '?' && k >= 0; --k) {
        --it;
    }
    if (k == -1) {
        std::vector<int> ans;
        for (int j = 0; j <= string.size() - pattern.size(); j++) {
            ans.push_back(j);
        }
        return ans;
    }
    else {
        trie.Find_all_position(std::string(string.begin(), it));
    }
    return trie.Get_answer();
}

void comp(int max_len, int count) {
    srand(time(NULL));
    std::string s, text;
    std::vector<int> answer1, answer2;
    for (int j = 0; j < count; ++j) {
        //std::cout << s << " " << text << '\n';
        int len = rand() % max_len + 1;
        s = "", text = "";
        for (int i = 0; i < max_len; ++i) {
            text += (char) (rand() % ('z' - 'a') + 'a');
        }
        for (int i = 0; i < len; ++i) {
            if (rand() % 5 > 3) {
                s += (char) (rand() % ('z' - 'a') + 'a');
            } else {
                s += '?';
            }
        }
        try {
            answer1 = algorithm(s, text);
        }
        catch (std::exception exception) {
            std::cout << s << " " << text;
            return;
        }
        answer2 = check(s, text);
        if (answer1 != answer2) {
            std::cout << "\ntest number\t" << j << "\n";
            std::cout << s << "\n" << text << "\nanswer1\t\t" << answer1.size() << "\n";
            for (int i : answer1) {
                std::cout << i << " ";
            }
            std::cout << "\nanswer2\t\t" << answer2.size() << "\n";
            for (int i : answer2) {
                std::cout << i << " ";
            }
            return;
        }
    }
    std::cout << "All tests are passed!" << std::endl;
}

int main() {
    comp(20, 700000);
    return 0;
}