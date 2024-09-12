#include <format>
#include <iostream>

#include "BodySanitizer.hpp"
#include "Exceptions.hpp"
#include "ListingLine.hpp"

std::optional<ListingLine> ListingLine::fromString(EbRom const & rom, uint32_t segmentStart, std::string line) {
    std::smatch m;
    if (not std::regex_match(line, m, Re::listingLine)) {
        return {};
    }
    // Get codeAddress
    int offset = std::stoi(m[1], nullptr, 16);
    if (offset < 0 or offset >= 65536) {
        throw malformed_listing("Line offset is out of range");
    }
    uint32_t codeAddress = offset + segmentStart;
    // Get includeDepth
    uint32_t includeDepth = std::stoi(m[2]);
    // Get codeBytes
    CodeBytes codeBytes{};
    {
        auto byteIt = std::sregex_iterator(m[3].first, m[3].second, Re::byteOrRelocation);
        for (int offset = 0; byteIt != std::sregex_iterator(); ++byteIt, ++offset) {
            uint8_t data;
            if (byteIt->str(1).length()) {
                uint32_t effectiveAddress = codeAddress + offset;
                data = rom.readSnesAddr(effectiveAddress);
            } else {
                data = std::stoi(byteIt->str(2), nullptr, 16);
            }
            codeBytes.emplace_back(data);
        }
    }
    // Get bodyText
    std::string bodyText = sanitizeBody(m.str(4));
    return ListingLine(codeAddress, includeDepth, std::move(codeBytes), std::move(bodyText));
}
