// charasyn 2024
// Public domain; see UNLICENSE.txt for more info

#include <format>
#include <iostream>

#include "Regexes.hpp"
#include "TextRenderer.hpp"

template <typename T>
static inline bool _shouldOutputIncludedFile(T includedFilename) {
    // We should output the file if it doesn't match one of the forbidden
    // includes.
    return not std::regex_match(includedFilename, Re::forbiddenIncludes);
}

void TextRenderer::changeFile(std::string const & filename){
    outputStream << ">>> " << filename << std::endl;
}

void TextRenderer::consumeLine(std::string const & filename, ListingLine const & line){
    if (lastCodeAddress != NO_PREVIOUS_ADDRESS and (line.codeAddress() - lastCodeAddress) > 12) {
        outputStream << "          ...omitted..." << std::endl;
    }
    lastCodeAddress = NO_PREVIOUS_ADDRESS;
    if (line.isEmpty()) {
        return;
    }
    if (filename != lastFilename){
        lastFilename = filename;
        shouldOutputFile = _shouldOutputIncludedFile(filename);
    }
    if (not shouldOutputFile) {
        return;
    }
    if (filename != lastFilenameOutputted) {
        lastFilenameOutputted = filename;
        changeFile(filename);
    }
    outputStream << std::format("  {:06X}: {} {} {} {} {}",
        line.codeAddress(),
        line.codeBytes().size() > 0 ? std::format("{:02X}", line.codeBytes()[0]) : "  ",
        line.codeBytes().size() > 1 ? std::format("{:02X}", line.codeBytes()[1]) : "  ",
        line.codeBytes().size() > 2 ? std::format("{:02X}", line.codeBytes()[2]) : "  ",
        line.codeBytes().size() > 3 ? std::format("{:02X}", line.codeBytes()[3]) : "  ",
        line.bodyText()
    ) << std::endl;
    for (uint32_t i = 4; i < line.codeBytes().size(); i += 4) {
    outputStream << std::format("          {} {} {} {} ",
        line.codeBytes().size() > (i+0) ? std::format("{:02X}", line.codeBytes()[i+0]) : "  ",
        line.codeBytes().size() > (i+1) ? std::format("{:02X}", line.codeBytes()[i+1]) : "  ",
        line.codeBytes().size() > (i+2) ? std::format("{:02X}", line.codeBytes()[i+2]) : "  ",
        line.codeBytes().size() > (i+3) ? std::format("{:02X}", line.codeBytes()[i+3]) : "  ",
        line.bodyText()
    ) << std::endl;
    }
    if (line.codeBytes().size() == 12) {
        lastCodeAddress = line.codeAddress();
    }
}
