#pragma once

#include <string>

#include "ListingLine.hpp"

class IRenderer {
public:
    virtual ~IRenderer() {}
    virtual void changeFile(std::string filename) = 0;
    virtual void consumeLine(ListingLine const & line) = 0;
};
