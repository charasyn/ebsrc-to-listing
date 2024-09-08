#pragma once

#include <optional>
#include <regex>
namespace Re {
    template <class T>
    inline std::optional<std::smatch> matchRegex(std::regex regex, T str) {
        std::smatch ret;
        if (std::regex_match(str, ret, regex)) {
            return ret;
        }
        return {};
    }

    extern const std::regex whitespace;
    extern const std::regex forbiddenIncludes;
    extern const std::regex listingLine;
    extern const std::regex byteOrRelocation;
    extern const std::regex includeBody;
    extern const std::regex segmentBody;
    extern const std::regex mapFileSegmentHeader;
    extern const std::regex mapFileSegmentLine;
}
