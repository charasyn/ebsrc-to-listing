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
    std::cout << ">>> " << filename << std::endl;
}

void TextRenderer::consumeLine(std::string const & filename, ListingLine const & line){
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
    std::cout << std::format("  {:06X}: {} {} {} {} {} {}",
        line.codeAddress(),
        line.codeBytes().size() > 0 ? std::format("{:02X}", line.codeBytes()[0]) : "  ",
        line.codeBytes().size() > 1 ? std::format("{:02X}", line.codeBytes()[1]) : "  ",
        line.codeBytes().size() > 2 ? std::format("{:02X}", line.codeBytes()[2]) : "  ",
        line.codeBytes().size() > 3 ? std::format("{:02X}", line.codeBytes()[3]) : "  ",
        line.codeBytes().size() > 4 ? "..." : "   ",
        line.bodyText()
    ) << std::endl;
}
