#include <format>
#include <iostream>

#include "BodySanitizer.hpp"
#include "Exceptions.hpp"
#include "ListingLine.hpp"

std::optional<ListingLine> ListingLine::fromString(uint32_t segmentStart, std::string line) {
    std::smatch m;
    if (not std::regex_match(line, m, lineRe)) {
        return {};
    }
    // Get codeAddress
    int offset = std::stoi(m[1], nullptr, 16);
    if (offset < 0 or offset >= 65536) {
        throw malformed_listing("Line offset is out of range");
    }
    uint32_t codeAddress = offset + segmentStart;
    // Get codeBytes
    CodeBytes codeBytes{};
    auto byteIt = std::sregex_iterator(m[2].first, m[2].second, byteOrRelocationRe);
    for (; byteIt != std::sregex_iterator(); ++byteIt) {
        if (byteIt->str(1).length()) {
            codeBytes.emplace_back();
        } else {
            uint8_t byteValue = std::stoi(byteIt->str(2), nullptr, 16);
            codeBytes.emplace_back(byteValue);
        }
    }
    // Get bodyText
    std::string bodyText = sanitizeBody(m.str(3));
    return ListingLine(codeAddress, std::move(codeBytes), std::move(bodyText));
}
std::string ListingLine::toString() const {
    return std::format("{:06X}: {} {} {} {} {} {}",
        codeAddress_,
        codeBytes_.size() > 0 ? (codeBytes_[0].has_value() ? std::format("{:02X}", codeBytes_[0].value()) : "??" ) : "  ",
        codeBytes_.size() > 1 ? (codeBytes_[1].has_value() ? std::format("{:02X}", codeBytes_[1].value()) : "??" ) : "  ",
        codeBytes_.size() > 2 ? (codeBytes_[2].has_value() ? std::format("{:02X}", codeBytes_[2].value()) : "??" ) : "  ",
        codeBytes_.size() > 3 ? (codeBytes_[3].has_value() ? std::format("{:02X}", codeBytes_[3].value()) : "??" ) : "  ",
        codeBytes_.size() > 4 ? "..." : "   ",
        bodyText_
    );
}
void ListingLine::updateRelocations(const EbRom & rom) {
    for (std::size_t offset = 0; offset < codeBytes_.size(); offset += 1) {
        if (codeBytes_[offset].has_value()) {
            continue;
        }
        uint32_t effectiveAddress = codeAddress_ + offset;
        uint8_t data = rom.readSnesAddr(effectiveAddress);
        codeBytes_[offset] = data;
    }
}
