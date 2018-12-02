//
// Created by nikita on 22.10.18.
//

#include "my.h"


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

