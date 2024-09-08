#include <format>
#include <iostream>

#include "TextRenderer.hpp"

void TextRenderer::changeFile(std::string filename){
    std::cout << ">>> " << filename << std::endl;
}

void TextRenderer::consumeLine(ListingLine const & line){
    std::cout << std::format("  {:06X}: {} {} {} {} {} {}",
        line.codeAddress(),
        line.codeBytes().size() > 0 ? (line.codeBytes()[0].has_value() ? std::format("{:02X}", line.codeBytes()[0].value()) : "??" ) : "  ",
        line.codeBytes().size() > 1 ? (line.codeBytes()[1].has_value() ? std::format("{:02X}", line.codeBytes()[1].value()) : "??" ) : "  ",
        line.codeBytes().size() > 2 ? (line.codeBytes()[2].has_value() ? std::format("{:02X}", line.codeBytes()[2].value()) : "??" ) : "  ",
        line.codeBytes().size() > 3 ? (line.codeBytes()[3].has_value() ? std::format("{:02X}", line.codeBytes()[3].value()) : "??" ) : "  ",
        line.codeBytes().size() > 4 ? "..." : "   ",
        line.bodyText()
    ) << std::endl;
}
