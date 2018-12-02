#include <iostream>
#include "my.h"
#include "right.h"

int main() {
    std::string str;
    for (int i = 0; i < 10; i++) {
        std::string r;
        for (int j = 0; j < 1000; j++) {
            char c = static_cast<char>(rand() % 127 + 1);
            r += c;
        }
        solver my(r);
        right right(r);
        if (my.GetAnswer() != right.GetAnswer()) {
            std::cout << r;
            return 0;
        }
//        std::cout << i << " ";
        if (i % 1 == 0) {
            std::cout << i << '\n';
        }
    }
    std::cout << "success";
    return 0;
}