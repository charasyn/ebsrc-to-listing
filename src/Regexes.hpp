#pragma once

#include <optional>
#include <regex>

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
extern const std::regex lineRe;
extern const std::regex byteOrRelocationRe;
extern const std::regex includeBodyRe;
extern const std::regex segmentBodyRe;
extern const std::regex mapFileSegmentHeaderRe;
extern const std::regex mapFileSegmentLineRe;
