#pragma once

#include <optional>
#include <regex>
#include <string>
#include <vector>

#include "Common.hpp"
#include "Regexes.hpp"

class ListingLine {
    using CodeBytes = std::vector<std::optional<uint8_t>>;
    CodeBytes codeBytes_;
    std::string bodyText_;
    uint32_t codeAddress_;
    explicit inline ListingLine(uint32_t codeAddress, CodeBytes && codeBytes, std::string && bodyText)
        : codeBytes_(std::move(codeBytes)), bodyText_(std::move(bodyText)), codeAddress_(codeAddress) {}
 
public:
    inline const CodeBytes & codeBytes() const { return codeBytes_; }
    inline const std::string & bodyText() const { return bodyText_; }
    inline uint32_t codeAddress() const { return codeAddress_; }
    static std::optional<ListingLine> fromString(uint32_t segmentStart, std::string line);
    std::string toString() const;
    void updateRelocations(const EbRom & rom);
    inline void extendBytes(const ListingLine & otherLine) {
        codeBytes_.insert(codeBytes_.end(), otherLine.codeBytes_.begin(), otherLine.codeBytes_.end());
    }
    inline bool isEmpty() const {
        return codeBytes_.size() == 0 and std::regex_match(bodyText_, whitespace);
    }
};
