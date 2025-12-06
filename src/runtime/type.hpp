#pragma once

#include "../include/utils.hpp"

#include <unordered_map>

struct Type {
    bool isPrimitive;
    std::unordered_map<std::string_view, Type> attributes;
};