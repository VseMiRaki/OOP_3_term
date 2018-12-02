#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <cmath>


class suffix_array {
public:
    explicit suffix_array(const std::string &str);

    std::vector<int> GetArray();

    char operator[](int i) { return string[i]; }

    bool Class_compair(int step, int i, int j) {
        return classes[step][i] == classes[step][j];
    }  //Сравниваем классы на определенном шаге.

private:

    void first_iteration();

    void iteration(int k);


    std::vector<std::vector<int> > classes;
    std::vector<int> p;
    std::string string;
    int number_of_classes;
}; // Класс с суф.массивом из 1ой задачи. Различия только в том, что я допольнительно храню классы суффиксов.

suffix_array::suffix_array(const std::string &str) : number_of_classes(1) {
    string = str;
    char c = 0;
    string += c;
}

void suffix_array::first_iteration() {
    p.resize(string.size());
    classes.emplace_back();
    classes[0].resize(string.size(), 0);

    //counting sort
    std::vector<int> count(1000, 0);
    for (char i : string) {
        count[i]++;
    }
    for (int i = 1; i < 1000; i++) {
        count[i] += count[i - 1];
    }
    for (int i = 0; i < string.size(); i++) {
        //--count[string[i]];
        p[--count[string[i]]] = i;
    }


    for (int i = 1; i < string.size(); i++) {       //Считаем количество различных букв
        if (string[p[i]] != string[p[i - 1]]) {     //и записываем для каждой ее класс
            number_of_classes++;
        }
        classes[0][p[i]] = number_of_classes - 1;

    }
}

void suffix_array::iteration(int k) {
    classes.emplace_back(string.size(), 0);

    std::vector<int> new_classes(string.size(), 0), temp_p(string.size(), 0);

    for (int i = 0; i < string.size(); i++) {
        temp_p[i] = (p[i] - (1 << k) + string.size()) % string.size();
    }

    //Counting sort
    std::vector<int> count(number_of_classes, 0);
    for (int i = 0; i < string.size(); i++) {
        count[classes[k][i]]++;
    }
    for (int i = 1; i < number_of_classes; i++) {
        count[i] += count[i - 1];
    }
    for (int i = string.size() - 1; i >= 0; i--) {
        count[classes[k][temp_p[i]]]--;
        p[count[classes[k][temp_p[i]]]] = temp_p[i];
    }

    number_of_classes = 1;
    for (int i = 1; i < string.size(); i++) {
        if (classes[k][p[i]] != classes[k][p[i - 1]] ||
            classes[k][(p[i] + (1 << k)) % string.size()] != classes[k][(p[i - 1] + (1 << k)) % string.size()]) {
            number_of_classes++;
        }
        new_classes[p[i]] = number_of_classes - 1;
    }
    classes[k + 1] = new_classes;
}

std::vector<int> suffix_array::GetArray() {
    this->first_iteration();
    for (int i = 0; (1 << i) < string.size(); i++) {
        this->iteration(i);
    }
    return p;
}


class solver {
public:
    explicit solver(const std::string &_str1, const std::string &_str2);

    void KthCommonSubstring(long long k);

private:
    suffix_array str;
    std::vector<int> p;

    int max_pref_by_indexes(int i, int j);


    int first_string_size;
    int second_string_size;
    std::vector<int> max_pref;
};


solver::solver(const std::string &_str1, const std::string &_str2) : str(_str1 + "$" + _str2),
                                                                     first_string_size(_str1.size()),
                                                                     second_string_size(_str2.size()) {
    p = move(str.GetArray());
    max_pref.resize(p.size());
}


int solver::max_pref_by_indexes(int i, int j) {
    int ans = 0;
    int k = static_cast<int>(std::log2(p.size()));
    for (; k >= 0; k--) {
        if (str.Class_compair(k, i, j)) {
            ans += 1 << k;
            j += 1 << k;
            i += 1 << k;
        }
    }
    return ans;

}

void solver::KthCommonSubstring(long long k) {

    int it1 = 1, it2 = 1;

    long long result = 0;
    int delta = 0;
    int prev_max_pref = 0;

    while (result < k) {
        if (it1 <= it2) {
            it1++;
            for (; it1 < p.size(); it1++) {
                if (p[it1] < first_string_size) {
                    break;
                }
            }
            if (it1 == p.size()) {
                std::cout << -1;
                return;
            }
        } else {
            it2++;
            for (; it2 < p.size(); it2++) {
                if (p[it2] >= first_string_size) {
                    break;
                }
            }
            if (it2 == p.size()) {
                std::cout << -1;
                return;
            }
        }
        int current_max_pref = this->max_pref_by_indexes(p[it1], p[it2]);
        delta = current_max_pref - prev_max_pref;
        if (delta > 0) {
            result += delta;
        }
        prev_max_pref = current_max_pref;
    }
    if (result == k) {
        std::string s;
        for (int i = 0; i < prev_max_pref; i++) {
            s += str[i + p[it1]];
        }
        std::cout << s;
        return;
    }
    std::string s;
    for (int i = 0; result - prev_max_pref + i < k; i++) {
        s += str[i + p[it1]];
    }
    std::cout << s;
}


int main() {
    std::string str, temp;
    std::cin >> str >> temp;
    long long k;
    std::cin >> k;
    solver A(str, temp);

    A.KthCommonSubstring(k);



    return 0;
}