//
// Created by nikita on 22.10.18.
//

#ifndef CHECK_FOR_SUF_ARRAY_SUF_H
#define CHECK_FOR_SUF_ARRAY_SUF_H


#include <string>
#include <vector>

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



#endif //CHECK_FOR_SUF_ARRAY_SUF_H
