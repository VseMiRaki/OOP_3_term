#include <utility>

//
// Created by nikita on 03.10.18.
//

#include "Perebor.h"
#include <iostream>
#include <string>
#include <vector>


Perebor::Perebor(std::string _pattern, std::string _string) {
    string = std::move(_string);
    pattern = std::move(_pattern);
}


