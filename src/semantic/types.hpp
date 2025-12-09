#pragma once

#include "../include/utils.hpp"

#include <unordered_map>

struct Type {
    std::string name;
    bool isPrimitive;
    int index;
    std::unordered_map<std::string, Type*> attributes;
};