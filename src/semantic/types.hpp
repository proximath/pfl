#pragma once

#include "../include/utils.hpp"

#include <unordered_map>

struct Type {
    std::string name;
    bool isPrimitive;
    int index;
    std::unordered_map<std::string, Type*> attributes;
    std::vector<Type*> genericParams;
};

/*
[int] => list[int]
(int, int) => tuple[int, int]
int? => maybe[int]
*/