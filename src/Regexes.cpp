#include "Regexes.hpp"

const std::regex whitespace{R"/(\s*$)/"};

// We don't care about printing out the listings for these files.
const std::regex forbiddenIncludes{
    "(?:"
    "structs|eventmacros|config|constants|enums|hardware|bin|common|"
    "macros|textmacros|symbols"
    ").*"
};

const std::regex lineRe{
    R"/(^([0-9A-F]{6}). \d+ +((?:[0-9A-F][0-9A-F] |rr |   ){4}) (.*)$)/"
};

const std::regex byteOrRelocationRe{
    R"/((rr)|([0-9A-F][0-9A-F]) )/"
};

const std::regex includeBodyRe{
    R"/(\.INCLUDE "([^"]+)")/"
};

const std::regex segmentBodyRe{
    R"/(\.SEGMENT "([^"]+)")/"
};

const std::regex mapFileSegmentHeaderRe{
    R"/(Segment list:)/"
};

const std::regex mapFileSegmentLineRe{
    R"/((\w+) +([0-9A-Fa-f]{6})  ([0-9A-Fa-f]{6})  [0-9A-Fa-f]{6}  [0-9A-Fa-f]{5})/"
};
