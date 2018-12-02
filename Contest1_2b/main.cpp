#include <iostream>
#include <vector>
#include <string>


std::vector<int> Z_function(std::string s) {
    std::vector<int> z_value(s.size(), 0);
    int left = 0, right = 0;
    for (int i = 1; i < s.size(); i++) {
        if (i > right) {
            while (i + z_value[i] < s.size() && s[i + z_value[i]] == s[z_value[i]]) {
                z_value[i]++;
            }
        }
        else {
            if (z_value[i - left] + i > right) {
                z_value[i] = right - i;
                while (i + z_value[i] < s.size() && s[i + z_value[i]] == s[z_value[i]]) {
                    z_value[i]++;
                }
            }
            else {
                z_value[i] = z_value[i - left];
            }
        }
        if (i + z_value[i] - 1 > right) {
            right = z_value[i] + i - 1;
            left = i;
        }
    }
    return z_value;
}


class string_refactoring {
public:
    std::vector<int> z_function(const std::string &s) const;

    std::string z_to_string(const std::vector<int> &z_values) const;

    std::vector<int> prefix_function(const std::string &s) const;

    std::string prefix_to_string(const std::vector<int> &p_values) const;

    std::vector<int> z_to_prefix(const std::vector<int> &z_values) const;

    std::vector<int> prefix_to_z(const std::vector<int> &p_values) const;
};

std::vector<int> string_refactoring::z_function(const std::string &s) const {
    std::vector<int> z_value(s.size(), 0);
    int left = 0, right = 0;
    for (int i = 1; i < s.size(); i++) {
        if (i > right) {
            while (i + z_value[i] < s.size() && s[i + z_value[i]] == s[z_value[i]]) {
                z_value[i]++;
            }
        }
        else {
            if (z_value[i - left] + i > right) {
                z_value[i] = right - i;
                while (i + z_value[i] < s.size() && s[i + z_value[i]] == s[z_value[i]]) {
                    z_value[i]++;
                }
            }
            else {
                z_value[i] = z_value[i - left];
            }
        }
        if (i + z_value[i] - 1 > right) {
            right = z_value[i] + i - 1;
            left = i;
        }
    }
    return z_value;
}

std::vector<int> string_refactoring::prefix_function(const std::string &s) const {
    return z_to_prefix(z_function(s));
}

std::vector<int> string_refactoring::z_to_prefix(const std::vector<int> &z_values) const {
    std::vector<int> p_values(z_values.size(), 0);
    for (int i = 1; i < z_values.size(); i++) {
        for (int j = z_values[i] - 1; j >= 0; j--) {
            if (p_values[i + j] > 0) {
                break;
            }
            p_values[i + j] = j + 1;
        }
    }
    return p_values;
}

std::string string_refactoring::prefix_to_string(const std::vector<int> &p_values) const {
    std::string str;
    str += 'a';
    for (int i = 1; i < p_values.size(); i++) {
        if (p_values[i] == 0) {
            std::vector<bool> used(26, 0);
            for (int j = p_values[i - 1]; j > 0; j = p_values[j - 1]) {
                used[str[j] - 'a'] = 1;
            }
            int j = 1;
            while (used[j]) {
                j++;
            }
            char c = static_cast<char>('a' + j);
            str += c;
        }
        else {
            str += str[p_values[i] - 1];
        }
    }
    return str;
}

std::string fromPiToStr(const std::vector<size_t> &pi) {
    std::string str = "a";
    for (size_t i = 1; i < pi.size(); i++) {
        if (pi[i] == 0) {
            std::vector<bool> used(26, false);
            size_t k = pi[i - 1];
            while (k > 0) {
                used[str[k] - 'a'] = true;
                k = pi[k - 1];
            }
            char c = 'b';
            while (used[c - 'a']) {
                c++;
            };
            str += c;
        } else {
            str += str[pi[i] - 1];
        }
    }
    return str;
}
std::string string_refactoring::z_to_string(const std::vector<int> &z_values) const {
    return prefix_to_string(z_to_prefix(z_values));
}

std::vector<int> string_refactoring::prefix_to_z(const std::vector<int> &p_values) const {
    return z_function(prefix_to_string(p_values));
}

int main() {
    std::vector<int> z_values;
    int value;
    while (std::cin >> value) {
        z_values.push_back(value);
    }
    std::cout << string_refactoring().z_to_string(z_values);
    return 0;
}
