//
// Created by nikita on 03.10.18.
//

#ifndef CONTEST1_C_STRESS_PEREBOR_H
#define CONTEST1_C_STRESS_PEREBOR_H

#include <string>
#include <vector>

class Perebor {
public:
    Perebor(std::string pattern, std::string string);
    std::vector<int> Get_answer();

private:
    std::string pattern;
    std::string string;
};


#endif //CONTEST1_C_STRESS_PEREBOR_H
