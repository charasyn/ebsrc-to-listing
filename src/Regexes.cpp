#include "Regexes.hpp"

const std::regex Re::whitespace{R"/(\s*$)/"};

// We don't care about printing out the listings for these files.
const std::regex Re::forbiddenIncludes{
    "(?:"
    "structs|eventmacros|config|constants|enums|hardware|bin|common|"
    "macros|textmacros|symbols|text_data/.*\\.symbols"
    ").*"
};

const std::regex Re::listingLine{
    R"/(^([0-9A-F]{6}). (\d{1,2}) +((?:[0-9A-F][0-9A-F] |rr |   ){4}) (.*)$)/"
};

const std::regex Re::currentFileLine{
    R"/(Current file: src/(\S+)$)/"
};

const std::regex Re::byteOrRelocation{
    R"/((rr)|([0-9A-F][0-9A-F]) )/"
};

const std::regex Re::includeBody{
    R"/(\s*(\.INCLUDE|LOCALEINCLUDE) "([^"]+)")/"
};

const std::regex Re::segmentBody{
    R"/(\.SEGMENT "([^"]+)")/"
};

const std::regex Re::mapFileSegmentHeader{
    R"/(Segment list:)/"
};

const std::regex Re::mapFileSegmentLine{
    R"/((\w+) +([0-9A-Fa-f]{6})  ([0-9A-Fa-f]{6})  [0-9A-Fa-f]{6}  [0-9A-Fa-f]{5})/"
};
