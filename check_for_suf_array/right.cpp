//
// Created by nikita on 22.10.18.
//

#include <set>
#include "right.h"

right::right(const std::string &str) {
    string = str;
}

long long right::GetAnswer() {
    std::set<std::string> s;
    for (auto it1 = string.begin(); it1 != string.end(); ++it1) {
        for (auto it2 = it1; it2 != string.end(); ++it2) {
            if (it1 != it2) {
                s.insert(std::string(it1, it2));
            }
        }
    }
    for (auto it1 = string.begin(); it1 != string.end(); ++it1) {
        s.insert(std::string(it1, string.end()));
    }
    return static_cast<long long int>(s.size());
}
