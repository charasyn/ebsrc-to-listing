#pragma once

#include <string>

#include "ListingLine.hpp"

class IRenderer {
public:
    virtual ~IRenderer() {}
    virtual void consumeLine(std::string const & filename, ListingLine const & line) = 0;
};
