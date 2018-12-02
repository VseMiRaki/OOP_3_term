#include <iostream>
#include <string>
#include <vector>

std::vector<int> Z_function(std::string s) {
    std::vector<int> z_value(s.size(), 0);
    int left = 0, right = 0;
    for (int i = 1; i < s.size(); i++) {
        if (i > right) {
            while (i + z_value[i] < s.size() && s[i + z_value[i]] == s[z_value[i]]) {
                z_value[i]++;
            }
        } else {
            if (z_value[i - left] + i > right) {
                z_value[i] = right - i;
                while (i + z_value[i] < s.size() && s[i + z_value[i]] == s[z_value[i]]) {
                    z_value[i]++;
                }
            } else {
                z_value[i] = z_value[i - left];
            }
        }
        if (i + z_value[i] - 1> right) {
            right = z_value[i] + i - 1;
            left = i;
        }
    }
    return z_value;
}

std::vector<int> naive_z(std::string s) {
    std::vector<int> z(s.size(), 0);
    for (int i = 1; i < s.size(); i++) {
        while (i + z[i] < s.size() && s[z[i]] == s[i + z[i]])
            z[i]++;
    }
    return z;
}


int main() {
    std::string pattern, str;
    std::cin >> pattern >> str;
    std::vector<int> z_values;
    z_values = Z_function(std::string(pattern + "\n" + str));
    for (int i = 0; i < str.size() + 1 + pattern.size(); i++) {
        if (z_values[i] == pattern.size()) {
            std::cout << i - 1 - pattern.size() << " ";
        }
    }
    return 0;
}