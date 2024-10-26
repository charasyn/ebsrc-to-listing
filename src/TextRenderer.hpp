#pragma once

#include <ostream>

#include "IRenderer.hpp"

class TextRenderer : public IRenderer {
    std::ostream & outputStream;
    std::string lastFilename;
    std::string lastFilenameOutputted;
    bool shouldOutputFile;
    void changeFile(std::string const & filename);
public:
    TextRenderer(std::ostream & outputStream_) :
        outputStream(outputStream_), lastFilename(), lastFilenameOutputted(), shouldOutputFile(false) {};
    virtual void consumeLine(std::string const & filename, ListingLine const & line);
};
