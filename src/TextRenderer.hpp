#pragma once

#include "IRenderer.hpp"

class TextRenderer : public IRenderer {
    std::string lastFilename;
    std::string lastFilenameOutputted;
    bool shouldOutputFile;
    void changeFile(std::string const & filename);
public:
    virtual void consumeLine(std::string const & filename, ListingLine const & line);
};
