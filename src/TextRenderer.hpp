// charasyn 2024
// Public domain; see UNLICENSE.txt for more info

#pragma once

#include <ostream>

#include "IRenderer.hpp"

class TextRenderer : public IRenderer {
    enum : uint32_t {
        NO_PREVIOUS_ADDRESS = 0
    };
    std::ostream & outputStream;
    std::string lastFilename;
    std::string lastFilenameOutputted;
    uint32_t lastCodeAddress;
    bool shouldOutputFile;
    void changeFile(std::string const & filename);
public:
    TextRenderer(std::ostream & outputStream_) :
        outputStream(outputStream_), lastFilename(), lastFilenameOutputted(),
        lastCodeAddress(NO_PREVIOUS_ADDRESS), shouldOutputFile(false) {};
    virtual void consumeLine(std::string const & filename, ListingLine const & line);
};
