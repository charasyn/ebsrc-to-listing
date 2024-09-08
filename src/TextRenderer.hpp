#pragma once

#include "IRenderer.hpp"

class TextRenderer : public IRenderer {
public:
    virtual void changeFile(std::string filename);
    virtual void consumeLine(ListingLine const & line);
};
