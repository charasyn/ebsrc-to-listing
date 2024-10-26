// charasyn 2024
// Public domain; see UNLICENSE.txt for more info

#pragma once

#include <cstdint>
#include <istream>
#include <string>
#include <unordered_map>

#include "Exceptions.hpp"

struct Segment {
    std::string name;
    uint32_t start;
    uint32_t end;
};

class MapFileReader {
    std::unordered_map<std::string, Segment> segments_;
public:
    static MapFileReader fromIstream(std::istream & mapFile);
    inline Segment const & getSegment(std::string const & name) const {
        if (segments_.contains(name)) {
            return segments_.at(name);
        }
        throw malformed_listing("Unrecognized segment name");
    }
};
