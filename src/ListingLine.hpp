// charasyn 2024
// Public domain; see UNLICENSE.txt for more info

#pragma once

#include <optional>
#include <regex>
#include <span>
#include <string>

#include "Common.hpp"
#include "MapFileReader.hpp"
#include "Regexes.hpp"

class ListingLine {
    using CodeBytes = std::span<uint8_t const>;
    CodeBytes codeBytes_;
    std::string bodyText_;
    const Segment * segment_;
    uint32_t codeAddress_;
    uint32_t includeDepth_;
    bool hasCodeBytesText_;
    explicit inline ListingLine(uint32_t codeAddress, int includeDepth, std::string && bodyText,
                                const Segment * segment, bool hasCodeBytesText)
        : codeBytes_{}, bodyText_(std::move(bodyText)), segment_(segment),
          codeAddress_(codeAddress), includeDepth_(includeDepth),
          hasCodeBytesText_(hasCodeBytesText) {}
 
public:
    inline CodeBytes const & codeBytes() const { return codeBytes_; }
    inline std::string const & bodyText() const { return bodyText_; }
    inline Segment const * segment() const { return segment_; }
    inline uint32_t codeAddress() const { return codeAddress_; }
    inline uint32_t includeDepth() const { return includeDepth_; }
    inline bool hasCodeBytesText() const { return hasCodeBytesText_; }

    static std::optional<ListingLine> fromString(const Segment * segment, std::string line);

    void setCodeBytes(EbRom const & rom, uint32_t nextLineCodeAddress);
    inline bool isEmpty() const {
        return std::regex_match(bodyText(), Re::whitespace) && !hasCodeBytesText();
    }
};
