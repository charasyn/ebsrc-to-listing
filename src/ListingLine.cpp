// charasyn 2024
// Public domain; see UNLICENSE.txt for more info

#include <format>
#include <iostream>

#include "BodySanitizer.hpp"
#include "Exceptions.hpp"
#include "ListingLine.hpp"

std::optional<ListingLine> ListingLine::fromString(const Segment * segment, std::string line) {
    std::smatch m;
    if (not std::regex_match(line, m, Re::listingLine)) {
        return {};
    }
    // Get codeAddress
    int offset = std::stoi(m[1], nullptr, 16);
    if (offset < 0 or offset > 65536) {
        throw malformed_listing("Line offset is out of range");
    }
    uint32_t codeAddress = offset + (segment ? segment->start : 0);
    // Get includeDepth
    uint32_t includeDepth = std::stoi(m[2]);
    // Check if line has code bytes
    bool hasCodeBytesText = not std::regex_match(m[3].first, m[3].second, Re::whitespace);
    // Get bodyText
    std::string bodyText = sanitizeBody(m.str(4));
    return ListingLine(codeAddress, includeDepth, std::move(bodyText), segment, hasCodeBytesText);
}

void ListingLine::setCodeBytes(EbRom const & rom, uint32_t nextLineCodeAddress) {
    if (not hasCodeBytesText()) {
        return;
    }
    if (not segment()) {
        throw malformed_listing("Bytes are defined outside of a segment");
    }
    uint32_t lastCodeAddress = nextLineCodeAddress - 1;
    if (lastCodeAddress > segment()->end) {
        lastCodeAddress = segment()->end;
    }
    uint8_t const * st = &rom.readSnesAddr(codeAddress());
    uint8_t const * ed = &rom.readSnesAddr(lastCodeAddress);
    codeBytes_ = CodeBytes(st, ed + 1);
}
