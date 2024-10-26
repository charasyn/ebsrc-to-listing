// charasyn 2024
// Public domain; see UNLICENSE.txt for more info

#pragma once

#include <optional>
#include <regex>
#include <string>
#include <vector>

#include "Common.hpp"
#include "Regexes.hpp"

class ListingLine {
    using CodeBytes = std::vector<uint8_t>;
    CodeBytes codeBytes_;
    std::string bodyText_;
    uint32_t codeAddress_;
    uint32_t includeDepth_;
    explicit inline ListingLine(uint32_t codeAddress, int includeDepth, CodeBytes && codeBytes, std::string && bodyText)
        : codeBytes_(std::move(codeBytes)), bodyText_(std::move(bodyText)), codeAddress_(codeAddress), includeDepth_(includeDepth) {}
 
public:
    inline const CodeBytes & codeBytes() const { return codeBytes_; }
    inline const std::string & bodyText() const { return bodyText_; }
    inline uint32_t codeAddress() const { return codeAddress_; }
    inline uint32_t includeDepth() const { return includeDepth_; }
    static std::optional<ListingLine> fromString(EbRom const & rom, uint32_t segmentStart, std::string line);
    inline void extendBytes(const ListingLine & otherLine) {
        codeBytes_.insert(codeBytes_.end(), otherLine.codeBytes_.begin(), otherLine.codeBytes_.end());
    }
    inline bool isEmpty() const {
        return codeBytes_.size() == 0 and std::regex_match(bodyText_, Re::whitespace);
    }
};
