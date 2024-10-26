// charasyn 2024
// Public domain; see UNLICENSE.txt for more info

#pragma once

#include <string>

#include "ListingLine.hpp"

class IRenderer {
public:
    virtual ~IRenderer() {}
    virtual void consumeLine(std::string const & filename, ListingLine const & line) = 0;
};
