#include <iostream>
#include <string>
#include <vector>
#include <limits>

class MyArray {
public:
    explicit MyArray(const std::string &_pattern);

    inline char operator[](size_t index);

    int Size() { return size; }

private:
    std::vector<char> array;
    std::string pattern;
    int p_size;
    int current_size;
    int first_index;
    int before_first_index;
    int end_of_string_checker;
    int size;
};

MyArray::MyArray(const std::string &_pattern) {
    pattern = _pattern;
    p_size = static_cast<int>(_pattern.size());
    current_size = 0;
    first_index = 0;
    before_first_index = p_size + 1;
    end_of_string_checker = 0;
    size = std::numeric_limits<int>::max();

}

inline char MyArray::operator[](size_t index) {
    if (index > p_size) {
        if (index >= current_size + before_first_index) {
            char c;
            if (end_of_string_checker) {
                return EOF;
            }
            std::cin.tie(nullptr);
            std::cin >> c;
            if (std::cin.eof()) {
                end_of_string_checker = 1;
                size = current_size + before_first_index;
                return EOF;
            }
            if (current_size == 2 * p_size) {
                array[first_index] = c;
                first_index = (first_index + 1) % (2 * p_size);
                before_first_index++;
                return array[(index - before_first_index + first_index) % (2 * p_size)];
            } else {
                current_size++;
                array.push_back(c);
                return c;
            }
        } else {
            return array[(index - before_first_index + first_index) % (2 * p_size)];

        }
    } else if (index < p_size) {
        return pattern[index];
    } else if (index == p_size) {
        return '#';
    }
}

class Array_for_z {
public:
    explicit Array_for_z(int _size);

    inline int &operator[](int _index);

private:
    int before_array, current_index, size, max_size;
    std::vector<int> array;
};

inline int &Array_for_z::operator[](int _index) {
    if (size == max_size && _index >= before_array + size) {
        array[current_index] = 0;
        current_index = (current_index + 1) % size;
        before_array++;
        return array[(current_index + size - 1) % size];
    }
    if (_index >= before_array + size) {
        size++;
        array.push_back(0);
        return array[size - 1];
    }
    if (_index < before_array + size) {
        return array[(_index - before_array + current_index) % max_size];
    }
}

Array_for_z::Array_for_z(int _size) : max_size(_size + 1), current_index(0), size(0), before_array(0) {}

void Z_function(MyArray &s, int pattern_size) {
    Array_for_z z_value(pattern_size);
    int left = 0, right = 0;
    for (int i = 1; i < s.Size() && s[i] != EOF; i++) {
        if (i > right) {
            while (i + z_value[i] < s.Size() && s[i + z_value[i]] != EOF && s[i + z_value[i]] == s[z_value[i]]) {
                z_value[i]++;
            }
        } else {
            if (z_value[i - left] + i > right) {
                z_value[i] = right - i;
                while (i + z_value[i] < s.Size() && s[i + z_value[i]] != EOF && s[i + z_value[i]] == s[z_value[i]]) {
                    z_value[i]++;
                }
            } else {
                z_value[i] = z_value[i - left];
            }
        }
        if (i + z_value[i] - 1 > right) {
            right = z_value[i] + i - 1;
            left = i;
        }
        if (z_value[i] == pattern_size) {
            std::cout << i - pattern_size - 1 << " ";
        }
    }
}

int main() {
    std::string pattern, str;
    std::cin >> pattern;
    MyArray v(pattern);
    std::vector<int> z_values;
    Z_function(v, static_cast<int>(pattern.size()));
    return 0;
}