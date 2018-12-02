#include <iostream>
#include <vector>
#include <string>
#include <algorithm>


class suffix_array {
public:
    explicit suffix_array(const std::string &str);

    std::vector<int> GetArray();

    char operator[](int i) { return string[i]; }

private:

    void first_iteration();

    void iteration(int k);


    std::vector<int> classes, p;
    std::string string;
    int number_of_classes;
};

suffix_array::suffix_array(const std::string &str) : number_of_classes(1) {
    string =  str;
    char c = 0;
    string += c;
}

void suffix_array::first_iteration() {
    p.resize(string.size());
    classes.resize(string.size(), 0);

    //counting sort
    std::vector<int> count(256, 0);
    for (char i : string) {
        count[i]++;
    }
    for (int i = 1; i < 256; i++) {
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
        classes[p[i]] = number_of_classes - 1;

    }
}

void suffix_array::iteration(int k) {
    std::vector<int> new_classes(string.size(), 0), temp_p(string.size(), 0);

    for (int i = 0; i < string.size(); i++) {
        temp_p[i] = (p[i] - (1 << k) + string.size()) % string.size();
    }

    //Counting sort
    std::vector<int> count(number_of_classes, 0);
    for (int i = 0; i < string.size(); i++) {
        count[classes[i]]++;
    }
    for (int i = 1; i < number_of_classes; i++) {
        count[i] += count[i - 1];
    }
    for (int i = string.size() - 1; i >= 0; i--) {
        count[classes[temp_p[i]]]--;
        p[count[classes[temp_p[i]]]] = temp_p[i];
    }

    number_of_classes = 1;
    for (int i = 1; i < string.size(); i++) {
        if (classes[p[i]] != classes[p[i - 1]] ||
            classes[(p[i] + (1 << k)) % string.size()] != classes[(p[i - 1] + (1 << k)) % string.size()]) {
            number_of_classes++;
        }
        new_classes[p[i]] = number_of_classes - 1;
    }
    classes = new_classes;
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
    explicit solver(const std::string &_str);

    long long GetAnswer();


private:
    suffix_array str;
    std::vector<int> p;

    void count_max_pref();

    std::vector<int> max_pref;
};

long long solver::GetAnswer() {
    long long ans = 0;
    for (int i = 1; i < p.size(); i++) {
        ans += p.size() - p[i];
    }
    for (int i = 1; i < p.size() - 1; i++) {
        ans -= max_pref[i];
    }
    return ans - p.size() + 1;
}

solver::solver(const std::string &_str) : str(_str) {
    p = move(str.GetArray());
    max_pref.resize(p.size());
    count_max_pref();
}

void solver::count_max_pref() {

    std::vector<int> rev_p(p.size());
    for (int i = 0; i < p.size(); i++) {
        rev_p[p[i]] = i;
    }
    int j = 0;
    for (int i = 0; i < p.size(); i++) {
        if (j > 0) {
            j--;
        }
        if (rev_p[i] == p.size() - 1) {
            max_pref[p.size() - 1] = -1;
            j = 0;
        } else {
            int k;
            for (k = p[rev_p[i] + 1]; std::max(i + j, k + j) < p.size() && str[i + j] == str[k + j]; j++) {}
            max_pref[rev_p[i]] = j;
        }
    }
}


int main() {
    std::string str, temp;
    std::cin >> str;

    suffix_array A(str);
    for (auto i :A.GetArray()) {
    std::cout << i << " "; }

    return 0;
}