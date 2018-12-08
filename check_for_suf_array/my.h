//
// Created by nikita on 22.10.18.
//

#pragma once



#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include "suf.h"





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
